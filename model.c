// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts

#ifdef HAVE_AUDIO
#include <SDL_mixer.h>
#endif

#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include "stack.h"

#define ROTATION 90
#define ZOOM 1

// Texture for background
#define BACKGROUND "res/BACKGROUNDV2.png"
#define MENU "res/menu.png"

// Textures for NOT connected pieces
#define EMPTY_T "res/EMPTY.png"
#define ENDPOINT_T "res/ENDPOINT.png"
#define SEGMENT_T "res/SEGMENT.png"
#define TEE_T "res/TEE.png"
#define CORNER_T "res/CORNER.png"
#define CROSS_T "res/CROSS.png"

// Textures for NOT connected pieces
#define ENDPOINT_T_C "res/ENDPOINT_T_C.png"
#define SEGMENT_T_C "res/SEGMENT_T_C.png"
#define TEE_T_C "res/TEE_T_C.png"
#define CORNER_T_C "res/CORNER_T_C.png"
#define CROSS_T_C "res/CROSS_T_C.png"

// Textures to represent the grid
#define BACKGROUND_PIECES "res/background_pieces.png"
#define BORDER_T "res/BORDER.png"
#define CORNER_BORDER_T "res/BORDER_CORNER.png"
#define WRAPPING_ON "res/WRAPPING_ON.png"
#define WRAPPING_OFF "res/WRAPPING_OFF.png"

// Texture for buttons
#define Z_BUTTON "res/Z_PINK_BUTTON.png"
#define Y_BUTTON "res/Y_PINK_BUTTON.png"
#define H_BUTTON "res/H_PINK_BUTTON.png"
#define AUDIO_ON "res/sound_ON.png"
#define AUDIO_OFF "res/sound_OFF.png"

#define Z_PRESSED_BUTTON "res/Z_BLUE_BUTTON.png"
#define Y_PRESSED_BUTTON "res/Y_BLUE_BUTTON.png"
#define H_PRESSED_BUTTON "res/H_BLUE_BUTTON.png"

#define WINFONT "res/LLPIXEL3.ttf"
#define TITLEFONT "res/futur.otf"
#define FONT "res/sutur.otf"
#define FONTSIZE 36
#define TILE_SIZE 40
#define BUTTON_SIZE 60

// Sound effects
#define PLUG_IN "res/Plug-in.wav"
#define PLUG_OUT "res/Plug-out.wav"
#define MUSIC "res/Polarity.mp3"
#define WIN "res/win.wav"
/* **************************************************************** */

typedef enum { NOTHING, EN_COURS, FAIL, SUCCESS } solve_etat;

typedef struct {
  int x, y;
} coord;

typedef int rotation;

typedef enum { NORTH_R = 0, EAST_R = 90, SOUTH_R = 180, WEST_R = 270 } direction_rotation;

typedef enum { NONE, Z, Y, H } buttons;

struct Env_t {
  SDL_Texture *background;
  SDL_Texture *win_text;
  SDL_Texture *background_pieces;
  SDL_Texture *border;
  SDL_Texture *border_corner;
  SDL_Texture *wrapping_on;
  SDL_Texture *wrapping_off;
  SDL_Texture *Z_button;
  SDL_Texture *Y_button;
  SDL_Texture *H_button;
  SDL_Texture *Z_pressed_button;
  SDL_Texture *Y_pressed_button;
  SDL_Texture *H_pressed_button;
  SDL_Texture *menu;
#ifdef HAVE_AUDIO
  SDL_Texture *audio_on;
  SDL_Texture *audio_off;
#endif

  Stack history;
  Stack redo;

  buttons pressed_button;

  bool is_button_pressed;
  bool overlay_help;
  bool overlay_load;
  bool overlay_save;
  bool overlay_exit;
  bool game_already_won;
#ifdef HAVE_AUDIO
  bool audio;
#endif

  SDL_Rect overlay_exit_yes;
  SDL_Rect overlay_exit_no;

  bool input;
  bool keyUp;
  bool playable;

  char inputText[26];

  solve_etat status;

  int window_w;
  int window_h;

#ifdef HAVE_AUDIO
  Mix_Chunk *plug_in;
  Mix_Chunk *plug_out;
  Mix_Music *music;
  Mix_Chunk *win;
#endif

  SDL_Texture **text_tab;

  coord *coord_tab;
  direction_rotation *dir_rot_tab;

  game g;
};

/*##################################################################*/

bool has_half_edge(game g, int i, int j) {
  for (direction d = NORTH; d <= WEST; d++) {
    if (game_check_edge(g, i, j, d) == MATCH) {
      return true;
    }
  }
  return false;
}

/*##################################################################*/

