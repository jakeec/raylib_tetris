#include "logger.h"
#include "tetromino.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define WINWIDTH 640
#define WINHEIGHT 480
#define TARGETFPS 60
#define TITLE "Tetris"
#define PADDING 10
#define ROWS 20
#define COLS 10
#define CELLSIZE 20
#define SPEEDBOOST 10

int main(void) {
  srand(time(0));
  SetTraceLogCallback(CustomRaylibLogCallback);

  InitWindow(WINWIDTH, WINHEIGHT, TITLE);

  SetTargetFPS(TARGETFPS);

  double fall_speed = 0.5;
  double move_speed = 0.25;
  double last_fall_update = GetTime();
  double last_move_update = GetTime();

  Tetromino active = tetromino_spawn();
  Tetromino next = tetromino_spawn();

  char grid[ROWS][COLS] = {0};

  bool move_right = false;
  bool move_left = false;
  bool fall_faster = false;
  bool rotate_cw = false;

  bool running = true;
  bool paused = false;
  while (running) {
    if (WindowShouldClose()) {
      running = false;
      break;
    }

    if (paused) {
      if (IsKeyPressed(KEY_P)) {
        paused = false;
      }
    } else {
      if (IsKeyPressed(KEY_P)) {
        paused = true;
      }
    }

    if (IsKeyDown(KEY_RIGHT)) {
      move_right = true;
    }

    if (IsKeyDown(KEY_LEFT)) {
      move_left = true;
    }

    if (IsKeyPressed(KEY_UP)) {
      rotate_cw = true;
      // tetromino_rotate_cw(&active);
    }

    fall_faster = IsKeyDown(KEY_DOWN);

    if (!paused) {
      double now = GetTime();
      if (now - last_move_update > move_speed) {
        last_move_update = now;

        if (rotate_cw) {
          tetromino_rotate_cw(&active);
          rotate_cw = false;
        }

        if (move_right) {
          active.x++;
          move_right = false;
        }

        if (move_left) {
          active.x--;
          move_left = false;
        }

        if (active.x > COLS - 1) {
          active.x = COLS - 1;
        }

        if (active.x < 0) {
          active.x = 0;
        }
      }

      // Run interval updates
      double final_fall_speed =
          fall_faster ? fall_speed / SPEEDBOOST : fall_speed;
      if (now - last_fall_update > final_fall_speed) {
        last_fall_update = now;

        // Collision
        if (active.y >= ROWS - 1) {
          active.y = ROWS - 1;
          grid[active.y][active.x] = active.type;
          active = next;
          next = tetromino_spawn();
        }

        active.y++;
      }
    }

    BeginDrawing();
#define BACKGROUND CLITERAL(Color){0x2E, 0x30, 0x3E, 0xFF} // 2E303E
    ClearBackground(BACKGROUND);

    // Draw the grid
    DrawRectangleLines(PADDING, PADDING, CELLSIZE * COLS, CELLSIZE * ROWS,
                       BROWN);

    // Draw cell outlines.
    // for (int x = 0, y = 0; y < ROWS; x = (x + 1) % COLS, x == 0 && y++) {
    //   DrawRectangleLines(PADDING + (x * CELLSIZE), PADDING + (y * CELLSIZE),
    //                      CELLSIZE, CELLSIZE, GRAY);
    // }

    // Draw placed tetrominoes.
    for (int x = 0, y = 0; y < ROWS; x = (x + 1) % COLS, x == 0 && y++) {
      char cell = grid[y][x];
      if (cell == '\0')
        continue;

      Color color = tetromino_get_color(cell);

      DrawRectangle(PADDING + (x * CELLSIZE), PADDING + (y * CELLSIZE),
                    CELLSIZE, CELLSIZE, color);
    }

    // Draw debug info.
    char debug_text[100];
    sprintf(debug_text, "%c - %c", active.type, active.orientation);
    DrawText(debug_text, 250, 200, 16, WHITE);

    // Draw active tetromino.
    TetrominoLayout layout = tetromino_get_layout(&active);
    Color color = tetromino_get_color(active.type);
    DrawRectangle(PADDING + ((active.x + layout.a.x) * CELLSIZE),
                  PADDING + ((active.y + layout.a.y) * CELLSIZE), CELLSIZE,
                  CELLSIZE, color);
    DrawRectangle(PADDING + ((active.x + layout.b.x) * CELLSIZE),
                  PADDING + ((active.y + layout.b.y) * CELLSIZE), CELLSIZE,
                  CELLSIZE, color);
    DrawRectangle(PADDING + ((active.x + layout.c.x) * CELLSIZE),
                  PADDING + ((active.y + layout.c.y) * CELLSIZE), CELLSIZE,
                  CELLSIZE, color);
    DrawRectangle(PADDING + ((active.x + layout.d.x) * CELLSIZE),
                  PADDING + ((active.y + layout.d.y) * CELLSIZE), CELLSIZE,
                  CELLSIZE, color);

    EndDrawing();
  }
}