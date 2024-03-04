#include <stdio.h>
#include <stdlib.h>

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
TreeNodePtr avlInsert(TreeNodePtr root, int key);
void splayInsert(TreeNodePtr* rootPtr, int key);
TreeNodePtr rightRotation(TreeNodePtr alpha);
TreeNodePtr leftRotation(TreeNodePtr alpha);
TreeNodePtr splay(TreeNodePtr root, TreeNodePtr X);
void preOrder(TreeNodePtr root);

int avlCost = 0;
int splayCost = 0;

int main (int argc, char* argv[]) {
    TreeNodePtr avlRoot = NULL;
    TreeNodePtr splayRoot = NULL;

    if (argc == 1) 
        printf("There is no filename\n");
    else if (argc >= 2) {
        readFile(argv[1], &avlRoot, &splayRoot);

        printf("AVL tree:\n");
        preOrder(avlRoot);
        printf("\nTotal cost: %d\n", avlCost);

        printf("\nSplay tree:\n");
        preOrder(splayRoot);
        printf("\nTotal cost: %d", splayCost);
    }
    return 0;
}

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

int getHeight(TreeNodePtr node) {
    if (node == NULL) 
        return -1;
    else 
        return node->height;
}

int getBalance(TreeNodePtr node) {
    return getHeight(node->left) - getHeight(node->right);
}

TreeNodePtr readFile(char fileName[], TreeNodePtr* avlRoot, TreeNodePtr* splayRoot) {
    FILE* input = fopen(fileName, "r");

    if (input != NULL) {
        while (!feof(input)) {
            int data = 0;
            fscanf(input, "%d", &data);
            
            *avlRoot = avlInsert(*avlRoot, data);
            splayInsert(splayRoot, data);
        }
    }
    else
        printf("File not found\n");
}

TreeNodePtr avlInsert(TreeNodePtr root, int key) {
    if (root == NULL)
        return newNode(key);
    
    if (key < root->key) {
        avlCost += 3;
        root->left = avlInsert(root->left, key);
    }
    else if (key > root->key) {
        avlCost += 3;
        root->right = avlInsert(root->right, key);
    }
    else {
        avlCost += 3;
        root->freq++;
        return root;
    }
    
    root->height = MAX(getHeight(root->left), getHeight(root->right)) + 1;
    int balance = getBalance(root);

    // RR case
    if (balance < -1 && key > root->right->key) {
        avlCost++;
        return leftRotation(root);
    }

    // LL case
    if (balance > 1 && key < root->left->key) {
        avlCost++;
        return rightRotation(root);
    }

    // RL case
    if (balance > 1 && key > root->left->key) {
        avlCost += 2;
        root->left = leftRotation(root->left);
        return rightRotation(root);
    }

    // LR case
    if (balance < -1 && key < root->right->key) {
        avlCost += 2;
        root->right = rightRotation(root->right);
        return leftRotation(root);
    }
    return root;
} 

void splayInsert(TreeNodePtr* rootPtr, int key) {
    if (*rootPtr == NULL) 
        *rootPtr = newNode(key);
    else {
        TreeNodePtr temp = *rootPtr;
        TreeNodePtr current = NULL;

        while (temp != NULL) {
            current = temp;
            if (key < temp->key) {
                splayCost += 3;
                temp = temp->left;
            }
            else if (key > temp->key) {
                splayCost += 3;
                temp = temp->right;
            }
            else {
                splayCost += 3;
                current->freq++;
                *rootPtr = splay(*rootPtr, temp);
                return;
            }
        }
        
        TreeNodePtr newPtr = newNode(key);
        newPtr->parent = current;   

        if (key < current->key)
            current->left = newPtr;
        else if (key > current->key)
            current->right = newPtr;

        *rootPtr = splay(*rootPtr, newPtr);
    }
}

TreeNodePtr splay(TreeNodePtr root, TreeNodePtr X) {
    while (X->parent != NULL) {
        TreeNodePtr P = X->parent;
        TreeNodePtr G = P->parent;

        if (G == NULL) {
            if (X == P->left) {    
                splayCost++;

                root->left = X->right;
                if (X->right != NULL)
                    X->right->parent = root;

                X->right = root;
                root->parent = X;

                X->parent = NULL;
            }
            else {
                splayCost++;

                root->right = X->left;
                if (X->left != NULL)
                    X->left->parent = root;

                X->left = root;
                root->parent = X;  

                X->parent = NULL;
            }
        }

        // zig-zig RR case
        else if (P == G->right && X == P->right) {
            splayCost += 2;

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
            
            G = leftRotation(G);
            P = leftRotation(P);
        }

        // zig-zig LL case
        else if (P == G->left && X == P->left) {
            splayCost += 2;

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

            G = rightRotation(G);
            P = rightRotation(P);
        }

        // zig-zag RL case
        else if (P == G->left && X == P->right) {
            splayCost += 2;

            if (X->right != NULL)
                X->right->parent = G;
            if (X->left != NULL)
                X->left->parent = P;
            if (G->parent != NULL) {
                if (G->parent->right == G)
                    G->parent->right = X;
                else
                    G->parent->left = X;
            }
            X->parent = G->parent;
            P->parent = X;
            G->parent = X;

            G->left = leftRotation(G->left);
            G = rightRotation(G);
        }

        // zig-zag LR case
        else if (P == G->right && X == P->left) {
            splayCost += 2;

            if (X->left != NULL)
                X->left->parent = G;
            if (X->right != NULL)
                X->right->parent = P;
            if (G->parent != NULL) {
                if (G->parent->right == G)
                    G->parent->right = X;
                else
                    G->parent->left = X;
            }
            X->parent = G->parent;
            P->parent = X;
            G->parent = X;

            G->right = rightRotation(G->right);
            G = leftRotation(G);
        }
    }
    return X;
}

TreeNodePtr rightRotation(TreeNodePtr alpha) {
    TreeNodePtr k2 = alpha->left;

    alpha->left = k2->right;
    k2->right = alpha;

    alpha->height = MAX(getHeight(alpha->left), getHeight(alpha->right)) + 1;
    k2->height = MAX(getHeight(k2->left), getHeight(k2->right)) + 1;

    return k2;  
}

TreeNodePtr leftRotation(TreeNodePtr alpha) {
    TreeNodePtr k2 = alpha->right;

    alpha->right = k2->left;
    k2->left = alpha;

    alpha->height = MAX(getHeight(alpha->left), getHeight(alpha->right)) + 1;
    k2->height = MAX(getHeight(k2->left), getHeight(k2->right)) + 1;

    return k2;  
}

void preOrder(TreeNodePtr root) {
    if (root == NULL) 
        return;

    printf("%d ", root->key);
    preOrder(root->left);
    preOrder(root->right);
}