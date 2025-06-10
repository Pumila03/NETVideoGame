#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__

#include "game.h"
#include "game_ext.h"
#include "stack.h"

#define GAME2TAB(i, j, g) (game_nb_cols(g) * (i) + (j))    // To find index x with the two position (i,j)
#define TAB2GAMEI(index, g) ((index) / (game_nb_cols(g)))  // To find position i with index  x of the array
#define TAB2GAMEJ(index, g) ((index) % (game_nb_cols(g)))  // To find position j with index  x of the array

struct game_s {
  uint nb_rows;
  uint nb_cols;
  shape *shape_tab;
  direction *orientation_tab;
  Stack history;
  Stack redo;  // Stack where previous play are stock
  bool wrapping;
};

#endif  // __GAME_STRUCT_H__
