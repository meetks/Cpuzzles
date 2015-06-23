#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define UNDIRECTED 0
#define DIRECTED   1
#define MAX_NODES  100

typedef struct node_s {
    struct node_s *next;
    int y;
    int weight;
} node_t;

typedef struct graph_s {
    struct node_t *edges[MAX_NODES];
    int V;                      // Number of vertices 
    int E;                      // Number of Edges 
    int directed;               // Directed or not
} graph_t;

node_t *
alloc_node()
{
    node_t *temp;

    temp = malloc(sizeof (node_t));
    if (temp == NULL) {
        printf("\n Wow what!!!");
        assert(0);
    }
    memset(temp, 0, sizeof (*temp));
}

void
insert_edge(int x, int y, graph_t * g)
{
    node_t *temp;

    temp = alloc_node();
    if (temp == NULL) {
        printf("\n %s: NULL Ptr", "insert_edge");
        return;
    }
    temp->y = y;
    if (g->edges[x] == NULL) {
        g->edges[x] = temp;
    } else {
        temp->next = g->edges[x];
        g->edges[x] = temp;
    }
}

void
init_graph(graph_t * g)
{
    if (g == NULL) {
        printf("\n %s: NULL Ptr", "init_graph");
        assert(0);
    }
    memset(g->edges, 0, sizeof (g->edges));
    g->V = 0;
    g->E = 0;
    g->directed = UNDIRECTED;
}

void
create_graph(int V, int E, graph_t * g)
{
    int i;
    int x,
      y;

    i = 0;
    if (g == NULL) {
        printf("\n %s: NULL Ptr", "create_graph");
        return;
    }
    g->V = V;
    g->E = E;

    while (i < E) {
        printf("Enter the egde :");
        scanf("%d %d", &x, &y);
        if (x > MAX_NODES || y > MAX_NODES) {
            printf("\n MAX NODE possile is %d", MAX_NODES);
            printf("\n reenter the edge");
            continue;
        }
        insert_edge(x, y, g);
        if (g->directed == UNDIRECTED) {
            insert_edge(y, x, g);
        }
        i++;
    }
}

void
adjacent(int x, graph_t *g)
{
    int i;

}

void rundfs(int start, graph_t *g, int *visited)
{
   int u;
   int i;

   while (i < g->V) {
       u = adjacent(start, g);
       if (visited[u] == -1) {
           visited[u] = 1;
           i++;
           rundfs(u, g, visited);
       }
   }
}

void dfs(graph_t *g, int x)
{
    char *visited;

    visited = malloc(g->V);
    if (visited == NULL) {
        printf("\n NULL pointer");
        return;
    }
    memset(visited, -1, sizeof(*visited));
    visited[x] = 1;
    rundfs(x, g, visited);
}

void
print_graph(graph_t * g)
{
    int i;
    node_t *temp;

    for (i = 0; i < MAX_NODES; i++) {
        if (g->edges[i] != NULL) {
            printf("\n %d", i);
            temp = g->edges[i];
            while (temp) {
                printf("%d ", temp->y);
                temp = temp->next;
            }
        }
    }
}

main()
{
    graph_t g;

    init_graph(&g);
    create_graph(5, 5, &g);
    print_graph(&g);

}
