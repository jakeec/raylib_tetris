#ifndef _TETROMINO_H
#define _TETROMINO_H

#include <raylib.h>
#include <stdlib.h>

#define ICOLOR CLITERAL(Color){0x6C, 0x6F, 0x93, 0xFF} // BLUE
#define OCOLOR CLITERAL(Color){0xFA, 0xC2, 0x9A, 0xFF} // YELLOW
#define TCOLOR CLITERAL(Color){0xBA, 0x77, 0xDB, 0xFF} // MAGENTA
#define SCOLOR CLITERAL(Color){0xE9, 0x56, 0x78, 0xFF} // RED
#define ZCOLOR CLITERAL(Color){0x1E, 0xB9, 0x80, 0xFF} // GREEN
#define JCOLOR CLITERAL(Color){0xEE, 0x64, 0xAE, 0xFF} // PINK
#define LCOLOR CLITERAL(Color){0xF0, 0x93, 0x83, 0xFF} // ORANGE

typedef struct {
  // Tetromino types: I O T S Z J L
  char type;
  // U D L R
  char orientation;
  int x, y;
} Tetromino;

char _types[7] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};

Tetromino tetromino_spawn() {
  char type = _types[rand() % 7];
  return (Tetromino){type, 'U', 4, 0};
}

Tetromino tetromino_clone(const Tetromino *src) {
  return (Tetromino){.orientation = src->orientation, .type = src->type, .x = src->x, .y = src->y};
}

Color tetromino_get_color(char tetromino_type) {
  switch (tetromino_type) {
  case 'I':
    return ICOLOR;
  case 'O':
    return OCOLOR;
  case 'T':
    return TCOLOR;
  case 'S':
    return SCOLOR;
  case 'Z':
    return ZCOLOR;
  case 'J':
    return JCOLOR;
  case 'L':
    return LCOLOR;
  case '\0':
    return (Color){150, 150, 150, 255};
  default:
    return ICOLOR;
  }
}

typedef struct {
  int x, y;
} Block;

typedef struct {
  Block a, b, c, d;
} TetrominoLayout;

// I
static TetrominoLayout IUp = {{-1, 0}, {0, 0}, {1, 0}, {2, 0}};
static TetrominoLayout IRight = {{0, -1}, {0, 0}, {0, 1}, {0, 2}};
static TetrominoLayout IDown = {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}};
static TetrominoLayout ILeft = {{0, -2}, {0, -1}, {0, 0}, {0, 1}};
// O
static TetrominoLayout OUp = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
static TetrominoLayout ORight = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
static TetrominoLayout ODown = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
static TetrominoLayout OLeft = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
// T
static TetrominoLayout TUp = {{0, 0}, {-1, 0}, {1, 0}, {0, 1}};
static TetrominoLayout TRight = {{0, 0}, {0, -1}, {0, 1}, {-1, 0}};
static TetrominoLayout TDown = {{0, 0}, {1, 0}, {-1, 0}, {0, -1}};
static TetrominoLayout TLeft = {{0, 0}, {0, 1}, {0, -1}, {1, 0}};
// S
static TetrominoLayout SUp = {{0, 0}, {1, 0}, {0, 1}, {-1, 1}};
static TetrominoLayout SRight = {{0, 0}, {0, -1}, {1, 0}, {1, 1}};
static TetrominoLayout SDown = {{0, 0}, {1, 0}, {0, 1}, {-1, 1}};
static TetrominoLayout SLeft = {{0, 0}, {0, -1}, {1, 0}, {1, 1}};
// Z
static TetrominoLayout ZUp = {{0, 0}, {-1, 0}, {0, 1}, {1, 1}};
static TetrominoLayout ZRight = {{0, 0}, {0, -1}, {-1, 0}, {-1, 1}};
static TetrominoLayout ZDown = {{0, 0}, {1, 0}, {0, -1}, {-1, -1}};
static TetrominoLayout ZLeft = {{0, 0}, {0, 1}, {1, 0}, {1, -1}};
// J
static TetrominoLayout JUp = {{0, 0}, {-1, 0}, {1, 0}, {1, 1}};
static TetrominoLayout JRight = {{0, 0}, {0, -1}, {0, 1}, {-1, 1}};
static TetrominoLayout JDown = {{0, 0}, {1, 0}, {-1, 0}, {-1, -1}};
static TetrominoLayout JLeft = {{0, 0}, {0, -1}, {0, 1}, {1, -1}};
// L
static TetrominoLayout LUp = {{0, 0}, {1, 0}, {-1, 0}, {-1, 1}};
static TetrominoLayout LRight = {{0, 0}, {0, 1}, {0, -1}, {-1, -1}};
static TetrominoLayout LDown = {{0, 0}, {-1, 0}, {1, 0}, {1, -1}};
static TetrominoLayout LLeft = {{0, 0}, {0, -1}, {0, 1}, {1, 1}};

