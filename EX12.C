#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Structure for an edge in the graph
struct Edge {
    int src, dest, weight;
};

// Structure for the graph
struct Graph {
    int V, E;
    struct Edge* edgeList;
};

// Structure for Disjoint Set Union (DSU)
struct DisjointSet {
    int *parent;
    int *rank;    // for union by rank
    int *size;    // for union by size
    int n;
};

// Structure for MST result
struct MST {
    struct Edge* edges;
    int totalEdges;
    int totalCost;
    int exists;
};

// Global variables
struct Graph graph = {0, 0, NULL};
struct MST mstResult = {NULL, 0, 0, 0};
int graphInitialized = 0;

// Function prototypes
struct Graph createGraph(int V, int E);
int inputGraph();
struct DisjointSet* createDSU(int n, int useSize);
int find(struct DisjointSet* dsu, int i);
void unionByRank(struct DisjointSet* dsu, int x, int y);
void unionBySize(struct DisjointSet* dsu, int x, int y);
int compareEdges(const void* a, const void* b);
int kruskalMST(int useSize);
void displayMST();
void freeMemory();
int validateGraph();
int isConnected();

// Utility function to swap two integers
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Create graph with given vertices and edges
struct Graph createGraph(int V, int E) {
    struct Graph graph;
    graph.V = V;
    graph.E = E;
    graph.edgeList = (struct Edge*)malloc(E * sizeof(struct Edge));
    return graph;
}

