#ifndef BOARD_HPP_
#define BOARD_HPP_


#include <stdio.h>
#include <string.h>

#include "trax.h"
#include "timer.hpp"


/**
 * blocks_ is row-major array
 */
class Board {

 protected:

  //----------------------------------------------------------------------------
  // Typedefs and Constants
  //----------------------------------------------------------------------------
  
  enum BoardRestriction {
    BOARD_MAX = 100,
    BOARD_CENTER = BOARD_MAX / 2,
    LINE_LENGTH = 8,
  };

  enum BitField {
    FIELD_TILE   = 0xf,  // 4bit [3:0]
    FIELD_PLACED = 0x10, // 1bit [4:4]
  };
  
  enum ColorPattern {
    COL_CLEAR = 0,
    COL_WHITE = 1,
    COL_RED   = 2,
  };

  /**
   * N: North, E: East, S: South, W: West
   */
  enum DirectionPattern {
    DIR_N = 0,
    DIR_E = 1,
    DIR_S = 2,
    DIR_W = 3,
  };

  /**
   * The bit-order is (W,S,E,N), N is LSB.
   * If the bit is one, the direction's color is red.
   */
  enum TilePattern {
    TILE_SPACE  = 0x0,  // ' ' (0000)
    TILE_RED_NS = 0x5,  // '+' (0101)
    TILE_RED_EW = 0xa,  // '+' (1010)
    TILE_RED_WN = 0x9,  // '/' (1001)
    TILE_RED_ES = 0x6,  // '/' (0110)
    TILE_RED_SW = 0xc,  // '\' (1100)
    TILE_RED_NE = 0x3,  // '\' (0011)
  };

  /**
   * Sum of tile-patterns constructed loop candidate
   *
   * WHITE or RED: loop color
   * 2 or 3:       # of tiles that construct loop
   * N, E, S, W:   loop direction
   */
  enum LoopPattern {
    LOOP_WHITE_2_S = TILE_RED_WN + TILE_RED_NE,
    LOOP_WHITE_2_N = TILE_RED_SW + TILE_RED_ES,
    LOOP_WHITE_2_E = TILE_RED_WN + TILE_RED_SW,
    LOOP_WHITE_2_W = TILE_RED_NE + TILE_RED_ES,
    LOOP_WHITE_3_S = LOOP_WHITE_2_S + TILE_RED_NS,
    LOOP_WHITE_3_N = LOOP_WHITE_2_N + TILE_RED_NS,
    LOOP_WHITE_3_E = LOOP_WHITE_2_E + TILE_RED_EW,
    LOOP_WHITE_3_W = LOOP_WHITE_2_W + TILE_RED_EW,
    LOOP_RED_2_S = TILE_RED_ES + TILE_RED_SW,
    LOOP_RED_2_N = TILE_RED_NE + TILE_RED_WN,
    LOOP_RED_2_E = TILE_RED_ES + TILE_RED_NE,
    LOOP_RED_2_W = TILE_RED_SW + TILE_RED_WN,
    LOOP_RED_3_S = LOOP_RED_2_S + TILE_RED_EW,
    LOOP_RED_3_N = LOOP_RED_2_N + TILE_RED_EW,
    LOOP_RED_3_E = LOOP_RED_2_E + TILE_RED_NS,
    LOOP_RED_3_W = LOOP_RED_2_W + TILE_RED_NS,
  };


  //----------------------------------------------------------------------------
  // Members
  //----------------------------------------------------------------------------

  // Board
  char blocks_[BOARD_MAX][BOARD_MAX];
  int border_n_, border_e_, border_s_, border_w_;

  // Timers
  Timer *set_move_time_;
  Timer *is_valid_move_time_;
  Timer *get_around_colors_time_;
  Timer *detect_loop_time_;


  //----------------------------------------------------------------------------
  // Methods
  //----------------------------------------------------------------------------

  /**
   * Some primitive getters from a block
   */
  static inline int IsPlaced(char block) {
    return (block >> 4) & 0x1;
    // return block & FIELD_PLACED;
  }
  static inline int GetColorN(char block) {
    return ((block >> 0) & 0x1) + IsPlaced(block);
  }
  static inline int GetColorE(char block) {
    return ((block >> 1) & 0x1) + IsPlaced(block);
  }
  static inline int GetColorS(char block) {
    return ((block >> 2) & 0x1) + IsPlaced(block);
  }
  static inline int GetColorW(char block) {
    return ((block >> 3) & 0x1) + IsPlaced(block);
  }
  static inline char GetTileField(char block) {
    return block & FIELD_TILE;
  }

