#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/*##################################################################*/

bool test_game_play_move(void) {
  game g = game_default();
  // Put the two first shape in North direction
  game_set_piece_orientation(g, 0, 0, NORTH);
  game_set_piece_orientation(g, 0, 1, NORTH);
  game copy_g = game_copy(g);
  if (!g || !copy_g) {
    return false;
  } else {
    game_play_move(g, 0, 0, 1);                         // Turn the (0,0) piece to EAST in game g
    if (game_get_piece_orientation(g, 0, 0) != EAST) {  // Check if (0,0) direction is EAST in game g
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_play_move(copy_g, 0, 0, 1);      // Turn the (0,0) piece to EAST in game copy_g
    if (!game_equal(copy_g, g, false)) {  // Check if copy_g is the same as g
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_play_move(g, 0, 1, -1);                        // Turn the (0,1) piece to WEST in game g
    if (game_get_piece_orientation(g, 0, 1) != WEST) {  // Check if move is done correctly
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_play_move(copy_g, 0, 1, -1);     // Turn the (0,1) piece to WEST in game copy_g
    if (!game_equal(copy_g, g, false)) {  // Check if copy_g is the same as g
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_play_move(g, 0, 2, -1);
    if (game_get_piece_shape(g, 0, 2) != game_get_piece_shape(copy_g, 0, 2)) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_play_move(g, 0, 3, -1);
    if (game_get_piece_orientation(g, 0, 3) == game_get_piece_orientation(copy_g, 0, 3)) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    copy_g = game_copy(g);
    shape original_shape = game_get_piece_shape(g, 0, 4);
    direction original_orientation = game_get_piece_orientation(g, 0, 4);
    game_play_move(g, 0, 4, 1);
    game_set_piece_shape(g, 0, 4, original_shape);
    game_set_piece_orientation(g, 0, 4, original_orientation);
    if (!game_equal(copy_g, g, false)) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_delete(g);
    game_delete(copy_g);
    return true;
  }
}

/*##################################################################*/

bool test_game_is_well_paired(void) {
  game g_solution = game_default_solution();
  game g = game_default();
  if (!game_is_well_paired(g_solution) || game_is_well_paired(g)) {  // Check if g_solution is well paired and g is not
    game_delete(g_solution);
    game_delete(g);
    return false;
  }
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      // Check if a piece of the array have a mismatch status, meaning g_solution is not well-paired
      if (game_check_edge(g_solution, i, j, NORTH) == MISMATCH ||
          game_check_edge(g_solution, i, j, SOUTH) == MISMATCH || game_check_edge(g_solution, i, j, EAST) == MISMATCH ||
          game_check_edge(g_solution, i, j, WEST) == MISMATCH) {
        game_delete(g_solution);
        game_delete(g);
        return false;
      }
    }
  }
  // Creating a new game
  shape shapes_tab[9] = {ENDPOINT, EMPTY, ENDPOINT, ENDPOINT, EMPTY, ENDPOINT, ENDPOINT, EMPTY, ENDPOINT};
  direction orientations_tab[9] = {WEST, NORTH, EAST, WEST, NORTH, EAST, WEST, NORTH, EAST};
  game g_ext = game_new_ext(3, 3, shapes_tab, orientations_tab, true);
  for (int i = 0; i < game_nb_rows(g_ext); i++) {
    for (int j = 0; j < game_nb_cols(g_ext); j++) {
      uint sinext, sjnext, ninext, njnext, winext, wjnext, einext,
          ejnext;  // Initialise variables to keep next i and j in s(south),n(north),w(west),e(east)
      // Get adjacent cell for each direction
      game_get_ajacent_square(g_ext, i, j, SOUTH, &sinext, &sjnext);
      game_get_ajacent_square(g_ext, i, j, NORTH, &ninext, &njnext);
      game_get_ajacent_square(g_ext, i, j, WEST, &winext, &wjnext);
      game_get_ajacent_square(g_ext, i, j, EAST, &einext, &ejnext);
      // Check for half-edge mismatched in every direction
      if ((game_has_half_edge(g_ext, sinext, sjnext, NORTH) == true &&
           game_has_half_edge(g_ext, i, j, SOUTH) == false) ||
          (game_has_half_edge(g_ext, sinext, sjnext, NORTH) == false &&
           game_has_half_edge(g_ext, i, j, SOUTH) == true)) {
        game_delete(g_ext);
        game_delete(g_solution);
        game_delete(g);
        return false;
      }
      if ((game_has_half_edge(g_ext, ninext, njnext, SOUTH) == true &&
           game_has_half_edge(g_ext, i, j, NORTH) == false) ||
          (game_has_half_edge(g_ext, njnext, njnext, SOUTH) == false &&
           game_has_half_edge(g_ext, i, j, NORTH) == true)) {
        game_delete(g_ext);
        game_delete(g_solution);
        game_delete(g);
        return false;
      }
      if ((game_has_half_edge(g_ext, einext, ejnext, WEST) == true && game_has_half_edge(g_ext, i, j, EAST) == false) ||
          (game_has_half_edge(g_ext, einext, ejnext, WEST) == false && game_has_half_edge(g_ext, i, j, EAST) == true)) {
        game_delete(g_ext);
        game_delete(g_solution);
        game_delete(g);
        return false;
      }
      if ((game_has_half_edge(g_ext, winext, wjnext, EAST) == true && game_has_half_edge(g_ext, i, j, WEST) == false) ||
          (game_has_half_edge(g_ext, winext, wjnext, EAST) == false && game_has_half_edge(g_ext, i, j, WEST) == true)) {
        game_delete(g_ext);
        game_delete(g_solution);
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);
  game_delete(g_solution);
  // Define a new game configuration (game_new2) where everything is well-paired
  shape shapes_tab2[25] = {EMPTY,  EMPTY, EMPTY,   EMPTY, EMPTY,   EMPTY, CORNER, SEGMENT, CORNER,
                           EMPTY,  EMPTY, SEGMENT, EMPTY, SEGMENT, EMPTY, EMPTY,  CORNER,  SEGMENT,
                           CORNER, EMPTY, EMPTY,   EMPTY, EMPTY,   EMPTY, EMPTY};
  direction orientations_tab2[25] = {NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, EAST,  EAST,  SOUTH,
                                     NORTH, NORTH, SOUTH, NORTH, SOUTH, NORTH, NORTH, NORTH, EAST,
                                     WEST,  NORTH, NORTH, NORTH, NORTH, NORTH, NORTH};
  game game_new2 = game_new(shapes_tab2, orientations_tab2);
  if (game_is_well_paired(game_new2) != true) {
    game_delete(game_new2);
    return false;
  }
  game_delete(game_new2);
  // Define a new game configuration (game_multi) where everything is well-paired
  shape shapes_game_multi[25] = {CORNER, SEGMENT, CORNER,   EMPTY, EMPTY,  CORNER, TEE,   CORNER, EMPTY,
                                 EMPTY,  EMPTY,   ENDPOINT, EMPTY, CORNER, CORNER, EMPTY, EMPTY,  ENDPOINT,
                                 TEE,    SEGMENT, EMPTY,    EMPTY, EMPTY,  CORNER, CORNER};
  direction direction_game_multi[25] = {EAST,  EAST,  SOUTH, NORTH, NORTH, NORTH, SOUTH, WEST,  NORTH,
                                        NORTH, NORTH, NORTH, NORTH, EAST,  SOUTH, NORTH, NORTH, EAST,
                                        WEST,  SOUTH, NORTH, NORTH, NORTH, NORTH, WEST};
  game game_multi = game_new(shapes_game_multi, direction_game_multi);
  if (game_is_well_paired(game_multi) != true) {
    game_delete(game_multi);
    return false;
  }
  game_delete(game_multi);
  // Define a new game configuration (game_multi_2) where it's not well-paired
  shape shapes_game_multi_2[25] = {CORNER, SEGMENT, CORNER,   EMPTY, EMPTY,  CORNER, TEE,   CORNER, EMPTY,
                                   EMPTY,  EMPTY,   ENDPOINT, EMPTY, CORNER, CORNER, EMPTY, EMPTY,  ENDPOINT,
                                   TEE,    SEGMENT, EMPTY,    EMPTY, EMPTY,  CORNER, CORNER};
  direction direction_game_multi_2[25] = {NORTH, EAST,  SOUTH, NORTH, NORTH, NORTH, SOUTH, WEST,  NORTH,
                                          NORTH, NORTH, NORTH, NORTH, EAST,  SOUTH, NORTH, NORTH, EAST,
                                          WEST,  SOUTH, NORTH, NORTH, NORTH, NORTH, WEST};
  game game_multi_2 = game_new(shapes_game_multi_2, direction_game_multi_2);
  if (game_is_well_paired(game_multi_2) != false) {
    game_delete(game_multi_2);
    return false;
  }
  game_delete(game_multi_2);
  // Define a new game configuration (game_wrapping) where everything is well-paired and we use wrapping
  shape A[1] = {CROSS};
  direction B[1] = {NORTH};
  game game_wrapping = game_new_ext(1, 1, A, B, true);
  if (game_is_well_paired(game_wrapping) == false) {
    game_delete(game_wrapping);
    return false;
  }
  game_delete(game_wrapping);
  // Define a new game configuration (g_invalid) where it's not well-paired and we use wrapping
  shape shapes_invalid[2] = {SEGMENT, SEGMENT};
  direction orientations_invalid[2] = {EAST, WEST};
  game g_invalid = game_new_ext(1, 2, shapes_invalid, orientations_invalid, true);
  if (game_is_well_paired(g_invalid) == false) {
    game_delete(g_invalid);
    return false;
  }
  game_delete(g_invalid);
  // Define a new game configuration (game_new_game) where everything is well-paired and we use wrapping
  shape new_test_1[9] = {CORNER, SEGMENT, CORNER, SEGMENT, CROSS, SEGMENT, CORNER, SEGMENT, CORNER};
  direction new_test_1d[9] = {WEST, SOUTH, NORTH, EAST, SOUTH, EAST, SOUTH, SOUTH, EAST};
  game game_new_game = game_new_ext(3, 3, new_test_1, new_test_1d, true);
  if (game_is_well_paired(game_new_game) == false) {
    game_delete(game_new_game);
    return false;
  }
  game_delete(game_new_game);
  // Define a new game configuration (game_new_game2) where everything is well-paired and we use wrapping
  shape new_test_2[2] = {SEGMENT, SEGMENT};
  direction new_test_2d[2] = {SOUTH, SOUTH};
  game game_new_game2 = game_new_ext(1, 2, new_test_2, new_test_2d, true);
  if (game_is_well_paired(game_new_game2) == false) {
    game_delete(game_new_game2);
    return false;
  }
  game_delete(game_new_game2);
  // Define a new game configuration (game_new_game3) where it's not well-paired and we use wrapping
  shape new_test_3[1] = {SEGMENT};
  direction new_test_3d[1] = {SOUTH};
  game game_new_game3 = game_new_ext(1, 1, new_test_3, new_test_3d, false);
  if (game_is_well_paired(game_new_game3) == true) {
    game_delete(game_new_game3);
    return false;
  }
  game_delete(game_new_game3);
  // Define a new game configuration (game_new_game4) where everything is well-paired and we use wrapping
  shape new_test_4[1] = {SEGMENT};
  direction new_test_4d[1] = {SOUTH};
  game game_new_game4 = game_new_ext(1, 1, new_test_4, new_test_4d, true);
  if (game_is_well_paired(game_new_game4) == false) {
    game_delete(game_new_game4);
    return false;
  }
  game_delete(game_new_game4);
  // Define a new game configuration (game_new_game5) where it's not well-paired and we use wrapping
  shape new_test_5[2] = {SEGMENT, TEE};
  direction new_test_5d[2] = {SOUTH, EAST};
  game game_new_game5 = game_new_ext(1, 2, new_test_5, new_test_5d, true);
  if (game_is_well_paired(game_new_game5) == true) {
    game_delete(game_new_game5);
    return false;
  }
  game_delete(game_new_game5);
  return true;
}

/*##################################################################*/

bool test_game_set_piece_orientation(void) {
  game g = game_default();
  if (!g) {
    return false;
  } else {
    game copy_g = game_copy(g);
    game_set_piece_orientation(g, 0, 0, NORTH);
    // Check if the fonction set correctly the orientation
    if (game_get_piece_orientation(g, 0, 0) != NORTH) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_set_piece_orientation(g, 0, 0, EAST);
    // Check if the fonction set correctly the orientation
    if (game_get_piece_orientation(g, 0, 0) != EAST) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_set_piece_orientation(g, 0, 0, SOUTH);
    // Check if the fonction set correctly the orientation
    if (game_get_piece_orientation(g, 0, 0) != SOUTH) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_set_piece_orientation(g, 0, 0, WEST);
    // Check if the fonction set correctly the orientation
    if (game_get_piece_orientation(g, 0, 0) != WEST) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    if (game_get_piece_shape(g, 0, 0) != game_get_piece_shape(copy_g, 0, 0)) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_set_piece_orientation(g, 0, 0, game_get_piece_orientation(copy_g, 0, 0));
    if (!game_equal(copy_g, g, false)) {
      game_delete(g);
      game_delete(copy_g);
      return false;
    }
    game_delete(g);
    game_delete(copy_g);
    return true;
  }
}

/*##################################################################*/

bool test_game_shuffle_orientation(void) {
  game g = game_default();
  game copy_g = game_copy(g);
  game_shuffle_orientation(g);
  // Check if the game_shuffle_orientation is working on (game) g
  if (game_equal(g, copy_g, false) != false || game_equal(g, copy_g, true) != true) {
    game_delete(g);
    game_delete(copy_g);
    return false;
  }
  bool diff = false;
  // Do additional shuffle and check that orientation change
  for (int i = 0; i < 10; i++) {
    game_shuffle_orientation(g);
    if (game_equal(g, copy_g, false) == false) {  // Detect any orientation difference
      diff = true;
      break;
    }
  }
  // Check if a difference has been detected
  if (diff != true) {
    game_delete(g);
    game_delete(copy_g);
    return false;
  }
  game_delete(g);
  game_delete(copy_g);

  game g1 = game_default();
  game g2 = game_copy(g1);
  // Variables to track orientation distributions and specific matchs
  int k = 0;
  int h = 0;
  int l = 0;
  int n = 0;
  int sameN = 0;
  int sameS = 0;
  int sameW = 0;
  int sameE = 0;
  game_shuffle_orientation(g2);
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      // Count the number of each orientation (N,S,E,W)
      if (game_get_piece_orientation(g2, i, j) == NORTH) {
        sameN++;
      }
      if (game_get_piece_orientation(g2, i, j) == EAST) {
        sameE++;
      }
      if (game_get_piece_orientation(g2, i, j) == WEST) {
        sameW++;
      }
      if (game_get_piece_orientation(g2, i, j) == SOUTH) {
        sameS++;
      }
      // Check orientation differences between original (g1) and shuffle game (g2)
      if (game_get_piece_orientation(g2, i, j) == (game_get_piece_orientation(g1, i, j) + 1) % 4) {
        k++;
      }
      if (game_get_piece_orientation(g2, i, j) == (game_get_piece_orientation(g1, i, j) + 2) % 4) {
        h++;
      }
      if (game_get_piece_orientation(g2, i, j) == (game_get_piece_orientation(g1, i, j) + 3) % 4) {
        l++;
      }
      if (game_get_piece_orientation(g2, i, j) == (game_get_piece_orientation(g1, i, j) + 4) % 4) {
        n++;
      }
    }
  }
  // Check if a single orientation is put
  if (k == 25 || h == 25 || l == 25 || n == 25 || sameS == 25 || sameE == 25 || sameW == 25 || sameN == 25) {
    game_delete(g1);
    game_delete(g2);
    return false;
  }
  // Check that each orientation has a minimum of occurence
  int min_count = (DEFAULT_SIZE * DEFAULT_SIZE) / 4 / 2;
  if (sameN < min_count || sameE < min_count || sameS < min_count || sameW < min_count) {
    game_delete(g1);
    game_delete(g2);
    return false;
  }
  game_delete(g1);
  game_delete(g2);
  return true;
}

/*##################################################################*/

bool test_game_reset_orientation(void) {
  game g = game_default();
  if (!g) {
    return false;
  } else {
    game_reset_orientation(g);
    // Check if every piece of the game have orientation = NORTH
    for (int i = 0; i < DEFAULT_SIZE; i++) {
      for (int j = 0; j < DEFAULT_SIZE; j++) {
        if (game_get_piece_orientation(g, i, j) != NORTH) {
          game_delete(g);
          return false;
        }
      }
    }
    game_delete(g);
    return true;
  }
}

/*##################################################################*/

bool test_game_copy(void) {
  game g = game_default();
  game copy_g = game_copy(g);
  if (!game_equal(g, copy_g, false)) {
    game_delete(copy_g);
    game_delete(g);
    return false;
  }
  if (game_is_wrapping(g)) {
    game_delete(g);
    game_delete(copy_g);
    return false;
  }
  game_delete(copy_g);
  game_delete(g);
  shape shapes_tab_wrapping[25] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                                   TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                                   CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab_wrapping[25] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                             EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                             WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};
  game g_wrapping = game_new_ext(5, 5, shapes_tab_wrapping, orientations_tab_wrapping, true);
  game copy_g_wrapping = game_copy(g_wrapping);
  if (!game_is_wrapping(copy_g_wrapping)) {
    game_delete(copy_g_wrapping);
    game_delete(g_wrapping);
    return false;
  }
  game_delete(copy_g_wrapping);
  game_delete(g_wrapping);
  return true;
}

/*##################################################################*/

bool test_game_set_piece_shape(void) {
  game g = game_default();
  if (!g) {
    return false;
  } else {
    game copy_g = game_copy(g);
    // Test to set a ENDPOINT
    game_set_piece_shape(g, 0, 0, ENDPOINT);
    if (game_get_piece_shape(g, 0, 0) != ENDPOINT) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    // Test to set a SEGMENT
    game_set_piece_shape(g, 0, 0, SEGMENT);
    if (game_get_piece_shape(g, 0, 0) != SEGMENT) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    // Test to set a CORNER
    game_set_piece_shape(g, 0, 0, CORNER);
    if (game_get_piece_shape(g, 0, 0) != CORNER) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    // Test to set a TEE
    game_set_piece_shape(g, 0, 0, TEE);
    if (game_get_piece_shape(g, 0, 0) != TEE) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    // Test to set a EMPTY space
    game_set_piece_shape(g, 0, 0, EMPTY);
    if (game_get_piece_shape(g, 0, 0) != EMPTY) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    // Test to set a CROSS
    game_set_piece_shape(g, 0, 0, CROSS);
    if (game_get_piece_shape(g, 0, 0) != CROSS) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    if (game_get_piece_orientation(g, 0, 0) != game_get_piece_orientation(copy_g, 0, 0)) {
      game_delete(copy_g);
      game_delete(g);
      return false;
    }
    game_set_piece_shape(g, 0, 0, game_get_piece_shape(copy_g, 0, 0));
    if (!game_equal(copy_g, g, false)) {
      game_delete(g);
      game_delete(copy_g);
      return false;
    }
    game_delete(g);
    game_delete(copy_g);
    return true;
  }
}

/*##################################################################*/

bool test_game_equal(void) {
  game g1 = game_default();
  game g2 = game_default();
  game g3 = game_default();
  shape shapes_tab[DEFAULT_SIZE * DEFAULT_SIZE] = {
      CORNER,   ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,    TEE,     TEE,      TEE, TEE,      ENDPOINT, ENDPOINT, TEE,
      ENDPOINT, SEGMENT,  ENDPOINT, TEE,    TEE,      CORNER, SEGMENT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab[DEFAULT_SIZE * DEFAULT_SIZE] = {
      WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST, EAST,  EAST, NORTH, WEST,
      WEST, EAST,  SOUTH, SOUTH, NORTH, WEST,  NORTH, EAST,  WEST, SOUTH, EAST, SOUTH};
  game g4 = game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, shapes_tab, orientations_tab, true);
  game_shuffle_orientation(g1);
  if (game_equal(g1, g2, false) != false) {  // Check if g1 (shuffled) is equal to g2
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    return false;
  }
  if (game_equal(g2, g4, false) != false) {  // Check if g2 and g4 (different of g1,g2 and g3) are equal
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    return false;
  }
  if (game_equal(g2, g3, false) != true) {  // Check if g2 and g3 (with g2=g3) are equal
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    return false;
  }
  if (game_equal(g1, g2, true) != true) {  // Check if g1 (shuffled) and g2 are equal if we ignore orientation
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    return false;
  }
  game_set_piece_shape(g1, 0, 0, EMPTY);
  if (game_equal(g1, g2, true) != false) {  // Check if g1 (shuffled) and g2 are equal if we ignore orientation and we
                                            // change the shape of the first cell
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    return false;
  }
  if (game_equal(g2, g3, true) != true) {  // Check if g2 and g3 (with g2=g3) are equal if we ignore orientation
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    return false;
  }
  game_set_piece_orientation(g2, 0, 0, NORTH);
  game_set_piece_orientation(g3, 0, 0, NORTH);
  game_set_piece_shape(g2, 0, 0, CORNER);
  game_set_piece_shape(g3, 0, 0, CORNER);
  if (game_equal(g2, g3, false) !=
      true) {  // Check if g2 and g3 (with g2=g3) are equal if we change the orientation and the shape in both games
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    return false;
  }
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  return true;
}

/*##################################################################*/

bool test_game_get_piece_shape(void) {
  game g = game_default();
  for (int q = 0; q < DEFAULT_SIZE; q++) {
    for (int s = 0; s < DEFAULT_SIZE; s++) {
      // Check if no cell are EMPTY
      if (game_get_piece_shape(g, q, s) == EMPTY) {
        game_delete(g);
        return false;
      }
    }
  }
  // Check if the fonction game_get_piece_shape return the correct shape
  game_set_piece_shape(g, 0, 0, ENDPOINT);
  if (game_get_piece_shape(g, 0, 0) != ENDPOINT) {
    game_delete(g);
    return false;
  }
  game_set_piece_shape(g, 0, 0, EMPTY);
  if (game_get_piece_shape(g, 0, 0) != EMPTY) {
    game_delete(g);
    return false;
  }
  game_set_piece_shape(g, 0, 0, CORNER);
  game_set_piece_shape(g, 4, 4, CORNER);
  if (game_get_piece_shape(g, 0, 0) != CORNER || game_get_piece_shape(g, 4, 4) != CORNER) {
    game_delete(g);
    return false;
  }
  game_shuffle_orientation(g);
  if (game_get_piece_shape(g, 0, 0) != CORNER || game_get_piece_shape(g, 4, 4) != CORNER) {
    game_delete(g);
    return false;
  }
  game_set_piece_shape(g, 0, 0, SEGMENT);
  if (game_get_piece_shape(g, 0, 0) != SEGMENT) {
    game_delete(g);
    return false;
  }
  game_set_piece_shape(g, 0, 0, TEE);
  if (game_get_piece_shape(g, 0, 0) != TEE) {
    game_delete(g);
    return false;
  }
  game_delete(g);
  shape shapes_game_multi[25] = {CORNER, SEGMENT, CORNER,   EMPTY, EMPTY,  CORNER, TEE,   CORNER, EMPTY,
                                 EMPTY,  EMPTY,   ENDPOINT, EMPTY, CORNER, CORNER, EMPTY, EMPTY,  ENDPOINT,
                                 TEE,    SEGMENT, EMPTY,    EMPTY, EMPTY,  CORNER, CORNER};
  direction direction_game_multi[25] = {EAST,  EAST,  SOUTH, NORTH, NORTH, NORTH, SOUTH, WEST,  NORTH,
                                        NORTH, NORTH, NORTH, NORTH, EAST,  SOUTH, NORTH, NORTH, EAST,
                                        WEST,  SOUTH, NORTH, NORTH, NORTH, NORTH, WEST};
  game game_multi = game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, shapes_game_multi, direction_game_multi, false);
  for (int j = 0; j < DEFAULT_SIZE; j++) {
    for (int z = 0; z < DEFAULT_SIZE; z++) {
      if (game_get_piece_shape(game_multi, z, j) !=
          shapes_game_multi[DEFAULT_SIZE * z +
                            j]) {  // Check if the function return the correct shape from the shapes_tab array
        game_delete(game_multi);
        return false;
      }
    }
  }
  game_delete(game_multi);
  game g3 = game_default();
  game_set_piece_shape(g3, 0, 0, ENDPOINT);
  game_set_piece_shape(g3, DEFAULT_SIZE - 1, 0, CORNER);
  game_set_piece_shape(g3, 0, DEFAULT_SIZE - 1, SEGMENT);
  game_set_piece_shape(g3, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, TEE);
  // Check if the function work for differents cells of the game
  if (game_get_piece_shape(g3, 0, 0) != ENDPOINT || game_get_piece_shape(g3, DEFAULT_SIZE - 1, 0) != CORNER ||
      game_get_piece_shape(g3, 0, DEFAULT_SIZE - 1) != SEGMENT ||
      game_get_piece_shape(g3, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1) != TEE) {
    game_delete(g3);
    return false;
  }
  game_delete(g3);
  return true;
}

