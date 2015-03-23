#ifndef BOARD_HPP_
#define BOARD_HPP_


#include <stdio.h>
#include <string.h>

#include "trax.h"
#include "timer.hpp"


/**
 * Column-major array
 */
class Board {
 private:

  //----------------------------------------------------------------------------
  // Typedefs and Constants
  //----------------------------------------------------------------------------
  
  enum BoardRestriction {
    BOARD_MAX = 100,
    BOARD_CENTER = BOARD_MAX / 2,
    LINE_LENGTH = 8,
  };
  
  enum ColorPatterns {
    COL_CLEAR = 0,
    COL_WHITE = 1,
    COL_RED = 2,
  };

  enum DirectionPatterns {
    DIR_N = 0,
    DIR_E = 1,
    DIR_S = 2,
    DIR_W = 3,
  };

  enum ValidMoveErrors {
    ERROR_NOT_EMPTY = 1,
    ERROR_CONSISTENT_PLACEMENT = 2,
    ERROR_LINE_COL_CONNECTED = 3,
    ERROR_PROHIBIT_3 = 4,
    ERROR_NOT_ISOLATED = 5,
  };


  //----------------------------------------------------------------------------
  // Members
  //----------------------------------------------------------------------------

  // Board
  char tiles_[BOARD_MAX][BOARD_MAX];
  char colors_[BOARD_MAX][BOARD_MAX];
  int border_n_, border_e_, border_s_, border_w_;

  Timer *get_around_colors_time_;


  //----------------------------------------------------------------------------
  // Methods
  //----------------------------------------------------------------------------

  /**
   * Get opposite color of argument color
   */
  inline int GetOppositeColor(const int color) {
    return (color == COL_WHITE) ? COL_RED :
        (color == COL_RED) ? COL_WHITE : COL_CLEAR;
  }

  /**
   * Get around (N, E, S, W) colors of (x, y)
   */
  inline void GetAroundColors(
      const int x, const int y,
      int &col_n, int &col_e,
      int &col_s, int &col_w) {
    get_around_colors_time_->Start();
    col_w = colors_[x - 1][y];
    col_e = (tiles_[x + 1][y] == '+') ? colors_[x + 1][y] :
        GetOppositeColor(colors_[x + 1][y]);
    col_n = (tiles_[x][y - 1] == '+') ? GetOppositeColor(colors_[x][y - 1]) :
        (tiles_[x][y - 1] == '/')  ? colors_[x][y - 1] :
        GetOppositeColor(colors_[x][y - 1]);
    col_s = (tiles_[x][y + 1] == '+') ? GetOppositeColor(colors_[x][y + 1]) :
        (tiles_[x][y + 1] == '\\') ? colors_[x][y + 1] :
        GetOppositeColor(colors_[x][y + 1]);
    get_around_colors_time_->Stop();
  }

  /**
   * Get (x, y)'s color
   */
  inline int GetColor(
      const char tile,
      const int col_n, const int col_e,
      const int col_s, const int col_w) {
    if (tile == '+') {
      return (col_w == COL_WHITE || col_e == COL_WHITE ||
              col_n == COL_RED || col_s == COL_RED) ? COL_WHITE : COL_RED;
    } else if (tile == '/') {
      return (col_w == COL_WHITE || col_n == COL_WHITE ||
              col_s == COL_RED || col_e == COL_RED) ? COL_RED : COL_WHITE;
    } else if (tile == '\\') {
      return (col_w == COL_WHITE || col_s == COL_WHITE ||
              col_n == COL_RED || col_e == COL_RED) ? COL_RED : COL_WHITE;
    }
    return COL_CLEAR;
  }

  /**
   * Get (x, y)'s color
   */
  inline int GetColor(const int x, const int y, const char tile) {
    int col_n, col_e, col_s, col_w;
    GetAroundColors(x, y, col_n, col_e, col_s, col_w);
    return GetColor(tile, col_n, col_e, col_s, col_w);
  }

