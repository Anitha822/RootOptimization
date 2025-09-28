#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MAX_POINTS 20

// Structure for Min-Heap node
typedef struct {
    char address[50];
    int time;
} HeapNode;

typedef struct {
    HeapNode heap[MAX_POINTS];
    int size;
} MinHeap;

// Delivery System structure
typedef struct {
    char delivery_points[MAX_POINTS][50];
    int distances[MAX_POINTS][MAX_POINTS];
    int count;
} DeliverySystem;

// Heap functions
void swap(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(MinHeap* h, int idx) {
    while (idx > 0 && h->heap[(idx - 1) / 2].time > h->heap[idx].time) {
        swap(&h->heap[(idx - 1) / 2], &h->heap[idx]);
        idx = (idx - 1) / 2;
    }
}

void heapify_down(MinHeap* h, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < h->size && h->heap[left].time < h->heap[smallest].time)
        smallest = left;
    if (right < h->size && h->heap[right].time < h->heap[smallest].time)
        smallest = right;

    if (smallest != idx) {
        swap(&h->heap[idx], &h->heap[smallest]);
        heapify_down(h, smallest);
    }
}

void push(MinHeap* h, char* address, int time) {
    strcpy(h->heap[h->size].address, address);
    h->heap[h->size].time = time;
    heapify_up(h, h->size);
    h->size++;
}

HeapNode pop(MinHeap* h) {
    HeapNode root = h->heap[0];
    h->heap[0] = h->heap[h->size - 1];
    h->size--;
    heapify_down(h, 0);
    return root;
}

// Delivery system functions
void init_system(DeliverySystem* system) {
    system->count = 0;
    for (int i = 0; i < MAX_POINTS; i++) {
        for (int j = 0; j < MAX_POINTS; j++) {
            system->distances[i][j] = 0;
        }
    }
}

void add_delivery_point(DeliverySystem* system, char* address) {
    strcpy(system->delivery_points[system->count], address);
    system->count++;
}

int get_index(DeliverySystem* system, char* address) {
    for (int i = 0; i < system->count; i++) {
        if (strcmp(system->delivery_points[i], address) == 0)
            return i;
    }
    return -1;
}

void set_distance(DeliverySystem* system, char* addr1, char* addr2, int dist) {
    int i = get_index(system, addr1);
    int j = get_index(system, addr2);
    if (i != -1 && j != -1) {
        system->distances[i][j] = dist;
        system->distances[j][i] = dist;
    }
}

void calculate_route(DeliverySystem* system, char* start_address) {
    int n = system->count;
    int visited[MAX_POINTS] = {0};
    int route[MAX_POINTS];
    int total_distance = 0;

    int start_idx = get_index(system, start_address);
    if (start_idx == -1) {
        printf("Start address not found!\n");
        return;
    }

    int current = start_idx;
    visited[current] = 1;
    route[0] = current;
    int route_size = 1;

    for (int step = 0; step < n - 1; step++) {
        int next_point = -1, min_dist = INT_MAX;
        for (int j = 0; j < n; j++) {
            if (!visited[j] && system->distances[current][j] > 0 && system->distances[current][j] < min_dist) {
                min_dist = system->distances[current][j];
                next_point = j;
            }
        }
        if (next_point != -1) {
            visited[next_point] = 1;
            total_distance += min_dist;
            current = next_point;
            route[route_size++] = current;
        }
    }

    printf("\nInitial Route: ");
    for (int i = 0; i < route_size; i++) {
        printf("%s ", system->delivery_points[route[i]]);
    }
    printf("\nTotal Distance: %d\n", total_distance);

    // Simulate traffic using Min-Heap
    MinHeap heap;
    heap.size = 0;
    srand(time(0));
    for (int i = 0; i < route_size; i++) {
        int traffic_time = (rand() % 20) + 1;
        push(&heap, system->delivery_points[route[i]], traffic_time);
    }

    printf("\nOptimized Route with Traffic Conditions:\n");
    while (heap.size > 0) {
        HeapNode node = pop(&heap);
        printf("%s (ETA: %d mins)\n", node.address, node.time);
    }
}

int main() {
    DeliverySystem system;
    init_system(&system);

    int num_points;
    printf("Enter number of delivery points (including Warehouse): ");
    scanf("%d", &num_points);
    getchar();

    for (int i = 0; i < num_points; i++) {
        char address[50];
        printf("Enter name for point %d: ", i + 1);
        fgets(address, sizeof(address), stdin);
        address[strcspn(address, "\n")] = 0;
        add_delivery_point(&system, address);
    }

    printf("\nEnter pairwise distances between points:\n");
    for (int i = 0; i < num_points; i++) {
        for (int j = i + 1; j < num_points; j++) {
            int dist;
            printf("Distance between %s and %s: ", system.delivery_points[i], system.delivery_points[j]);
            scanf("%d", &dist);
            set_distance(&system, system.delivery_points[i], system.delivery_points[j], dist);
        }
    }

    char start[50];
    printf("\nEnter starting point: ");
    scanf("%s", start);

    calculate_route(&system, start);

    return 0;
}