/*##################################################################*/

bool test_game_get_piece_orientation(void) {
  game g = game_default();
  // Check if the function game_get_piece_orientation return the correct orientation
  game_set_piece_orientation(g, 0, 0, EAST);
  if (game_get_piece_orientation(g, 0, 0) != EAST) {
    game_delete(g);
    return false;
  }
  game_set_piece_orientation(g, 0, 0, NORTH);
  if (game_get_piece_orientation(g, 0, 0) != NORTH) {
    game_delete(g);
    return false;
  }
  game_set_piece_orientation(g, 0, 0, WEST);
  if (game_get_piece_orientation(g, 0, 0) != WEST) {
    game_delete(g);
    return false;
  }
  game_set_piece_orientation(g, 0, 0, SOUTH);
  if (game_get_piece_orientation(g, 0, 0) != SOUTH) {
    game_delete(g);
    return false;
  }
  game_delete(g);
  game g3 = game_default();
  game_set_piece_orientation(g3, 0, 0, NORTH);
  game_set_piece_orientation(g3, DEFAULT_SIZE - 1, 0, SOUTH);
  game_set_piece_orientation(g3, 0, DEFAULT_SIZE - 1, EAST);
  game_set_piece_orientation(g3, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, WEST);
  // Check if the function work for differents cells of the game
  if (game_get_piece_orientation(g3, 0, 0) != NORTH || game_get_piece_orientation(g3, DEFAULT_SIZE - 1, 0) != SOUTH ||
      game_get_piece_orientation(g3, 0, DEFAULT_SIZE - 1) != EAST ||
      game_get_piece_orientation(g3, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1) != WEST) {
    game_delete(g3);
    return false;
  }
  game_delete(g3);
  return true;
}

