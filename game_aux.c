#include "game_aux.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_ext.h"
#include "game_graph.h"
#include "game_struct.h"
#include "stack.h"

/*##################################################################*/

void game_print(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Problem of argument\n");
    exit(EXIT_FAILURE);
  }
  int count = 0;
  printf("   ");
  for (int i = 0; i < game_nb_cols(g); i++) {
    printf("%d ", i);  // Print the numbers at the top
  }
  printf("\n   ");
  for (int i = 0; i < 2 * game_nb_cols(g); i++) {
    printf("-");  // Horizontal separator
  }
  printf("\n%d |", count);
  count++;
  // Printing of the game shapes
  for (int i = 0; i < game_nb_cols(g) * game_nb_rows(g); i++) {
    // Printing Empty shapes
    if (g->shape_tab[i] == EMPTY) {
      printf("  ");
    }
    // Printing Endpoint depends of the orientation_tab
    if (g->shape_tab[i] == ENDPOINT) {
      if (g->orientation_tab[i] == NORTH) {
        printf("^ ");
      } else if (g->orientation_tab[i] == EAST) {
        printf("> ");
      } else if (g->orientation_tab[i] == SOUTH) {
        printf("v ");
      } else if (g->orientation_tab[i] == WEST) {
        printf("< ");
      }
    }
    // Printing Segment depends of the orientation_tab
    if (g->shape_tab[i] == SEGMENT) {
      if (g->orientation_tab[i] == NORTH || g->orientation_tab[i] == SOUTH) {
        printf("| ");
      } else if (g->orientation_tab[i] == EAST || g->orientation_tab[i] == WEST) {
        printf("- ");
      } else {
        fprintf(stderr, "Problem of direction\n");
        game_delete((game)g);
        exit(EXIT_FAILURE);
      }
    }
    // Printing Corner depends of the orientation_tab
    if (g->shape_tab[i] == CORNER) {
      if (g->orientation_tab[i] == NORTH) {
        printf("└ ");
      } else if (g->orientation_tab[i] == EAST) {
        printf("┌ ");
      } else if (g->orientation_tab[i] == SOUTH) {
        printf("┐ ");
      } else if (g->orientation_tab[i] == WEST) {
        printf("┘ ");
      }
    }
    // Printing Tee depends of the orientation_tab
    if (g->shape_tab[i] == TEE) {
      if (g->orientation_tab[i] == NORTH) {
        printf("┴ ");
      } else if (g->orientation_tab[i] == EAST) {
        printf("├ ");
      } else if (g->orientation_tab[i] == SOUTH) {
        printf("┬ ");
      } else if (g->orientation_tab[i] == WEST) {
        printf("┤ ");
      }
    }
    // Printing Cross
    if (g->shape_tab[i] == CROSS) {
      printf("┼ ");
    }
    if ((i + 1) % game_nb_cols(g) == 0 && i != 0) {  // Layout of the game
      if (count == game_nb_rows(g)) {                // last row of the game
        printf("|\n");
      } else {  // Move to next row if the current row is complete
        printf("|\n%d |", count);
        count++;
      }
    }
  }
  printf("   ");
  for (int i = 0; i < 2 * game_nb_cols(g); i++) {
    printf("-");  // Horizontal separator
  }
  printf("\n");
}

/*##################################################################*/

game game_default(void) {
  shape shapes_tab[DEFAULT_SIZE * DEFAULT_SIZE] = {
      CORNER,   ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,    TEE,     TEE,      TEE, TEE,      ENDPOINT, ENDPOINT, TEE,
      ENDPOINT, SEGMENT,  ENDPOINT, TEE,    TEE,      CORNER, SEGMENT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab[DEFAULT_SIZE * DEFAULT_SIZE] = {
      WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST, EAST,  EAST, NORTH, WEST,
      WEST, EAST,  SOUTH, SOUTH, NORTH, WEST,  NORTH, EAST,  WEST, SOUTH, EAST, SOUTH};
  game g = game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, shapes_tab, orientations_tab, false);
  return g;
}

/*##################################################################*/