void update_game(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  int nb_rows = game_nb_rows(env->g);
  int nb_cols = game_nb_cols(env->g);

  // Create 3 array to stock every piece of the game grid informations (SDL_texture, coordonate and rotation)
  env->text_tab = malloc(sizeof(SDL_Texture *) * nb_rows * nb_cols);
  env->coord_tab = malloc(sizeof(coord) * nb_rows * nb_cols);
  env->dir_rot_tab = malloc(sizeof(direction_rotation) * nb_cols * nb_rows);

  int offset_i = (env->window_w - nb_cols * TILE_SIZE) / 2;
  int offset_j = (env->window_h - nb_rows * TILE_SIZE) / 2;

  for (int i = 0; i < nb_rows; i++) {
    for (int j = 0; j < nb_cols; j++) {
      // get the shape in the array
      shape s = game_get_piece_shape(env->g, i, j);
      // For each shape case, we put the corresponding IMG in the SDL_texture game array
      switch (s) {
        case EMPTY:
          env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, EMPTY_T);
          break;
        case ENDPOINT:
          // If the (i,j) piece has half edge, we put a green picture, otherwise we put a pink picture in the array
          if (has_half_edge(env->g, i, j)) {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, ENDPOINT_T_C);
          } else {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, ENDPOINT_T);
          }
          break;
        case SEGMENT:
          if (has_half_edge(env->g, i, j)) {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, SEGMENT_T_C);
          } else {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, SEGMENT_T);
          }
          break;
        case TEE:
          if (has_half_edge(env->g, i, j)) {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, TEE_T_C);
          } else {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, TEE_T);
          }
          break;
        case CORNER:
          if (has_half_edge(env->g, i, j)) {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, CORNER_T_C);
          } else {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, CORNER_T);
          }
          break;
        case CROSS:
          if (has_half_edge(env->g, i, j)) {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, CROSS_T_C);
          } else {
            env->text_tab[GAME2TAB(i, j, env->g)] = IMG_LoadTexture(ren, CROSS_T);
          }
          break;
        default:
          break;
      }
      // We put the correct orientation to the dir_rot_tab array for the actual shape
      env->dir_rot_tab[GAME2TAB(i, j, env->g)] = game_get_piece_orientation(env->g, i, j) * 90;
      // Put the coordinate of the of the piece in the window
      env->coord_tab[GAME2TAB(i, j, env->g)] = (coord){offset_i, offset_j};
      offset_i += TILE_SIZE;
    }
    // Set the next coordinate in the array
    offset_i = (env->window_w - nb_cols * TILE_SIZE) / 2;
    offset_j += TILE_SIZE;
  }
}

/*#############################INIT#################################*/

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));

  SDL_GetWindowSize(win, &(env->window_w), &(env->window_h));

  // Load necessary images to print the game
  env->background_pieces = IMG_LoadTexture(ren, BACKGROUND_PIECES);
  env->menu = IMG_LoadTexture(ren, MENU);
  env->border = IMG_LoadTexture(ren, BORDER_T);  // border of the array
  env->border_corner = IMG_LoadTexture(ren, CORNER_BORDER_T);
  env->wrapping_on = IMG_LoadTexture(ren, WRAPPING_ON);            // small button with green light
  env->wrapping_off = IMG_LoadTexture(ren, WRAPPING_OFF);          // small button in black
  env->Z_button = IMG_LoadTexture(ren, Z_BUTTON);                  // pink button to undo
  env->Y_button = IMG_LoadTexture(ren, Y_BUTTON);                  // pink button to redo
  env->H_button = IMG_LoadTexture(ren, H_BUTTON);                  // pink button to access to the help
  env->Z_pressed_button = IMG_LoadTexture(ren, Z_PRESSED_BUTTON);  // pink button to undo
  env->Y_pressed_button = IMG_LoadTexture(ren, Y_PRESSED_BUTTON);  // pink button to redo
  env->H_pressed_button = IMG_LoadTexture(ren, H_PRESSED_BUTTON);  // pink button to access to the help
#ifdef HAVE_AUDIO
  env->audio_on = IMG_LoadTexture(ren, AUDIO_ON);
  env->audio_off = IMG_LoadTexture(ren, AUDIO_OFF);
#endif

  // Set flags used to use or not use game features depending on the circumstances
  env->overlay_help = false;
  env->overlay_save = false;
  env->overlay_exit = false;
  env->input = false;
  env->playable = true;
  env->keyUp = true;
  env->is_button_pressed = false;
  env->game_already_won = false;
  env->status = NOTHING;
#ifdef HAVE_AUDIO
  env->audio = true;
#endif

  // Set stack for undo and redo function
  env->history = asde_stack_create_empty();
  env->redo = asde_stack_create_empty();

#ifdef HAVE_AUDIO
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    printf("Erreur SDL_mixer: %s\n", Mix_GetError());
  }

  env->plug_in = Mix_LoadWAV(PLUG_IN);
  if (!env->plug_in) ERROR("Mix_LoadWAV: %s\n", PLUG_IN);
  env->plug_out = Mix_LoadWAV(PLUG_OUT);
  if (!env->plug_out) ERROR("Mix_LoadWAV: %s\n", PLUG_OUT);
  env->music = Mix_LoadMUS(MUSIC);
  if (!env->music) ERROR("Mix_LoadMUS: %s\n", MUSIC);
  env->win = Mix_LoadWAV(WIN);
  if (!env->win) ERROR("Mix_LoadMUS: %s\n", WIN);
  Mix_PlayMusic(env->music, -1);
