// CSE2225 PROJECT2
// UMUT OZIL 150121019

#include <stdio.h>
#include <stdlib.h>

// Struct for the tree node
struct treeNode {
    int key;
    int height;
    int freq;

    struct treeNode* parent;
    struct treeNode* left;
    struct treeNode* right;
};

typedef struct treeNode TreeNode;
typedef struct treeNode* TreeNodePtr;

#define MAX(a,b) ((a>b)? a:b)

TreeNodePtr newNode(int item);
int getHeight(TreeNodePtr node);
int getBalance(TreeNodePtr node);
TreeNodePtr readFile(char fileName[], TreeNodePtr* avlRoot, TreeNodePtr* splayRoot);
void avlInsert(TreeNodePtr* rootPtr, int key);
void splayInsert(TreeNodePtr* rootPtr, int key);
TreeNodePtr singleRotation(TreeNodePtr alpha, int direction);
TreeNodePtr doubleRotation(TreeNodePtr alpha, int direction);
TreeNodePtr splay(TreeNodePtr root, TreeNodePtr X);
TreeNodePtr zigzig(TreeNodePtr G, int direction);
void preOrder(TreeNodePtr root);

int avlCost = 0;
int splayCost = 0;

int main (int argc, char* argv[]) {
    // Create a root pointers for the avl and splay trees
    TreeNodePtr avlRoot = NULL;
    TreeNodePtr splayRoot = NULL;

    if (argc == 1) 
        printf("There is no filename\n");
    else if (argc >= 2) {
        readFile(argv[1], &avlRoot, &splayRoot);

        // Print the avl and spaly tree as preorder traversal and their costs
        printf("AVL tree:\n");
        preOrder(avlRoot);
        printf("\nTotal cost: %d\n", avlCost);

        printf("\nSplay tree:\n");
        preOrder(splayRoot);
        printf("\nTotal cost: %d\n", splayCost);
    }
    return 0;
}

/* This function creates new node with given key */
TreeNodePtr newNode(int item) {
    TreeNodePtr newPtr = malloc(sizeof(TreeNode));

    if (newPtr != NULL) {
        newPtr->key = item;
        newPtr->parent = NULL;
        newPtr->left = NULL;
        newPtr->right = NULL;
        newPtr->height = 0;
        newPtr->freq = 1;
    }
    else
        printf("No memory available.\n");

    return newPtr;
}

/* This function returns given node's height, if the given node is null returns -1 */
int getHeight(TreeNodePtr node) {
    if (node == NULL) 
        return -1;
    else 
        return node->height;
}

/* This function returns balance factor for the avl tree 
subtracting height of right child from height of left child of the given node */
int getBalance(TreeNodePtr node) {
    return getHeight(node->left) - getHeight(node->right);
}

/* This function reads numbers from input file 
and calls the funcitons to insert the numbers to the avl and splay trees */
TreeNodePtr readFile(char fileName[], TreeNodePtr* avlRoot, TreeNodePtr* splayRoot) {
    FILE* input = fopen(fileName, "r");

    if (input != NULL) {
        int data = 0;
        // Get next number and if it is not equal to "EOF" insert the number to the avl and splay trees
        while (fscanf(input, "%d", &data) != EOF) {
            avlInsert(avlRoot, data);
            splayInsert(splayRoot, data);
        }
    }
    else
        printf("File not found\n");
}

