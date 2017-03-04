#include <stdio.h>
#include <stdlib.h>
#include "header/list.h"

int main(int argc, char* argv[]) {
  if(argc < 2) {
    printf("ERROR: usage ./test_int <number_of_items>");
    return 1;
  }

  int size = strtol(argv[1], NULL, 10);
  int i;
  Lista* int_list;
  _LIST_INIT(&int_list);

  printf("Type %d numbers: ", size); //scanf("%d ", &i);
  while(_GET_SIZE(int_list) < size) {
    scanf("%d", &i);
    _INSERT_LIST(int_list, (void*) i);
  }

  printf("The list has %d elements: ", _GET_SIZE(int_list));
  for(i = 0; i < _GET_SIZE(int_list)-1; i++) {
    printf("%d ", _GET_ELEM_LIST(int_list, i));
  }
  printf("%d\n", _GET_ELEM_LIST(int_list, i));

  return 0;
}