#endif

  // if we want to load a file for a specific game
  if (argc == 1) {
    env->g = game_default();
  } else if (argc == 2) {
    env->g = game_load(argv[1]);
  } else {
    fprintf(stderr, "Usage: ./game_sdl <game>\n");
    exit(EXIT_FAILURE);
  }

  // set the game
  update_game(win, ren, env);
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture: %s\n", BACKGROUND);

  SDL_Color win_color = {173, 216, 230, 0};  // Rouge pour la victoire
  TTF_Font *win_font = TTF_OpenFont(WINFONT, FONTSIZE);
  if (!win_font) {
    fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }
  // Print the victory text
  SDL_Surface *win_surf = TTF_RenderText_Blended(win_font, "YOU WIN", win_color);
  if (!win_surf) {
    fprintf(stderr, "Erreur création surface : %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }
  env->win_text = SDL_CreateTextureFromSurface(ren, win_surf);
  SDL_FreeSurface(win_surf);
  TTF_CloseFont(win_font);

  return env;  // Return the all environment
}

/*##################################################################*/
// Fonction to render text as an SDL_Texture
SDL_Texture *renderText(const char *text, SDL_Color color, SDL_Renderer *ren, int size, const char *font) {
  // Load the font with a specific size and create a texture for this rendered text
  TTF_Font *fontT = TTF_OpenFont(font, size);
  SDL_Surface *textSurface = TTF_RenderText_Blended(fontT, text, color);
  if (!textSurface) {
    printf("Error creating text surface: %s\n", TTF_GetError());
    return NULL;
  }
  // Generate texture from the genereated surface
  SDL_Texture *textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
  // Free surface, no longer needed
  SDL_FreeSurface(textSurface);
  return textTexture;
}

// Function to render text at a specified position on the screen
void renderTextAt(const char *texting, SDL_Color color, SDL_Renderer *ren, int size, bool sub, int x, int y, int divx,
                  int divy, int pasx, int pasy, const char *font) {
  // Create an SDL_texture from the given text
  SDL_Texture *text = renderText(texting, color, ren, size, font);
  int textWidth = 0, textHeight = 0;
  // Retrieve the width and height of the texture
  SDL_QueryTexture(text, NULL, NULL, &textWidth, &textHeight);
  // Determine the x coordinate for rendering
  int xvalue;
  if (sub) {
    xvalue = x;
  } else {  // Align text differently depending of sub boolean value
    xvalue = x - textWidth;
  }
  // Define where the rectangle where the text will be rendered
  SDL_Rect textRect = {(xvalue) / divx + pasx, (y - textHeight) / divy + pasy, textWidth, textHeight};
  // copy the texture onto the renderer at textRect location
  SDL_RenderCopy(ren, text, NULL, &textRect);
  SDL_DestroyTexture(text);
}

// Function to render the input interface for saving or loading a game
void renderInput(SDL_Window *win, SDL_Renderer *ren, Env *env, bool trueIFsave) {
  SDL_Rect rect;

  // Define the overlay rectangle centered in the window
  SDL_Rect overlay_rect = {env->window_w / 4, env->window_h / 4, env->window_w / 2, env->window_h / 2};
  // Ajust overlay to match the texture's dimensions
  SDL_QueryTexture(env->menu, NULL, NULL, &rect.w, &rect.h);
  // render the overlay background texture
  SDL_RenderCopy(ren, env->menu, NULL, &overlay_rect);
  SDL_Color textColor = {255, 255, 255, 255};  // White
  // Create a texture for the title in case of "load" or "save"
  SDL_Texture *text;
  if (trueIFsave) {
    text = renderText("Save game", textColor, ren, 24, TITLEFONT);
  } else {
    text = renderText("load game", textColor, ren, 24, TITLEFONT);
  }
  // Get text dimensions
  int textWidth = 0, textHeight = 0;
  SDL_QueryTexture(text, NULL, NULL, &textWidth, &textHeight);
  // Define the rectangle to position the title text at the top
  SDL_Rect textRect = {(env->window_w - textWidth) / 2, (env->window_h - textHeight) / 3, textWidth, textHeight};
  // Render the title text
  SDL_RenderCopy(ren, text, NULL, &textRect);
  SDL_DestroyTexture(text);

  // Create and render an "Input" label
  SDL_Texture *text2 = renderText("Input", textColor, ren, 18, FONT);
  int textWidth2 = 0, textHeight2 = 0;
  SDL_QueryTexture(text2, NULL, NULL, &textWidth2, &textHeight2);
  SDL_Rect textRect2 = {(env->window_w - textWidth2) / 3, (env->window_h - textHeight2) / 3 + 45, textWidth2,
                        textHeight2};
  SDL_RenderCopy(ren, text2, NULL, &textRect2);
  SDL_DestroyTexture(text2);

  // Draw the white input box
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_Rect overlay_rect2 = {env->window_w / 3, env->window_h / 3 + 65, env->window_w / 3, 25};
  SDL_RenderFillRect(ren, &overlay_rect2);
  // Define text color for input
  SDL_Color textColorBlack = {0, 0, 0, 255};  // Black
  int textWidthText = 0, textHeightText = 0;
  // Render the user's input if inputText is not empty
  if (strlen(env->inputText) != 0) {
    SDL_Texture *text1 = renderText(env->inputText, textColorBlack, ren, 12, FONT);
    SDL_QueryTexture(text1, NULL, NULL, &textWidthText, &textHeightText);
    SDL_Rect textRect1 = {(env->window_w) / 3 + 5, (env->window_h - textHeightText) / 3 + 75, textWidthText,
                          textHeightText};
    SDL_RenderCopy(ren, text1, NULL, &textRect1);
    SDL_DestroyTexture(text1);
  } else {
    // Display an error message because the input field is empty
    SDL_Color textColorRed = {255, 0, 0, 255};
    SDL_Texture *text1 = renderText("The input field can't be blank!", textColorRed, ren, 16, FONT);
    int textWidth1 = 0, textHeight1 = 0;
    SDL_QueryTexture(text1, NULL, NULL, &textWidth1, &textHeight1);
    SDL_Rect textRect1 = {(env->window_w - textWidth1) / 2, (env->window_h - textHeight1) / 3 + 125, textWidth1,
                          textHeight1};
    SDL_RenderCopy(ren, text1, NULL, &textRect1);
    SDL_DestroyTexture(text1);
  }
  // Render a blinking cursor every second
  if ((int)time(NULL) % 2 == 0) {
    int textWidth1 = 0, textHeight1 = 0;
    SDL_Texture *text1 = renderText("|", textColorBlack, ren, 12, FONT);
    SDL_QueryTexture(text1, NULL, NULL, &textWidth1, &textHeight1);
    SDL_Rect textRect1 = {(env->window_w) / 3 + textWidthText + 5, (env->window_h - textHeight1) / 3 + 75, textWidth1,
                          textHeight1};
    SDL_RenderCopy(ren, text1, NULL, &textRect1);
    SDL_DestroyTexture(text1);
  }
}

void renderButtons(SDL_Window *win, SDL_Renderer *ren, Env *env, int x, int y) {
  SDL_Rect rect;
  // Define the rectangle for button placement
  rect.x = x;
  rect.y = y;
  // Load the default button textures
  SDL_Texture *ZB = env->Z_button;  // undo
  SDL_Texture *YB = env->Y_button;  // redo
  SDL_Texture *HB = env->H_button;  // help

  // If a button is pressed, update the corresponding texture (blue instead of pink button)
  if (env->is_button_pressed) {
    if (env->pressed_button == Z) {
      ZB = env->Z_pressed_button;
    } else if (env->pressed_button == Y) {
      YB = env->Y_pressed_button;
    } else if (env->pressed_button == H) {
      HB = env->H_pressed_button;
    }
  }
  // Render Z button
  SDL_QueryTexture(ZB, NULL, NULL, &rect.w, &rect.h);
  SDL_RenderCopy(ren, ZB, NULL, &rect);

  // Render Y button next to Z button
  rect.x = x + BUTTON_SIZE;
  SDL_QueryTexture(YB, NULL, NULL, &rect.w, &rect.h);
  SDL_RenderCopy(ren, YB, NULL, &rect);

  // Render H button next to Y button
  rect.x = x + BUTTON_SIZE * 2;
  SDL_QueryTexture(HB, NULL, NULL, &rect.w, &rect.h);
  SDL_RenderCopy(ren, HB, NULL, &rect);
}

void animate(SDL_Window *win, SDL_Renderer *ren, Env *env, Uint32 ms) {
  // Pause execution for the specified duration
  SDL_Delay(ms);
  // Clear the renderer to remove previous frames
  SDL_RenderClear(ren);
  // Render the updated scene
  render(win, ren, env);
  // Present the updated frame to the screen
  SDL_RenderPresent(ren);
}
/*#########################RENDER###################################*/

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  SDL_Rect rect;

  // Get the current window size
  SDL_GetWindowSize(win, &(env->window_w), &(env->window_h));
  // Render the background texture covering the entire window
  SDL_RenderCopy(ren, env->background, NULL, NULL);

#ifdef HAVE_AUDIO
  if (env->audio) {
    SDL_QueryTexture(env->audio_on, NULL, NULL, &rect.w, &rect.h);
    rect.x = env->window_w - rect.w - 10;
    rect.y = 10;
    SDL_RenderCopy(ren, env->audio_on, NULL, &rect);
  } else {
    SDL_QueryTexture(env->audio_off, NULL, NULL, &rect.w, &rect.h);
    rect.x = env->window_w - rect.w - 10;
    rect.y = 10;
    SDL_RenderCopy(ren, env->audio_off, NULL, &rect);
  }
#endif

  int nb_rows = game_nb_rows(env->g);
  int nb_cols = game_nb_cols(env->g);

  // Calculate the offsets to center the grid within the window
  int offset_i = (env->window_h - nb_rows * TILE_SIZE) / 2;
  int offset_j = (env->window_w - nb_cols * TILE_SIZE) / 2;

  // Loop through each cell in the game grid
  for (int i = 0; i < nb_rows; i++) {
    for (int j = 0; j < nb_cols; j++) {
      int index = GAME2TAB(i, j, env->g);
      env->coord_tab[index].y = offset_i + i * TILE_SIZE;
      env->coord_tab[index].x = offset_j + j * TILE_SIZE;

      // Render the title background
      SDL_QueryTexture(env->background_pieces, NULL, NULL, &rect.w, &rect.h);
      rect.x = env->coord_tab[index].x;
      rect.y = env->coord_tab[index].y;
      SDL_RenderCopy(ren, env->background_pieces, NULL, &rect);

      SDL_QueryTexture(env->text_tab[index], NULL, NULL, &rect.w, &rect.h);
      SDL_RenderCopyEx(ren, env->text_tab[index], NULL, &rect, env->dir_rot_tab[index], NULL, SDL_FLIP_NONE);

      // Render borders around the grid
      if (i == 0) {
        // Render the top border
        rect.y = env->coord_tab[index].y - TILE_SIZE;
        rect.x = env->coord_tab[index].x;
        SDL_QueryTexture(env->border, NULL, NULL, &rect.w, &rect.h);
        SDL_RenderCopyEx(ren, env->border, NULL, &rect, NORTH_R, NULL, SDL_FLIP_NONE);
      } else if (i == nb_rows - 1) {
        // Render the bottom border
        rect.y = env->coord_tab[index].y + TILE_SIZE;
        rect.x = env->coord_tab[index].x;
        SDL_QueryTexture(env->border, NULL, NULL, &rect.w, &rect.h);
        SDL_RenderCopyEx(ren, env->border, NULL, &rect, SOUTH_R, NULL, SDL_FLIP_NONE);
      }
      if (j == 0) {  // Left border
        rect.y = env->coord_tab[index].y;
        rect.x = env->coord_tab[index].x - TILE_SIZE;
        SDL_QueryTexture(env->border, NULL, NULL, &rect.w, &rect.h);
        SDL_RenderCopyEx(ren, env->border, NULL, &rect, WEST_R, NULL, SDL_FLIP_NONE);
      } else if (j == nb_cols - 1) {  // Right border
        rect.y = env->coord_tab[index].y;
        rect.x = env->coord_tab[index].x + TILE_SIZE;
        SDL_QueryTexture(env->border, NULL, NULL, &rect.w, &rect.h);
        SDL_RenderCopyEx(ren, env->border, NULL, &rect, EAST_R, NULL, SDL_FLIP_NONE);
      }
      // Render corner borders (Top left angle)
      if (i == 0 && j == 0) {
        rect.y = env->coord_tab[index].y - TILE_SIZE;
        rect.x = env->coord_tab[index].x - TILE_SIZE;
        SDL_QueryTexture(env->border_corner, NULL, NULL, &rect.w, &rect.h);
        SDL_RenderCopyEx(ren, env->border_corner, NULL, &rect, NORTH_R, NULL, SDL_FLIP_NONE);
      }
      if (i == 0 && j == nb_cols - 1) {  // Top Right angle
        rect.y = env->coord_tab[index].y - TILE_SIZE;
        rect.x = env->coord_tab[index].x + TILE_SIZE;
        if (game_is_wrapping(env->g)) {
          SDL_QueryTexture(env->wrapping_on, NULL, NULL, &rect.w, &rect.h);
          SDL_RenderCopyEx(ren, env->wrapping_on, NULL, &rect, NORTH_R, NULL, SDL_FLIP_NONE);
        } else {
          SDL_QueryTexture(env->wrapping_off, NULL, NULL, &rect.w, &rect.h);
          SDL_RenderCopyEx(ren, env->wrapping_off, NULL, &rect, NORTH_R, NULL, SDL_FLIP_NONE);
        }
      }
      if (i == nb_rows - 1 && j == 0) {  // Bottom Left angle
        rect.y = env->coord_tab[index].y + TILE_SIZE;
        rect.x = env->coord_tab[index].x - TILE_SIZE;
        SDL_QueryTexture(env->border_corner, NULL, NULL, &rect.w, &rect.h);
        SDL_RenderCopyEx(ren, env->border_corner, NULL, &rect, WEST_R, NULL, SDL_FLIP_NONE);
      }
      if (i == nb_rows - 1 && j == nb_cols - 1) {  // Bottom Right angle
        rect.y = env->coord_tab[index].y + TILE_SIZE;
        rect.x = env->coord_tab[index].x + TILE_SIZE;
        SDL_QueryTexture(env->border_corner, NULL, NULL, &rect.w, &rect.h);
        SDL_RenderCopyEx(ren, env->border_corner, NULL, &rect, SOUTH_R, NULL, SDL_FLIP_NONE);
      }
    }
  }

  // Render undo, redo and help buttons at the top

  renderButtons(win, ren, env, env->window_w / 2 - 90, env->coord_tab[0].y - TILE_SIZE - 60);

  // Render winning text if the game is won
  rect.x = env->window_w / 2 + 30;
  rect.y = env->coord_tab[0].y + nb_rows * TILE_SIZE;
  if (game_won(env->g)) {
    if (!env->game_already_won) {
      env->game_already_won = true;
#ifdef HAVE_AUDIO
      Mix_PlayChannel(-1, env->win, 0);
#endif
    }
    SDL_Rect win_rect = {env->window_w / 2 - 150, rect.y + TILE_SIZE, 300, 100};
    SDL_RenderCopy(ren, env->win_text, NULL, &win_rect);
  }

  // Render HELP overlay
  if (env->overlay_help) {
    SDL_Rect overlay_rect = {env->window_w / 4, env->window_h / 4, env->window_w / 2, env->window_h / 2};
    SDL_QueryTexture(env->menu, NULL, NULL, &rect.w, &rect.h);
    SDL_RenderCopy(ren, env->menu, NULL, &overlay_rect);
    SDL_Color textColor = {255, 255, 255, 255};

    renderTextAt("Help tab", textColor, ren, 24, false, env->window_w, env->window_h, 2, 3, 0, 0, TITLEFONT);

    renderTextAt("'R': Create a random game", textColor, ren, 12, true, env->window_w, env->window_h, 3, 3, 0, 50,
                 FONT);

    renderTextAt("'S': Save the game", textColor, ren, 12, true, env->window_w, env->window_h, 3, 3, 0, 75, FONT);

    renderTextAt("'Z': Undo the last move", textColor, ren, 12, true, env->window_w, env->window_h, 3, 3, 0, 100, FONT);

    renderTextAt("'Y': Redo the last move", textColor, ren, 12, true, env->window_w, env->window_h, 3, 3, 0, 125, FONT);

    renderTextAt("'K': Solve the game, up to 10 seconds", textColor, ren, 12, true, env->window_w, env->window_h, 3, 3,
                 0, 150, FONT);

    renderTextAt("'M': Shuffle the game", textColor, ren, 12, true, env->window_w, env->window_h, 3, 3, 0, 175, FONT);

    renderTextAt("'ESC': Leave the game", textColor, ren, 12, true, env->window_w, env->window_h, 3, 3, 0, 200, FONT);

  }
  // Render SAVE overlay
  else if (env->overlay_save) {
    renderInput(win, ren, env, true);
  }
  // Render LOAD overlay
  else if (env->overlay_load) {
    renderInput(win, ren, env, false);
  }
  // Render ESC overlay
  else if (env->overlay_exit) {
    SDL_Rect overlay_rect = {env->window_w / 4, env->window_h / 4, env->window_w / 2, env->window_h / 2};
    SDL_QueryTexture(env->menu, NULL, NULL, &rect.w, &rect.h);
    SDL_RenderCopy(ren, env->menu, NULL, &overlay_rect);

    SDL_Color textColor = {255, 255, 255, 255};
    renderTextAt("Leave the game?", textColor, ren, 24, false, env->window_w, env->window_h, 2, 3, 0, 0, TITLEFONT);

    SDL_SetRenderDrawColor(ren, 25, 25, 25, 1);
    SDL_Rect overlay_yes = {env->window_w / 4 + 50, env->window_h / 4 + 100, env->window_w / 8, env->window_h / 18};
    env->overlay_exit_yes = overlay_yes;
    SDL_RenderFillRect(ren, &overlay_yes);

    SDL_SetRenderDrawColor(ren, 25, 25, 25, 1);
    SDL_Rect overlay_no = {env->window_w / 4 + env->window_w / 8 + 100, env->window_h / 4 + 100, env->window_w / 8,
                           env->window_h / 18};
    env->overlay_exit_no = overlay_no;
    SDL_RenderFillRect(ren, &overlay_no);

    SDL_Color textColorYes = {255, 128, 237, 255};
    SDL_Color textColorNo = {116, 255, 246, 255};
    renderTextAt("Yes", textColorYes, ren, 16, true, env->window_w, env->window_h, 4, 4, 50 + overlay_yes.w / 3,
                 100 + overlay_yes.h / 3, FONT);
    renderTextAt("No", textColorNo, ren, 16, true, env->window_w, env->window_h, 4, 4,
                 env->window_w / 8 + 100 + overlay_no.w / 3, 100 + overlay_no.h / 3, FONT);
  }
  if (env->status == FAIL) {
    SDL_Color textColor = {255, 255, 255, 255};
    renderTextAt("Not able to solve this game...", textColor, ren, 24, false, env->window_h, rect.y, 2, 1, 0,
                 2 * TILE_SIZE, FONT);
  }
}
/*##################################################################*/

