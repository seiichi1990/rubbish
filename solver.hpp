#ifndef TRAX_SOLVER_HPP_
#define TRAX_SOLVER_HPP_


#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "trax.h"
#include "test_board.hpp"
#include "board.hpp"
// #include "board_osana.hpp"
#include "timer.hpp"

#define FIRST_MOVE_0 "@0/"
#define FIRST_MOVE_1 "@0+"

class TraxSolver {
 private:

  //----------------------------------------------------------------------------
  // Typedefs and Constants
  //----------------------------------------------------------------------------

  static const int PLAYER_WHITE = 1;
  static const int PLAYER_RED = 2;
  static const int BOARD_MAX = 100;
  static const int BOARD_MAX_HALF = BOARD_MAX / 2;
  static const int TILE_PATTERNS = 3;


  //----------------------------------------------------------------------------
  // Members
  //----------------------------------------------------------------------------

  int player_;
  Board board_;

  // Timers
  Timer *think_time_;

  
  //----------------------------------------------------------------------------
  // Methods
  //----------------------------------------------------------------------------

  inline char GetTileShape(int tile_id) {
    return (tile_id == 0) ? '+' : (tile_id == 1) ? '/' : '\\';
  }

  /**
   * Pick up all valid moves
   */
  void GatherValidMoves(std::vector<move> &valid_moves) {
    for (int y = -BOARD_MAX_HALF; y < BOARD_MAX_HALF; y++) {
      int yy = y + board_.top;      
      if (yy <= 0) continue;
      for (int x = -BOARD_MAX_HALF; x < BOARD_MAX_HALF; x++) {
        int xx = x + board_.left;
        if (xx <= 0) continue;
        if (!board_.IsEmpty(xx, yy)) continue;
        if (board_.IsIsolated(xx, yy)) continue;
        for (int t = 0; t < TILE_PATTERNS; t++) {
          char tile = GetTileShape(t);
          if (!board_.IsValidMove(xx, yy, tile)) continue;
          valid_moves.push_back(move(GetMoveString(x, y, tile)));
        }
      }
    }
    // printf("# valid moves: %ld\n", valid_moves.size());
  }
  
  move ThinkMoveRandom() {
    std::vector<move> valid_moves;
    GatherValidMoves(valid_moves);
    int num_moves = valid_moves.size();
    return valid_moves[rand() % num_moves];
  }  

  /**
   * Find a move that checkmate or defence loop atack
   * If no such move, return FIRST_MOVE_0
   */
  move LoopAtack(std::vector<move> &valid_moves) {
    int selected_x = 0, selected_y = 0;
    char selected_shape = '/';
    move selected_move("");
    for (std::vector<move>::iterator itr = valid_moves.begin();
         itr != valid_moves.end(); itr++) {
      const move m = *itr;
      move temp_move0(""), temp_move1("");
      int temp_x, temp_y;
      char temp_shape;
      int loop_atack_type = 0;
      // loop_atack_type = board_.SelectLoopAtackMove(
      //     player_, m.x , m.y, temp_x, temp_y, temp_shape);
      loop_atack_type = board_.SelectLoopAtackMove(
          player_, m.x , m.y, temp_move0, temp_move1);
      if (loop_atack_type == 0) {
        continue;
      } else if (loop_atack_type == 1) {  // if checkmate
        // return move(GetMoveString(temp_x, temp_y, temp_shape));
        selected_move = temp_move0;        
        return selected_move;
      } else {
        if (board_.IsValidMove(temp_move0)) {
          selected_move = temp_move0;
          continue;
        }
        printf("loop defence skip 0\n");
        if (board_.IsValidMove(temp_move1)) {
          selected_move = temp_move1;
          continue;
        }
        printf("loop defence candidate is error\n");
        // selected_move = temp_move0;
        // selected_x = temp_x;
        // selected_y = temp_y;
        // selected_shape = temp_shape;
      }
    }
    // return move(GetMoveString(selected_x, selected_y, selected_shape));
    return selected_move;
  }
  
  move ThinkMoveLoopAtack() {
    std::vector<move> valid_moves;
    GatherValidMoves(valid_moves);    
    int num_moves = valid_moves.size();
    move loop_atack_move = LoopAtack(valid_moves);
    if (loop_atack_move.x != 0 || loop_atack_move.y != 0) {
      return loop_atack_move;
    }
    return valid_moves[rand() % num_moves];
  }  
  
  void PrintProfile() {
    printf("Time (ms)\tCalls\tAvg (ms)\tMin (ms) \tMax (ms)\tName\n");
    think_time_->PrintTime();
    board_.PrintProfile();
  }
  
  
 public:

  //----------------------------------------------------------------------------
  // Methods
  //----------------------------------------------------------------------------
  
  TraxSolver(int player) :
      player_(player),
      think_time_ (new Timer("ThinkMove")) {
    // srand(0);
    srand(time(0));
    // TestBoard test_board;
    // test_board.TestGetColorX();
    // test_board.TestSetTile();
    // test_board.TestSetMove();
    // test_board.TestDetectLoop();
    // exit(0);
  }

  ~TraxSolver() {
    delete think_time_;
  }

  std::string GetMoveString(int x, int y, char tile) {
    static const int ALPHABETS = 26;
    // x string
    std::string str_x = "";
    if (x == 0) {
      str_x = "@";
    } else {
      char digit;
      char least_digit = x % ALPHABETS;
      for (; x > ALPHABETS; x /= ALPHABETS) {
        digit = 'A' + (x / ALPHABETS) - 1;
        str_x.append(std::string(1, digit));
      }
      digit = 'A' + least_digit - 1;
      str_x.append(std::string(1, digit));      
    }
    // y and tile string
    char buf[BUFSIZ];
    sprintf(buf, "%d%c", y, tile);
    return str_x.append(buf);
  }

  std::string GetMoveString(move m) {
    return GetMoveString(m.x, m.y, m.tile);
  }

  void MyTurn(int turn, const move opp_move, move &my_move) {
    if (turn == 0) {
      my_move = move(FIRST_MOVE_1);
      board_.SetMove(my_move);
    } else {
      board_.SetMove(opp_move);
      think_time_->Start();
      // my_move = ThinkMoveRandom();
      my_move = ThinkMoveLoopAtack();
      think_time_->Stop();
      board_.SetMove(my_move);
      printf("Set (X: %d, Y: %d, Tile: %c)\n",
             my_move.x + board_.left, my_move.y + board_.top, my_move.tile);
      // board_.PrintBorder();
      // board_.PrintBoard();
      // PrintProfile();
    }
  }
};


#endif  // TRAX_SOLVER_HPP_