/*##################################################################*/

bool test_game_default(void) {
  game g = game_default();
  shape shapes_tab[25] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                          TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                          CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab[25] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                    EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                    WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      printf("%d == %d et %d == %d\n", game_get_piece_shape(g, j, i), shapes_tab[DEFAULT_SIZE * i + j],
             game_get_piece_orientation(g, j, i), orientations_tab[DEFAULT_SIZE * i + j]);
      if (game_get_piece_shape(g, i, j) != shapes_tab[DEFAULT_SIZE * i + j] ||
          game_get_piece_orientation(g, i, j) != orientations_tab[DEFAULT_SIZE * i + j]) {
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);
  return true;
}

/*##################################################################*/

bool test_game_new_empty() {
  game g = game_new_empty();
  shape tab[25] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
  direction tab2[25] = {NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                        NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH};
  game g_em_cus = game_new(tab, tab2);
  // Check if a manual empty game is equal to g, user of game_new_empty
  if (game_equal(g, g_em_cus, false) != true) {
    game_delete(g);
    game_delete(g_em_cus);
    return false;
  }
  game_delete(g_em_cus);
  for (int j = 0; j < DEFAULT_SIZE; j++) {
    for (int i = 0; i < DEFAULT_SIZE; i++) {
      game g2 = game_new_empty();
      // Check if every cells are EMPTY, with NORTH orientation
      if (game_get_piece_orientation(g, i, j) != NORTH || game_get_piece_shape(g, i, j) != EMPTY ||
          game_equal(g, g2, false) != true) {
        game_delete(g);
        game_delete(g2);
        return false;
      }
      game_delete(g2);
    }
  }
  game_delete(g);
  return true;
}