bool grid_to_ind(SDL_Window *win, Env *env, int x, int y, int *i, int *j) {
  int nb_rows = game_nb_rows(env->g);
  int nb_cols = game_nb_cols(env->g);

  for (int row = 0; row < nb_rows; row++) {
    for (int col = 0; col < nb_cols; col++) {
      int index = GAME2TAB(row, col, env->g);  // Convert grid position to index
      int cell_x = env->coord_tab[index].x;    // Get x
      int cell_y = env->coord_tab[index].y;    // Get y
      int cell_size = TILE_SIZE;
      // Check if the given (x,y) coordinates is inside this cell
      if (x >= cell_x && x < cell_x + cell_size && y >= cell_y && y < cell_y + cell_size) {
        *i = row;     // Store the row index
        *j = col;     // Store the column index
        return true;  // Valid cell was found
      }
    }
  }
  return false;
}

buttons grid_to_button(SDL_Window *win, Env *env, int x, int y) {
  // Check if y is within the button row, located aboe coord_tab[0].y
  if (env->coord_tab[0].y - TILE_SIZE - 60 < y && y < env->coord_tab[0].y - 60) {
    // Check x position to determine which button was clicked
    if (env->window_w / 2 - 90 < x && x < env->window_w / 2 - 30 && env->playable) {
      return Z;
    } else if (env->window_w / 2 - 30 < x && x < env->window_w / 2 + 30 && env->playable) {
      return Y;
    } else if (env->window_w / 2 + 30 < x && x < env->window_w / 2 + 90) {
      return H;
    }
  }
  return NONE;
}

