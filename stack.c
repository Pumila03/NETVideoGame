#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

#define INIT_SIZE 2

// Implementation of stacks (Pile en français) which we use to implement the history (so we use them for Undo and Redo)
/*##################################################################*/

struct Stack {
  move* content;
  unsigned int size;
  unsigned int index_avail;  // index of next available array entry
};

/*##################################################################*/

Stack asde_stack_create_empty() {
  Stack s = malloc(sizeof(struct Stack));
  if (s == NULL) {
    fprintf(stderr, "s is NULL in asde_stack_create_empty\n");
    exit(EXIT_FAILURE);
  }
  s->content = (move*)malloc(sizeof(move) * INIT_SIZE);
  if (s->content == NULL) {
    fprintf(stderr, "content is null in asde_stack_create_empty\n");
    exit(EXIT_FAILURE);
  }
  s->size = INIT_SIZE;
  s->index_avail = 0;
  return s;
}

/*##################################################################*/

bool asde_stack_isEmpty(Stack S) {
  if (S == NULL) {
    fprintf(stderr, "s is NULL in asde_stack_isEmpty\n");
    exit(EXIT_FAILURE);
  }
  if (S->index_avail == 0) {
    return true;
  }
  return false;
}

/*##################################################################*/

void asde_stack_free(Stack S) {
  if (S != NULL && S->content != NULL) {
    free(S->content);
  }
  free(S);
}
move asde_stack_top(Stack S) {
  if (S == NULL) {
    fprintf(stderr, "S is NULL in asde_stack_top\n");
    exit(EXIT_FAILURE);
  }
  return S->content[S->index_avail - 1];
}

/*##################################################################*/

Stack asde_stack_push(Stack S, move data) {
  if (S == NULL) {
    fprintf(stderr, "s is NULL in asde_stack_push\n");
    exit(EXIT_FAILURE);
  }
  if (S->index_avail >= S->size) {
    S->content = realloc(S->content, 2 * S->size * sizeof(move));
    if (S->content == NULL) {
      fprintf(stderr, "content is NULL in asde_stack_push\n");
      return NULL;
    }
    S->size = S->size * 2;
  }
  S->content[S->index_avail] = data;
  S->index_avail++;
  return S;
}

/*##################################################################*/

Stack asde_stack_pop(Stack S) {
  if (S == NULL || asde_stack_isEmpty(S)) {
    fprintf(stderr, "s==NULL or sde_stack_isEmpty(S) is True in asde_stack_pop ");
    exit(EXIT_FAILURE);
  }
  S->index_avail--;
  return S;
}

/*##################################################################*/