/*##################################################################*/

bool test_game_new() {
  shape shapes_tab[25] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                          TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                          CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab[25] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                    EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                    WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};
  shape shapes1_tab[25] = {CORNER, ENDPOINT, EMPTY,    CORNER, ENDPOINT, TEE,      TEE,      TEE, EMPTY,
                           TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, EMPTY,
                           CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations1_tab[25] = {EAST,  NORTH, WEST,  NORTH, SOUTH, EAST, WEST,  NORTH, EAST,
                                     EAST,  EAST,  NORTH, SOUTH, WEST,  EAST, SOUTH, SOUTH, NORTH,
                                     SOUTH, NORTH, EAST,  WEST,  SOUTH, EAST, EAST};
  game g1 = game_new(shapes1_tab, orientations1_tab);
  game g = game_new(shapes_tab, orientations_tab);
  if (g == NULL || g1 == NULL) {
    game_delete(g);
    game_delete(g1);
    return false;
  }
  int i = 0;
  for (int j = 0; j < DEFAULT_SIZE; j++) {
    for (int z = 0; z < DEFAULT_SIZE; z++) {
      // Check if every cells of the two games have the correct shape and orientation comparate to the initial 4 arrays
      if (game_get_piece_orientation(g, j, z) != orientations_tab[i] ||
          game_get_piece_shape(g, j, z) != shapes_tab[i] ||
          game_get_piece_orientation(g1, j, z) != orientations1_tab[i] ||
          game_get_piece_shape(g1, j, z) != shapes1_tab[i]) {
        game_delete(g1);
        game_delete(g);
        return false;
      }
      i++;
    }
  }
  game_delete(g);
  game_delete(g1);
  return true;
}

/*##################################################################*/

