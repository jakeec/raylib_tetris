#include "logger.h"
#include "tetromino.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define WINWIDTH 680
#define WINHEIGHT 480
#define TARGETFPS 60
#define TITLE "Tetris"
#define PADDING 20
#define ROWS 22
#define COLS 10
#define CELLSIZE 20
#define SPEEDBOOST 10
#define NEXT_COUNT 4

void draw_tetromino_cell(int px, int py, int x, int y, char tetromino_type,
                         bool shadow);
void draw_grid_tetromino(Tetromino *tetromino);
void draw_tetromino(Tetromino *tetromino, int px, int py);

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
  Tetromino next[NEXT_COUNT] = {0};
  for (int i = 0; i < NEXT_COUNT; i++) {
    next[i] = tetromino_spawn();
  }

  char grid[ROWS][COLS] = {0};

  size_t score = 0;
  size_t display_score = 0;

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
          Tetromino clone = tetromino_clone(&active);
          tetromino_rotate_cw(&clone);
          TetrominoLayout layout = tetromino_get_absolute_layout(&clone);
          // TODO Decide whether to prevent invalid moves or use nudge
          // strategies.
          bool collision = false;
          do {
            if (layout.a.x > COLS - 1 || layout.a.x < 0) {
              collision = true;
              break;
            }

            if (layout.b.x > COLS - 1 || layout.b.x < 0) {
              collision = true;
              break;
            }

            if (layout.c.x > COLS - 1 || layout.c.x < 0) {
              collision = true;
              break;
            }

            if (layout.d.x > COLS - 1 || layout.d.x < 0) {
              collision = true;
              break;
            }

            if (grid[layout.a.y][layout.a.x] != '\0') {
              collision = true;
              break;
            }

            if (grid[layout.b.y][layout.b.x] != '\0') {
              collision = true;
              break;
            }

            if (grid[layout.c.y][layout.c.x] != '\0') {
              collision = true;
              break;
            }

            if (grid[layout.d.y][layout.d.x] != '\0') {
              collision = true;
              break;
            }
          } while (0);
          if (!collision) {
            active = clone;
          }
          rotate_cw = false;
        }

        Tetromino clone = tetromino_clone(&active);
        // Update clone.
        if (move_right) {
          clone.x++;
          move_right = false;
        }

        if (move_left) {
          clone.x--;
          move_left = false;
        }

        // Check if move was valid.
        TetrominoLayout layout = tetromino_get_absolute_layout(&clone);
        bool collision = false;

        do {
          if (layout.a.x >= COLS || layout.b.x >= COLS || layout.c.x >= COLS ||
              layout.d.x >= COLS) {
            collision = true;
          }

          if (layout.a.x < 0 || layout.b.x < 0 || layout.c.x < 0 ||
              layout.d.x < 0) {
            collision = true;
          }

          if (grid[layout.a.y][layout.a.x] != '\0') {
            collision = true;
            break;
          }

          if (grid[layout.b.y][layout.b.x] != '\0') {
            collision = true;
            break;
          }

          if (grid[layout.c.y][layout.c.x] != '\0') {
            collision = true;
            break;
          }

          if (grid[layout.d.y][layout.d.x] != '\0') {
            collision = true;
            break;
          }
        } while (0);

        if (!collision) {
          active = clone;
        }
      }

      // Run interval updates
      // TODO Failure state
      double final_fall_speed =
          fall_faster ? fall_speed / SPEEDBOOST : fall_speed;
      if (now - last_fall_update > final_fall_speed) {
        last_fall_update = now;

        // Collision
        TetrominoLayout layout = tetromino_get_absolute_layout(&active);
        bool collision = false;
        do {
          if (grid[layout.a.y + 1][layout.a.x] != '\0') {
            collision = true;
            break;
          }

          if (grid[layout.b.y + 1][layout.b.x] != '\0') {
            collision = true;
            break;
          }

          if (grid[layout.c.y + 1][layout.c.x] != '\0') {
            collision = true;
            break;
          }

          if (grid[layout.d.y + 1][layout.d.x] != '\0') {
            collision = true;
            break;
          }

          if (layout.a.y >= ROWS - 1 || layout.b.y >= ROWS - 1 ||
              layout.c.y >= ROWS - 1 || layout.d.y >= ROWS - 1) {
            collision = true;
            break;
          }
        } while (0);

        if (collision) {
          grid[layout.a.y][layout.a.x] = active.type;
          grid[layout.b.y][layout.b.x] = active.type;
          grid[layout.c.y][layout.c.x] = active.type;
          grid[layout.d.y][layout.d.x] = active.type;
          active = next[0];
          for (int i = 0; i < NEXT_COUNT - 1; i++) {
            next[i] = next[i + 1];
          }
          next[NEXT_COUNT - 1] = tetromino_spawn();
        } else {
          active.y++;
        }

        // Check for completed lines.
        int full_lines[ROWS] = {false};
        int full_line_count = 0;
        for (size_t y = 0; y < ROWS; y++) {
          bool full_line = true;
          for (size_t x = 0; x < COLS; x++) {
            if (grid[y][x] == '\0') {
              full_line = false;
            }
          }
          if (full_line) {
            LDEBUG("FULL LINE");
            full_lines[y] = true;
            full_line_count++;
          }
        }

        // Delete completed lines.
        for (int i = 0; i < ROWS; i++) {
          if (full_lines[i]) {
            for (int x = 0; x < COLS; x++) {
              grid[i][x] = '\0';
            }
          }
        }

        // Shift lines down.
        for (int y = 0; y < ROWS; y++) {
          if (full_lines[y]) {
            // Shift all lines above down.
            for (int ya = y; ya > 0; ya--) {
              for (int x = 0; x < COLS; x++) {
                grid[ya][x] = grid[ya - 1][x];
              }
            }
          }
        }

        // Award points.
        // TODO Move points to defines.
        switch (full_line_count) {
        case 1:
          score += 100;
          break;
        case 2:
          score += 300;
          break;
        case 3:
          score += 500;
          break;
        case 4:
          score += 800;
          break;
        }
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

    // Draw score.
    if (display_score < score) {
      display_score += 1;
    }
    char points_text[100];
    sprintf(points_text, "SCORE: %d", display_score);
    DrawText(points_text, 265, 125, 30, (Color){0, 0, 0, 150});
    DrawText(points_text, 260, 120, 30, WHITE);

    // Draw active tetromino.
    draw_grid_tetromino(&active);

    // Draw placed tetrominoes.
    for (int x = 0, y = 0; y < ROWS; x = (x + 1) % COLS, x == 0 && y++) {
      char cell = grid[y][x];
      if (cell == '\0')
        continue;

      draw_tetromino_cell(PADDING, PADDING, x, y, cell, true);
    }

    // Draw grid border.
    for (int x = 0; x < COLS + 24; x++) {
      draw_tetromino_cell(0, 0, x, 0, '\0', false);
      draw_tetromino_cell(0, (ROWS + 1) * CELLSIZE, x, 0, '\0', false);
    }

    for (int y = 1; y < ROWS + 1; y++) {
      draw_tetromino_cell(0, y * CELLSIZE, 0, 0, '\0', false);
      draw_tetromino_cell((COLS + 1) * CELLSIZE, y * CELLSIZE, 0, 0, '\0', false);
      draw_tetromino_cell((COLS + 23) * CELLSIZE, y * CELLSIZE, 0, 0, '\0', false);
    }

    // Draw next tetromino.
    for (int i = 0; i < NEXT_COUNT; i++) {
      draw_tetromino(&next[i], (14 * CELLSIZE) + (i * 100), CELLSIZE * 2);
    }

    EndDrawing();
  }
}

