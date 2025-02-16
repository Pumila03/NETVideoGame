#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

// @copyright University of Bordeaux. All rights reserved, 2024.

/* ************************************************************************** */

/** @brief Hard-coding of pieces (shape & orientation) in an integer array.
 * @details The 4 least significant bits encode the presence of an half-edge in
 * the N-E-S-W directions (in that order). Thus, binary coding 1100 represents
 * the piece "└" (a corner in north orientation).
 */
static uint _code[NB_SHAPES][NB_DIRS] = {
    {0b0000, 0b0000, 0b0000, 0b0000},  // EMPTY {" ", " ", " ", " "}
    {0b1000, 0b0100, 0b0010, 0b0001},  // ENDPOINT {"^", ">", "v", "<"},
    {0b1010, 0b0101, 0b1010, 0b0101},  // SEGMENT {"|", "-", "|", "-"},
    {0b1100, 0b0110, 0b0011, 0b1001},  // CORNER {"└", "┌", "┐", "┘"}
    {0b1101, 0b1110, 0b0111, 0b1011},  // TEE {"┴", "├", "┬", "┤"}
    {0b1111, 0b1111, 0b1111, 0b1111}   // CROSS {"+", "+", "+", "+"}
};

/* ************************************************************************** */

/** encode a shape and an orientation into an integer code */
static uint _encode_shape(shape s, direction o) { return _code[s][o]; }

/* ************************************************************************** */

/** decode an integer code into a shape and an orientation */
static bool _decode_shape(uint code, shape* s, direction* o) {
  assert(code >= 0 && code < 16);
  assert(s);
  assert(o);
  for (int i = 0; i < NB_SHAPES; i++)
    for (int j = 0; j < NB_DIRS; j++)
      if (code == _code[i][j]) {
        *s = i;
        *o = j;
        return true;
      }
  return false;
}

/* ************************************************************************** */

/** add an half-edge in the direction d */
static void _add_half_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  shape s = game_get_piece_shape(g, i, j);
  direction o = game_get_piece_orientation(g, i, j);
  uint code = _encode_shape(s, o);
  uint mask = 0b1000 >> d;     // mask with half-edge in the direction d
  assert((code & mask) == 0);  // check there is no half-edge in the direction d
  uint newcode = code | mask;  // add the half-edge in the direction d
  shape news;
  direction newo;
  bool ok = _decode_shape(newcode, &news, &newo);
  assert(ok);
  game_set_piece_shape(g, i, j, news);
  game_set_piece_orientation(g, i, j, newo);
}

/* ************************************************************************** */

#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)

/* ************************************************************************** */

/**
 * @brief Add an edge between two adjacent squares.
 * @details This is done by modifying the pieces of the two adjacent squares.
 * More precisely, we add an half-edge to each adjacent square, so as to build
 * an edge between these two squares.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param d the direction of the adjacent square
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game height
 * @pre @p j < game width
 * @return true if an edge can be added, false otherwise
 */
static bool _add_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  uint nexti, nextj;
  bool next = game_get_ajacent_square(g, i, j, d, &nexti, &nextj);
  if (!next) return false;

  // check if the two half-edges are free
  bool he = game_has_half_edge(g, i, j, d);
  if (he) return false;
  bool next_he = game_has_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));
  if (next_he) return false;

  _add_half_edge(g, i, j, d);
  _add_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));

  return true;
}

/* ************************************************************************** */

typedef struct {
  uint i;
  uint j;
  bool is_surrounded;
  bool is_checked;
} piece;

/*##################################################################*/

