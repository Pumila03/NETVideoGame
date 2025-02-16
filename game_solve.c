#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/*##################################################################*/

int main(int argc, char** argv) {
  game g = game_load("random");
  game_print(g);
  printf("Passage de solv en cours...\n");
  if(game_solve(g)) {
    game_print(g);
  } else {
    fprintf(stdout, "Jeu sans solution apparente...");
  }
  game_delete(g);
  return EXIT_SUCCESS;
}