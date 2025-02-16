/*
 * This file is a copy of the orginal.
 * Modification will not be taken into account.
 */
#ifndef ASDE_STACK_H
#define ASDE_STACK_H

#include <stdbool.h>

typedef struct {
  int i;
  int j;
  int move;
} move;

typedef int data_type;

typedef struct Stack *Stack;
// creates an empty Stack -> creerPile(P:pile d'objet):pile d'objet
extern Stack asde_stack_create_empty();
// tests whether a Stack is empty -> pileVide(P:pile d'objet): booléen
extern bool asde_stack_isEmpty(Stack S);
// frees the Stack  -> detruirePile(P:pile d'objet):vide
extern void asde_stack_free(Stack S);
// returns the value on top of the Stack -> fonction valeurPile(P:pile d'objet): objet
extern move asde_stack_top(Stack S);
// adds an element on top of the Stack  -> empiler(P:pile d'objet, x objet):pile d'objet
extern Stack asde_stack_push(Stack S, move data);
// erases the element on top of the Stack  -> depiler(P:pile d'objet):pile d'objet
extern Stack asde_stack_pop(Stack S);

extern void asde_stack_copy(Stack source, Stack destination);

#endif /* ASDE_STACK_H */