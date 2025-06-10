#include "game_graph.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"

#define NONE -1

// Implementation of graph (graphe en français) which we use to implement the function game_is_connected and get which
// cell is connected and which one is not, all of those functions are used on main implementation files.
//

/*##################################################################*/

// With this function we get the list of neighbours of a given cell
position* get_neighbour_list(cgame g, int i, int j) {
  position arround_coordinate[4] = {NONE, NONE, NONE, NONE};  // last values is the number of elements
  for (direction d = NORTH; d <= WEST; d++) {
    unsigned int x, y;
    if (game_get_ajacent_square(g, i, j, d, &x, &y)) {
      arround_coordinate[d] = GAME2TAB(x, y, g);
    }
  }
  // calcul neighbour(connected pieces arround)

  position* connected_pieces = malloc(sizeof(position) * 5);

  direction directions[4] = {NORTH, EAST, SOUTH, WEST};
  for (int z = 0; z < 4; z++) {
    if (arround_coordinate[z] != NONE) {
      if (game_check_edge(g, TAB2GAMEI(arround_coordinate[z], g), TAB2GAMEJ(arround_coordinate[z], g),
                          (directions[z] + 2) % 4) == MATCH &&
          arround_coordinate[z] != NONE) {
        connected_pieces[z] = arround_coordinate[z];
      } else {
        connected_pieces[z] = NONE;
      }

    } else {
      connected_pieces[z] = NONE;
    }
  }
  return connected_pieces;
}

/*##################################################################*/

// This function check every piece of the game and returns the first accessible piece among the neighbours of the piece.
position accessible_piece(cgame g, bool is_marked[]) {
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      position* connected_pieces = get_neighbour_list(g, i, j);

      for (int k = 0; k < 4; k++) {
        if (connected_pieces[k] != NONE) {
          if (is_marked[GAME2TAB(i, j, g)] == false && is_marked[connected_pieces[k]] == true) {
            free(connected_pieces);
            return GAME2TAB(i, j, g);
          }
        }
      }
      free(connected_pieces);
    }
  }
  return NONE;
}

/*##################################################################*/
// This function is useful to be able to mark an accessible cell
void mark_accessible(cgame g, bool is_marked[], position s) {
  is_marked[s] = true;
  while (accessible_piece(g, is_marked) != NONE) {
    is_marked[accessible_piece(g, is_marked)] = true;
  }
}

/*#################################################################*/
// With this function we check if every cell of the game is marked, if yes, we return true
bool every_pieces_is_marked(cgame g, bool is_marked[]) {
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if (game_get_piece_shape(g, i, j) == EMPTY) {
        is_marked[GAME2TAB(i, j, g)] = true;
      }
      if (is_marked[GAME2TAB(i, j, g)] == false) {
        return false;
      }
    }
  }
  return true;
}

/*##################################################################*/