  /**
   * Check whether (x, y) is consistent placement
   */
  inline bool IsConsistentPlacement(int x, int y, char tile) {
    int col_n, col_e, col_s, col_w;
    GetAroundColors(x, y, col_n, col_e, col_s, col_w);
    if (tile == '+') {
      if ((col_w != col_e && (col_w != COL_CLEAR && col_e != COL_CLEAR)) ||
          (col_n != col_s && (col_n != COL_CLEAR && col_s != COL_CLEAR))) {
        return false;
      }
    }
    if (tile == '/') {
      if ((col_w != col_n && (col_w != COL_CLEAR && col_n != COL_CLEAR)) ||
          (col_s != col_e && (col_s != COL_CLEAR && col_e != COL_CLEAR))) {
        return false;
      }
    }
    if (tile == '\\') {
      if ((col_w != col_s && (col_w != COL_CLEAR && col_s != COL_CLEAR)) ||
          (col_n != col_e && (col_n != COL_CLEAR && col_e != COL_CLEAR))) {
        return false;
      }
    }
    return true;
  }

  /**
   * Check whether (x, y) is line color connected
   */
  inline bool IsLineColorConnected(const int x, const int y, const char tile) {
    int col_n, col_e, col_s, col_w;
    int my_col_n, my_col_e, my_col_s, my_col_w;
    GetAroundColors(x, y, col_n, col_e, col_s, col_w);
    my_col_e = GetColor(x, y, tile);
    my_col_w = (tile == '+')  ? my_col_e : GetOppositeColor(my_col_e);
    my_col_n = (tile == '\\') ? my_col_e : GetOppositeColor(my_col_e);
    my_col_s = (tile == '/')  ? my_col_e : GetOppositeColor(my_col_e);
    if ((my_col_e != col_e && (col_e != COL_CLEAR)) ||
        (my_col_w != col_w && (col_w != COL_CLEAR)) ||
        (my_col_n != col_n && (col_n != COL_CLEAR)) ||
        (my_col_s != col_s && (col_s != COL_CLEAR))) {
      return false;
    }
    return true;
  }

  /**
   * Check whether (x, y) is prohibit pattern
   */
  inline bool IsProhibited3(const int x, const int y) {
    int col_n, col_e, col_s, col_w;
    GetAroundColors(x, y, col_n, col_e, col_s, col_w);
    if ((col_w == col_e && col_e == col_n && col_n != COL_CLEAR) ||
        (col_e == col_n && col_n == col_s && col_s != COL_CLEAR) ||
        (col_n == col_s && col_s == col_w && col_w != COL_CLEAR) ||
        (col_s == col_w && col_w == col_e && col_e != COL_CLEAR)) {
      return true;
    }
    return false;
  }

  /**
   * Trace board to find loop
   */
  bool TraceLoop(const int x, const int y, const int d) {
    int xx = x, yy = y, dir = d;
    do {
      int next_dir;
      switch (dir) {
        case DIR_N:
          yy--;
          next_dir = tiles_[xx][yy] == '/' ?
              DIR_E : tiles_[xx][yy] == '\\' ? DIR_W : DIR_N;
          break;
        case DIR_E:
          xx++;
          next_dir = tiles_[xx][yy] == '/' ?
              DIR_N : tiles_[xx][yy] == '\\' ? DIR_S : DIR_E;
          break;
        case DIR_S:
          yy++;
          next_dir = tiles_[xx][yy] == '/' ?
              DIR_W : tiles_[xx][yy] == '\\' ? DIR_E : DIR_S;
          break;
        case DIR_W:
          xx--;
          next_dir = tiles_[xx][yy] == '/' ?
              DIR_S : tiles_[xx][yy] == '\\' ? DIR_N : DIR_W;
          break;
      }
      dir = next_dir;
      if (xx == x && yy == y) {
        // Loop found !
        return true;
      }
    } while (tiles_[xx][yy] != ' ');
    return false;
  }

  /**
   * Trace board to find loop
   */
  bool TraceLoop(const int x, const int y) {
    bool found = false;
    // trace N, E, S, W
    for (int d = DIR_N; d <= DIR_W; d++) {
      if (TraceLoop(x, y, d)) {
        found = true;
      }
    }
    return found;
  }