bool grid_to_audio(SDL_Window *win, Env *env, int x, int y) {
  if (env->window_w - 80 < x && x < env->window_w - 10 && 10 < y && y < 80) {
    return true;
  }
  return false;
}

bool grid_to_cross(SDL_Window *win, Env *env, int x, int y) {
  int overlay_x = env->window_w / 4;
  int overlay_y = env->window_h / 4;
  int overlay_w = env->window_w / 2;
  int cross_size = 18;

  int cross_x = overlay_x + overlay_w - cross_size;
  int cross_y = overlay_y;

  if (x > cross_x && x < cross_x + cross_size && y > cross_y && y < cross_y + cross_size) {
    return true;
  }
  return false;
}

// Funtion to animate the movement of a game piece with rotation
void animate_move(SDL_Window *win, SDL_Renderer *ren, Env *env, int i, int j, int rotation) {
  int index = GAME2TAB(i, j, env->g);

  // Incrementally rotate the object in steps of 10 degrees
  for (int x = 0; x < ROTATION; x = x + 10) {
    // Update the rotation angle
    env->dir_rot_tab[index] = (env->dir_rot_tab[index] + 10 * rotation + 360) % 360;
    // Call animate function to render the updated state
    animate(win, ren, env, 2);
  }
}

#ifdef HAVE_AUDIO
// Function to plays a sound effect when a game piece gains or loses a half edge
void sound_piece(SDL_Window *win, SDL_Renderer *ren, Env *env, int i, int j, bool had_half_edge) {
  // If the piece did not previously have a half-edge but now does, play the plug-in sound
  if (!had_half_edge && has_half_edge(env->g, i, j)) {
    Mix_PlayChannel(-1, env->plug_in, 0);
  }  // If the piece is now diconnected, play the plug-out sound
  else if (had_half_edge && !has_half_edge(env->g, i, j)) {
    Mix_PlayChannel(-1, env->plug_out, 0);
  }
}
#endif

