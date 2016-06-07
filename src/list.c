#include "header/list.h"
#include <stdlib.h>

void insert_list_end(_node** n, void* v) {
	_node* novo = malloc(sizeof(_node));
	novo->val = v;
	novo->next = NULL;
	while((*n) != NULL) n = &(*n)->next;
	(*n) = novo;
}

void remove_list(_node** n, unsigned int k) {
	if(k == 0) {
		if((*n) == NULL) return; // não existiu o elemento
		_node* trash = (*n);
		(*n) = (*n)->next;
		free(trash);
		return;
	}
	while(k-- > 1 && (*n) != NULL) (*n) = (*n)->next;
	if((*n)->next == NULL) return; // não existiu o elemento
	_node* trash = (*n)->next;
	(*n)->next = (*n)->next->next;
	free(trash);
}

void* get_elem_list(_node* n, unsigned int k) {
	while(k-- > 0 && n != NULL) n = n->next;
	if(n == NULL) return NULL;
	return n->val;
}

int isempty_list(_node** n) {
	if((*n) == NULL)
		return 1;
	return 0;
}

void free_list(_node* n) {
	if(n == NULL) return;
	free_list(n->next);
	free(n);
}

void list_init(_node** n) { (*n) = NULL; }

int is_inside(void* elem, _node* n, int (*compare)(void*, void*)) {
	while(n != NULL) {
		if(compare(elem, n->val))
			return 1;
		n = n->next;
	}
	return 0;
}