bool test_game_get_ajacent_square() {
  shape shapes_tab[25] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                          TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                          CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab[25] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                    EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                    WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};
  game g = game_new(shapes_tab, orientations_tab);
  // Allocate memory to store the coordinates of the adjacent square
  uint* pi_next = malloc(sizeof(uint));
  uint* pj_next = malloc(sizeof(uint));
  // Test on bottom-right corner cell, with no adjacent square in south direction
  if (game_get_ajacent_square(g, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, SOUTH, pi_next, pj_next) != false) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  // Check that coordinates are not modified
  if (*pi_next != 0 || *pj_next != 0) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  // Test on Top-left corner cell, with no adjacent square in west direction
  if (game_get_ajacent_square(g, 0, 0, WEST, pi_next, pj_next) != false) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  if (*pi_next != 0 || *pj_next != 0) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  // Test on bottom-right corner cell, with adjacent square in north direction
  if (game_get_ajacent_square(g, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, NORTH, pi_next, pj_next) != true) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  // Check if pi_next and pj_next have the correct value
  if (*pi_next != DEFAULT_SIZE - 2 || *pj_next != DEFAULT_SIZE - 1) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  // Test on Top-left corner cell, with adjacent square in east direction
  if (game_get_ajacent_square(g, 0, 0, EAST, pi_next, pj_next) != true) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  // Check if pi_next and pj_next have the correct value
  if (*pi_next != 0 || *pj_next != 1) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  // Test on Top-left corner cell, with no adjacent square in north direction
  if (game_get_ajacent_square(g, 0, 0, NORTH, pi_next, pj_next) != false) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  // Check if coordinates are not modified
  if (*pi_next != 0 || *pj_next != 1) {
    free(pi_next);
    free(pj_next);
    game_delete(g);
    return false;
  }
  free(pi_next);
  free(pj_next);
  game_delete(g);
  // Test of the function in wrapping mode
  shape shapes_tab_wrapping[25] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                                   TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                                   CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab_wrapping[25] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                             EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                             WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};
  game g_wrapping = game_new_ext(5, 5, shapes_tab_wrapping, orientations_tab_wrapping, true);
  uint* pi_next_wrapping = malloc(sizeof(uint));
  uint* pj_next_wrapping = malloc(sizeof(uint));
  // Test on top-left corner cell, with adjacent square in west direction (wrapping option on)
  if (game_get_ajacent_square(g_wrapping, 0, 0, WEST, pi_next_wrapping, pj_next_wrapping) != true) {
    free(pi_next_wrapping);
    free(pj_next_wrapping);
    game_delete(g_wrapping);
    return false;
  }
  // Check if the pi_next_wrapping and pj_next_wrapping values are correct
  if (*pi_next_wrapping != 0 || *pj_next_wrapping != 4) {
    free(pi_next_wrapping);
    free(pj_next_wrapping);
    game_delete(g_wrapping);
    return false;
  }
  free(pi_next_wrapping);
  free(pj_next_wrapping);
  game_delete(g_wrapping);
  return true;
}

/*##################################################################*/

bool test_game_delete() {
  game g = game_new_empty();
  game_delete(g);
  return true;
}

bool test_game_check_edge() {
  shape shapes_tab_wrapping[25] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                                   TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                                   CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab_wrapping[25] = {EAST, WEST,  WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                             EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                             WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};
  game g3 = game_new(shapes_tab_wrapping, orientations_tab_wrapping);
  // Check game_check_edge for a specific cells and orientation ((0,0) and EAST)
  if (game_check_edge(g3, 0, 0, EAST) != MATCH) {
    game_delete(g3);
    return false;
  }
  game_delete(g3);
  game g = game_default();
  for (int i = 0; i < DEFAULT_SIZE - 1; i++) {
    for (int j = 0; j < DEFAULT_SIZE - 1; j++) {
      // Check vertical edges (SOUTH and NORTH)
      if (game_has_half_edge(g, i, j, SOUTH) == true && game_has_half_edge(g, i + 1, j, NORTH) == true) {
        // Check if the function game_chek_eges return the correct answer : MATCH
        if (game_check_edge(g, i, j, SOUTH) != MATCH) {
          game_delete(g);
          return false;
        }
      } else if (game_has_half_edge(g, i, j, SOUTH) == false && game_has_half_edge(g, i + 1, j, NORTH) == false) {
        // Check if the function game_check_edge return the correct answer : NOEDGE (half-edge are false for both)
        if (game_check_edge(g, i, j, SOUTH) != NOEDGE) {
          game_delete(g);
          return false;
        }
      } else {
        // The only correct solution after last tests is that (i,j) with South direction is a mismatch
        if (game_check_edge(g, i, j, SOUTH) != MISMATCH) {
          game_delete(g);
          return false;
        }
      }
      // Check horizontal edges (EAST and WEST)
      if (game_has_half_edge(g, i, j, EAST) == true && game_has_half_edge(g, i, j + 1, WEST) == true) {
        // Check if the function game_chek_eges return the correct answer : MATCH
        if (game_check_edge(g, i, j, EAST) != MATCH) {
          game_delete(g);
          return false;
        }
      }  // Check if the function game_check_edge return the correct answer : NOEDGE (half-edge are false for both)
      else if (game_has_half_edge(g, i, j, EAST) == false && game_has_half_edge(g, i, j + 1, WEST) == false) {
        if (game_check_edge(g, i, j, EAST) != NOEDGE) {
          game_delete(g);
          return false;
        }
      } else {
        // The only correct solution after last tests is that (i,j) with East direction is a mismatch
        if (game_check_edge(g, i, j, EAST) != MISMATCH) {
          game_delete(g);
          return false;
        }
      }
    }
  }
  // Check of the edge status of the bottom-right cell with west direction (if the function work = mismatch)
  if (game_check_edge(g, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, WEST) != MISMATCH) {
    game_delete(g);
    return false;
  }
  game_delete(g);
  return true;
}

/*##################################################################*/