  /**
   * Some primitive getters from coordinates
   */
  inline int IsPlaced(int x, int y) { return IsPlaced(blocks_[y][x]); }
  inline int GetColorN(int x, int y) { return GetColorN(blocks_[y][x]); }
  inline int GetColorE(int x, int y) { return GetColorE(blocks_[y][x]); }
  inline int GetColorS(int x, int y) { return GetColorS(blocks_[y][x]); }
  inline int GetColorW(int x, int y) { return GetColorW(blocks_[y][x]); }
  inline char GetTileField(int x, int y) { return GetTileField(blocks_[y][x]); }
   
  /**
   * Get opposite color of argument color
   */
  static inline int GetOppositeColor(const int color) {
    return (color == COL_WHITE) ? COL_RED :
        (color == COL_RED) ? COL_WHITE : COL_CLEAR;
  }

  /**
   * Get around (N, E, S, W) colors of (x, y)
   */
  inline void GetAroundColors(const int x, const int y,
                              int &col_n, int &col_e,
                              int &col_s, int &col_w) {
    get_around_colors_time_->Start();
    col_n = GetColorS(x, y - 1);
    col_e = GetColorW(x + 1, y);
    col_s = GetColorN(x, y + 1);
    col_w = GetColorE(x - 1, y);
    get_around_colors_time_->Stop();
  }

  /**
   * Get (x, y)'s color
   */
  inline int GetColor(const char shape,
                      const int col_n, const int col_e,
                      const int col_s, const int col_w) {
    if (shape == '+') {
      return (col_w == COL_WHITE || col_e == COL_WHITE ||
              col_n == COL_RED || col_s == COL_RED) ? COL_WHITE : COL_RED;
    } else if (shape == '/') {
      return (col_w == COL_WHITE || col_n == COL_WHITE ||
              col_s == COL_RED || col_e == COL_RED) ? COL_RED : COL_WHITE;
    } else if (shape == '\\') {
      return (col_w == COL_WHITE || col_s == COL_WHITE ||
              col_n == COL_RED || col_e == COL_RED) ? COL_RED : COL_WHITE;
    }
    return COL_CLEAR;
  }

  /**
   * Get (x, y)'s color
   */
  inline int GetColor(const int x, const int y, const char shape) {
    int col_n, col_e, col_s, col_w;
    GetAroundColors(x, y, col_n, col_e, col_s, col_w);
    return GetColor(shape, col_n, col_e, col_s, col_w);
  }