game game_load(char* filename) {
  FILE* file = fopen(filename, "r");
  int nb_rows, nb_cols, wrap_val;
  bool wrapping;
  fscanf(file, "%d %d %d\n", &nb_rows, &nb_cols, &wrap_val);
  if (wrap_val == 1) {
    wrapping = true;
  } else {
    wrapping = false;
  }
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  for (int i = 0; i < nb_rows; i++) {
    for (int j = 0; j < nb_cols; j++) {
      char shape, orientation;
      fscanf(file, "%c%c ", &shape, &orientation);
      // To print the shape
      if (shape == 'E') {
        game_set_piece_shape(g, i, j, EMPTY);
      } else if (shape == 'N') {
        game_set_piece_shape(g, i, j, ENDPOINT);
      } else if (shape == 'S') {
        game_set_piece_shape(g, i, j, SEGMENT);
      } else if (shape == 'C') {
        game_set_piece_shape(g, i, j, CORNER);
      } else if (shape == 'T') {
        game_set_piece_shape(g, i, j, TEE);
      } else if (shape == 'X') {
        game_set_piece_shape(g, i, j, CROSS);
      } else {
        fprintf(stderr, "Bad shapes\n");
        exit(EXIT_FAILURE);
      }
      // To print the orientation
      if (orientation == 'N') {
        game_set_piece_orientation(g, i, j, NORTH);
      } else if (orientation == 'E') {
        game_set_piece_orientation(g, i, j, EAST);
      } else if (orientation == 'S') {
        game_set_piece_orientation(g, i, j, SOUTH);
      } else if (orientation == 'W') {
        game_set_piece_orientation(g, i, j, WEST);
      } else {
        fprintf(stderr, "Bad orientation\n");
        exit(EXIT_FAILURE);
      }
    }
    fscanf(file, "\n");
  }

  fclose(file);
  return g;
}

/*##################################################################*/

void game_save(cgame g, char* filename) {
  if (filename == NULL) {
    fprintf(stderr, "erreur de sauvegarde: nom de fichier invalide\n");
    return;
  }
  if (g == NULL) {
    fprintf(stderr, "erreur de sauvegarde: jeu invalide");
    return;
  }
  FILE* file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "erreur de sauvegarde: impossible d'ouvrir le fichier");
    return;
  }
  char c_rows = game_nb_rows(g) + '0';
  char c_cols = game_nb_cols(g) + '0';
  bool wrapping = game_is_wrapping(g);
  if (fputc(c_rows, file) == EOF) {
    fprintf(stderr, "erreur de sauvegarde");
    fclose(file);
    return;
  }
  fprintf(file, " ");
  if (fputc(c_cols, file) == EOF) {
    fprintf(stderr, "erreur de sauvegarde");
    fclose(file);
    return;
  }
  fprintf(file, " ");
  if (wrapping) {
    if (fputc('1', file) == EOF) {
      fprintf(stderr, "erreur de sauvegarde");
      fclose(file);
      return;
    }
  } else {
    if (fputc('0', file) == EOF) {
      fprintf(stderr, "erreur de sauvegarde");
      fclose(file);
      return;
    }
  }
  fprintf(file, "\n");
  uint rows = game_nb_rows(g);
  uint cols = game_nb_cols(g);
  for (uint i = 0; i < rows; i++) {
    for (uint j = 0; j < cols; j++) {
      char S = 'E';
      char D = 'N';
      if (game_get_piece_shape(g, i, j) == EMPTY) {
        S = 'E';
      } else if (game_get_piece_shape(g, i, j) == ENDPOINT) {
        S = 'N';
      } else if (game_get_piece_shape(g, i, j) == SEGMENT) {
        S = 'S';
      } else if (game_get_piece_shape(g, i, j) == CORNER) {
        S = 'C';
      } else if (game_get_piece_shape(g, i, j) == TEE) {
        S = 'T';
      } else if (game_get_piece_shape(g, i, j) == CROSS) {
        S = 'X';
      }
      if (game_get_piece_orientation(g, i, j) == NORTH) {
        D = 'N';
      } else if (game_get_piece_orientation(g, i, j) == EAST) {
        D = 'E';
      } else if (game_get_piece_orientation(g, i, j) == SOUTH) {
        D = 'S';
      } else if (game_get_piece_orientation(g, i, j) == WEST) {
        D = 'W';
      }
      if (fputc(S, file) == EOF) {
        fprintf(stderr, "erreur de sauvegarde");
        fclose(file);
        return;
      }
      if (fputc(D, file) == EOF) {
        fprintf(stderr, "erreur de sauvegarde");
        fclose(file);
        return;
      }
      fprintf(file, " ");
    }
    fprintf(file, "\n");
  }
  fclose(file);
}

