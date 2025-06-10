#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/*##################################################################*/

int main(int argc, char** argv) {
  game g;
  if (argc <= 6 || argc > 8 || atoi(argv[6]) < 0 || atoi(argv[6]) > 1) {
    printf(
        "Usage Usage ./game_random <nb_rows> <nb_cols> <wrapping> <nb_empty> <nb_extra> <shuffle> "
        "[<filename>]\nExample: ./game_random 4 4 0 0 0 0 random.sol\n");
    return EXIT_SUCCESS;
  }
  g = game_random(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
  if (atoi(argv[6]) == 1) {
    game_shuffle_orientation(g);
  }
  printf("nb_rows = %d nb_cols = %d wrapping = %d nb_empty = %d nb_extra = %d shuffle = %d\n", atoi(argv[1]),
         atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
  game_print(g);
  if (argc == 8) {
    game_save(g, argv[7]);
  }
  game_delete(g);
  return EXIT_SUCCESS;
}