/* This funciton inserts the given key to the given root of avl tree */
void avlInsert(TreeNodePtr* rootPtr, int key) {
    // If the tree is empty directly insert the key and make it root
    if (*rootPtr == NULL) 
        *rootPtr = newNode(key);
    else {
        TreeNodePtr temp = *rootPtr;
        TreeNodePtr current = NULL;

        // Navigate the tree to find where to insert the new node
        while (temp != NULL) {
            current = temp;
            // if key is smaller than node go to the left child
            if (key < temp->key) {
                avlCost++;
                temp = temp->left;
            }
            // if key is bigger than node go to the right child
            else if (key > temp->key) {
                avlCost++;
                temp = temp->right;
            }
            // if key is equal to the node increase its frequency and exit the function 
            else {
                avlCost++;
                current->freq++;
                return;
            }
        }

        // Create a new node and assign its parent
        TreeNodePtr newPtr = newNode(key);
        newPtr->parent = current;   

        // Insert the node to the right or left of the leaf according to its key
        if (key < current->key)
            current->left = newPtr;
        else if (key > current->key)
            current->right = newPtr;

        // Update the heights and check the balance factors of node's by going from new inserted node to the root
        for(; current != NULL; current = current->parent) {
            current->height = MAX(getHeight(current->left), getHeight(current->right)) + 1;
            int balance = getBalance(current);

            // RR case
            if (balance < -1 && key > current->right->key) {
                avlCost++;
                current = singleRotation(current, 0);
                if (current->parent == NULL)
                    *rootPtr = current;
            }

            // LL case
            else if (balance > 1 && key < current->left->key) {
                avlCost++;
                current = singleRotation(current, 1);
                if (current->parent == NULL)
                    *rootPtr = current;
            }   

            // RL case
            else if (balance > 1 && key > current->left->key) {
                avlCost += 2;
                current = doubleRotation(current, 0);
                if (current->parent == NULL)
                    *rootPtr = current;
            }

            // LR case
            else if (balance < -1 && key < current->right->key) {
                avlCost += 2;
                current = doubleRotation(current, 1);
                if (current->parent == NULL)
                    *rootPtr = current;
            }
        }
    }
}

/* This funciton inserts the given key to the given root of spaly tree and splays it */
void splayInsert(TreeNodePtr* rootPtr, int key) {
    // If the tree is empty directly insert the key and make it root
    if (*rootPtr == NULL) 
        *rootPtr = newNode(key);
    else {
        TreeNodePtr temp = *rootPtr;
        TreeNodePtr current = NULL;

        // Navigate the tree to find where to insert the new node
        while (temp != NULL) {
            current = temp;
            // if key is smaller than node go to the left child
            if (key < temp->key) {
                splayCost++;
                temp = temp->left;
            }
            // if key is bigger than node go to the right child
            else if (key > temp->key) {
                splayCost++;
                temp = temp->right;
            }
            // if key is equal to the node increase its frequency and splay it to the root 
            else {
                splayCost++;
                current->freq++;
                *rootPtr = splay(*rootPtr, current);
                return;
            }
        }

        // Create a new node and assign its parent
        TreeNodePtr newPtr = newNode(key);
        newPtr->parent = current;   

        // Insert the node to the right or left of the leaf according to its key
        if (key < current->key)
            current->left = newPtr;
        else if (key > current->key)
            current->right = newPtr;

        // After inserting the new node splay it to the root
        *rootPtr = splay(*rootPtr, newPtr);
        
    }
}

/* This function performs a single rotation if the direction is 0 to the left and if the direction is 1 to the right
and reassigns the parent's of the rotated nodes */
TreeNodePtr singleRotation(TreeNodePtr alpha, int direction) {
    TreeNodePtr k2 = NULL;

    if (direction == 0) {
        k2 = alpha->right;

        if (k2->left != NULL)
            k2->left->parent = alpha;
        if (alpha->parent != NULL) {
            if (alpha->parent->right == alpha) 
                alpha->parent->right = k2;
            else
                alpha->parent->left = k2;
        }
        k2->parent = alpha->parent;
        alpha->parent = k2;

        alpha->right = k2->left;
        k2->left = alpha;
    }
    else {
        k2 = alpha->left;

        if (k2->right != NULL)
            k2->right->parent = alpha;
        if (alpha->parent != NULL) {
            if (alpha->parent->right == alpha) 
                alpha->parent->right = k2;
            else
                alpha->parent->left = k2;
        }
        k2->parent = alpha->parent;
        alpha->parent = k2;

        alpha->left = k2->right;
        k2->right = alpha;
    }
    alpha->height = MAX(getHeight(alpha->left), getHeight(alpha->right)) + 1;
    k2->height = MAX(getHeight(k2->left), getHeight(k2->right)) + 1;

    return k2;
}