bool test_game_is_connected() {
  game g = game_default();
  game g_solution = game_default_solution();
  if (game_is_connected(g) != false) {  // The default game is not suppose to be connected
    game_delete(g);
    return false;
  }
  game_delete(g);
  if (game_is_connected(g_solution) == false) {  // The solution game has to be connected
    game_delete(g_solution);
    return false;
  }
  game_delete(g_solution);
  game g_new = game_new_empty();
  if (game_is_connected(g_new) != true) {  // A empty game is logically connected
    game_delete(g_new);
    return false;
  }
  game_delete(g_new);
  // Check if the function work with the wrapping mode
  shape shapes_tab_wrap[25] = {CROSS, CROSS, CROSS, CROSS, CROSS, CROSS, CROSS, CROSS, CROSS,
                               CROSS, CROSS, CROSS, CROSS, CROSS, CROSS, CROSS, CROSS, CROSS,
                               CROSS, CROSS, CROSS, CROSS, CROSS, CROSS, CROSS};
  direction orientation_tab_wrap[25] = {NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                        NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                        NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH};
  game g_wrap_is_on = game_new_ext(5, 5, shapes_tab_wrap, orientation_tab_wrap, true);
  if (game_is_connected(g_wrap_is_on) != true) {  // g_wrap_is_on should be connected because there are only CROSS
                                                  // shapes
    game_delete(g_wrap_is_on);
    return false;
  }
  game_delete(g_wrap_is_on);
  // Check on a specific disconnected game
  shape shapes_tab[25] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                          TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                          CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab[25] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                    EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                    WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};
  game g_new_so = game_new(shapes_tab, orientations_tab);
  if (game_is_connected(g_new_so) != false) {  // This specific configuration is expected to be disconnected
    game_delete(g_new_so);
    return false;
  }
  game_delete(g_new_so);
  // Check on a random game with possible connectivity mismatch
  direction orientation[25] = {SOUTH, NORTH, WEST,  EAST, SOUTH, WEST, NORTH, EAST, SOUTH, EAST,  NORTH, WEST, EAST,
                               SOUTH, WEST,  NORTH, EAST, SOUTH, EAST, NORTH, WEST, EAST,  SOUTH, WEST,  NORTH};
  shape dimension[25] = {TEE, SEGMENT, ENDPOINT, CORNER,   TEE,    SEGMENT,  SEGMENT, ENDPOINT, CORNER,
                         TEE, SEGMENT, CORNER,   ENDPOINT, CORNER, TEE,      SEGMENT, ENDPOINT, CORNER,
                         TEE, SEGMENT, ENDPOINT, CORNER,   TEE,    ENDPOINT, CORNER};
  game game_random = game_new(dimension, orientation);
  if (game_is_connected(game_random) == true && game_is_well_paired(game_random) == false) {
    game_delete(game_random);
    return false;
  }
  if (game_is_connected(game_random) == false && game_is_well_paired(game_random) == true) {
    game_delete(game_random);
    return false;
  }
  game_delete(game_random);
  // Check on a connected game with EMPTY shapes
  shape shapes_tab2[25] = {EMPTY,  EMPTY, EMPTY,   EMPTY, EMPTY,   EMPTY, CORNER, SEGMENT, CORNER,
                           EMPTY,  EMPTY, SEGMENT, EMPTY, SEGMENT, EMPTY, EMPTY,  CORNER,  SEGMENT,
                           CORNER, EMPTY, EMPTY,   EMPTY, EMPTY,   EMPTY, EMPTY};
  direction orientations_tab2[25] = {NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, EAST,  EAST,  SOUTH,
                                     NORTH, NORTH, SOUTH, NORTH, SOUTH, NORTH, NORTH, NORTH, EAST,
                                     WEST,  NORTH, NORTH, NORTH, NORTH, NORTH, NORTH};
  game game_new2 = game_new_ext(5, 5, shapes_tab2, orientations_tab2, false);
  game_print(game_new2);
  if (game_is_connected(game_new2) != true) {
    game_delete(game_new2);
    return false;
  }
  // Check on a connected game with a move add
  game_play_move(game_new2, 1, 2, 1);
  game_print(game_new2);
  if (game_is_connected(game_new2) != false) {  // If the cell moved isn't a CROSS, the game should be disconnected
    game_delete(game_new2);
    return false;
  }
  game_delete(game_new2);
  // Check on a multi-cell diconnected game
  shape shapes_game_multi[25] = {CORNER, SEGMENT, CORNER,   EMPTY, EMPTY,  CORNER, TEE,   CORNER, EMPTY,
                                 EMPTY,  EMPTY,   ENDPOINT, EMPTY, CORNER, CORNER, EMPTY, EMPTY,  ENDPOINT,
                                 TEE,    SEGMENT, EMPTY,    EMPTY, EMPTY,  CORNER, CORNER};
  direction direction_game_multi[25] = {EAST,  EAST,  SOUTH, NORTH, NORTH, NORTH, SOUTH, WEST,  NORTH,
                                        NORTH, NORTH, NORTH, NORTH, EAST,  SOUTH, NORTH, NORTH, EAST,
                                        WEST,  SOUTH, NORTH, NORTH, NORTH, NORTH, WEST};
  game game_multi = game_new(shapes_game_multi, direction_game_multi);
  if (game_is_connected(game_multi) != false) {  // The game should be disconnected
    game_delete(game_multi);
    return false;
  }
  game_delete(game_multi);
  return true;
}

/*##################################################################*/

bool test_game_won() {
  game g = game_default();
  game g_solution = game_default_solution();
  shape shapes_tab[25] = {SEGMENT, EMPTY,    ENDPOINT, SEGMENT, ENDPOINT, SEGMENT,  CORNER,   ENDPOINT, ENDPOINT,
                          EMPTY,   ENDPOINT, ENDPOINT, TEE,     ENDPOINT, SEGMENT,  ENDPOINT, TEE,      TEE,
                          CORNER,  SEGMENT,  ENDPOINT, TEE,     ENDPOINT, ENDPOINT, EMPTY};
  direction orientations_tab[25] = {SOUTH, SOUTH, EAST, EAST,  SOUTH, SOUTH, WEST, SOUTH, NORTH,
                                    SOUTH, WEST,  EAST, SOUTH, NORTH, NORTH, EAST, WEST,  EAST,
                                    WEST,  NORTH, EAST, WEST,  SOUTH, EAST,  SOUTH};
  game g_new = game_new_ext(5, 5, shapes_tab, orientations_tab, false);
  game g_empty = game_new_empty_ext(5, 5, false);
  // Check that a empty game has to be considerate as won and the custom one (g_new) is not
  if (game_won(g_empty) != true || game_won(g_new) != false) {
    game_delete(g);
    game_delete(g_empty);
    game_delete(g_new);
    game_delete(g_solution);
    return false;
  }
  // g a game_default has to be not won and g_solution (the game default solution) as to
  if (game_won(g) != false || game_won(g_solution) != true) {
    game_delete(g);
    game_delete(g_empty);
    game_delete(g_new);
    game_delete(g_solution);
    return false;
  }
  // Verify the conditions of winning states
  if (game_is_well_paired(g_solution) == true && game_is_connected(g_solution) == true &&
      game_won(g_solution) == false) {  // A well-paired and connected game should be considered as a won game
    game_delete(g);
    game_delete(g_empty);
    game_delete(g_new);
    game_delete(g_solution);
    return false;
  }
  game_play_move(g_solution, 0, 0, 1);
  if (game_won(g_solution) ==
      true) {  // After a move in the default solution, the game should be no more considered as won
    game_delete(g);
    game_delete(g_empty);
    game_delete(g_new);
    game_delete(g_solution);
    return false;
  }
  game_delete(g);
  game_delete(g_solution);
  game_delete(g_new);
  game_delete(g_empty);
  return true;
}

/*##################################################################*/