/*##################################################################*/

bool game_empty_cell_around(game g, uint i, uint j, uint* next_i, uint* next_j) {
  uint get_aj_i;
  uint get_aj_j;
  uint nb_empty = 0;
  uint tab_i[4] = {};
  uint tab_j[4] = {};
  if (game_get_ajacent_square(g, i, j, NORTH, &get_aj_i, &get_aj_j) == true &&
      game_get_piece_shape(g, get_aj_i, get_aj_j) == EMPTY) {
    tab_i[nb_empty] = get_aj_i;
    tab_j[nb_empty] = get_aj_j;
    nb_empty++;
  }
  if (game_get_ajacent_square(g, i, j, SOUTH, &get_aj_i, &get_aj_j) == true &&
      game_get_piece_shape(g, get_aj_i, get_aj_j) == EMPTY) {
    tab_i[nb_empty] = get_aj_i;
    tab_j[nb_empty] = get_aj_j;
    nb_empty++;
  }
  if (game_get_ajacent_square(g, i, j, WEST, &get_aj_i, &get_aj_j) == true &&
      game_get_piece_shape(g, get_aj_i, get_aj_j) == EMPTY) {
    tab_i[nb_empty] = get_aj_i;
    tab_j[nb_empty] = get_aj_j;
    nb_empty++;
  }
  if (game_get_ajacent_square(g, i, j, EAST, &get_aj_i, &get_aj_j) == true &&
      game_get_piece_shape(g, get_aj_i, get_aj_j) == EMPTY) {
    tab_i[nb_empty] = get_aj_i;
    tab_j[nb_empty] = get_aj_j;
    nb_empty++;
  }
  if (nb_empty == 0) {
    return false;
  } else {
    uint random = rand() % (nb_empty);
    *next_i = tab_i[random];
    *next_j = tab_j[random];
    return true;
  }
}

/*##################################################################*/

direction pieces_direction(game g, uint i1, uint j1, uint i2, uint j2) {
  int i = (int)i1 - (int)i2;
  int j = (int)j1 - (int)j2;

  if (!(game_is_wrapping(g))) {
    if (i == 0) {  // Same row
      if (j == 1) return WEST;
      if (j == -1) return EAST;
    } else if (j == 0) {  // Same col
      if (i == 1) return NORTH;
      if (i == -1) return SOUTH;
    }
  } else {
    if (i == 0) {  // Same row
      if (j == 1 || (j1 == 0 && j2 == g->nb_cols - 1)) return WEST;
      if (j == -1 || (j1 == g->nb_cols - 1 && j2 == 0)) return EAST;
    } else if (j == 0) {  // Same col
      if (i == 1 || (i1 == 0 && i2 == g->nb_rows - 1)) return NORTH;
      if (i == -1 || (i1 == g->nb_rows - 1 && i2 == 0)) return SOUTH;
    }
  }
  return NB_DIRS;
}

/*##################################################################*/

bool every_pieces_checked(piece tab_pieces[], uint nb_pieces) {
  for (int i = 0; i < nb_pieces; i++) {
    if (tab_pieces[i].is_checked == false) {
      return false;
    }
  }
  return true;
}

/*##################################################################*/

