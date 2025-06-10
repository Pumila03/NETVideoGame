#ifndef __GAME_GRAPH_H__
#define __GAME_GRAPH_H__

#include <stdbool.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"

typedef int position;

position* get_neighbour_list(cgame g, int i, int j);

position accessible_piece(cgame g, bool* is_marked);

void mark_accessible(cgame g, bool* is_marked, position s);

bool every_pieces_is_marked(cgame g, bool* is_marked);

#endif  // __GAME_STRUCT_H__
