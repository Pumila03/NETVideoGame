#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/*##################################################################*/

int main(int argc, char** argv) {
  game g;
  if (argc == 1) {
    g = game_default();
  } else {
    g = game_load(argv[1]);
  }
  while (!game_won(g)) {
    game_print(g);
    char c;
    scanf(" %c", &c);
    if (c == 'h') {
      printf(
          "h -> Afficher l'aide\nr -> Reinitialiser la grille\nq -> Quitter\nc -> Tourner la pièce dans la case "
          "(<i>,<j>) (clockwise)\na -> Tourner la pièce dans la case (<i>,<j>) (anti-clockwise)\nz -> Revenir en "
          "arrière\ny -> Revenir en avant\n");
    } else if (c == 'r') {
      game_shuffle_orientation(g);
    } else if (c == 'q') {
      printf("shame\n");
      game_delete(g);
      return EXIT_SUCCESS;
    } else if (c == 'y') {
      game_redo(g);
    } else if (c == 'z') {
      game_undo(g);
    } else if (c == 's') {
      char filename[100];
      scanf("%s", filename);
      game_save(g, filename);
    } else if (c == 'c' || c == 'a') {
      int i;
      int j;
      scanf("%d %d", &i, &j);
      if (c == 'c') {
        game_play_move(g, i, j, 1);
      }
      if (c == 'a') {
        game_play_move(g, i, j, -1);
      }
    }
  }
  game_print(g);
  printf("congratulation\n");
  game_delete(g);
  return EXIT_SUCCESS;
}
