#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 100
#define MAX_VERTICES_COUNT 30

void printCycles(int cycleCount, int cycles[][MAX_SIZE], int cycleLengths[], int maxCycleLength, int perimeters[]);

typedef struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode *next;
} AdjListNode;

typedef struct AdjList
{
    struct AdjListNode *head;
} AdjList;

typedef struct Graph
{
    int vertexCount;
    struct AdjList *array;
} Graph;

AdjListNode *newAdjListNode(int dest, int weight)
{
    AdjListNode *newNode = (AdjListNode *)malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

Graph *createGraph(int V)
{
    Graph *graph = (Graph *)malloc(sizeof(struct Graph));
    graph->vertexCount = V;

    graph->array = (AdjList *)malloc(V * sizeof(struct AdjList));
    if (graph->array == NULL) {
        perror("Memory allocation failed for adjacency list array");
        free(graph);
        return NULL;
    }

    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;

    return graph;
}

void addEdge(Graph *graph, int src, int dest, int weight)
{
    AdjListNode *newNode = newAdjListNode(dest, weight);
    if (newNode == NULL) {
        perror("Memory allocation failed for new node");
        return;
    }
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    newNode = newAdjListNode(src, weight);
    if (newNode == NULL) {
        perror("Memory allocation failed for new node");
        return;
    }
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

int isNewCycle(int *cycle, int cycleSize, int cycles[][MAX_SIZE], int cycleLengths[], int cycleCount)
{
    int i, j, k;
    for (i = 0; i < cycleCount; i++)
    {
        if (cycleLengths[i] == cycleSize)
        {
            int match = 1;
            for (j = 0; j < cycleSize; j++)
            {
                int found = 0;
                for (k = 0; k < cycleSize; k++)
                {
                    if (cycle[j] == cycles[i][k])
                    {
                        found = 1;
                        break;
                    }
                }
                if (found == 0)
                {
                    match = 0;
                    break;
                }
            }
            if (match == 1)
                return 0;
        }
    }
    return 1;
}

void DFS(Graph *graph, int u, int *visited, int *stack, int *stackSize, int cycles[][MAX_SIZE], int *cycleLengths, int *cycleCount, int *perimeters)
{
    visited[u] = 1;
    stack[(*stackSize)++] = u;

    AdjListNode *node = graph->array[u].head;
    int cycleSize, perimeter;
    int i;

    while (node != NULL)
    {
        int v = node->dest;

        if (visited[v] != 1)
        {
            DFS(graph, v, visited, stack, stackSize, cycles, cycleLengths, cycleCount, perimeters);
        }
        else if (v == stack[0])
        {
            cycleSize = *stackSize;
            if (isNewCycle(stack, cycleSize, cycles, cycleLengths, *cycleCount) == 1)
            {
                memcpy(cycles[*cycleCount], stack, cycleSize * sizeof(int));
                cycleLengths[*cycleCount] = cycleSize;
                perimeter = 0;

                for (i = 0; i < cycleSize; i++)
                {
                    AdjListNode *temp = graph->array[stack[i]].head;
                    while (temp != NULL)
                    {
                        if (temp->dest == stack[(i + 1) % cycleSize])
                        {
                            perimeter = perimeter + temp->weight;
                            break;
                        }
                        temp = temp->next;
                    }
                }
                perimeters[*cycleCount] = perimeter;
                (*cycleCount)++;
            }
        }

        node = node->next;
    }

    visited[u] = 0;
    (*stackSize)--;
}

void findCycles(Graph *graph)
{
    int i;
    int stackSize = 0, cycleCount = 0, numberOfRealCycles = 0;
    int visited[MAX_VERTICES_COUNT] = {0};
    int stack[MAX_VERTICES_COUNT];
    int cycles[MAX_SIZE][MAX_SIZE];
    int cycleLengths[MAX_SIZE];
    int perimeters[MAX_SIZE];
    int shape_frequency[MAX_VERTICES_COUNT] = {0};

    for (i = 0; i < graph->vertexCount; i++)
    {
        if (visited[i] == 0)
        {
            DFS(graph, i, visited, stack, &stackSize, cycles, cycleLengths, &cycleCount, perimeters);
        }
    }

    for (i = 0; i < cycleCount; i++)
    {
        if (cycleLengths[i] > 2)
        {
            numberOfRealCycles++;
            if (cycleLengths[i] < MAX_VERTICES_COUNT) {
                 shape_frequency[cycleLengths[i]]++;
            }

        }
    }

    printf("Number of shapes: %d\n", numberOfRealCycles);
    for (i = 0; i < MAX_VERTICES_COUNT; i++)
    {
        if (shape_frequency[i] > 0)
        {
            printf("Number of %d-gons: %d\n", i, shape_frequency[i]);
        }
    }
    printf("\n");

    int maxCycleLength = 0;
    for (i = 0; i < cycleCount; i++)
    {
        if (cycleLengths[i] > maxCycleLength)
        {
            maxCycleLength = cycleLengths[i];
        }
    }

    printCycles(cycleCount, cycles, cycleLengths, maxCycleLength, perimeters);
}

void printCycles(int cycleCount, int cycles[][MAX_SIZE], int cycleLengths[], int maxCycleLength, int perimeters[])
{
    int i, j, k;
    for (i = 3; i <= maxCycleLength; i++)
    {
        int n = 1;
        for (j = 0; j < cycleCount; j++)
        {
            if (cycleLengths[j] == i)
            {
                printf("%d. %d-gon: ", n, i);
                for (k = 0; k < i; k++)
                {
                    printf("%c ", cycles[j][k] + 'A');
                }
                printf("%c", cycles[j][0] + 'A');
                printf(" Length: %d\n", perimeters[j]);
                n++;
            }
        }
        if (n > 1) printf("\n");
    }
}

void readEdgesFromFile(Graph *graph, char *filename)
{
    char to, from;
    int weight;
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    while (fscanf(file, " %c %c %d", &to, &from, &weight) == 3)
    {
        addEdge(graph, to - 'A', from - 'A', weight);
    }

    fclose(file);
}

int main()
{
    char *filename = "Sample3.txt";

    Graph *graph = createGraph(MAX_VERTICES_COUNT);
    if (graph == NULL) {
        return 1;
    }

    printf("Cycles in: %s\n", filename);

    readEdgesFromFile(graph, filename);

    findCycles(graph);

    int i;
    for (i = 0; i < graph->vertexCount; i++) {
        AdjListNode *currentNode = graph->array[i].head;
        while (currentNode != NULL) {
            AdjListNode *temp = currentNode;
            currentNode = currentNode->next;
            free(temp);
        }
    }
    free(graph->array);
    free(graph);


    return 0;
}