  /**
   * Check whether (x, y) is consistent placement
   */
  inline bool IsConsistentPlacement(int x, int y, char shape) {
    int col_n, col_e, col_s, col_w;
    GetAroundColors(x, y, col_n, col_e, col_s, col_w);
    if (shape == '+') {
      if ((col_w != col_e && (col_w != COL_CLEAR && col_e != COL_CLEAR)) ||
          (col_n != col_s && (col_n != COL_CLEAR && col_s != COL_CLEAR))) {
        return false;
      }
    }
    if (shape == '/') {
      if ((col_w != col_n && (col_w != COL_CLEAR && col_n != COL_CLEAR)) ||
          (col_s != col_e && (col_s != COL_CLEAR && col_e != COL_CLEAR))) {
        return false;
      }
    }
    if (shape == '\\') {
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
  inline bool IsLineColorConnected(const int x, const int y, const char shape) {
    int col_n, col_e, col_s, col_w;
    int my_col_n, my_col_e, my_col_s, my_col_w;
    GetAroundColors(x, y, col_n, col_e, col_s, col_w);
    my_col_e = GetColor(x, y, shape);
    my_col_w = (shape == '+')  ? my_col_e : GetOppositeColor(my_col_e);
    my_col_n = (shape == '\\') ? my_col_e : GetOppositeColor(my_col_e);
    my_col_s = (shape == '/')  ? my_col_e : GetOppositeColor(my_col_e);
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

  void CreateTimer() {
    set_move_time_ = new Timer("SetMove");
    is_valid_move_time_ = new Timer("IsValidMove");
    get_around_colors_time_ = new Timer("GetAroundColors");
    detect_loop_time_ = new Timer("Detect{White,Red}Loop");
  }

  void DestroyTimer() {
    delete set_move_time_;
    delete is_valid_move_time_;
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
  explicit Board() :
      border_n_(BOARD_CENTER),
      border_e_(BOARD_CENTER),
      border_s_(BOARD_CENTER),
      border_w_(BOARD_CENTER) {
    memset(blocks_, 0, sizeof(char) * BOARD_MAX * BOARD_MAX);
    CreateTimer();
  }

  /**
   * Destructor
   */
  ~Board() {
    DestroyTimer();
  }

  static inline char GetTileShape(char block) {
    switch (block & FIELD_TILE) {
      case TILE_RED_NS: return '+'; break;
      case TILE_RED_EW: return '+'; break;
      case TILE_RED_WN: return '/'; break;
      case TILE_RED_ES: return '/'; break;
      case TILE_RED_SW: return '\\'; break;
      case TILE_RED_NE: return '\\'; break;
      default: return ' ';
    }
    return ' ';
  }

  inline char GetTileShape(int x, int y) {  
    return GetTileShape(blocks_[y][x]);
  }
    
  /**
   * Return east color of tile
   */
  static inline int GetTileColor(char block) {
    switch (block & FIELD_TILE) {
      case TILE_RED_NS: return COL_WHITE; break;
      case TILE_RED_EW: return COL_RED; break;
      case TILE_RED_WN: return COL_WHITE; break;
      case TILE_RED_ES: return COL_RED; break;
      case TILE_RED_SW: return COL_WHITE; break;
      case TILE_RED_NE: return COL_RED; break;
      default: return COL_CLEAR;
    }
    return COL_CLEAR;
  }

  inline char GetTileColor(int x, int y) {  
    return GetTileColor(blocks_[y][x]);
  }

  inline char GetTileFormat(int x, int y, char shape) {
    int color = GetTileColor(x, y);
    char ret_tile = TILE_SPACE;
    if (color == COL_WHITE) {
      switch (shape) {
        case '+':  ret_tile = TILE_RED_NS; break;
        case '/':  ret_tile = TILE_RED_WN; break;
        case '\\': ret_tile = TILE_RED_SW; break;
      }
    } else {
      switch (shape) {
        case '+':  ret_tile = TILE_RED_EW; break;
        case '/':  ret_tile = TILE_RED_ES; break;
        case '\\': ret_tile = TILE_RED_NE; break;
      }
    }
    return ret_tile;
  }

  inline char GetTileFormat(const move m) {
    return GetTileFormat(m.x + border_w_, m.y + border_n_, m.tile);
  }

  static inline char FlipTileHorizontal(const char tile) {
    char flipped = tile;
    switch (tile) {
      case TILE_RED_ES: flipped = TILE_RED_SW; break;
      case TILE_RED_SW: flipped = TILE_RED_ES; break;
      case TILE_RED_WN: flipped = TILE_RED_NE; break;
      case TILE_RED_NE: flipped = TILE_RED_WN; break;
    }
    return flipped;
  }

  static inline char FlipTileVertical(const char tile) {
    char flipped = tile;
    switch (tile) {
      case TILE_RED_ES: flipped = TILE_RED_NE; break;
      case TILE_RED_NE: flipped = TILE_RED_ES; break;
      case TILE_RED_WN: flipped = TILE_RED_SW; break;
      case TILE_RED_SW: flipped = TILE_RED_WN; break;
    }
    return flipped;
  }
  
  /**
   * Check whether (x, y) is empty
   */
  inline bool IsEmpty(const int x, const int y) {
    return !IsPlaced(x, y);
  }

  /**
   * Check whether (x, y) is isolated
   * NOTE: x > 0, y > 0
   */
  inline bool IsIsolated(const int x, const int y) {
    if (blocks_[y][x - 1] == 0 && blocks_[y][x + 1] == 0 &&
        blocks_[y - 1][x] == 0 && blocks_[y + 1][x] == 0) {
      return true;
    }
    return false;
  }

  /**
   * Check to set a move on board
   */
  bool IsValidMove(const int x, const int y, const char shape) {
    is_valid_move_time_->Start();
    bool is_valid = true;
    do {
#if 0  // USE_SAFETY_CHECK
      if (IsPlaced(x, y)) {
        is_valid = false;
        break;
      }
      if (IsIsolated(x, y)) {
        is_valid = false;
        break;
      }
#endif  // end USE_SAFETY_CHECK
      if (!IsLineColorConnected(x, y, shape)) {
        is_valid = false;
        break;
      }
      if (IsProhibited3(x, y)) {
        is_valid = false;
        break;
      }
      if (!IsConsistentPlacement(x, y, shape)) {
        is_valid = false;
        break;
      }
    } while (0);    
    is_valid_move_time_->Stop();
    return is_valid;
  }

  /**
   * Check to set a move on board
   */
  bool IsValidMove(const move m) {
    is_valid_move_time_->Start();
    return IsValidMove(m.x + border_w_, m.y + border_n_, m.tile);
    is_valid_move_time_->Stop();
  }

  /**
   * Seach and set forced moves
   */
  bool ScanForced() {
    for(int y = border_n_; y <= border_s_; y++) {
      for(int x = border_w_; x <= border_e_; x++) {
        if(IsPlaced(blocks_[y][x])) continue;
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
   * Set a tile on board
   */
  void SetTile(int x, int y, char shape) {
    int color = GetColor(x, y, shape);
    if (shape == '+' && color == COL_WHITE) {
      blocks_[y][x] = FIELD_PLACED | TILE_RED_NS;
    } else if (shape == '+' && color == COL_RED) {
      blocks_[y][x] = FIELD_PLACED | TILE_RED_EW;
    } else if (shape == '/' && color == COL_WHITE) {
      blocks_[y][x] = FIELD_PLACED | TILE_RED_WN;
    } else if (shape == '/' && color == COL_RED) {
      blocks_[y][x] = FIELD_PLACED | TILE_RED_ES;
    } else if (shape == '\\' && color == COL_WHITE) {
      blocks_[y][x] = FIELD_PLACED | TILE_RED_SW;
    } else if (shape == '\\' && color == COL_RED) {
      blocks_[y][x] = FIELD_PLACED | TILE_RED_NE;
    }
  }

  /**
   * Set move on board
   */
  bool SetMove(const int x, const int y, const char shape) {
    set_move_time_->Start();
    if (x == border_w_) border_w_--;
    if (y == border_n_) border_n_--;
    if (x > border_e_) border_e_ = x;
    if (y > border_s_) border_s_ = y;
    SetTile(x, y, shape);
    // printf("in ScanForced (%d, %d, %c)\n", x, y, shape);
    ScanForced();
    set_move_time_->Stop();
    return true;
  }

  /**
   * Set move on board
   */
  bool SetMove(move m) {
    return SetMove(m.x + border_w_, m.y + border_n_, m.tile);
  }

  inline void CheckWhiteLoopPatterns(
      const int x, const int y,
      char &loop_2_s, char &loop_2_n, char &loop_2_e, char &loop_2_w,
      char &loop_3_s, char &loop_3_n, char &loop_3_e, char &loop_3_w) {
    loop_2_s = IsEmpty(x + 1, y + 0) &&
        GetTileField(x + 0, y - 1) == TILE_RED_WN &&
        GetTileField(x + 1, y - 1) == TILE_RED_NE;
    loop_2_n = IsEmpty(x + 1, y + 0) &&
        GetTileField(x + 0, y + 1) == TILE_RED_SW &&
        GetTileField(x + 1, y + 1) == TILE_RED_ES;
    loop_2_e = IsEmpty(x + 0, y + 1) &&
        GetTileField(x - 1, y + 0) == TILE_RED_WN &&
        GetTileField(x - 1, y + 1) == TILE_RED_SW;
    loop_2_w = IsEmpty(x + 0, y + 1) &&
        GetTileField(x + 1, y + 0) == TILE_RED_NE &&
        GetTileField(x + 1, y + 1) == TILE_RED_ES;
    loop_3_s = IsEmpty(x + 1, y + 0) && IsEmpty(x + 2, y + 0) &&
        GetTileField(x + 0, y - 1) == TILE_RED_WN &&
        GetTileField(x + 1, y - 1) == TILE_RED_NS &&
        GetTileField(x + 2, y - 1) == TILE_RED_NE;
    loop_3_n = IsEmpty(x + 1, y + 0) && IsEmpty(x + 2, y + 0) &&
        GetTileField(x + 0, y + 1) == TILE_RED_SW &&
        GetTileField(x + 1, y + 1) == TILE_RED_NS &&
        GetTileField(x + 2, y + 1) == TILE_RED_ES;
    loop_3_e = IsEmpty(x + 0, y + 1) && IsEmpty(x + 0, y + 2) &&
        GetTileField(x - 1, y + 0) == TILE_RED_WN &&
        GetTileField(x - 1, y + 1) == TILE_RED_EW &&
        GetTileField(x - 1, y + 2) == TILE_RED_SW;
    loop_3_w = IsEmpty(x + 0, y + 1) && IsEmpty(x + 0, y + 2) &&
        GetTileField(x + 1, y + 0) == TILE_RED_NE &&
        GetTileField(x + 1, y + 1) == TILE_RED_EW &&
        GetTileField(x + 1, y + 2) == TILE_RED_ES;
  }

  inline void CheckRedLoopPatterns(
      const int x, const int y,
      char &loop_2_s, char &loop_2_n, char &loop_2_e, char &loop_2_w,
      char &loop_3_s, char &loop_3_n, char &loop_3_e, char &loop_3_w) {
    loop_2_s = IsEmpty(x + 1, y + 0) &&
        GetTileField(x + 0, y - 1) == TILE_RED_ES &&
        GetTileField(x + 1, y - 1) == TILE_RED_SW;
    loop_2_n = IsEmpty(x + 1, y + 0) &&
        GetTileField(x + 0, y + 1) == TILE_RED_NE &&
        GetTileField(x + 1, y + 1) == TILE_RED_WN;
    loop_2_e = IsEmpty(x + 0, y + 1) &&
        GetTileField(x - 1, y + 0) == TILE_RED_ES &&
        GetTileField(x - 1, y + 1) == TILE_RED_NE;
    loop_2_w = IsEmpty(x + 0, y + 1) &&
        GetTileField(x + 1, y + 0) == TILE_RED_SW &&
        GetTileField(x + 1, y + 1) == TILE_RED_WN;
    loop_3_s = IsEmpty(x + 1, y + 0) && IsEmpty(x + 2, y + 0) &&
        GetTileField(x + 0, y - 1) == TILE_RED_ES &&
        GetTileField(x + 1, y - 1) == TILE_RED_EW &&
        GetTileField(x + 2, y - 1) == TILE_RED_SW;
    loop_3_n = IsEmpty(x + 1, y + 0) && IsEmpty(x + 2, y + 0) &&
        GetTileField(x + 0, y + 1) == TILE_RED_NE &&
        GetTileField(x + 1, y + 1) == TILE_RED_EW &&
        GetTileField(x + 2, y + 1) == TILE_RED_WN;
    loop_3_e = IsEmpty(x + 0, y + 1) && IsEmpty(x + 0, y + 2) &&
        GetTileField(x - 1, y + 0) == TILE_RED_ES &&
        GetTileField(x - 1, y + 1) == TILE_RED_NS &&
        GetTileField(x - 1, y + 2) == TILE_RED_NE;
    loop_3_w = IsEmpty(x + 0, y + 1) && IsEmpty(x + 0, y + 2) &&
        GetTileField(x + 1, y + 0) == TILE_RED_SW &&
        GetTileField(x + 1, y + 1) == TILE_RED_NS &&
        GetTileField(x + 1, y + 2) == TILE_RED_WN;
  }
  
  /**
   * Detect white small loop candidate.
   * Return a tile pattern that avoid making loop.
   * If no loop candidate is detected, return TILE_SPACE.
   */
  inline char DetectWhiteLoop(const int x, const int y) {
    detect_loop_time_->Start();
    char loop_2_s, loop_2_n, loop_2_e, loop_2_w;
    char loop_3_s, loop_3_n, loop_3_e, loop_3_w;
    char ret_tile = TILE_SPACE;
    CheckWhiteLoopPatterns(x, y, loop_2_s, loop_2_n, loop_2_e, loop_2_w,
                           loop_3_s, loop_3_n, loop_3_e, loop_3_w);
    do {
      if (loop_2_s || loop_2_e) {
        ret_tile = TILE_RED_ES;
        break;
      }
      if (loop_2_n || loop_2_w) {
        ret_tile = TILE_RED_NE;
        break;
      }
      if (loop_3_s || loop_3_e) {
        break;
      }
      if (loop_3_n || loop_3_w) {
        break;
      }
    } while (0);
    detect_loop_time_->Stop();
    return ret_tile;
  }

  /**
   * Select move atack/defence with loop atack
   * If atack (checkmate) then return 1, else if defence then return 2
   * else return 0
   */
  inline int SelectLoopAtackMove(
      const int player, const int x, const int y,
      move &selected_move0, move &selected_move1) {
    char loop_2_s, loop_2_n, loop_2_e, loop_2_w;
    char loop_3_s, loop_3_n, loop_3_e, loop_3_w;
    int ret_val = 0;
    CheckWhiteLoopPatterns(x + border_w_, y + border_n_,
                           loop_2_s, loop_2_n, loop_2_e, loop_2_w,
                           loop_3_s, loop_3_n, loop_3_e, loop_3_w);
    if (player == COL_WHITE) {
      // White player checkmate
      if (loop_2_s || loop_2_e) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_SW);
        ret_val = 1;
      }
      if (loop_2_n || loop_2_w) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_WN);
        ret_val = 1;
      }
      if (loop_3_s || loop_3_n) {
        selected_move0.x = x + 1;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_NS);
        ret_val = 1;
      }
      if (loop_3_e || loop_3_w) {
        selected_move0.x = x;
        selected_move0.y = y + 1;
        selected_move0.tile = GetTileShape(TILE_RED_EW);
        ret_val = 1;
      }
      if (loop_2_s || loop_2_e || loop_3_s || loop_3_e ||
          loop_2_n || loop_2_w || loop_3_n || loop_3_w) {
        printf("-------- Detect white loop at (%d, %d), checkmate!\n",
               selected_move0.x, selected_move0.y);
      }
    } else {
      // Red player defence
      if (loop_2_s) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_ES);
        selected_move1.x = x + 1;
        selected_move1.y = y;
        selected_move1.tile = GetTileShape(TILE_RED_SW);
        ret_val = 2;
      }
      if (loop_2_n) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_NE);
        selected_move1.x = x + 1;
        selected_move1.y = y;
        selected_move1.tile = GetTileShape(TILE_RED_WN);
        ret_val = 2;
      }
      if (loop_2_e) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_ES);
        selected_move1.x = x;
        selected_move1.y = y + 1;
        selected_move1.tile = GetTileShape(TILE_RED_NE);
        ret_val = 2;
      }
      if (loop_2_w) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_SW);
        selected_move1.x = x;
        selected_move1.y = y + 1;
        selected_move1.tile = GetTileShape(TILE_RED_WN);
        ret_val = 2;
      }
      if (loop_3_s) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_ES);
        selected_move1.x = x + 2;
        selected_move1.y = y;
        selected_move1.tile = GetTileShape(TILE_RED_SW);
        ret_val = 2;
      }
      if (loop_3_n) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_NE);
        selected_move1.x = x + 2;
        selected_move1.y = y;
        selected_move1.tile = GetTileShape(TILE_RED_WN);
        ret_val = 2;
      }
      if (loop_3_e) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_ES);
        selected_move1.x = x;
        selected_move1.y = y + 2;
        selected_move1.tile = GetTileShape(TILE_RED_NE);
        ret_val = 2;
      }
      if (loop_3_w) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_SW);
        selected_move1.x = x;
        selected_move1.y = y + 2;
        selected_move1.tile = GetTileShape(TILE_RED_WN);
        ret_val = 2;
      }
      if (loop_2_s || loop_2_e || loop_3_s || loop_3_e ||
          loop_2_n || loop_2_w || loop_3_n || loop_3_w) {
        printf("-------- Detect red loop at (%d, %d), defence!\n",
               selected_move0.x, selected_move0.y);
      }
    }
    if (ret_val == 1) return ret_val;
    CheckRedLoopPatterns(x + border_w_, y + border_n_,
                         loop_2_s, loop_2_n, loop_2_e, loop_2_w,
                         loop_3_s, loop_3_n, loop_3_e, loop_3_w);
    if (player == COL_RED) {
      // Red player checkmate
      if (loop_2_s || loop_2_e) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_NE);
        ret_val = 1;
      }
      if (loop_2_n || loop_2_w) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_ES);
        ret_val = 1;
      }
      if (loop_3_s || loop_3_n) {
        selected_move0.x = x + 1;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_EW);
        ret_val = 1;
      }
      if (loop_3_e || loop_3_w) {
        selected_move0.x = x;
        selected_move0.y = y + 1;
        selected_move0.tile = GetTileShape(TILE_RED_NS);
        ret_val = 1;
      }
      if (loop_2_s || loop_2_e || loop_3_s || loop_3_e ||
          loop_2_n || loop_2_w || loop_3_n || loop_3_w) {
        printf("-------- Detect red loop at (%d, %d), checkmate!\n",
               selected_move0.x, selected_move0.y);
      }
    } else {
      // White player defence
      if (loop_2_s) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_WN);
        selected_move1.x = x + 1;
        selected_move1.y = y;
        selected_move1.tile = GetTileShape(TILE_RED_NE);
        ret_val = 2;
      }
      if (loop_2_n) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_SW);
        selected_move1.x = x + 1;
        selected_move1.y = y;
        selected_move1.tile = GetTileShape(TILE_RED_ES);
        ret_val = 2;
      }
      if (loop_2_e) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_WN);
        selected_move1.x = x;
        selected_move1.y = y + 1;
        selected_move1.tile = GetTileShape(TILE_RED_SW);
        ret_val = 2;
      }
      if (loop_2_w) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_ES);
        selected_move1.x = x;
        selected_move1.y = y + 1;
        selected_move1.tile = GetTileShape(TILE_RED_NE);
        ret_val = 2;
      }
      if (loop_3_s) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_WN);
        selected_move1.x = x + 2;
        selected_move1.y = y;
        selected_move1.tile = GetTileShape(TILE_RED_NE);
        ret_val = 2;
      }
      if (loop_3_n) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_SW);
        selected_move1.x = x + 2;
        selected_move1.y = y;
        selected_move1.tile = GetTileShape(TILE_RED_ES);
        ret_val = 2;
      }
      if (loop_3_e) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_WN);
        selected_move1.x = x;
        selected_move1.y = y + 2;
        selected_move1.tile = GetTileShape(TILE_RED_SW);
        ret_val = 2;
      }
      if (loop_3_w) {
        selected_move0.x = x;
        selected_move0.y = y;
        selected_move0.tile = GetTileShape(TILE_RED_ES);
        selected_move1.x = x;
        selected_move1.y = y + 2;
        selected_move1.tile = GetTileShape(TILE_RED_NE);
        ret_val = 2;
      }
      if (loop_2_s || loop_2_e || loop_3_s || loop_3_e ||
          loop_2_n || loop_2_w || loop_3_n || loop_3_w) {
        printf("-------- Detect white loop at (%d, %d), defence!\n",
               selected_move0.x, selected_move0.y);
      }
    }
    if (ret_val) return ret_val;
    
    // Not found loop atack
    selected_move0.x = 0;
    selected_move0.y = 0;
    selected_move0.tile = '/';
    return 0;
  }
  
  /**
   * Detect red small loop candidate.
   * Return a tile pattern that avoid maiking loop.
   * If no loop candidate is detected, return TILE_SPACE.
   */
  inline char DetectRedLoop(const int x, const int y) {
    detect_loop_time_->Start();
    char loop_2_s, loop_2_n, loop_2_e, loop_2_w;
    char loop_3_s, loop_3_n, loop_3_e, loop_3_w;
    char ret_tile = TILE_SPACE;
    CheckRedLoopPatterns(x, y, loop_2_s, loop_2_n, loop_2_e, loop_2_w,
                         loop_3_s, loop_3_n, loop_3_e, loop_3_w);
    if (loop_2_s || loop_2_e || loop_3_s || loop_3_e) {
      ret_tile = TILE_RED_WN;
    }
    if (loop_2_n || loop_2_w || loop_3_n || loop_3_w) {
      ret_tile = TILE_RED_SW;
    }
    detect_loop_time_->Stop();
    return ret_tile;
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
   * Print full board data for debug
   */
  void PrintFullBoardDebug() {
    PrintFullLine();
    for (int y = 0; y < BOARD_MAX; y++) {
      printf("|");
      for (int x = 0; x < BOARD_MAX; x++) {
        if (blocks_[y][x]) {
          printf("%2d", blocks_[y][x]);
        } else {
          printf("  ");
        }
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
    set_move_time_->PrintTime();
    is_valid_move_time_->PrintTime();
    get_around_colors_time_->PrintTime();
    detect_loop_time_->PrintTime();
  }
};


#endif  // end BOARD_HPP_