game game_default_solution(void) {
  shape shapes_tab[DEFAULT_SIZE * DEFAULT_SIZE] = {
      CORNER,   ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,    TEE,     TEE,      TEE, TEE,      ENDPOINT, ENDPOINT, TEE,
      ENDPOINT, SEGMENT,  ENDPOINT, TEE,    TEE,      CORNER, SEGMENT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab[DEFAULT_SIZE * DEFAULT_SIZE] = {
      EAST, WEST,  EAST, SOUTH, SOUTH, EAST,  SOUTH, SOUTH, NORTH, WEST, NORTH, NORTH, EAST,
      WEST, SOUTH, EAST, SOUTH, NORTH, SOUTH, NORTH, EAST,  NORTH, WEST, NORTH, NORTH};
  game g = game_new_ext(5, 5, shapes_tab, orientations_tab, false);
  return g;
}

/*##################################################################*/

bool game_get_ajacent_square(cgame g, uint i, uint j, direction d, uint* pi_next, uint* pj_next) {
  uint next_i = i;
  uint next_j = j;
  if (g->wrapping == false) {
    if (d == NORTH) {
      if (i >= 1) {
        next_i--;
      } else {
        return false;
      }
    } else if (d == EAST) {
      next_j++;
    } else if (d == SOUTH) {
      next_i++;
    } else if (d == WEST) {
      if (j >= 1) {
        next_j--;
      } else {
        return false;
      }
    }
    if (next_i >= game_nb_rows(g) || next_j >= game_nb_cols(g)) {
      return false;
    } else {
      *pi_next = next_i;
      *pj_next = next_j;
      return true;
    }
  } else {  // Case where there are the wrapping mode
    if (d == NORTH) {
      *pi_next = (i - 1 + game_nb_rows(g)) % game_nb_rows(g);
      *pj_next = j;
    } else if (d == SOUTH) {
      *pi_next = (i + 1 + game_nb_rows(g)) % game_nb_rows(g);
      *pj_next = j;
    } else if (d == WEST) {
      *pi_next = i;
      *pj_next = (j - 1 + game_nb_cols(g)) % game_nb_cols(g);
    } else {
      *pi_next = i;
      *pj_next = (j + 1 + game_nb_cols(g)) % game_nb_cols(g);
    }
    return true;
  }
}

/*##################################################################*/

bool game_has_half_edge(cgame g, uint i, uint j, direction d) {
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "Erreur : Indices hors des limites de la grille");
    game_delete((game)g);
    exit(EXIT_FAILURE);
  }
  shape piece_shape = game_get_piece_shape(g, i, j);
  direction piece_orientation = game_get_piece_orientation(g, i, j);
  if (piece_shape == ENDPOINT) {
    if (piece_orientation != d) {
      return false;
    }
  }
  if (piece_shape == SEGMENT) {
    if (piece_orientation != d && (piece_orientation + 2) % 4 != d) {
      return false;
    }
  }
  if (piece_shape == CORNER) {
    if (piece_orientation != d && (piece_orientation + 1) % 4 != d) {
      return false;
    }
  }
  if (piece_shape == TEE) {
    if ((piece_orientation + 2) % 4 == d) {
      return false;
    }
  }
  if (piece_shape == CROSS) {
    return true;
  }
  if (piece_shape == EMPTY) {
    return false;
  }
  return true;
}

/*##################################################################*/

edge_status game_check_edge(cgame g, uint i, uint j, direction d) {
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "oui");
    game_delete((game)g);
    exit(EXIT_FAILURE);
  }
  // Variables to store coordinates of the adjacent square
  uint a = 0;
  uint b = 0;
  uint* i_next = &a;
  uint* j_next = &b;
  if (game_get_ajacent_square(g, i, j, d, i_next, j_next) == false) {  // There are no adjacent square inside the grid
    return NOEDGE;
  }
  if (game_has_half_edge(g, i, j, d) &&
      game_has_half_edge(g, *i_next, *j_next,
                         (d - 2) % 4)) {  // Current and adjacent square have both compatible half-eges
    return MATCH;
  } else if (game_has_half_edge(g, i, j, d) ||
             game_has_half_edge(g, *i_next, *j_next,
                                (d - 2) % 4)) {  // One of the squares have a half-ege but the other does not
    return MISMATCH;
  }
  return NOEDGE;  // both have no half-edge
}

/*##################################################################*/

bool game_is_well_paired(cgame g) {
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      int matches = 0;
      // Count for each direction if the (i,j) position have a MATCH
      for (int k = NORTH; k <= WEST; k++) {
        if (game_check_edge(g, i, j, k) == MATCH) {
          matches++;
        }
      }
      // The (i,j) shape have the right number of matches
      if (game_get_piece_shape(g, i, j) == ENDPOINT && matches != 1) {
        return false;
      }
      if (game_get_piece_shape(g, i, j) == SEGMENT && matches != 2) {
        return false;
      }
      if (game_get_piece_shape(g, i, j) == CORNER && matches != 2) {
        return false;
      }
      if (game_get_piece_shape(g, i, j) == TEE && matches != 3) {
        return false;
      }
      if (game_get_piece_shape(g, i, j) == CROSS && matches != 4) {
        return false;
      }
    }
  }
  return true;
}

/*##################################################################*/

bool game_is_connected(cgame g) {
  bool is_marked[game_nb_cols(g) * game_nb_rows(g)];
  for (int i = 0; i < game_nb_cols(g) * game_nb_rows(g); i++) {
    is_marked[i] = false;
  }
  position pos = 0;
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY) {
        pos = GAME2TAB(i, j, g);
      }
    }
  }
  mark_accessible(g, is_marked, pos);
  return every_pieces_is_marked(g, is_marked);
}

/*##################################################################*/