TetrominoLayout tetromino_get_layout(Tetromino *tetromino) {
  if (tetromino->type == 'I') {
    if (tetromino->orientation == 'U')
      return IUp;
    if (tetromino->orientation == 'R')
      return IRight;
    if (tetromino->orientation == 'D')
      return IDown;
    if (tetromino->orientation == 'L')
      return ILeft;
  }
  if (tetromino->type == 'O') {
    if (tetromino->orientation == 'U')
      return OUp;
    if (tetromino->orientation == 'R')
      return ORight;
    if (tetromino->orientation == 'D')
      return ODown;
    if (tetromino->orientation == 'L')
      return OLeft;
  }
  if (tetromino->type == 'T') {
    if (tetromino->orientation == 'U')
      return TUp;
    if (tetromino->orientation == 'R')
      return TRight;
    if (tetromino->orientation == 'D')
      return TDown;
    if (tetromino->orientation == 'L')
      return TLeft;
  }
  if (tetromino->type == 'S') {
    if (tetromino->orientation == 'U')
      return SUp;
    if (tetromino->orientation == 'R')
      return SRight;
    if (tetromino->orientation == 'D')
      return SDown;
    if (tetromino->orientation == 'L')
      return SLeft;
  }
  if (tetromino->type == 'Z') {
    if (tetromino->orientation == 'U')
      return ZUp;
    if (tetromino->orientation == 'R')
      return ZRight;
    if (tetromino->orientation == 'D')
      return ZDown;
    if (tetromino->orientation == 'L')
      return ZLeft;
  }
  if (tetromino->type == 'J') {
    if (tetromino->orientation == 'U')
      return JUp;
    if (tetromino->orientation == 'R')
      return JRight;
    if (tetromino->orientation == 'D')
      return JDown;
    if (tetromino->orientation == 'L')
      return JLeft;
  }
  if (tetromino->type == 'L') {
    if (tetromino->orientation == 'U')
      return LUp;
    if (tetromino->orientation == 'R')
      return LRight;
    if (tetromino->orientation == 'D')
      return LDown;
    if (tetromino->orientation == 'L')
      return LLeft;
  }

  exit(1);
}

TetrominoLayout tetromino_get_layout_up(Tetromino *tetromino) {
  Tetromino clone = tetromino_clone(tetromino);
  clone.orientation = 'U';
  return tetromino_get_layout(&clone);
}

TetrominoLayout tetromino_get_absolute_layout(Tetromino *tetromino) {
  TetrominoLayout layout = tetromino_get_layout(tetromino);
  layout.a.x += tetromino->x;
  layout.a.y += tetromino->y;
  layout.b.x += tetromino->x;
  layout.b.y += tetromino->y;
  layout.c.x += tetromino->x;
  layout.c.y += tetromino->y;
  layout.d.x += tetromino->x;
  layout.d.y += tetromino->y;
  return layout;
}

void tetromino_rotate_cw(Tetromino *tetromino) {
  switch (tetromino->orientation) {
  case 'U':
    tetromino->orientation = 'R';
    break;
  case 'R':
    tetromino->orientation = 'D';
    break;
  case 'D':
    tetromino->orientation = 'L';
    break;
  case 'L':
    tetromino->orientation = 'U';
    break;
  }
}

#endif //_TETROMINO_H