void press_button(SDL_Window *win, SDL_Renderer *ren, Env *env, buttons b) {
  if (b == Z) {  // Undo last move
    if (!asde_stack_isEmpty(env->history)) {
      move m = asde_stack_top(env->history);  // get the last move
#ifdef HAVE_AUDIO
      bool had_half_edge = has_half_edge(env->g, m.i, m.j);  // check if it had a half-edge
#endif
      m.move = -m.move;  // Reverse the move direction
      animate_move(win, ren, env, m.i, m.j, m.move);
      game_undo(env->g);              // Undo the move in the game state
      asde_stack_pop(env->history);   // Pop from history stack
      asde_stack_push(env->redo, m);  // Push to redo stack
#ifdef HAVE_AUDIO
      sound_piece(win, ren, env, m.i, m.j, had_half_edge);  // Play sound effect
#endif
    }

  } else if (b == Y) {  // Redo last move
    if (!asde_stack_isEmpty(env->redo)) {
      move m = asde_stack_top(env->redo);  // Get the last undone move
#ifdef HAVE_AUDIO
      bool had_half_edge = has_half_edge(env->g, m.i, m.j);  // check if it had a half-edge
#endif
      m.move = -m.move;
      animate_move(win, ren, env, m.i, m.j, m.move);
      game_redo(env->g);
      asde_stack_pop(env->redo);         // Pop move
      asde_stack_push(env->history, m);  // Push move in history
#ifdef HAVE_AUDIO
      sound_piece(win, ren, env, m.i, m.j, had_half_edge);
#endif
    }
  } else if (b == H) {                       // Toggle help overlay
    env->overlay_help = !env->overlay_help;  // Show or hide help
    env->playable = !env->playable;          // TOggle game interactivity
  }
  // Indicate that a button has been pressed
  env->pressed_button = b;
  env->is_button_pressed = true;
  animate(win, ren, env, 1);  // Animate it
  env->is_button_pressed = false;
  env->pressed_button = NONE;
  animate(win, ren, env, 500);
  update_game(win, ren, env);  // Update the game state after the action
}

