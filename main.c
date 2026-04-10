#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MAX_NAME 50
#define INITIAL_CITIES 100
#define MAX_EDGES 10000

char **cities = NULL;
int cityCount = 0;
int maxCities = 0;

struct Edge {
    int src;
    int dst;
    int weight;
};

struct Edge edges[MAX_EDGES];
int edgeCount = 0;

struct AdjNode {
    int dst;
    int weight;
    struct AdjNode* next;
};

struct AdjList {
    struct AdjNode* head;
};

struct Graph {
    int V;
    struct AdjList array[MAX_EDGES];
};

struct Subset {
    int parent;
    int rank;
};

struct MinHeapNode {
    int v;
    int key;
};

struct MinHeap {
    int size;
    int capacity;
    int *pos;
    struct MinHeapNode **array;
};

int findCityIndex(const char *name) {
    for (int i = 0; i < cityCount; i++)
        if (strcmp(cities[i], name) == 0)
            return i;

    if (cityCount >= maxCities) {
        maxCities *= 2;
        cities = realloc(cities, sizeof(char*) * maxCities);
    }
    cities[cityCount] = malloc(MAX_NAME);
    strcpy(cities[cityCount], name);
    cityCount++;
    return cityCount - 1;
}

void addEdge(struct Graph* graph, int src, int dst, int weight) {
    struct AdjNode* newNode = (struct AdjNode*)malloc(sizeof(struct AdjNode));
    newNode->dst = dst;
    newNode->weight = weight;
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
}

void loadCities(struct Graph* graph) {
    edgeCount = 0;
    cityCount = 0;
    for (int i = 0; i < MAX_EDGES; i++)
        graph->array[i].head = NULL;

    FILE *file = fopen("cities.txt", "r");
    if (!file) {
        printf("Could not open cities.txt\n");
        return;
    }

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char city1[MAX_NAME], city2[MAX_NAME];
        int weight;
        if (sscanf(line, " %[^#]#%[^#]#%d", city1, city2, &weight) == 3) {
            int idx1 = findCityIndex(city1);
            int idx2 = findCityIndex(city2);

            edges[edgeCount].src = idx1;
            edges[edgeCount].dst = idx2;
            edges[edgeCount].weight = weight;

            edgeCount++;

            addEdge(graph, idx1, idx2, weight);
            addEdge(graph, idx2, idx1, weight);
        }
    }
    graph->V = cityCount;
    fclose(file);
    printf("Graph loaded: Order = %d, Size = %d\n", graph->V, edgeCount);
}

int find(struct Subset subsets[], int i) {
    if (subsets[i].parent != i)
        subsets[i].parent = find(subsets, subsets[i].parent);
    return subsets[i].parent;
}

void Union(struct Subset subsets[], int x, int y) {
    int xroot = find(subsets, x);
    int yroot = find(subsets, y);
    if (subsets[xroot].rank < subsets[yroot].rank)
        subsets[xroot].parent = yroot;
    else if (subsets[xroot].rank > subsets[yroot].rank)
        subsets[yroot].parent = xroot;
    else {
        subsets[yroot].parent = xroot;
        subsets[xroot].rank++;
    }
}