// Input graph from user with validation
int inputGraph() {
    // Free previous memory if any
    freeMemory();
    
    int V, E;
    
    printf("\n=== GRAPH INPUT ===\n");
    
    // Input number of vertices with validation
    while (1) {
        printf("Enter number of vertices: ");
        if (scanf("%d", &V) != 1) {
            printf("Error: Invalid input. Please enter an integer.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        if (V <= 0) {
            printf("Error: Number of vertices must be positive.\n");
            continue;
        }
        if (V > 1000) {
            printf("Warning: Large number of vertices may affect performance.\n");
        }
        break;
    }
    
    // Input number of edges with validation
    while (1) {
        printf("Enter number of edges: ");
        if (scanf("%d", &E) != 1) {
            printf("Error: Invalid input. Please enter an integer.\n");
            while (getchar() != '\n');
            continue;
        }
        if (E < 0) {
            printf("Error: Number of edges cannot be negative.\n");
            continue;
        }
        if (E > V * (V - 1) / 2) {
            printf("Error: Too many edges for %d vertices (maximum: %d).\n", V, V * (V - 1) / 2);
            continue;
        }
        break;
    }
    
    // Create graph
    graph = createGraph(V, E);
    
    if (E == 0) {
        printf("Graph with %d vertices and 0 edges created.\n", V);
        graphInitialized = 1;
        return 1;
    }
    
    printf("\nEnter edges (source destination weight):\n");
    printf("Note: Vertices are numbered from 0 to %d\n", V - 1);
    
    for (int i = 0; i < E; i++) {
        int src, dest, weight;
        int validInput = 0;
        
        while (!validInput) {
            printf("Edge %d: ", i + 1);
            if (scanf("%d %d %d", &src, &dest, &weight) != 3) {
                printf("Error: Invalid input. Please enter three integers.\n");
                while (getchar() != '\n');
                continue;
            }
            
            // Validate vertex indices
            if (src < 0 || src >= V || dest < 0 || dest >= V) {
                printf("Error: Vertex indices must be between 0 and %d.\n", V - 1);
                continue;
            }
            
            // Check for self-loop
            if (src == dest) {
                printf("Error: Self-loops are not allowed in MST algorithms.\n");
                continue;
            }
            
            // Ensure src <= dest for consistency
            if (src > dest) {
                swap(&src, &dest);
            }
            
            // Check for duplicate edges
            int duplicate = 0;
            for (int j = 0; j < i; j++) {
                if (graph.edgeList[j].src == src && graph.edgeList[j].dest == dest) {
                    printf("Error: Duplicate edge between %d and %d.\n", src, dest);
                    duplicate = 1;
                    break;
                }
            }
            if (duplicate) {
                continue;
            }
            
            graph.edgeList[i].src = src;
            graph.edgeList[i].dest = dest;
            graph.edgeList[i].weight = weight;
            validInput = 1;
        }
    }
    
    graphInitialized = 1;
    printf("\nGraph created successfully with %d vertices and %d edges.\n", V, E);
    return 1;
}

// Create Disjoint Set Union structure
struct DisjointSet* createDSU(int n, int useSize) {
    struct DisjointSet* dsu = (struct DisjointSet*)malloc(sizeof(struct DisjointSet));
    dsu->n = n;
    dsu->parent = (int*)malloc(n * sizeof(int));
    dsu->rank = (int*)malloc(n * sizeof(int));
    dsu->size = (int*)malloc(n * sizeof(int));
    
    for (int i = 0; i < n; i++) {
        dsu->parent[i] = i;
        dsu->rank[i] = 0;
        dsu->size[i] = 1;
    }
    
    return dsu;
}

// Find with path compression
int find(struct DisjointSet* dsu, int i) {
    if (dsu->parent[i] != i) {
        dsu->parent[i] = find(dsu, dsu->parent[i]); // Path compression
    }
    return dsu->parent[i];
}

// Union by rank
void unionByRank(struct DisjointSet* dsu, int x, int y) {
    int rootX = find(dsu, x);
    int rootY = find(dsu, y);
    
    if (rootX != rootY) {
        if (dsu->rank[rootX] < dsu->rank[rootY]) {
            dsu->parent[rootX] = rootY;
        } else if (dsu->rank[rootX] > dsu->rank[rootY]) {
            dsu->parent[rootY] = rootX;
        } else {
            dsu->parent[rootY] = rootX;
            dsu->rank[rootX]++;
        }
    }
}

// Union by size
void unionBySize(struct DisjointSet* dsu, int x, int y) {
    int rootX = find(dsu, x);
    int rootY = find(dsu, y);
    
    if (rootX != rootY) {
        if (dsu->size[rootX] < dsu->size[rootY]) {
            dsu->parent[rootX] = rootY;
            dsu->size[rootY] += dsu->size[rootX];
        } else {
            dsu->parent[rootY] = rootX;
            dsu->size[rootX] += dsu->size[rootY];
        }
    }
}

// Comparator function for sorting edges by weight
int compareEdges(const void* a, const void* b) {
    struct Edge* edge1 = (struct Edge*)a;
    struct Edge* edge2 = (struct Edge*)b;
    return edge1->weight - edge2->weight;
}

// Check if graph is connected using BFS/DFS (simplified DSU check)
int isConnected() {
    if (graph.V == 0) return 0;
    if (graph.V == 1) return 1;
    if (graph.E == 0) return 0;
    
    struct DisjointSet* dsu = createDSU(graph.V, 0);
    
    for (int i = 0; i < graph.E; i++) {
        int x = find(dsu, graph.edgeList[i].src);
        int y = find(dsu, graph.edgeList[i].dest);
        if (x != y) {
            unionByRank(dsu, x, y);
        }
    }
    
    // Check if all vertices are in the same set
    int root = find(dsu, 0);
    for (int i = 1; i < graph.V; i++) {
        if (find(dsu, i) != root) {
            free(dsu->parent);
            free(dsu->rank);
            free(dsu->size);
            free(dsu);
            return 0;
        }
    }
    
    free(dsu->parent);
    free(dsu->rank);
    free(dsu->size);
    free(dsu);
    return 1;
}

// Validate graph for MST computation
int validateGraph() {
    if (!graphInitialized) {
        printf("Error: No graph data available. Please input a graph first.\n");
        return 0;
    }
    
    if (graph.V == 0) {
        printf("Error: Graph has no vertices.\n");
        return 0;
    }
    
    if (!isConnected()) {
        printf("Error: Graph is not connected. MST does not exist for disconnected graphs.\n");
        return 0;
    }
    
    return 1;
}

// Kruskal's algorithm implementation
int kruskalMST(int useSize) {
    if (!validateGraph()) {
        return 0;
    }
    
    // Free previous MST result if any
    if (mstResult.edges != NULL) {
        free(mstResult.edges);
        mstResult.edges = NULL;
    }
    
    // For single vertex graph
    if (graph.V == 1) {
        mstResult.edges = (struct Edge*)malloc(0);
        mstResult.totalEdges = 0;
        mstResult.totalCost = 0;
        mstResult.exists = 1;
        printf("MST computed: Single vertex graph (no edges).\n");
        return 1;
    }
    
    // Sort all edges in non-decreasing order of their weight
    qsort(graph.edgeList, graph.E, sizeof(graph.edgeList[0]), compareEdges);
    
    // Allocate memory for MST (V-1 edges maximum)
    struct Edge* mstEdges = (struct Edge*)malloc((graph.V - 1) * sizeof(struct Edge));
    int mstIndex = 0;
    int totalCost = 0;
    
    // Create DSU
    struct DisjointSet* dsu = createDSU(graph.V, useSize);
    
    // Process each edge in sorted order
    for (int i = 0; i < graph.E && mstIndex < graph.V - 1; i++) {
        int src = graph.edgeList[i].src;
        int dest = graph.edgeList[i].dest;
        
        int rootSrc = find(dsu, src);
        int rootDest = find(dsu, dest);
        
        // If including this edge doesn't cause cycle, include it in MST
        if (rootSrc != rootDest) {
            mstEdges[mstIndex] = graph.edgeList[i];
            mstIndex++;
            totalCost += graph.edgeList[i].weight;
            
            if (useSize) {
                unionBySize(dsu, rootSrc, rootDest);
            } else {
                unionByRank(dsu, rootSrc, rootDest);
            }
        }
    }
    
    // Store MST result
    mstResult.edges = mstEdges;
    mstResult.totalEdges = mstIndex;
    mstResult.totalCost = totalCost;
    mstResult.exists = 1;
    
    // Free DSU memory
    free(dsu->parent);
    free(dsu->rank);
    free(dsu->size);
    free(dsu);
    
    printf("MST computed successfully using %s.\n", useSize ? "Union by Size" : "Union by Rank");
    return 1;
}

// Display MST edges and total cost
void displayMST() {
    if (!graphInitialized) {
        printf("Error: No graph data available.\n");
        return;
    }
    
    if (!mstResult.exists) {
        printf("Error: MST has not been computed yet.\n");
        return;
    }
    
    printf("\n=== MINIMUM SPANNING TREE ===\n");
    printf("Number of vertices: %d\n", graph.V);
    printf("Number of edges in MST: %d\n", mstResult.totalEdges);
    
    if (mstResult.totalEdges == 0) {
        if (graph.V == 1) {
            printf("MST for single vertex graph (no edges).\n");
        } else {
            printf("No MST edges found (graph might be disconnected).\n");
        }
        printf("Total Minimum Cost: 0\n");
        return;
    }
    
    printf("\nMST Edges:\n");
    for (int i = 0; i < mstResult.totalEdges; i++) {
        printf("Edge %d: %d - %d (Weight: %d)\n", 
               i + 1, mstResult.edges[i].src, mstResult.edges[i].dest, mstResult.edges[i].weight);
    }
    
    printf("\nTotal Minimum Cost: %d\n", mstResult.totalCost);
}

// Free allocated memory
void freeMemory() {
    if (graph.edgeList != NULL) {
        free(graph.edgeList);
        graph.edgeList = NULL;
    }
    if (mstResult.edges != NULL) {
        free(mstResult.edges);
        mstResult.edges = NULL;
    }
    graphInitialized = 0;
    mstResult.exists = 0;
    graph.V = 0;
    graph.E = 0;
    mstResult.totalEdges = 0;
    mstResult.totalCost = 0;
}

// Main menu-driven program
int main() {
    int choice;
    int graphCreated = 0;
    
    printf("=== KRUSKAL'S ALGORITHM FOR MINIMUM SPANNING TREE ===\n");
    printf("Implementation with Disjoint Set Union (DSU)\n");
    printf("Features: Path Compression, Union by Rank/Size\n\n");
    
    while (1) {
        printf("\n=== MAIN MENU ===\n");
        printf("1. Input Graph\n");
        printf("2. Find MST using Union by Rank\n");
        printf("3. Find MST using Union by Size\n");
        printf("4. Display MST\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Error: Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1:
                if (inputGraph()) {
                    graphCreated = 1;
                    mstResult.exists = 0; // Reset MST when new graph is input
                }
                break;
                
            case 2:
                if (graphInitialized) {
                    kruskalMST(0); // 0 for Union by Rank
                } else {
                    printf("Error: Please input a graph first.\n");
                }
                break;
                
            case 3:
                if (graphInitialized) {
                    kruskalMST(1); // 1 for Union by Size
                } else {
                    printf("Error: Please input a graph first.\n");
                }
                break;
                
            case 4:
                displayMST();
                break;
                
            case 5:
                printf("Exiting program. Goodbye!\n");
                freeMemory();
                exit(0);
                
            default:
                printf("Error: Invalid choice. Please enter 1-5.\n");
        }
    }
    
    return 0;
}