bool is_clickeable(SDL_Window *win, Env *env, int x, int y) {
  int i, j;
  if (env->overlay_exit && x > env->overlay_exit_yes.x && x < env->overlay_exit_yes.x + env->overlay_exit_yes.w &&
      y > env->overlay_exit_yes.y && y < env->overlay_exit_yes.y + env->overlay_exit_yes.h) {
    return true;
  } else if (env->overlay_exit && x > env->overlay_exit_no.x && x < env->overlay_exit_no.x + env->overlay_exit_no.w &&
             y > env->overlay_exit_no.y && y < env->overlay_exit_no.y + env->overlay_exit_no.h) {
    return true;
  } else if (grid_to_cross(win, env, x, y) &&
             (env->overlay_exit || env->overlay_help || env->overlay_save || env->overlay_load)) {
    return true;
  } else if (grid_to_button(win, env, x, y) != NONE) {
    return true;
  } else if (grid_to_ind(win, env, x, y, &i, &j) &&
             !(env->overlay_exit || env->overlay_help || env->overlay_save || env->overlay_load)) {
    return true;
  }
  return false;
}

#ifdef HAVE_AUDIO
void toggle_music(Env *env) {
  if (env->audio) {
    Mix_PauseMusic();
  } else {
    Mix_ResumeMusic();
  }
  env->audio = !env->audio;
}
#endif

/*############################PROCESS################################*/

