#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1
#define INFINITE 99999999
#define NIL -1
#define LCHILD(x) 2 * x + 1
#define RCHILD(x) 2 * x + 2
#define PARENT(x) (x - 1) / 2

/* Variaveis Globais */
int vertice_minimo=1, valor_minimo=INFINITE;

typedef struct edge {
	int valor, peso, peso_rep;
	struct edge *next;
} *Edge;

typedef struct vertix {
	int index, d, d_chapeu, h;
	int canBeRelaxed;
	struct vertix *parent;
	Edge edges;
} *nodeLink;

typedef struct graph {
	int nVertix, nEdge;
	nodeLink *nodes;
} *Graph;

/* ######################### HEAP STRUCTURE ################################*/

typedef struct minHeap {
    int size ;
    int *elem ;
} minHeap ;


/* #########################################################################*/

/* ######################### GRAPH FUNCTIONS ###############################*/
Edge newEdge(int valor, int peso,int peso_rep, Edge next){
	Edge e = malloc(sizeof(struct edge));
	e->valor = valor;
	e->next = next;
	e->peso = peso;
    e->peso_rep = peso_rep;
	return e;
}

nodeLink newNode(int index){
	nodeLink x = malloc(sizeof(struct vertix));
	x->index = index;
	x->d = x->d_chapeu = x->h = 0;
    x->canBeRelaxed = 1;
	x->parent = NULL; 
	x->edges = NULL;
	return x;
}

Graph GRAPHinit(int nVertix, int nEdge){
	int i;
	Graph g = malloc(sizeof(struct graph));
	g->nVertix = nVertix;
	g->nEdge = nEdge;
	g->nodes = malloc((nVertix + 1) * sizeof(nodeLink));
	for(i = 0; i <= nVertix; i++)
		g->nodes[i] = newNode(i);
	return g;
}

void createConnection(Graph g, int vertix1, int vertix2, int peso, int peso_rep){
	nodeLink node = g->nodes[vertix1];
	node->edges = newEdge(vertix2, peso, peso_rep, node->edges);
}

/* ######################### HEAP FUNCTIONS ########################## */
/*
    Function to initialize the min heap with size = 0
*/
minHeap *initMinHeap() {
    minHeap *hp = malloc(sizeof(minHeap));
    hp->size = 0 ;
    return hp ;
}

void swap(int *n1, int *n2) {
    int temp = *n1 ;
    *n1 = *n2 ;
    *n2 = temp ;
}

void heapify(Graph g,minHeap *hp, int i) {
    int l = LCHILD(i);
    int r = RCHILD(i);
    int smallest = i;

    if (l < hp->size && g->nodes[hp->elem[l]]->d < g->nodes[hp->elem[i]]->d)
        smallest = l;

    if (r < hp->size && g->nodes[hp->elem[r]]->d < g->nodes[hp->elem[smallest]]->d)
        smallest = r;

    if (i != smallest) {
        swap(&hp->elem[i], &hp->elem[smallest]);
        heapify(g, hp, smallest);
    }
}

void buildMinHeap(Graph g, minHeap *hp, int size) {
    int i;

    for(i = 0; i < size; i++) {
        if(hp->size) {
            hp->elem = realloc(hp->elem, (hp->size + 1) * sizeof(int)) ;
        } else {
            hp->elem = malloc(sizeof(int)) ;
        }
        hp->elem[(hp->size)++] = i+1 ;

    }
    
}



/*
    Function to delete a node from the min heap
    It shall remove the root node, and place the last node in its place
    and then call heapify function to make sure that the heap property
    is never violated
*/
void deleteNode(Graph g,minHeap *hp) {
    if(hp->size) {
        swap(&hp->elem[0],&hp->elem[--(hp->size)]);
        hp->elem = realloc(hp->elem, hp->size * sizeof(int)) ;
        heapify(g, hp, 0);
    } else {
        free(hp->elem) ;
    }
}


/*
    Function to clear the memory allocated for the min heap
*/
void deleteMinHeap(minHeap *hp) {
    free(hp->elem) ;
    free(hp);
}


/* ######################### GRAPH ALGORITHMS ####################### */

void inicializarSingleSource(Graph g, nodeLink s){
	int i;
	for(i = 1; i <= g->nVertix; i++){
		g->nodes[i]->d = INFINITE*2;
		g->nodes[i]->parent = NULL;
	}
	s->d = 0;
}

void bellman_ford(Graph g, nodeLink s){
	int i, j, hasChange = 0;
	Edge aux_edge;
	inicializarSingleSource(g, s);
    for(j = 1; j <= g->nVertix-1; j++){
        hasChange = 0;
        for(i = 1; i <= g->nVertix; i++){
            if(g->nodes[i]->canBeRelaxed == 1){
        		for(aux_edge = g->nodes[i]->edges ; aux_edge != NULL ; aux_edge = aux_edge->next){
        			if (g->nodes[aux_edge->valor]->d > g->nodes[i]->d + aux_edge->peso){
                        hasChange = 1;
        				g->nodes[aux_edge->valor]->d = g->nodes[i]->d + aux_edge->peso;
        				g->nodes[aux_edge->valor]->parent = g->nodes[i];
        			}
        		}
                if(hasChange == 0)
                    g->nodes[i]->canBeRelaxed = 0;
            }
        }
        if(hasChange == 0)
            return;
    }
}