bool test_game_has_half_edge() {
  game g = game_default();
  for (int i = 0; i < DEFAULT_SIZE - 1; i++) {
    for (int j = 0; j < DEFAULT_SIZE - 1; j++) {
      // Check if there are a match for (i,j) with SOUTH direction
      if (game_check_edge(g, i, j, SOUTH) == MATCH) {
        // (i,j) with south direction and (i+1,j) with north direction have to both return true to game_has_half_edge
        if (game_has_half_edge(g, i, j, SOUTH) != true && game_has_half_edge(g, i + 1, j, NORTH) != true) {
          game_delete(g);
          return false;
        }
      } else if (game_check_edge(g, i, j, SOUTH) == MISMATCH) {
        if (game_has_half_edge(g, i, j, SOUTH) == true && game_has_half_edge(g, i + 1, j, NORTH) == true) {
          // we have here a match and not a mismatch
          game_delete(g);
          return false;
        }
      } else if (game_check_edge(g, i, j, SOUTH) == NOEDGE) {
        if ((game_has_half_edge(g, i, j, SOUTH) == true && game_has_half_edge(g, i + 1, j, NORTH) == false) ||
            (game_has_half_edge(g, i, j, SOUTH) == false && game_has_half_edge(g, i + 1, j, NORTH) == true)) {
          // We have here a mismatch and not a noedge
          game_delete(g);
          return false;
        }
      }
      if (game_check_edge(g, i, j, EAST) == MATCH) {
        if (game_has_half_edge(g, i, j, EAST) != true && game_has_half_edge(g, i, j + 1, WEST) != true) {
          // We have here a Noedge and not a match
          game_delete(g);
          return false;
        }
      } else if (game_check_edge(g, i, j, EAST) == MISMATCH) {
        if (game_has_half_edge(g, i, j, EAST) == true && game_has_half_edge(g, i, j + 1, WEST) == true) {
          // We have here a match and not a mismatch
          game_delete(g);
          return false;
        }
      } else if (game_check_edge(g, i, j, EAST) == NOEDGE) {
        if ((game_has_half_edge(g, i, j, EAST) == true && game_has_half_edge(g, i, j + 1, WEST) == false) ||
            (game_has_half_edge(g, i, j, EAST) == false && game_has_half_edge(g, i, j + 1, WEST) == true)) {
          // We have here a mismatch and not a noedge
          game_delete(g);
          return false;
        }
      }
      game_set_piece_shape(g, 2, 2, CROSS);

      if (!game_has_half_edge(g, 2, 2, NORTH) || !game_has_half_edge(g, 2, 2, EAST) ||
          !game_has_half_edge(g, 2, 2, SOUTH) ||
          !game_has_half_edge(g, 2, 2, WEST)) {  // Every direction of a CROSS is a half edge
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);
  return true;
}

/*##################################################################*/

bool test_game_print() {
  game g = game_default();
  game_print(g);
  game_delete(g);
  game g2 = game_default_solution();
  game_print(g2);
  game_delete(g2);
  return true;
}

/*##################################################################*/

bool test_default_solution() {
  game g_solution = game_default_solution();
  if (game_won(g_solution) != true) {  // The solution have bring all winning states
    game_delete(g_solution);
    return false;
  }
  game_delete(g_solution);
  return true;
}

/*##################################################################*/

bool test_undo() {
  shape shapes_tab[25] = {ENDPOINT, SEGMENT,  CORNER,   TEE,     CROSS,  CROSS, TEE,      CORNER,  SEGMENT,
                          ENDPOINT, ENDPOINT, SEGMENT,  CORNER,  TEE,    CROSS, ENDPOINT, SEGMENT, CORNER,
                          TEE,      CROSS,    ENDPOINT, SEGMENT, CORNER, TEE,   CROSS};
  direction orientation_tab[25] = {EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST,
                                   EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST, EAST};
  game g1 = game_new_ext(5, 5, shapes_tab, orientation_tab, false);
  game g2 = game_new_ext(5, 5, shapes_tab, orientation_tab, false);
  game_play_move(g1, 2, 2, 1);
  if (game_equal(g1, g2, false)) {  // g1 is g2 with another move on (2,2)
    game_delete(g1);
    game_delete(g2);
    return false;
  }
  game_undo(g1);
  if (!game_equal(g1, g2, false)) {  // Removing the move on (2,2), g1=g1
    game_delete(g1);
    game_delete(g2);
    return false;
  }
  game_undo(g1);                     // We had made just one move and redo it, so g1 stay the same
  if (!game_equal(g1, g2, false)) {  // if the funtion work, g1=g2
    game_delete(g1);
    game_delete(g2);
    return false;
  }
  game_delete(g1);
  game_delete(g2);
  return true;
}

/*##################################################################*/

bool test_redo() {
  game g = game_default();
  game copy_g = game_copy(g);
  game_redo(g);
  if (!game_equal(g, copy_g, false)) {  // copy_g is a copy of g, so g = copy_g
    game_delete(g);
    game_delete(copy_g);
    return false;
  }
  game_play_move(g, 0, 0, 1);
  game_play_move(copy_g, 0, 0, 1);
  game_undo(g);
  game_redo(g);
  if (!game_equal(g, copy_g, false)) {  // We undo and redo the move on g so it's as if we did nothing, so g = copy_g
    game_delete(g);
    game_delete(copy_g);
    return false;
  }
  game_play_move(g, 0, 1, 1);
  game_undo(g);
  game_play_move(g, 0, 1, 1);
  game_redo(g);
  game_play_move(copy_g, 0, 1, 1);
  if (!game_equal(g, copy_g, false)) {  // Check if g and copy_g are the same
    game_delete(g);
    game_delete(copy_g);
    return false;
  }
  // We do 4 moves on g
  game_play_move(g, 1, 0, 1);
  game_play_move(g, 1, 1, 1);
  game_play_move(g, 1, 2, 1);
  game_play_move(g, 1, 3, 1);
  // We do 4 moves on copy_g like in g
  game_play_move(copy_g, 1, 0, 1);
  game_play_move(copy_g, 1, 1, 1);
  game_play_move(copy_g, 1, 2, 1);
  game_play_move(copy_g, 1, 3, 1);
  // We undo the 4 moves on g
  game_undo(g);
  game_undo(g);
  game_undo(g);
  game_undo(g);
  // We redo the 4 moves on g
  game_redo(g);
  game_redo(g);
  game_redo(g);
  game_redo(g);
  // We check that copy_g and g are the same
  if (!game_equal(g, copy_g, false)) {
    game_delete(g);
    game_delete(copy_g);
    return false;
  }
  game_delete(g);
  game_delete(copy_g);
  return true;
}

/*##################################################################*/

bool test_game_new_empty_ext(void) {
  uint nb_rows = 2;
  uint nb_cols = 8;
  bool wrapping = false;
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (g == NULL) {
    return false;
  }
  // Check if the function correctly keep the number of rows and columns
  if (game_nb_rows(g) != nb_rows || game_nb_cols(g) != nb_cols) {
    game_delete(g);
    return false;
  }
  // Check if the function correctly keep the boolean of the wrapping mode
  if (game_is_wrapping(g) != wrapping) {
    game_delete(g);
    return false;
  }
  // Check if the function asign correctly EMPTY shapes to all shapes_tab[i] and NORTH to every orientation_tab[i]
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY || game_get_piece_orientation(g, i, j) != NORTH) {
        game_delete(g);
        return false;
      }
    }
  }

  game_delete(g);
  return true;
}

/*##################################################################*/

bool test_game_new_ext() {
  shape shapes_tab[25] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                          TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                          CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction orientations_tab[25] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                    EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                    WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};
  game g = game_new_ext(5, 5, shapes_tab, orientations_tab, false);
  if (g == NULL) {
    game_delete(g);
    return false;
  }
  if (game_is_wrapping(g)) {  // Check if the function correctly keep the boolean of the wrapping mode
    game_delete(g);
    return false;
  }
  // Check if the function correctly keep the number of rows and columns
  if (game_nb_cols(g) != 5 || game_nb_rows(g) != 5) {
    game_delete(g);
    return false;
  }
  int i = 0;
  for (int j = 0; j < 5; j++) {
    for (int z = 0; z < 5; z++) {
      // Check if the function asign correctly shapes from the array to (game)shapes_tab and orientation array to
      // (game)orientation_tab
      if (game_get_piece_orientation(g, j, z) != orientations_tab[i] ||
          game_get_piece_shape(g, j, z) != shapes_tab[i]) {
        game_delete(g);
        return false;
      }
      i++;
    }
  }
  game_delete(g);
  // Check for wrapping mode
  shape shapes_tab1[6] = {CROSS, CORNER, EMPTY, TEE, SEGMENT, CROSS};
  direction orientations_tab1[6] = {NORTH, WEST, NORTH, WEST, NORTH, WEST};
  game g1 = game_new_ext(4, 2, shapes_tab1, orientations_tab1, true);
  if (!game_is_wrapping(g1)) {
    game_delete(g1);
    return false;
  }
  if (game_nb_cols(g1) != 2 || game_nb_rows(g1) != 4) {
    game_delete(g1);
    return false;
  }
  i = 0;
  for (int j = 0; j < 4; j++) {
    for (int z = 0; z < 2; z++) {
      if (game_get_piece_orientation(g1, j, z) != orientations_tab1[i] ||
          game_get_piece_shape(g1, j, z) != shapes_tab1[i]) {
        game_delete(g1);
        return false;
      }
      i++;
    }
  }
  game_delete(g1);
  return true;
}

/*##################################################################*/

bool test_nb_cols() {
  game g1 = game_new_ext(5, 10, NULL, NULL, false);  // columns = 10
  if (game_nb_cols(g1) != 10) {
    game_delete(g1);
    return false;
  }
  game_delete(g1);
  game g2 = game_new_ext(5, 1, NULL, NULL, false);  // columns = 1
  if (game_nb_cols(g2) != 1) {
    game_delete(g2);
    return false;
  }
  game_delete(g2);
  return true;
}

/*##################################################################*/

bool test_nb_rows() {
  game g1 = game_new_ext(5, 10, NULL, NULL, false);  // rows = 5
  if (game_nb_rows(g1) != 5) {
    game_delete(g1);
    return false;
  }
  game_delete(g1);
  game g2 = game_new_ext(1, 5, NULL, NULL, false);  // rows = 1
  if (game_nb_rows(g2) != 1) {
    game_delete(g2);
    return false;
  }
  game_delete(g2);
  return true;
}