// This is used to set a timeout if solve (key 'k') takes more than 10 seconds.
jmp_buf buf;
void timeout_handler(int sig) {
  (void)sig;
  longjmp(buf, 1);
}
//

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  SDL_Point mouse;
  SDL_GetMouseState(&mouse.x, &mouse.y);
  int mouse_x = mouse.x / ZOOM;
  int mouse_y = mouse.y / ZOOM;

  // Update whether the game is still playable
  if (env->overlay_exit || env->overlay_help || env->overlay_load || env->overlay_save) {
    env->playable = false;
  } else {
    env->playable = !(game_won(env->g));
  }

  int w, h;
  SDL_GetWindowSize(win, &w, &h);  // Get the window size

  if (is_clickeable(win, env, mouse_x, mouse_y)) {
    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
  } else {
    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
  }

  if (e->type == SDL_QUIT) {  // If the window is being closed, return true to quit the game
    return true;
  }
  // If the ESC key is released, set the keyup flag to true in env
  if (e->type == SDL_KEYUP && e->key.keysym.sym == SDLK_ESCAPE) {
    env->keyUp = true;
  }
  // Handle mouse button click (Left or right) when the game is playable
  if (e->type == SDL_MOUSEBUTTONDOWN && !(env->status == EN_COURS)) {
    env->status = NOTHING;
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    int mouse_x = mouse.x / ZOOM;
    int mouse_y = mouse.y / ZOOM;
    int i, j;
    // Convert the mouse position to grid coordinates
    if (grid_to_ind(win, env, mouse_x, mouse_y, &i, &j) && env->playable) {
// Check if there was a half edge at the grid position
#ifdef HAVE_AUDIO
      bool had_half_edge = has_half_edge(env->g, i, j);  // check if it had a half-edge
#endif
      // Handle left mouse button click
      if (e->button.button == SDL_BUTTON_LEFT) {
        animate_move(win, ren, env, i, j, -1);
        game_play_move(env->g, i, j, -1);
        asde_stack_push(env->history, (move){i, j, -1});
      }
      // Handle right mouse button click
      else if (e->button.button == SDL_BUTTON_RIGHT) {
        animate_move(win, ren, env, i, j, 1);
        game_play_move(env->g, i, j, 1);
        asde_stack_push(env->history, (move){i, j, 1});
      }
#ifdef HAVE_AUDIO
      sound_piece(win, ren, env, i, j, had_half_edge);  // Play sound for the piece
#endif
      update_game(win, ren, env);  // Update the game state and render the game
    }
#ifdef HAVE_AUDIO
    if (grid_to_audio(win, env, mouse_x, mouse_y)) {
      toggle_music(env);  // Toggle the music
    }
#endif
    // Handle button clicks on the grid
    buttons b = grid_to_button(win, env, mouse_x, mouse_y);
    if (b != NONE) {
      press_button(win, ren, env, b);  // Execute button press action
    }
    if (grid_to_cross(win, env, mouse_x, mouse_y)) {
      env->overlay_save = false;
      env->overlay_help = false;
      env->overlay_load = false;
      env->keyUp = true;
      env->overlay_exit = false;
      if (env->input) {
        env->input = false;
        SDL_StopTextInput();  // Stop text input mode
      }
    }
    if (env->overlay_exit && mouse_x > env->overlay_exit_yes.x &&
        mouse_x < env->overlay_exit_yes.x + env->overlay_exit_yes.w && mouse_y > env->overlay_exit_yes.y &&
        mouse_y < env->overlay_exit_yes.y + env->overlay_exit_yes.h) {
      return true;
    } else if (env->overlay_exit && mouse_x > env->overlay_exit_no.x &&
               mouse_x < env->overlay_exit_no.x + env->overlay_exit_no.w && mouse_y > env->overlay_exit_no.y &&
               mouse_y < env->overlay_exit_no.y + env->overlay_exit_no.h) {
      env->overlay_exit = false;
    }
  }

  // Handle keydown events
  if (e->type == SDL_KEYDOWN && !(env->status == EN_COURS)) {
    env->status = NOTHING;
    // If ESC key is pressed, handle overlay visibility or exit
    if (e->key.keysym.sym == SDLK_ESCAPE) {
      if (env->overlay_save || env->overlay_help || env->overlay_load || env->overlay_exit) {
        // Close any open overlays and stop text input
        env->overlay_save = false;
        env->overlay_help = false;
        env->overlay_load = false;
        env->overlay_exit = false;
        env->keyUp = false;
        if (env->input) {
          env->input = false;
          SDL_StopTextInput();  // Stop text input mode
        }
      } else if (env->keyUp) {
        env->overlay_exit = true;
        // return true;
      }
    }
    if (e->key.keysym.sym == SDLK_k && !env->input) {
      signal(SIGALRM, timeout_handler);
      alarm(10);
      env->status = EN_COURS;
      if (setjmp(buf) == 0) {
        game_solve(env->g);
        env->status = SUCCESS;
        alarm(0);
      } else {
        env->status = FAIL;
      }
      update_game(win, ren, env);
    }
    // Handle the R key for randoming the game
    if (e->key.keysym.sym == SDLK_r && !env->input) {
      game_delete(env->g);  // Delete current game
      asde_stack_free(env->history);
      asde_stack_free(env->redo);
      env->history = asde_stack_create_empty();
      env->redo = asde_stack_create_empty();
      env->game_already_won = false;
      int cols = rand() % 8 + 2;
      int rows = rand() % 8 + 2;
      int wrapping = rand() % 2;
      env->g = game_random(rows, cols, wrapping, 0, 0);  // Create a new random game
      game_shuffle_orientation(env->g);
      update_game(win, ren, env);  // Update and render the new game
    }
    // Handle H key for help
    if (e->key.keysym.sym == SDLK_h && !env->input) {
      press_button(win, ren, env, H);
    }
    // Handle E key to shuffle the game
    if (e->key.keysym.sym == SDLK_m && !env->input) {
      game_shuffle_orientation(env->g);
      while (game_won(env->g)) {
        game_shuffle_orientation(env->g);
      }
      update_game(win, ren, env);
    }
    // Handle S key to save the game
    if (e->key.keysym.sym == SDLK_s && !env->input) {
      env->overlay_save = 1;
      env->inputText[0] = '\0';
    }
    // Handle Z key to undo
    if (e->key.keysym.sym == SDLK_z && !env->input) {
      press_button(win, ren, env, Z);
    }
    // Handle Y key to redo
    if (e->key.keysym.sym == SDLK_y && !env->input) {
      press_button(win, ren, env, Y);
    }
    // Handle the Enter key for saving or loading
    if (e->key.keysym.sym == SDLK_RETURN && env->input) {
      if (strlen(env->inputText) != 0) {
        if (env->overlay_load) {
          FILE *file;
          if ((file = fopen(env->inputText, "r"))) {
            fclose(file);
            env->overlay_load = false;
            env->input = false;
            SDL_StopTextInput();
            game_delete(env->g);                 // Delete current game
            env->g = game_load(env->inputText);  // Load the game from file
            update_game(win, ren, env);          // Update and render the game state
          }
        } else {  // Otherwise, save the game to specified file
          env->overlay_save = 0;
          env->input = false;
          SDL_StopTextInput();
          game_save(env->g, env->inputText);  // Save the game state
        }
      }
    }
    // Handle BACKSPACE key to delete last character in input mode
    if (e->key.keysym.sym == SDLK_BACKSPACE && env->input) {
      int len = strlen(env->inputText);
      if (len > 0) {
        env->inputText[len - 1] = '\0';  // Remove last character
      }
    }
    // Handle L key to show load overlay and reset input text
    if (e->key.keysym.sym == SDLK_l && !env->input) {
      env->overlay_load = 1;
      env->inputText[0] = '\0';  // Clear the input text
    }
    if (e->key.keysym.sym == SDLK_RETURN && env->overlay_exit) {
      return true;
    }
  }  // Handle key release events for S and L keys to start text input
  else if (e->type == SDL_KEYUP &&
           ((e->key.keysym.sym == SDLK_s && env->overlay_save) || (e->key.keysym.sym == SDLK_l && env->overlay_load))) {
    SDL_StartTextInput();  // Start text input mode
    env->input = true;     // Set input mode to true
  }                        // Handle text input events to append text
  else if (e->type == SDL_TEXTINPUT && env->input) {
    if (strlen(env->inputText) + strlen(e->text.text) < 26) {
      strcat(env->inputText, e->text.text);  // Append the input text
    }
  }
  return false;
}
/*############################CLEAN##################################*/

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  // Destroy SDL texture for every texture of the environment
  SDL_DestroyTexture(env->background_pieces);
  SDL_DestroyTexture(env->border);
  SDL_DestroyTexture(env->border_corner);
  SDL_DestroyTexture(env->wrapping_on);
  SDL_DestroyTexture(env->wrapping_off);
  SDL_DestroyTexture(env->Z_button);
  SDL_DestroyTexture(env->Y_button);
  SDL_DestroyTexture(env->H_button);
  // Destroy SDL texture from the game array
  for (int i = 0; i < game_nb_cols(env->g) * game_nb_rows(env->g); i++) {
    SDL_DestroyTexture(env->text_tab[i]);
  }
  // Free every array used
  free(env->text_tab);
  free(env->coord_tab);
  free(env->dir_rot_tab);
  // Delete the game and free env
  game_delete(env->g);
  free(env);
}
