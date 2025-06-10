/**
 * @file wrapper.js
 * @brief Game Binding to JavaScript (based on Emscripten & Wasm)
 * @author aurelien.esnard@u-bordeaux.fr
 * @copyright University of Bordeaux. All rights reserved, 2023.
 **/

#include <emscripten.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/* ******************** Game WASM API ******************** */

EMSCRIPTEN_KEEPALIVE
game new_default(void) { return game_default(); }

EMSCRIPTEN_KEEPALIVE
void delete (game g) { game_delete(g); }

EMSCRIPTEN_KEEPALIVE
void play_move(game g, uint i, uint j, int nb_quarter_turns) { game_play_move(g, i, j, nb_quarter_turns); }

EMSCRIPTEN_KEEPALIVE
void restart(game g) { game_shuffle_orientation(g); }

EMSCRIPTEN_KEEPALIVE
bool won(game g) { return game_won(g); }

EMSCRIPTEN_KEEPALIVE
shape get_piece_shape(cgame g, uint i, uint j) { return game_get_piece_shape(g, i, j); }

EMSCRIPTEN_KEEPALIVE
direction get_piece_orientation(cgame g, uint i, uint j) { return game_get_piece_orientation(g, i, j); }

EMSCRIPTEN_KEEPALIVE
uint nb_rows(cgame g) { return game_nb_rows(g); }

EMSCRIPTEN_KEEPALIVE
uint nb_cols(cgame g) { return game_nb_cols(g); }

EMSCRIPTEN_KEEPALIVE
edge_status check_edge(cgame g, uint i, uint j, direction d) { return game_check_edge(g, i, j, d); }

EMSCRIPTEN_KEEPALIVE
void undo(game g) { game_undo(g); }

EMSCRIPTEN_KEEPALIVE
void redo(game g) { game_redo(g); }

EMSCRIPTEN_KEEPALIVE
bool is_wrapping(cgame g) { return game_is_wrapping(g); }
/* Add more function wrappers here, if needed... */

EMSCRIPTEN_KEEPALIVE
bool solve(game g) { return game_solve(g); }

// Met le jeu en string pour pouvoir le passer entre fichier (worker et demo.js)
EMSCRIPTEN_KEEPALIVE
char* jeu_en_string(game g) {
  char* resultat = malloc(2048);
  sprintf(resultat, "{\"rows\":%d,\"cols\":%d,\"wrap\":%d,\"shapes\":[", g->nb_rows, g->nb_cols, g->wrapping);

  for (uint i = 0; i < (g->nb_rows*g->nb_cols); i++) {
    sprintf(resultat + strlen(resultat), "%d", g->shape_tab[i]);
    if (i != g->nb_rows*g->nb_cols-1) {
      strcat(resultat, ",");
    }
  }

  strcat(resultat, "],\"orientations\":[");
  for (uint i = 0; i < (g->nb_rows*g->nb_cols); i++) {
    sprintf(resultat + strlen(resultat), "%d", g->orientation_tab[i]);
    if (i != g->nb_rows*g->nb_cols-1) strcat(resultat, ",");
  }
  strcat(resultat, "]}");
  return resultat;
}

// Recrée le jeu depuis les données
EMSCRIPTEN_KEEPALIVE
game new_game_from_data(int nb_rows, int nb_cols, bool wrapping, int* shapes, int* orientations) {
  uint size = nb_rows * nb_cols;
  shape* shapes_tab = malloc(sizeof(shape) * size);
  direction* orientations_tab = malloc(sizeof(direction) * size);

  for (uint i = 0; i < size; i++) {
    shapes_tab[i] = (shape)shapes[i];
    orientations_tab[i] = (direction)orientations[i];
  }

  return game_new_ext(nb_rows, nb_cols, shapes_tab, orientations_tab, wrapping);
}

EMSCRIPTEN_KEEPALIVE
void* alloc(int size) { return malloc(size); }

EMSCRIPTEN_KEEPALIVE
void free_alive(void* pointeur) { free(pointeur); }

EMSCRIPTEN_KEEPALIVE
game new_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_empty, uint nb_extra) {
  return game_random(nb_rows, nb_cols, wrapping, nb_empty, nb_extra);
}


EMSCRIPTEN_KEEPALIVE
game load(char* filename) {
  return game_load(filename);
}
EMSCRIPTEN_KEEPALIVE
void save(cgame g, char* filename) {
  game_save(g, filename);
} 
// EOF