/*##################################################################*/

bool test_game_is_wrapping() {
  game g1 = game_new_empty_ext(5, 4, false);
  if (game_is_wrapping(g1) == true) {  // g1 has the boolean of wrapping mode = false
    game_delete(g1);
    return false;
  }
  game_delete(g1);
  game g2 = game_new_empty_ext(1, 5, true);
  if (game_is_wrapping(g2) == false) {  // g2 has the boolean of wrapping mode = true
    game_delete(g2);
    return false;
  }
  game_delete(g2);
  return true;
}
/*##################################################################*/

bool test_game_load() {
  // Check the default game

  FILE* f_default = fopen("./default_test", "w");
  fprintf(f_default, "5 5 0\nCW NN NW CN NS\nTS TW TN TE TE\nNE NN TW NW SE\nNS TS TN CW SN\n NE TW NS NE NS\n");
  fclose(f_default);
  game g_file_default = game_load("./default_test");
  game g_default = game_default();
  if (!game_equal(g_file_default, g_default, false)) {
    game_delete(g_file_default);
    game_delete(g_default);
    remove("./default_test");
    return false;
  }
  remove("./default_test");
  game_delete(g_file_default);
  game_delete(g_default);

  // Check the default solution

  FILE* f_default_solution = fopen("./default_solution_test", "w");
  fprintf(f_default_solution,
          "5 5 0\nCE NW NE CS NS\n TE TS TS TN TW\n NN NN TE NW SS\nNE TS TN CS SN\nNE TN NW NN NN\n");
  fclose(f_default_solution);
  game g_file_default_solution = game_load("./default_solution_test");
  game g_default_solution = game_default_solution();
  if (!game_equal(g_file_default_solution, g_default_solution, false)) {
    game_delete(g_file_default_solution);
    game_delete(g_default_solution);
    remove("./default_solution_test");
    return false;
  }
  game_delete(g_file_default_solution);
  game_delete(g_default_solution);
  remove("./default_solution_test");

  // Check a game with CROSS pieces and wrapping

  FILE* f = fopen("./cross_test", "w");
  fprintf(f, "2 2 1\nXN XN\nXN XN\n");
  fclose(f);
  game g_file = game_load("./cross_test");
  shape shape_tab[4] = {CROSS, CROSS, CROSS, CROSS};
  direction orientation_tab[4] = {NORTH, NORTH, NORTH, NORTH};
  game g = game_new_ext(2, 2, shape_tab, orientation_tab, true);
  if (!game_equal(g, g_file, false)) {
    game_delete(g);
    game_delete(g_file);
    remove("./cross_test");
    return false;
  }
  game_delete(g);
  game_delete(g_file);
  remove("./cross_test");

  return true;
}

/*##################################################################*/

bool test_game_save() {
  // test the begining of a default game

  game g_default = game_default();
  game_save(g_default, "./test_default");
  FILE* file_default = fopen("./test_default", "r");
  int nb_rows_default, nb_cols_default, wrap_val_default;
  if (!fscanf(file_default, "%d %d %d\n", &nb_rows_default, &nb_cols_default, &wrap_val_default)) {
    fclose(file_default);
    remove("./test_default");
    game_delete(g_default);
    return false;
  }
  if (nb_rows_default != 5 || nb_cols_default != 5 || wrap_val_default != 0) {
    fclose(file_default);
    remove("./test_default");
    game_delete(g_default);
    return false;
  }
  char shape1, shape2, orientation1, orientation2;
  if (!fscanf(file_default, "%c%c %c%c", &shape1, &orientation1, &shape2, &orientation2)) {
    fclose(file_default);
    remove("./test_default");
    game_delete(g_default);
    return false;
  }
  if (shape1 != 'C' || orientation1 != 'W' || shape2 != 'N' || orientation2 != 'N') {
    fclose(file_default);
    remove("./test_default");
    game_delete(g_default);
    return false;
  }
  fclose(file_default);
  remove("./test_default");
  game_delete(g_default);

  // test with all other values

  shape shape_tab[6] = {SEGMENT, CORNER, ENDPOINT, CROSS, TEE, EMPTY};
  direction orientation_tab[6] = {NORTH, EAST, SOUTH, WEST, NORTH, SOUTH};
  game g = game_new_ext(3, 2, shape_tab, orientation_tab, true);
  game_save(g, "./test_all");
  FILE* file = fopen("./test_all", "r");
  int nb_rows, nb_cols, wrap_val;
  if (!fscanf(file, "%d %d %d\n", &nb_rows, &nb_cols, &wrap_val)) {
    fclose(file);
    remove("./test_all");
    game_delete(g);
    return false;
  }
  if (nb_rows != 3 || nb_cols != 2 || wrap_val != 1) {
    fclose(file);
    remove("./test_all");
    game_delete(g);
    return false;
  }
  char s_tab[6] = {'S', 'C', 'N', 'X', 'T', 'E'};
  char o_tab[6] = {'N', 'E', 'S', 'W', 'N', 'S'};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      char shape, orientation;
      if (!fscanf(file, "%c%c ", &shape, &orientation)) {
        fclose(file);
        remove("./test_all");
        game_delete(g);
        return false;
      }
      if (shape != s_tab[GAME2TAB(i, j, g)] || orientation != o_tab[GAME2TAB(i, j, g)]) {
        fclose(file);
        remove("./test_all");
        game_delete(g);
        return false;
      }
    }
  }
  fclose(file);
  remove("./test_all");
  game_delete(g);
  return true;
}

/*##################################################################*/

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Only 1 arguments required !");
    return EXIT_FAILURE;
  } else {
    char* arg = argv[1];
    if (strcmp(arg, "game_shuffle_orientation") == 0) {
      if (test_game_shuffle_orientation()) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_set_piece_orientation") == 0) {
      if (test_game_set_piece_orientation()) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_reset_orientation") == 0) {
      if (test_game_reset_orientation()) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_copy") == 0) {
      if (test_game_copy()) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_default") == 0) {
      if (test_game_default()) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_set_piece_shape") == 0) {
      if (test_game_set_piece_shape()) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_equal") == 0) {
      if (test_game_equal()) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_get_piece_shape") == 0) {
      if (test_game_get_piece_shape()) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_get_piece_orientation") == 0) {
      if (test_game_get_piece_orientation()) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_new_empty") == 0) {
      if (test_game_new_empty() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_new") == 0) {
      if (test_game_new() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_get_ajacent_square") == 0) {
      if (test_game_get_ajacent_square() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_delete") == 0) {
      if (test_game_delete() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_check_edge") == 0) {
      if (test_game_check_edge() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_is_connected") == 0) {
      if (test_game_is_connected() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_won") == 0) {
      if (test_game_won() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_has_half_edge") == 0) {
      if (test_game_has_half_edge() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_print") == 0) {
      if (test_game_print() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_default_solution") == 0) {
      if (test_default_solution() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_play_move") == 0) {
      if (test_game_play_move() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_is_well_paired") == 0) {
      if (test_game_is_well_paired() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "undo") == 0) {
      if (test_undo() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "redo") == 0) {
      if (test_redo() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_new_ext") == 0) {
      if (test_game_new_ext() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_new_ext_empty") == 0) {
      if (test_game_new_empty_ext() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_nb_cols") == 0) {
      if (test_nb_cols() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_nb_rows") == 0) {
      if (test_nb_rows() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_is_wrapping") == 0) {
      if (test_game_is_wrapping() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_load") == 0) {
      if (test_game_load() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    if (strcmp(arg, "game_save") == 0) {
      if (test_game_save() == true) {
        return EXIT_SUCCESS;
      } else {
        return EXIT_FAILURE;
      }
    }
    return EXIT_SUCCESS;
  }
}
