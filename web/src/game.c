#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "stack.h"

/*##################################################################*/

game game_new_empty(void) { return game_new(NULL, NULL); }

/*##################################################################*/

game game_new(shape* shapes, direction* orientations) {
  game g = malloc((sizeof(struct game_s)));
  g->nb_cols = 5;
  g->nb_rows = 5;
  g->shape_tab = malloc(DEFAULT_SIZE * DEFAULT_SIZE * sizeof(shape));
  g->orientation_tab = malloc(DEFAULT_SIZE * DEFAULT_SIZE * sizeof(direction));
  g->history = asde_stack_create_empty();
  g->redo = asde_stack_create_empty();
  if (g == NULL) {
    fprintf(stderr, "Problème d'espace mémoire");
    exit(EXIT_FAILURE);
  }
  if (shapes == NULL) {
    for (int i = 0; i < DEFAULT_SIZE * DEFAULT_SIZE; i++) {
      g->shape_tab[i] = EMPTY;
    }
  } else {  // initialisation of the shape_tab array
    for (int i = 0; i < DEFAULT_SIZE * DEFAULT_SIZE; i++) {
      g->shape_tab[i] = shapes[i];
    }
  }
  if (orientations == NULL) {
    for (int i = 0; i < DEFAULT_SIZE * DEFAULT_SIZE; i++) {
      g->orientation_tab[i] = NORTH;
    }
  } else {  // initialisation of the orientation_tab array
    for (int i = 0; i < DEFAULT_SIZE * DEFAULT_SIZE; i++) {
      g->orientation_tab[i] = orientations[i];
    }
  }
  return g;
}

/*##################################################################*/

game game_copy(cgame g) {
  direction orientation_tab[game_nb_cols(g) * game_nb_rows(g)];
  shape shape_tab[game_nb_cols(g) * game_nb_rows(g)];
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      orientation_tab[GAME2TAB(i, j, g)] = game_get_piece_orientation(g, i, j);  // copy of the g's orientation_tab
                                                                                 // array
      shape_tab[GAME2TAB(i, j, g)] = game_get_piece_shape(g, i, j);              // copy of the g's shape_tab array
    }
  }
  game copy_g = game_new_ext(game_nb_rows(g), game_nb_cols(g), shape_tab, orientation_tab, g->wrapping);
  return copy_g;
}

/*##################################################################*/

bool game_equal(cgame g1, cgame g2, bool ignore_orientation) {
  if (game_nb_cols(g1) != game_nb_cols(g2) || game_nb_rows(g1) != game_nb_rows(g2)) {
    return false;
  }
  if ((game_nb_cols(g1) != game_nb_cols(g2)) || game_nb_rows(g1) != game_nb_rows(g2)) {
    return false;
  }
  for (int i = 0; i < game_nb_rows(g1); i++) {
    for (int j = 0; j < game_nb_cols(g2); j++) {
      if (ignore_orientation == false) {
        if (game_get_piece_orientation(g1, i, j) != game_get_piece_orientation(g2, i, j)) {
          if (game_get_piece_shape(g1, i, j) == SEGMENT && game_get_piece_shape(g2, i, j) == SEGMENT) {
            uint dir = game_get_piece_orientation(g1, i, j);
            uint dir2 = game_get_piece_orientation(g2, i, j);
            if ((dir + dir2) % 2 != 0) {
              return false;
            }
          } else if (game_get_piece_shape(g1, i, j) != CROSS && game_get_piece_shape(g2, i, j) != CROSS) {
            return false;
          }
          // Verification  of the orientation
        }
      }
      if (game_get_piece_shape(g1, i, j) != game_get_piece_shape(g2, i, j)) {  // Verification of the shape
        return false;
      }
    }
  }
  if (g1->wrapping != g2->wrapping) {  // Verification of the boolean if the game is wrapping
    return false;
  }
  return true;
}

/*##################################################################*/

void game_delete(game g) {
  free(g->orientation_tab);
  free(g->shape_tab);
  asde_stack_free(g->history);
  asde_stack_free(g->redo);
  free(g);
}

/*##################################################################*/

void game_set_piece_shape(game g, uint i, uint j, shape s) {
  if (g == NULL) {
    fprintf(stderr, "Erreur jeu non valide");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "Erreur : Indices hors des limites de la grille 1");
    game_delete((game)g);
    exit(EXIT_FAILURE);
  }
  g->shape_tab[GAME2TAB(i, j, g)] = s;
}

/*##################################################################*/

void game_set_piece_orientation(game g, uint i, uint j, direction o) {
  if (g == NULL) {
    fprintf(stderr, "Erreur jeu non valide");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "Erreur : Indices hors des limites de la grille 2");
    game_delete((game)g);
    exit(EXIT_FAILURE);
  }
  g->orientation_tab[GAME2TAB(i, j, g)] = o;
}

/*##################################################################*/

shape game_get_piece_shape(cgame g, uint i, uint j) {
  if (g == NULL) {
    fprintf(stderr, "Erreur jeu non valide");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "Erreur : Indices hors des limites de la grille 3");
    game_delete((game)g);
    exit(EXIT_FAILURE);
  }
  return g->shape_tab[GAME2TAB(i, j, g)];
}

/*##################################################################*/

direction game_get_piece_orientation(cgame g, uint i, uint j) {
  if (g == NULL) {
    fprintf(stderr, "Erreur jeu non valide");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "Erreur : Indices hors des limites de la grille 4");
    game_delete((game)g);
    exit(EXIT_FAILURE);
  }
  return g->orientation_tab[GAME2TAB(i, j, g)];
}

/*##################################################################*/

void game_play_move(game g, uint i, uint j, int nb_quater_turns) {
  if (g == NULL) {
    fprintf(stderr, "Erreur jeu non valide");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "Erreur : Indices hors des limites de la grille 5");
    game_delete((game)g);
    exit(EXIT_FAILURE);
  }
  move vals;  // Structure of the current move
  vals.i = i;
  vals.j = j;
  vals.move = nb_quater_turns;
  g->history = asde_stack_push(g->history, vals);  // Push the last move in the history Stack (historic of moves)
  while (!asde_stack_isEmpty(g->redo)) {
    asde_stack_pop(g->redo);
  }
  int direction = game_get_piece_orientation(g, i, j);
  int new_dir = (direction + nb_quater_turns + NB_DIRS) % NB_DIRS;
  game_set_piece_orientation(g, i, j, new_dir);
}

/*##################################################################*/

bool game_won(cgame g) {
  if (game_is_well_paired(g) && game_is_connected(g)) {
    return true;
  }
  return false;
}

/*##################################################################*/

void game_reset_orientation(game g) {
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      game_set_piece_orientation(g, i, j, NORTH);  // Every piece position at North
    }
  }
}

/*##################################################################*/

void game_shuffle_orientation(game g) {
  if (g == NULL) {
    fprintf(stderr, "Erreur jeu non valide");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      int random = rand() % 4;
      game_set_piece_orientation(g, i, j, random);
    }
  }
}

/*##################################################################*/