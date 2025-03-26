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
  default:
    return ICOLOR;
  }
}

#endif //_TETROMINO_H