/* This function performs a single rotation if direction is 0 it means a RL case if direction is 1 it means a LR case
and reassigns the parent's of the rotated nodes */
TreeNodePtr doubleRotation(TreeNodePtr alpha, int direction) {
    TreeNodePtr k2 = NULL;
    TreeNodePtr k3 = NULL;

    if (direction == 0) {
        k2 = alpha->left;
        k3 = k2->right;

        if (k3->right != NULL)
                k3->right->parent = alpha;
        if (k3->left != NULL)
            k3->left->parent = k2;
        if (alpha->parent != NULL) {
            if (alpha->parent->left == alpha)
                alpha->parent->left = k3;
            else
                alpha->parent->right = k3;
        }

        k3->parent = alpha->parent;
        k2->parent = k3;
        alpha->parent = k3;

        alpha->left = k3->right;
        k2->right = k3->left;
        k3->right = alpha;
        k3->left = k2; 
    }
    else {
        k2 = alpha->right;
        k3 = k2->left;

        if (k3->left != NULL)
                k3->left->parent = alpha;
        if (k3->right != NULL)
            k3->right->parent = k2;
        if (alpha->parent != NULL) {
            if (alpha->parent->right == alpha)
                alpha->parent->right = k3;
            else
                alpha->parent->left = k3;
        }

        k3->parent = alpha->parent;
        k2->parent = k3;
        alpha->parent = k3;

        alpha->right = k3->left;
        k2->left = k3->right;
        k3->left = alpha;
        k3->right = k2; 
    }
    alpha->height = MAX(getHeight(alpha->left), getHeight(alpha->right)) + 1;
    k2->height = MAX(getHeight(k2->left), getHeight(k2->right)) + 1;
    k3->height = MAX(getHeight(k3->left), getHeight(k3->right)) + 1;

    return k3;
}

/* This funciton performes the spaly operations to move the given node to the root*/
TreeNodePtr splay(TreeNodePtr root, TreeNodePtr X) {
    // Find the given node's parent and grandparent
    while (X->parent != NULL) {
        TreeNodePtr P = X->parent;
        TreeNodePtr G = P->parent;

        // If the node is the child of the root performe single rotation
        if (G == NULL) {
            splayCost++;
            if (X == P->left)
                root = singleRotation(root, 1);
            else
                root = singleRotation(root, 0);
        }

        // zig-zig RR case
        else if (P == G->right && X == P->right) {
            splayCost += 2;
            G = zigzig(G, 0);
        }

        // zig-zig LL case
        else if (P == G->left && X == P->left) {
            splayCost += 2;
            G = zigzig(G, 1);
        }

        // zig-zag RL case
        else if (P == G->left && X == P->right) {
            splayCost += 2;
            G = doubleRotation(G, 0);
        }

        // zig-zag LR case
        else if (P == G->right && X == P->left) {
            splayCost += 2;
            G = doubleRotation(G, 1);
        }
    }
    return X;
}

/* This function performs a zigzig rotation if the direction is 0 it means RR case and if the direction is 1 it means LL case
and reassigns the parent's of the rotated nodes */
TreeNodePtr zigzig(TreeNodePtr G, int direction) {
    TreeNodePtr P = NULL;
    TreeNodePtr X = NULL;

    if (direction == 0) {
        P = G->right;
        X = P->right;

        if (X->left != NULL)
            X->left->parent = P;
        if (P->left != NULL)
            P->left->parent = G;
        if (G->parent != NULL) {
            if (G->parent->right == G)
                G->parent->right = X;
            else
                G->parent->left = X;
        }   
        X->parent = G->parent;
        G->parent = P;
        P->parent = X;

        G->right = P->left;
        P->right = X->left;
        P->left = G;
        X->left = P;
    }
    else {
        P = G->left;
        X = P->left;

        if (X->right != NULL)
            X->right->parent = P;
        if (P->right != NULL)
            P->right->parent = G;
        if (G->parent != NULL) {
            if (G->parent->right == G)
                G->parent->right = X;
            else
                G->parent->left = X;
        }
        X->parent = G->parent;
        G->parent = P;
        P->parent = X;

        G->left = P->right;
        P->left = X->right;
        P->right = G;
        X->right = P;
    }
    return X;
}

/* This funtion prints the tree by preorder traversal*/
void preOrder(TreeNodePtr root) {
    if (root == NULL) 
        return;

    printf("%d ", root->key);
    preOrder(root->left);
    preOrder(root->right);
}