  /**
   * Trace board to find eight line
   */
  bool TraceLine(const int x, const int y, const int d) {
    int xx = x, yy = y, dir = d;
    do {
      int next_dir;
      switch (dir) {
        case DIR_N:
          next_dir = tiles_[xx][yy] == '/' ?
              DIR_E : tiles_[xx][yy] == '\\' ? DIR_W : DIR_N;
          break;
        case DIR_E:
          next_dir = tiles_[xx][yy] == '/' ?
              DIR_N : tiles_[xx][yy] == '\\' ? DIR_S : DIR_E;
          break;
        case DIR_S:
          next_dir = tiles_[xx][yy] == '/' ?
              DIR_W : tiles_[xx][yy] == '\\' ? DIR_E : DIR_S;
          break;
        case DIR_W:
          next_dir = tiles_[xx][yy] == '/' ?
              DIR_S : tiles_[xx][yy] == '\\' ? DIR_N : DIR_W;
          break;
      }
      dir = next_dir;
      switch (dir) {
        case DIR_N: yy--; break;
        case DIR_E: xx++; break;
        case DIR_S: yy++; break;
        case DIR_W: xx--; break;
      }
      if ((d == DIR_E && xx == border_e_ + 1 &&
           (border_e_ - border_w_) >= LINE_LENGTH) ||
          (d == DIR_S && yy == border_s_ + 1 &&
           (border_s_ - border_n_) >= LINE_LENGTH)) {
        // Line found !
        return true;
      }
    } while (tiles_[xx][yy] != ' ');

    return false;
  }

  /**
   * Trace board to find eight line
   */
  bool TraceLine() {
    bool found = false;
    // horizontal candidate!
    for (int y = border_n_; y <= border_s_; y++) {
      if (tiles_[border_w_ + 1][y] != ' ') {
        if (TraceLine(border_w_ + 1, y, DIR_E)) {
          found = true;
        }
      }
    }
    // vertical candidate!
    for (int x = border_w_; x <= border_e_; x++) {
      if (tiles_[x][border_n_ + 1] != ' ') {
        if (TraceLine(x, border_n_ + 1, DIR_S)) {
          found = true;   
        }
      }
    }
    return found;
  }

  void CreateTimer() {
    get_around_colors_time_ = new Timer("GetAroundColors");
  }

  void DestroyTimer() {
    delete get_around_colors_time_;
  }


 public:

  //----------------------------------------------------------------------------
  // Members
  //----------------------------------------------------------------------------

  // for debug
  int &left = border_w_, &right = border_e_;
  int &top = border_n_, &bottom = border_s_;


  //----------------------------------------------------------------------------
  // Methods
  //----------------------------------------------------------------------------

  /**
   * Constractor
   */
  Board() :
      border_n_(BOARD_CENTER),
      border_e_(BOARD_CENTER),
      border_s_(BOARD_CENTER),
      border_w_(BOARD_CENTER) {
     for (int x = 0; x < BOARD_MAX; x++) {
      for (int y = 0; y < BOARD_MAX; y++) {
        tiles_[x][y] = ' ';
        colors_[x][y] = 0;
      }
      CreateTimer();
    }
  }

  /**
   * Destructor
   */
  ~Board() {
    DestroyTimer();
  }

  /**
   * Check whether (x, y) is empty
   */
  inline bool IsEmpty(const int x, const int y) {
    return (tiles_[x][y] == ' ');
  }

  /**
   * Check whether (x, y) is isolated
   */
  inline bool IsIsolated(const int x, const int y) {
    if (tiles_[x - 1][y] == ' ' && tiles_[x + 1][y] == ' ' &&
        tiles_[x][y - 1] == ' ' && tiles_[x][y + 1] == ' ') {
      return true;
    }
    return false;
  }

  /**
   * Check to set a move on board
   */
  bool IsValidMove(const int x, const int y, const char tile) {
#if 0  // USE_SAFETY_CHECK
    if (!IsEmpty(x, y)) return false;
    if (IsIsolated(x, y)) return false;
#endif  // end USE_SAFETY_CHECK
    if (!IsLineColorConnected(x, y, tile)) return false;
    return true;
  }

  /**
   * Check to set a move on board
   */
  bool IsValidMove(const move m) {
    return IsValidMove(m.x + border_w_, m.y + border_n_, m.tile);
    // return IsValidMove(m.x, m.y, m.tile);
  }

  /**
   * Seach and set forced moves
   */
  bool ScanForced() {
    for(int y = border_n_; y <= border_s_; y++) {
      for(int x = border_w_; x <= border_e_; x++) {
        if(colors_[x][y] != 0) continue;
        int col_n, col_e, col_s, col_w;
        GetAroundColors(x, y, col_n, col_e, col_s, col_w);
        char forced = ' ';
        if ((col_w == col_n && col_n != COL_CLEAR) ||
            (col_s == col_e && col_e != COL_CLEAR)) {
          forced = '/';
        }
        if ((col_e == col_n && col_n != COL_CLEAR) ||
            (col_s == col_w && col_w != COL_CLEAR)) {
          forced = '\\';
        }
        if ((col_w == col_e && col_e != COL_CLEAR &&
             col_e != col_s && col_e != col_n) ||
            (col_n == col_s && col_s != COL_CLEAR &&
             col_s != col_w && col_s != col_w)) {
          forced = '+';
        }
        if (forced != ' ') {
          SetMove(x, y, forced);
          return true; 
        }
      }
    }
    // No more forced play here
    return false;
  }

