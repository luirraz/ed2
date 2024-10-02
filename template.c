#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0
#define INT_MAX 32000
#define START 1
typedef int bool;
typedef int TIPOPESO;

typedef struct adjacencia
{
    int vertice;
    TIPOPESO peso;
    struct adjacencia *prox;
} ADJACENCIA;

typedef struct vertice
{
    ADJACENCIA *cab;
} VERTICE;

typedef struct grafo
{
    int vertices;
    int arestas;
    VERTICE *adj;
} GRAFO;

GRAFO *criarGrafo(int v)
{
    GRAFO *g = (GRAFO *)malloc(sizeof(GRAFO));
    g->vertices = v;
    g->arestas = 0;
    g->adj = (VERTICE *)malloc(v * sizeof(VERTICE));
    for (int i = 0; i < v; i++)
        g->adj[i].cab = NULL;
    return g;
}

ADJACENCIA *criaAdj(int v, int peso)
{
    ADJACENCIA *temp = (ADJACENCIA *)malloc(sizeof(ADJACENCIA));
    temp->vertice = v;
    temp->peso = peso;
    temp->prox = NULL;
    return temp;
}

ADJACENCIA *inseriListAdj(ADJACENCIA *adj, int vi, int vf, TIPOPESO p)
{
    ADJACENCIA *ant = adj, *atual = adj;
    ADJACENCIA *novo = criaAdj(vf, p);

    while (atual != NULL && atual->vertice < vf)
    {
        ant = atual;
        atual = atual->prox;
    }

    if (ant == NULL)
    {
        novo->prox = atual;
        return novo;
    }
    else
    {
        novo->prox = atual;
        ant->prox = novo;
    }

    return adj;
}

bool criaAresta(GRAFO *gr, int vi, int vf, TIPOPESO p)
{
    if (!gr)
        return false;
    if (vf < 0 || vf >= gr->vertices || vi < 0 || vi >= gr->vertices)
        return false;

    gr->adj[vi].cab = inseriListAdj(gr->adj[vi].cab, vi, vf, p);
    gr->arestas++;
    return true;
}

void imprime(GRAFO *gr)
{
    printf("Vertices: %d. Arestas: %d\n", gr->vertices, gr->arestas);
    for (int i = START; i < gr->vertices; i++)
    {
        printf("v%d: ", i);
        ADJACENCIA *ad = gr->adj[i].cab;
        while (ad)
        {
            printf("v%d(%d) ", ad->vertice, ad->peso);
            ad = ad->prox;
        }
        printf("\n");
    }
}

// ----------- DIJKSTRA ---------------------------
int *dijkstra(GRAFO *g, int s, int *p)
{
    int *dist = (int *)malloc(g->vertices * sizeof(int));
    bool *visitado = (bool *)malloc(g->vertices * sizeof(bool));

    for (int i = 0; i < g->vertices; i++)
    {
        dist[i] = INT_MAX;
        visitado[i] = false;
        p[i] = -1;
    }

    dist[s] = 0;

    for (int i = 0; i < g->vertices - 1; i++)
    {
        int u = -1;
        for (int v = 0; v < g->vertices; v++)
            if (!visitado[v] && (u == -1 || dist[v] < dist[u]))
                u = v;

        if (dist[u] == INT_MAX)
            break;

        visitado[u] = true;

        ADJACENCIA *adj = g->adj[u].cab;
        while (adj != NULL)
        {
            int v = adj->vertice;
            int peso = adj->peso;

            if (dist[u] + peso < dist[v])
            {
                dist[v] = dist[u] + peso;
                p[v] = u;
            }
            adj = adj->prox;
        }
    }

    free(visitado);
    return dist;
}

// ----------- BELLMAN FORD ---------------------------
int *bellman_ford(GRAFO *g, int s, int *p)
{
    int *dist = (int *)malloc(g->vertices * sizeof(int));
    for (int i = 0; i < g->vertices; i++)
    {
        dist[i] = INT_MAX;
        p[i] = -1;
    }

    dist[s] = 0;

    for (int i = 1; i < g->vertices; i++)
    {
        for (int u = 0; u < g->vertices; u++)
        {
            ADJACENCIA *adj = g->adj[u].cab;
            while (adj != NULL)
            {
                int v = adj->vertice;
                int peso = adj->peso;
                if (dist[u] != INT_MAX && dist[u] + peso < dist[v])
                {
                    dist[v] = dist[u] + peso;
                    p[v] = u;
                }
                adj = adj->prox;
            }
        }
    }

    for (int u = 0; u < g->vertices; u++)
    {
        ADJACENCIA *adj = g->adj[u].cab;
        while (adj != NULL)
        {
            int v = adj->vertice;
            int peso = adj->peso;
            if (dist[u] != INT_MAX && dist[u] + peso < dist[v])
            {
                printf("O grafo contém ciclo negativo!\n");
                return NULL;
            }
            adj = adj->prox;
        }
    }

    return dist;
}

// Função para imprimir o caminho
void caminho(GRAFO *g, int from, int to, int *p)
{
    if (to == from)
    {
        printf("%d ", from);
    }
    else if (p[to] == -1)
    {
        printf("Não há caminho de %d para %d\n", from, to);
    }
    else
    {
        caminho(g, from, p[to], p);
        printf("%d ", to);
    }
}

int main()
{
    char nomearquivo[] = "USA-road-d.CAL.gr";

    GRAFO *gr;
    FILE *arq;
    char entrada[50], c;
    int qtdnos, arcs, n1, n2, peso, i, j;
    int *p;

    arq = fopen(nomearquivo, "r");
    if (arq == NULL)
    {
        puts("Deu ruim");
        return 0;
    }

    j = 0;
    int cont = 0;
    while (cont < 100 && fscanf(arq, "%c", &c) != EOF)
    {
        if (c == 'c')
        {
            fscanf(arq, "%[^\n]", entrada);
        }
        else if (c == 'p')
        {
            fscanf(arq, " sp %d %d", &qtdnos, &arcs);
            gr = criarGrafo(qtdnos + 1);
            puts("#vertices criados#...");
            p = (int *)malloc(gr->vertices * sizeof(int));
        }
        else if (c == 'a')
        {
            fscanf(arq, " %d %d %d", &n1, &n2, &peso);
            printf("a-%d (%d,%d)[%d]\n", j++, n1, n2, peso);
            criaAresta(gr, n1, n2, peso);
        }
    }

    puts("Arestas carregadas #...");
    imprime(gr);

    int find = 1;
    int no1 = 4, no2 = 2, no3 = 5;

    int *r = dijkstra(gr, find, p);
    // int *r = bellman_ford(gr, find, p);

    for (int i = START; i < gr->vertices; i++)
        printf("D(v%d -> v%d) = %d\n", find, i, r[i]);

    caminho(gr, find, no1, p);
    printf("\n");
    caminho(gr, find, no2, p);
    printf("\n");
    caminho(gr, find, no3, p);
    printf("\n");

    free(r);
    free(p);
    return 0;
}