game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_empty, uint nb_extra) {
  // SHOULD ADD VERIFICATION OF THE PARAMETERS !!!

  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  bool is_a_border = true;
  ;

  // Choose a position for the first ENDPOINT

  uint i = (((double)rand() * nb_rows) / RAND_MAX);
  uint j = (((double)rand() * nb_cols) / RAND_MAX);
  uint d = (((double)rand() * NB_DIRS) / RAND_MAX);

  uint next_i, next_j;

  piece tab_pieces[nb_cols * nb_rows];
  uint nb_of_pieces = 0;
  // Look for the direction of the next ENDPOINT which is not a border

  while (is_a_border) {
    next_i = i;
    next_j = j;
    if (d == NORTH) {
      next_i--;
    } else if (d == EAST) {
      next_j++;
    } else if (d == SOUTH) {
      next_i++;
    } else if (d == WEST) {
      next_j--;
    }
    if (wrapping == false && (next_i < 0 || next_i >= nb_rows || next_j < 0 || next_j >= nb_cols)) {
      d = (d + 1) % NB_DIRS;
    } else {
      is_a_border = false;
    }
  }

  _add_edge(g, i, j, d);  // Put the two first ENDPOINTS
  tab_pieces[nb_of_pieces] = (piece){i, j, false, false};
  nb_of_pieces++;
  uint other_empty_i, other_empty_j;
  game_get_ajacent_square(g, i, j, d, &other_empty_i, &other_empty_j);
  tab_pieces[nb_of_pieces] = (piece){other_empty_i, other_empty_j, false, false};
  nb_of_pieces++;
  uint to_fill = nb_rows * nb_cols - 2;

  while (to_fill > nb_empty) {
    piece* r_piece = (tab_pieces + (uint)(((double)rand() * nb_of_pieces) / RAND_MAX));
    uint next_i, next_j;
    if (game_empty_cell_around(g, r_piece->i, r_piece->j, &next_i, &next_j) ==
        false) {  // Check if if there is empty neighbours and get one
      r_piece->is_surrounded = true;
    }
    while (r_piece->is_surrounded == true) {
      r_piece = (tab_pieces + (uint)(((double)rand() * nb_of_pieces) / RAND_MAX));
      if (game_empty_cell_around(g, r_piece->i, r_piece->j, &next_i, &next_j) == false) {
        r_piece->is_surrounded = true;
      }
    }

    direction dir_p1 = pieces_direction(g, r_piece->i, r_piece->j, next_i, next_j);

    _add_edge(g, r_piece->i, r_piece->j, dir_p1);
    tab_pieces[nb_of_pieces] = (piece){next_i, next_j, false, false};
    nb_of_pieces++;
    to_fill--;
  }
  int edge_added = 0;
  while (edge_added < nb_extra && !every_pieces_checked(tab_pieces, nb_of_pieces))  // add extra edges
  {
    piece* r_piece = (tab_pieces + (uint)(((double)rand() * nb_of_pieces) / RAND_MAX));
    uint pi_next, pj_next;
    for (int d = NORTH; d < NB_DIRS; d++) {
      if (game_get_ajacent_square(g, r_piece->i, r_piece->j, d, &pi_next, &pj_next) == true) {
        if (game_get_piece_shape(g, pi_next, pj_next) != EMPTY) {
          if (_add_edge(g, r_piece->i, r_piece->j, d)) {
            edge_added++;
            break;
          }
        }
      }
      if (d == WEST) {
        r_piece->is_checked = true;
      }
    }
  }
  return g;
}

/*##################################################################*/
/*############################Solveur###############################*/
/*##################################################################*/

/*##################################################################*/

void game_solve_aux(int pos, direction* tab, shape* tabs, int len, game g, unsigned long *count) {
    if(pos == len) {
      game test = game_new_ext(game_nb_rows(g), game_nb_cols(g), tabs, tab, false);
      if(game_won(test)) {
        game_print(test);
        (*count)++;
        game_delete(test);
        return;
      }
      game_delete(test);
    }  
      // extend current word recursively with value 0 at position pos
      tab[pos] = NORTH;
      game_solve_aux(pos + 1, tab, tabs, len, g, count);
      tab[pos] = EAST;
      game_solve_aux(pos + 1, tab, tabs, len, g, count);
      tab[pos] = SOUTH;
      game_solve_aux(pos + 1, tab, tabs, len, g, count);
      tab[pos] = WEST;
      game_solve_aux(pos + 1, tab, tabs, len, g, count);
}

bool game_solve(game g) {
  int len = game_nb_cols(g)*game_nb_rows(g);
  direction tab[len];
  shape tabs[len];
  for(int i=0; game_nb_rows(g) > i; i++) {
    for(int j=0; game_nb_cols(g) > j; j++) {
      tabs[GAME2TAB(i, j, g)] = game_get_piece_shape(g, i,j);
    }
  }
  unsigned long count = 0;
  game_solve_aux(0, tab, tabs, len, g, &count);
  if(count == 0) {
    return false;
  } else {
    return true;
  }
}