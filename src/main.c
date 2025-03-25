#include "logger.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define PADDING 10
#define ROWS 20
#define COLS 10
#define CELLSIZE 20

typedef struct {
  // Tetromino types: I O T S Z J L
  char type;
  int x, y;
  bool falling;
  // U D L R
  char orientation;
} Tetromino;

int main(void) {
  SetTraceLogCallback(CustomRaylibLogCallback);

  InitWindow(640, 480, "Tetris");

  SetTargetFPS(60);

  double game_speed = 0.125;
  double last_update = GetTime();

  Tetromino tetrominos[(ROWS * COLS) / 4];
  int tetrominos_count = 0;
  tetrominos[tetrominos_count++] = (Tetromino){'I', 3, 3, true};

  bool move_right = false;
  bool move_left = false;

  bool running = true;
  while (running) {
    if (WindowShouldClose()) {
      running = false;
      break;
    }

    if (IsKeyDown(KEY_RIGHT)) {
      move_right = true;
    }

    if (IsKeyDown(KEY_LEFT)) {
      move_left = true;
    }

    // Run interval updates
    double now = GetTime();
    if (now - last_update > game_speed) {
      last_update = now;

      Tetromino *active = &tetrominos[tetrominos_count - 1];

      if (move_right) {
        active->x++;
        move_right = false;
      }

      if (move_left) {
        active->x--;
        move_left = false;
      }

      if (active->x > COLS - 1) {
        active->x = COLS - 1;
      }

      if (active->x < 0) {
        active->x = 0;
      }

      if (active->falling) {
        active->y++;
      }

      if (active->y >= ROWS - 1) {
        active->y = ROWS - 1;
        active->falling = false;
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw the grid
    DrawRectangleLines(PADDING, PADDING, CELLSIZE * COLS, CELLSIZE * ROWS,
                       BROWN);

    for (int x = 0, y = 0; y < ROWS; x = (x + 1) % COLS, x == 0 && y++) {
      DrawRectangleLines(PADDING + (x * CELLSIZE), PADDING + (y * CELLSIZE),
                         CELLSIZE, CELLSIZE, GRAY);
    }

    // Draw tetrominos
    for (int i = 0; i < tetrominos_count; i++) {
      Tetromino this = tetrominos[i];
      if (this.type == 'I') {
        DrawRectangle(PADDING + (this.x * CELLSIZE),
                      PADDING + (this.y * CELLSIZE), CELLSIZE, CELLSIZE, BLACK);
        DrawRectangle(PADDING + (this.x * CELLSIZE),
                      PADDING + ((this.y - 1) * CELLSIZE), CELLSIZE, CELLSIZE,
                      BLACK);
        DrawRectangle(PADDING + (this.x * CELLSIZE),
                      PADDING + ((this.y - 2) * CELLSIZE), CELLSIZE, CELLSIZE,
                      BLACK);
        DrawRectangle(PADDING + (this.x * CELLSIZE),
                      PADDING + ((this.y - 3) * CELLSIZE), CELLSIZE, CELLSIZE,
                      BLACK);
      }
    }

    EndDrawing();
  }
}