void Dijkstra(Graph g, nodeLink s){
    int nVertix, i, hasChange;
    Edge aux_edge;
    minHeap *Q;
    nodeLink u, v;
    nVertix = g->nVertix;

    Q = initMinHeap();
    inicializarSingleSource(g,s);
    buildMinHeap(g, Q, nVertix);
    swap(&Q->elem[0], &Q->elem[s->index-1]);
    while(Q->size >= 1) {
        hasChange = 0;
        u = g->nodes[Q->elem[0]];
        deleteNode(g, Q);
        if(u->d_chapeu < INFINITE)
            u->d_chapeu += u->d + u->h - s->h;
        if(u->d < INFINITE){
            if(u->d_chapeu < g->nodes[vertice_minimo]->d_chapeu)
                vertice_minimo = u->index;
            if(u->d_chapeu == g->nodes[vertice_minimo]->d_chapeu && u->index < vertice_minimo)
                vertice_minimo = u->index;
            for(aux_edge = u->edges ; aux_edge != NULL && (aux_edge->valor != -1); aux_edge = aux_edge->next){

                v = g->nodes[aux_edge->valor];
                if(v->d > u->d + aux_edge->peso_rep){
                    hasChange = 1; 
                    v->d = u->d + aux_edge->peso_rep;
                    v->parent = u;
                }
            }
            if(hasChange == 1)
                for(i = (Q->size - 1) / 2; i >= 0; i--) 
                    heapify(g, Q, i);
        }

    }
    deleteMinHeap(Q);
}
void Johnson(Graph g, int *listaFiliais, int nFiliais){
	int i;
	Edge aux_edge;

	/* Criar novo G com fonte s */
	for(i = 1; i <= g->nVertix; i++)
		createConnection(g, 0, i, 0, 0);
	
	bellman_ford(g, g->nodes[0]);

	for(i = 1; i <= g->nVertix; i++)
		g->nodes[i]->h = g->nodes[i]->d; 
    
    /*Faz a repesagem*/
	for(i = 1; i <= g->nVertix; i++){
		for(aux_edge = g->nodes[i]->edges; aux_edge != NULL; aux_edge = aux_edge->next)
			aux_edge->peso_rep = aux_edge->peso + g->nodes[i]->h - g->nodes[aux_edge->valor]->h;
	}
	
    
	for(i = 0; i < nFiliais; i++){
		Dijkstra(g, g->nodes[listaFiliais[i]]);
    }
}

void grafoTransposto(Graph g){
    int i;
    Edge edge, edge_anterior;
    
    for(i = 1; i <= g->nVertix ; i++)
        createConnection(g, i, -1, 0, 0);

    for(i = 1; i <= g->nVertix; i++){
        for(edge = g->nodes[i]->edges; edge->valor != -1 ; edge = edge->next);
        edge_anterior = edge;
        edge = edge->next;

        while(edge != NULL){
            createConnection(g, edge->valor, i, edge->peso, edge->peso_rep);
            edge_anterior = edge;
            edge = edge->next;
            free(edge_anterior);
        }

    }
    
    
}


int main(){
	int i, nVertix, nEdge, nFiliais;
    int input1, input2, input3;
    int *listaFiliais;

    Graph g;

 	scanf("%d %d %d", &nVertix, &nFiliais, &nEdge);
 	g = GRAPHinit(nVertix, nEdge);

 	/* Inicializar lista de filiais */
 	listaFiliais = malloc(sizeof(int) * nFiliais);
 	for(i = 0; i < nFiliais; i++){
 		scanf("%d", &listaFiliais[i]);
 	}

    for(i = 1; i <= nEdge; i++) {
        scanf("%d %d %d", &input1, &input2, &input3);
        createConnection(g, input1, input2, input3,0); /*Saves a connection between person number input1 and person number input2*/
    }

    
    Johnson(g, listaFiliais, nFiliais);
    valor_minimo = g->nodes[vertice_minimo]->d_chapeu;
    /* primeiro resultado */
    if(valor_minimo < INFINITE)
        printf("%d %d\n",vertice_minimo,valor_minimo);
    else{
        printf("N\n");
        return 0;
    }

    grafoTransposto(g);

    /* Aplica o Disjkstra do ponto de encontro para as filiais e calcula o valor da filial */
    Dijkstra(g, g->nodes[vertice_minimo]);
    for(i = 0; i < nFiliais;i++ )
        printf("%d ", g->nodes[listaFiliais[i]]->d - g->nodes[listaFiliais[i]]->h + g->nodes[vertice_minimo]->h);
    
    printf("\n");
    
	return 0;
}