void merge(struct Edge arr[], int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    struct Edge L[n1], R[n2];
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
        arr[k++] = (L[i].weight <= R[j].weight) ? L[i++] : R[j++];
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(struct Edge arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int kruskalMST() {
    struct Edge result[MAX_EDGES];
    int e = 0, i = 0, total_cost = 0;

    mergeSort(edges, 0, edgeCount - 1);

    struct Subset subsets[cityCount];
    for (int v = 0; v < cityCount; v++) {
        subsets[v].parent = v;
        subsets[v].rank = 0;
    }

    while (e < cityCount - 1 && i < edgeCount) {
        struct Edge next = edges[i++];
        int x = find(subsets, next.src);
        int y = find(subsets, next.dst);
        if (x != y) {
            result[e++] = next;
            Union(subsets, x, y);
            total_cost += next.weight;
        }
    }

    printf("Kruskal's MST Edges:\n");
    for (int j = 0; j < e; j++)
        printf("%s - %s : %d\n",cities[result[j].src], cities[result[j].dst], result[j].weight);
    printf("Total Cost: %d\n", total_cost);
    return total_cost;
}

struct MinHeapNode* newMinHeapNode(int v, int key) {
    struct MinHeapNode* node = (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
    node->v = v;
    node->key = key;
    return node;
}

struct MinHeap* createMinHeap(int capacity) {
    struct MinHeap* heap = (struct MinHeap*) malloc(sizeof(struct MinHeap));
    heap->pos = (int*) malloc(capacity * sizeof(int));
    heap->size = 0;
    heap->capacity = capacity;
heap->array = (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
    return heap;
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(struct MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size && heap->array[left]->key < heap->array[smallest]->key)
        smallest = left;
    if (right < heap->size && heap->array[right]->key < heap->array[smallest]->key)
        smallest = right;

    if (smallest != idx) {
        struct MinHeapNode* smallestNode = heap->array[smallest];
        struct MinHeapNode* idxNode = heap->array[idx];

        heap->pos[smallestNode->v] = idx;
        heap->pos[idxNode->v] = smallest;

        swapMinHeapNode(&heap->array[smallest], &heap->array[idx]);
        minHeapify(heap, smallest);
    }
}

int isEmpty(struct MinHeap* heap) {
    return heap->size == 0;
}

struct MinHeapNode* extractMin(struct MinHeap* heap) {
    if (isEmpty(heap))
        return NULL;

    struct MinHeapNode* root = heap->array[0];
    struct MinHeapNode* lastNode = heap->array[heap->size - 1];
    heap->array[0] = lastNode;

    heap->pos[root->v] = heap->size - 1;
    heap->pos[lastNode->v] = 0;

    --heap->size;
    minHeapify(heap, 0);

    return root;
}

void decreaseKey(struct MinHeap* heap, int v, int key) {
    int i = heap->pos[v];
    heap->array[i]->key = key;

    while (i && heap->array[i]->key < heap->array[(i - 1) / 2]->key) {
        heap->pos[heap->array[i]->v] = (i - 1) / 2;
        heap->pos[heap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&heap->array[i], &heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int isInMinHeap(struct MinHeap *heap, int v) {
    return heap->pos[v] < heap->size;
}

int primMST(struct Graph* graph) {
    int V = graph->V;
    int *parent = malloc(V * sizeof(int));
    int *key = malloc(V * sizeof(int));
    int total_cost = 0;

    struct MinHeap* heap = createMinHeap(V);
    for (int v = 0; v < V; v++) {
        parent[v] = -1;
        key[v] = INT_MAX;
        heap->array[v] = newMinHeapNode(v, key[v]);
        heap->pos[v] = v;
    }

    key[0] = 0;
    decreaseKey(heap, 0, 0);
    heap->size = V;

    while (!isEmpty(heap)) {
        struct MinHeapNode* minNode = extractMin(heap);
        int u = minNode->v;
        free(minNode);

        struct AdjNode* current = graph->array[u].head;
        while (current) {
            int v = current->dst;
            if (isInMinHeap(heap, v) && current->weight < key[v]) {
                key[v] = current->weight;
                parent[v] = u;
                decreaseKey(heap, v, key[v]);
            }
            current = current->next;
        }
    }

    printf("Prim's MST:\n");
    for (int i = 1; i < V; i++) {
        if (parent[i] != -1) {
            printf("%s - %s : %d\n", cities[parent[i]], cities[i], key[i]);
            total_cost += key[i];
        }
    }
    printf("Total Cost: %d\n", total_cost);

    free(heap->array);
    free(heap->pos);
    free(heap);
    free(parent);
    free(key);
    return total_cost;
}

int main() {
    struct Graph graph = {0};
    int choice;

    maxCities = INITIAL_CITIES;
    cities = malloc(sizeof(char*) * maxCities);

    do {
printf(
    "\n========== Menu ==========\n"
    "1. Load cities\n"
    "2. Apply Prim's Algorithm\n"
    "3. Apply Kruskal's Algorithm\n"
    "4. Compare\n"
    "5. Exit\n"
    "============================\n"
    "Choice: "
);
       char input[10];
if (!fgets(input, sizeof(input), stdin)) break;
if (sscanf(input, "%d", &choice) != 1) continue;


        if (choice == 1) loadCities(&graph);
        else if (choice == 2) primMST(&graph);
        else if (choice == 3) kruskalMST();

        else if (choice == 4) {
            clock_t start1 = clock(); primMST(&graph); clock_t end1 = clock();
            clock_t start2 = clock(); kruskalMST(); clock_t end2 = clock();
            printf("Prim time: %.6f s, Kruskal time: %.6f s\n", (double)(end1 - start1) / CLOCKS_PER_SEC, (double)(end2 - start2) / CLOCKS_PER_SEC);
        }
    } while (choice != 5);

    for (int i = 0; i < cityCount; i++) free(cities[i]);
    free(cities);

    return 0;
}
