#ifndef _LIST_H
#define _LIST_H

#include <stdlib.h>

/* Macros para a struct Lista */

#define _INSERT_LIST(a, b) \
	do { \
		insert_list_end(&a->inicio, b); \
		a->tamanho += 1; \
	} while(0)

#define _REMOVE_LIST(a, b) \
	do { \
		if(!isempty_list(&a->inicio)) { \
			remove_list(&a->inicio, b); \
			a->tamanho -= 1; } \
	} while(0)

#define _GET_ELEM_LIST(a, b) \
	get_elem_list(a->inicio, b)

#define _FREE_LIST(a) \
	do { \
		free_list(a->inicio); \
		free(a); \
	} while(0)

#define _LIST_INIT(a) \
	do { \
		(*a) = malloc(sizeof(Lista)); \
		list_init(&(*a)->inicio); \
		(*a)->tamanho = 0; \
	} while(0)

#define _IS_INSIDE(a, b, c)	\
	is_inside(a, b->inicio, c)

#define _GET_SIZE(a) \
	a->tamanho

/* Estruturas */

struct nolist {
	void* val;
	struct nolist* next;
};

typedef struct nolist _node;

struct lista {
	int tamanho;
	_node* inicio;
};

/* Funções */

typedef struct lista Lista;

void insert_list_end(_node**,
						void*);

void remove_list(_node**,
					unsigned int);

void* get_elem_list(_node*,
					unsigned int);

int isempty_list(_node**);

void free_list(_node*);

void list_init(_node**);

int is_inside(void*, _node*, int (*compare)(void*, void*));

#endif
