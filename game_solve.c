#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/*##################################################################*/

int main(int argc, char** argv) {
  if (argc < 2 || argc > 4) {
    fprintf(stderr, "usage : ./game_solve <option> <input> [<output>]\n");
    exit(EXIT_SUCCESS);
  }
  bool flag;
  if (argc == 3) {
    flag = false;
  } else {
    flag = true;
  }
  game g = game_load(argv[2]);
  printf("Passage de solv en cours...\n");
  bool is_solutionned = game_solve(g);
  if (strcmp(argv[1], "-s") == 0) {
    if (is_solutionned) {
      if (flag == true) {
        game_save(g, argv[3]);
      } else {
        game_print(g);
      }

    } else {
      fprintf(stderr, "Jeu sans solution apparente...\n");
      exit(EXIT_FAILURE);
    }
  } else if (strcmp(argv[1], "-c") == 0) {
    int nb_sol = game_nb_solutions(g);
    if (flag) {
      FILE* fichier = fopen(argv[3], "w");
      fprintf(fichier, "%d", nb_sol);
      fclose(fichier);
    } else {
      printf("%d\n", nb_sol);
    }
  }
  game_delete(g);
  return EXIT_SUCCESS;
}