  /**
   * Set move on board
   */
  bool SetMove(const int x, const int y, const char tile) {
    if (x == border_w_) border_w_--;
    if (y == border_n_) border_n_--;
    if (x > border_e_) border_e_ = x;
    if (y > border_s_) border_s_ = y;
    tiles_[x][y] = tile;
    colors_[x][y] = GetColor(x, y, tile);
    printf("SetTile(%d, %d, '%c');\n", x, y, tile);
    ScanForced();
    return true;
  }

  /**
   * Set move on board
   */
  bool SetMove(move m) {
    return SetMove(m.x + border_w_, m.y + border_n_, m.tile);
  }

  /**
   * Get tile shape
   */
  inline int GetTileShape(int x, int y) {
    return tiles_[x][y];
  }

  /**
   * Get tile color
   */
  inline int GetTileColor(int x, int y) {
    return colors_[x][y];
  }

  /**
   * Get tile data
   */
  inline void GetTile(const int x, const int y, char &shape, int &color) {
    shape = GetTileShape(x, y);
    color = GetTileColor(x, y);
  }
  
  /**
   * Copy board from Osana-sensei's trax form
   */
  void CopyBoard(
      char tiles[BOARD_MAX][BOARD_MAX], char colors[BOARD_MAX][BOARD_MAX],
      const int left, const int right, const int top, const int bottom) {
    memcpy(tiles_, tiles, sizeof(char) * BOARD_MAX * BOARD_MAX);
    memcpy(colors_, colors, sizeof(char) * BOARD_MAX * BOARD_MAX);
    border_n_ = top;
    border_e_ = right;
    border_s_ = bottom;
    border_w_ = left;
  }

  Board& operator=(Board &board) {
    memcpy(this->tiles_, board.tiles_, sizeof(char) * BOARD_MAX * BOARD_MAX);
    memcpy(this->colors_, board.colors_, sizeof(char) * BOARD_MAX * BOARD_MAX);
    this->border_n_ = board.border_n_;
    this->border_e_ = board.border_e_;
    this->border_s_ = board.border_s_;
    this->border_w_ = board.border_w_;
    return *this;
  }

  void PrintBorder() {
    printf("Border (N, E, S, W): (%d, %d, %d, %d)\n",
           border_n_, border_e_, border_s_, border_w_);
  }

  /**
   * Print board data for debug
   */
  void PrintBoard() {
    printf("    ");
    for (int x = border_w_; x <= border_e_; x++) {
      printf("%2d", x % 10);
    }
    printf("\n");
    printf("    ");
    for (int x = border_w_; x <= border_e_; x++) {
      printf("--");
    }
    printf("\n");
    for (int y = border_n_; y <= border_s_; y++) {
      printf("%2d |", y);
      for (int x = border_w_; x <= border_e_; x++) {
        int color = GetTileColor(x, y);
        char shape = GetTileShape(x, y);
        printf("%c%c", (shape == ' ') ? ' ' : shape,
               (color == COL_CLEAR) ? ' ' : color + '0');
      }
      printf("|\n");
    }
    printf("    ");
    for (int x = border_w_; x <= border_e_; x++) {
      printf("--");
    }
    printf("\n");
  }
  
  inline void PrintFullLine() {
    printf(" ");
    for (int x = 0; x < BOARD_MAX; x++) {
      printf("--");
    }
    printf("\n");    
  }

  /**
   * Print full board data for debug
   */
  void PrintFullBoard() {
    PrintFullLine();
    for (int y = 0; y < BOARD_MAX; y++) {
      printf("|");
      for (int x = 0; x < BOARD_MAX; x++) {
        int color = GetTileColor(x, y);
        char shape = GetTileShape(x, y);
        printf("%c%c", (shape == ' ') ? ' ' : shape,
               (color == COL_CLEAR) ? ' ' : color + '0');
      }
      printf("|\n");
    }
    PrintFullLine();
    printf("\n");
  }

  /**
   * Print profile data
   */
  void PrintProfile() {
    get_around_colors_time_->PrintTime();
  }
};


#endif  // end BOARD_HPP_
