#include "game_ext.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "stack.h"

/*##################################################################*/

game game_new_ext(uint nb_rows, uint nb_cols, shape* shapes, direction* orientations, bool wrapping) {
  if (nb_rows == 0 || nb_cols == 0 || nb_rows > 10 || nb_cols > 10) {
    fprintf(stderr, "sizing error\n");
    exit(EXIT_FAILURE);
  }
  game g = malloc((sizeof(struct game_s)));  // Creation of the game
  if (g == NULL) {
    fprintf(stderr, "allocation error\n");
    exit(EXIT_FAILURE);
  }
  // Add parameters to the g game
  g->nb_cols = nb_cols;
  g->nb_rows = nb_rows;
  g->wrapping = wrapping;
  g->shape_tab = malloc(nb_cols * nb_rows * sizeof(shape));  // Allocation of the shapes array
  if (g->shape_tab == NULL) {
    fprintf(stderr, "allocation error\n");
    free(g);
    exit(EXIT_FAILURE);
  }
  g->orientation_tab = malloc(nb_cols * nb_rows * sizeof(direction));  // Allocation of the orientations array
  if (g->orientation_tab == NULL) {
    fprintf(stderr, "allocation error\n");
    free(g->shape_tab);
    free(g);
    exit(EXIT_FAILURE);
  }
  // Creation of the history and redo Stacks
  g->history = asde_stack_create_empty();
  g->redo = asde_stack_create_empty();
  if (shapes == NULL) {
    for (int i = 0; i < nb_cols * nb_rows; i++) {
      g->shape_tab[i] = EMPTY;
    }
  } else {
    for (int i = 0; i < nb_rows * nb_cols; i++) {  // initialisation of the shape_tab array
      g->shape_tab[i] = shapes[i];
    }
  }
  if (orientations == NULL) {
    for (int i = 0; i < nb_rows * nb_cols; i++) {
      g->orientation_tab[i] = NORTH;
    }
  } else {
    for (int i = 0; i < nb_rows * nb_cols; i++) {  // initialisation of the orientation_tab array
      g->orientation_tab[i] = orientations[i];
    }
  }
  return g;
}

/*##################################################################*/

game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping) {
  return game_new_ext(nb_rows, nb_cols, NULL, NULL, wrapping);
}

/*##################################################################*/

uint game_nb_rows(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Erreur jeu non valide");
    exit(EXIT_FAILURE);
  }
  return g->nb_rows;
}

/*##################################################################*/

uint game_nb_cols(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Erreur jeu non valide");
    exit(EXIT_FAILURE);
  }
  return g->nb_cols;
}

/*##################################################################*/

bool game_is_wrapping(cgame g) {
  if (g->wrapping == true) {
    return true;
  }
  return false;
}

/*##################################################################*/

void game_undo(game g) {
  if (asde_stack_isEmpty(g->history)) {  // Check if there is an historic of the previous plays
    return;
  }
  move vals = asde_stack_top(g->history);    // Recover the last move from the history stack
  g->redo = asde_stack_push(g->redo, vals);  // Push (move)vals in the redo stack
  if (g->redo == NULL) {
    game_delete(g);
    exit(EXIT_FAILURE);
  }
  g->history = asde_stack_pop(g->history);  // Remove (move)vals from history stack
  // Do undo on the (move) vals position (i,j)
  direction pidir = game_get_piece_orientation(g, vals.i, vals.j);
  game_set_piece_orientation(g, vals.i, vals.j, ((pidir - vals.move + NB_DIRS) % NB_DIRS));
  return;
}

/*##################################################################*/

void game_redo(game g) {
  if (asde_stack_isEmpty(g->redo)) {  // Check if there is an historic of cancelled move
    return;
  }
  move vals = asde_stack_top(g->redo);             // Recover the last undo move from the redo stack
  g->history = asde_stack_push(g->history, vals);  // Push the (move)vals in the history stack
  if (g->history == NULL) {
    game_delete(g);
    exit(EXIT_FAILURE);
  }
  g->redo = asde_stack_pop(g->redo);  // Remove the move from redo stack
  // Reply the move
  direction pidir = game_get_piece_orientation(g, vals.i, vals.j);
  game_set_piece_orientation(g, vals.i, vals.j, (pidir + vals.move) % 4);
  return;
}

/*##################################################################*/
