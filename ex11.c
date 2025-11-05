#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// Treap node structure
typedef struct Node {
    int key;
    int priority;
    struct Node *left;
    struct Node *right;
} Node;

// Function prototypes
Node* createNode(int key);
Node* rotateRight(Node* y);
Node* rotateLeft(Node* x);
Node* insert(Node* root, int key);
Node* delete(Node* root, int key);
Node* search(Node* root, int key);
void display(Node* root, int space);
void freeTreap(Node* root);
int isValidTreap(Node* root);
void inOrderTraversal(Node* root);

// Create a new node with random priority
Node* createNode(int key) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    newNode->key = key;
    newNode->priority = rand() % 1000; // Random priority between 0-999
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Right rotation
Node* rotateRight(Node* y) {
    if (y == NULL || y->left == NULL) return y;
    
    Node* x = y->left;
    Node* T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    return x;
}

// Left rotation
Node* rotateLeft(Node* x) {
    if (x == NULL || x->right == NULL) return x;
    
    Node* y = x->right;
    Node* T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    return y;
}

// Insert operation with error handling
Node* insert(Node* root, int key) {
    // Base case: create new node
    if (root == NULL) {
        Node* newNode = createNode(key);
        if (newNode == NULL) {
            return NULL; // Memory allocation failed
        }
        printf("Inserted key %d with priority %d\n", key, newNode->priority);
        return newNode;
    }
    
    // Check for duplicate key
    if (key == root->key) {
        printf("Error: Key %d already exists in the Treap!\n", key);
        return root;
    }
    
    // BST insertion
    if (key < root->key) {
        root->left = insert(root->left, key);
        
        // Fix heap property if violated
        if (root->left != NULL && root->left->priority > root->priority) {
            root = rotateRight(root);
        }
    } else {
        root->right = insert(root->right, key);
        
        // Fix heap property if violated
        if (root->right != NULL && root->right->priority > root->priority) {
            root = rotateLeft(root);
        }
    }
    
    return root;
}

// Delete operation with error handling
Node* delete(Node* root, int key) {
    if (root == NULL) {
        printf("Error: Key %d not found for deletion!\n", key);
        return NULL;
    }
    
    if (key < root->key) {
        root->left = delete(root->left, key);
    } else if (key > root->key) {
        root->right = delete(root->right, key);
    } else {
        // Key found
        printf("Deleted key %d with priority %d\n", key, root->priority);
        
        // Case 1: Node is leaf
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }
        // Case 2: Node has only right child
        else if (root->left == NULL) {
            Node* temp = root->right;
            free(root);
            return temp;
        }
        // Case 3: Node has only left child
        else if (root->right == NULL) {
            Node* temp = root->left;
            free(root);
            return temp;
        }
        // Case 4: Node has two children
        else {
            // Rotate based on priorities and recursively delete
            if (root->left->priority > root->right->priority) {
                root = rotateRight(root);
                root->right = delete(root->right, key);
            } else {
                root = rotateLeft(root);
                root->left = delete(root->left, key);
            }
        }
    }
    
    return root;
}

// Search operation
Node* search(Node* root, int key) {
    if (root == NULL || root->key == key) {
        return root;
    }
    
    if (key < root->key) {
        return search(root->left, key);
    } else {
        return search(root->right, key);
    }
}

// Display Treap structure (visual representation)
void display(Node* root, int space) {
    const int COUNT = 5; // Space between levels
    
    if (root == NULL) {
        return;
    }
    
    // Increase distance between levels
    space += COUNT;
    
    // Process right child first
    display(root->right, space);
    
    // Print current node
    printf("\n");
    for (int i = COUNT; i < space; i++) {
        printf(" ");
    }
    printf("(%d, %d)\n", root->key, root->priority);
    
    // Process left child
    display(root->left, space);
}

// Free memory allocated for Treap
void freeTreap(Node* root) {
    if (root == NULL) return;
    
    freeTreap(root->left);
    freeTreap(root->right);
    free(root);
}

// Validate Treap properties (for debugging)
int isValidTreap(Node* root) {
    if (root == NULL) return 1;
    
    // Check BST property
    if (root->left != NULL && root->left->key >= root->key) {
        return 0;
    }
    if (root->right != NULL && root->right->key <= root->key) {
        return 0;
    }
    
    // Check heap property
    if (root->left != NULL && root->left->priority > root->priority) {
        return 0;
    }
    if (root->right != NULL && root->right->priority > root->priority) {
        return 0;
    }
    
    // Recursively check subtrees
    return isValidTreap(root->left) && isValidTreap(root->right);
}

// In-order traversal to display sorted keys
void inOrderTraversal(Node* root) {
    if (root == NULL) return;
    
    inOrderTraversal(root->left);
    printf("(%d, %d) ", root->key, root->priority);
    inOrderTraversal(root->right);
}

// Menu-driven interface
void displayMenu() {
    printf("\n=== Treap Operations Menu ===\n");
    printf("1. Insert a key\n");
    printf("2. Delete a key\n");
    printf("3. Search for a key\n");
    printf("4. Display Treap structure\n");
    printf("5. Display in-order traversal\n");
    printf("6. Validate Treap properties\n");
    printf("7. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    Node* root = NULL;
    int choice, key;
    
    // Seed random number generator
    srand(time(NULL));
    
    printf("Treap Implementation in C\n");
    
    while (1) {
        displayMenu();
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        switch (choice) {
            case 1:
                printf("Enter key to insert: ");
                if (scanf("%d", &key) != 1) {
                    printf("Invalid key input!\n");
                    while (getchar() != '\n');
                    break;
                }
                root = insert(root, key);
                break;
                
            case 2:
                if (root == NULL) {
                    printf("Treap is empty! Cannot delete.\n");
                    break;
                }
                printf("Enter key to delete: ");
                if (scanf("%d", &key) != 1) {
                    printf("Invalid key input!\n");
                    while (getchar() != '\n');
                    break;
                }
                root = delete(root, key);
                break;
                
            case 3:
                if (root == NULL) {
                    printf("Treap is empty! No elements to search. Please insert elements first.\n");
                    break;
                }
                printf("Enter key to search: ");
                if (scanf("%d", &key) != 1) {
                    printf("Invalid key input!\n");
                    while (getchar() != '\n');
                    break;
                }
                Node* result = search(root, key);
                if (result != NULL) {
                    printf("Key %d found with priority %d\n", key, result->priority);
                } else {
                    printf("Key %d not found in the Treap\n", key);
                }
                break;
                
            case 4:
                if (root == NULL) {
                    printf("Treap is empty! Please insert elements first.\n");
                } else {
                    printf("Treap Structure (rotated 90 degrees counter-clockwise):\n");
                    display(root, 0);
                    printf("\n");
                }
                break;
                
            case 5:
                if (root == NULL) {
                    printf("Treap is empty! Please insert elements first.\n");
                } else {
                    printf("In-order traversal (sorted by key): ");
                    inOrderTraversal(root);
                    printf("\n");
                }
                break;
                
            case 6:
                if (root == NULL) {
                    printf("Treap is empty! Please insert elements first.\n");
                } else {
                    if (isValidTreap(root)) {
                        printf("Treap properties are valid!\n");
                    } else {
                        printf("ERROR: Treap properties violated!\n");
                    }
                }
                break;
                
            case 7:
                printf("Exiting program...\n");
                freeTreap(root);
                exit(0);
                
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    
    return 0;
}