void draw_tetromino_cell(int px, int py, int x, int y, char tetromino_type,
                         bool shadow) {
  Color color = tetromino_get_color(tetromino_type);
  int cx = px + (x * CELLSIZE);
  int cy = py + (y * CELLSIZE);

  // Shadow
  int s = 4; // Shadow size.
  if (shadow)
    DrawRectangle(cx + s, cy + s, CELLSIZE, CELLSIZE, (Color){0, 0, 0, 40});

  int b = 4; // Bevel size
  DrawRectangle(cx, cy, CELLSIZE, CELLSIZE, color);
  DrawRectangle(cx, cy, CELLSIZE - b, CELLSIZE - b,
                (Color){255, 255, 255, 100});
  DrawRectangle(cx + b, cy + b, CELLSIZE - b, CELLSIZE - b,
                (Color){0, 0, 0, 40});
  DrawRectangle(cx + b, cy + b, CELLSIZE - 2 * b, CELLSIZE - 2 * b, color);
  DrawRectangleLines(cx, cy, CELLSIZE, CELLSIZE, BLACK);
}

void draw_grid_tetromino(Tetromino *tetromino) {
  TetrominoLayout layout = tetromino_get_absolute_layout(tetromino);
  draw_tetromino_cell(PADDING, PADDING, layout.a.x, layout.a.y, tetromino->type,
                      true);
  draw_tetromino_cell(PADDING, PADDING, layout.b.x, layout.b.y, tetromino->type,
                      true);
  draw_tetromino_cell(PADDING, PADDING, layout.c.x, layout.c.y, tetromino->type,
                      true);
  draw_tetromino_cell(PADDING, PADDING, layout.d.x, layout.d.y, tetromino->type,
                      true);
}

void draw_tetromino(Tetromino *tetromino, int px, int py) {
  TetrominoLayout layout = tetromino_get_layout(tetromino);
  draw_tetromino_cell(px, py, layout.a.x, layout.a.y, tetromino->type, true);
  draw_tetromino_cell(px, py, layout.b.x, layout.b.y, tetromino->type, true);
  draw_tetromino_cell(px, py, layout.c.x, layout.c.y, tetromino->type, true);
  draw_tetromino_cell(px, py, layout.d.x, layout.d.y, tetromino->type, true);
}