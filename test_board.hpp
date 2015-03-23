#ifndef TEST_BOARD_HPP_
#define TEST_BOARD_HPP_


#include <stdio.h>
#include <string.h>

#include "trax.h"
#include "board.hpp"


class TestBoard : public Board {

 private:

  //----------------------------------------------------------------------------
  // Methods
  //----------------------------------------------------------------------------

  /**
   * Print ideal board at test for SetMove and SetTile
   */
  void PrintIdealBoard() {
    printf("Ideal board is ...\n");
    printf(" --------------\n");
    printf("|              |\n");
    printf("|  +2\\1/2+2\\1  |\n");
    printf("|  +2/1+1/2/1\\2|\n");
    printf("|  \\1/2/1\\2\\1/2|\n");
    printf("|  \\2/1/2+2/1  |\n");
    printf(" --------------\n");
  }

  /**
   * For TestSetMove
   */
  inline void TestSetMoveOneSet(int x, int y, char tile) {
    printf("Set (X: %d, Y: %d, Tile: %c)\n", x, y, tile);
    bool is_valid = IsValidMove(x, y, tile);
    printf("Is valid Move?: %s\n", is_valid ? "Yes" : "No");
    if (!is_valid && x != 50 && y != 50) {
      printf("Test Error\n");
      PrintIdealBoard();
      exit(1);
    }
    SetMove(x, y, tile);
    PrintBorder();
    PrintBoard();
    // PrintIdealBoard();
  }


 public:
  
  //----------------------------------------------------------------------------
  // Methods
  //----------------------------------------------------------------------------
  
  /**
   * Test GetColor{N,E,S,W}
   * OK
   */
  void TestGetColorX() {
    printf("GetColorN: TILE_SPACE (  ) -> %d\n", GetColorN(TILE_SPACE));
    printf("GetColorN: TILE_RED_NS(+1) -> %d\n", GetColorN(FIELD_PLACED | TILE_RED_NS));
    printf("GetColorN: TILE_RED_EW(+2) -> %d\n", GetColorN(FIELD_PLACED | TILE_RED_EW));
    printf("GetColorN: TILE_RED_WN(/1) -> %d\n", GetColorN(FIELD_PLACED | TILE_RED_WN));
    printf("GetColorN: TILE_RED_ES(/2) -> %d\n", GetColorN(FIELD_PLACED | TILE_RED_ES));
    printf("GetColorN: TILE_RED_SW(\\1) -> %d\n", GetColorN(FIELD_PLACED | TILE_RED_SW));
    printf("GetColorN: TILE_RED_NE(\\2) -> %d\n", GetColorN(FIELD_PLACED | TILE_RED_NE));
    printf("\n");
    printf("GetColorE: TILE_SPACE (  ) -> %d\n", GetColorE(TILE_SPACE));
    printf("GetColorE: TILE_RED_NS(+1) -> %d\n", GetColorE(FIELD_PLACED | TILE_RED_NS));
    printf("GetColorE: TILE_RED_EW(+2) -> %d\n", GetColorE(FIELD_PLACED | TILE_RED_EW));
    printf("GetColorE: TILE_RED_WN(/1) -> %d\n", GetColorE(FIELD_PLACED | TILE_RED_WN));
    printf("GetColorE: TILE_RED_ES(/2) -> %d\n", GetColorE(FIELD_PLACED | TILE_RED_ES));
    printf("GetColorE: TILE_RED_SW(\\1) -> %d\n", GetColorE(FIELD_PLACED | TILE_RED_SW));
    printf("GetColorE: TILE_RED_NE(\\2) -> %d\n", GetColorE(FIELD_PLACED | TILE_RED_NE));
    printf("\n");
    printf("GetColorS: TILE_SPACE (  ) -> %d\n", GetColorS(TILE_SPACE));
    printf("GetColorS: TILE_RED_NS(+1) -> %d\n", GetColorS(FIELD_PLACED | TILE_RED_NS));
    printf("GetColorS: TILE_RED_EW(+2) -> %d\n", GetColorS(FIELD_PLACED | TILE_RED_EW));
    printf("GetColorS: TILE_RED_WN(/1) -> %d\n", GetColorS(FIELD_PLACED | TILE_RED_WN));
    printf("GetColorS: TILE_RED_ES(/2) -> %d\n", GetColorS(FIELD_PLACED | TILE_RED_ES));
    printf("GetColorS: TILE_RED_SW(\\1) -> %d\n", GetColorS(FIELD_PLACED | TILE_RED_SW));
    printf("GetColorS: TILE_RED_NE(\\2) -> %d\n", GetColorS(FIELD_PLACED | TILE_RED_NE));
    printf("\n");
    printf("GetColorW: TILE_SPACE (  ) -> %d\n", GetColorW(TILE_SPACE));
    printf("GetColorW: TILE_RED_NS(+1) -> %d\n", GetColorW(FIELD_PLACED | TILE_RED_NS));
    printf("GetColorW: TILE_RED_EW(+2) -> %d\n", GetColorW(FIELD_PLACED | TILE_RED_EW));
    printf("GetColorW: TILE_RED_WN(/1) -> %d\n", GetColorW(FIELD_PLACED | TILE_RED_WN));
    printf("GetColorW: TILE_RED_ES(/2) -> %d\n", GetColorW(FIELD_PLACED | TILE_RED_ES));
    printf("GetColorW: TILE_RED_SW(\\1) -> %d\n", GetColorW(FIELD_PLACED | TILE_RED_SW));
    printf("GetColorW: TILE_RED_NE(\\2) -> %d\n", GetColorW(FIELD_PLACED | TILE_RED_NE));
  }

  /**
   * Test SetTile
   * OK
   */  
  void TestSetTile() {
    SetTile(50, 50, '+');
    SetTile(50, 50, '+');
    SetTile(51, 50, '/');
    SetTile(51, 50, '/');
    SetTile(51, 51, '/');
    SetTile(50, 51, '\\');
    SetTile(51, 51, '/');
    SetTile(50, 51, '\\');
    SetTile(52, 50, '+');
    SetTile(52, 51, '/');
    SetTile(52, 50, '+');
    SetTile(52, 51, '/');
    SetTile(51, 52, '/');
    SetTile(50, 52, '\\');
    SetTile(52, 52, '/');
    SetTile(51, 52, '/');
    SetTile(50, 52, '\\');
    SetTile(52, 52, '/');
    SetTile(51, 49, '\\');
    SetTile(51, 49, '\\');
    SetTile(50, 49, '+');
    SetTile(50, 49, '+');
    SetTile(52, 49, '/');
    SetTile(52, 49, '/');
    SetTile(53, 52, '+');
    SetTile(53, 51, '\\');
    SetTile(53, 52, '+');
    SetTile(53, 51, '\\');
    SetTile(54, 51, '\\');
    SetTile(54, 52, '/');
    SetTile(54, 51, '\\');
    SetTile(54, 52, '/');
    SetTile(54, 50, '/');
    SetTile(53, 50, '/');
    SetTile(54, 50, '/');
    SetTile(53, 50, '/');
    SetTile(53, 49, '+');
    SetTile(54, 49, '\\');
    SetTile(53, 49, '+');
    SetTile(54, 49, '\\');
    SetTile(55, 51, '/');
    SetTile(55, 50, '\\');
    PrintBoard();
    PrintIdealBoard();
  }

  /**
   * Test SetMove
   * OK
   */  
  void TestSetMove() {
    TestSetMoveOneSet(50, 50, '+');
    TestSetMoveOneSet(51, 50, '/');
    TestSetMoveOneSet(51, 51, '/');
    TestSetMoveOneSet(52, 50, '+');
    TestSetMoveOneSet(51, 52, '/');
    TestSetMoveOneSet(51, 49, '\\');
    TestSetMoveOneSet(50, 49, '+');
    TestSetMoveOneSet(52, 49, '/');
    TestSetMoveOneSet(53, 52, '+');
    TestSetMoveOneSet(54, 51, '\\');
    TestSetMoveOneSet(54, 50, '/');
    TestSetMoveOneSet(53, 49, '+');
    TestSetMoveOneSet(55, 51, '/');
    PrintIdealBoard();
  }

  void EvaluateLoop(int x, int y) {
    printf("(%d, %d) ",  x, y);
    if (DetectWhiteLoop(x, y)) {
      printf("Detect white loop\n");
    }
    if (DetectRedLoop(x, y)) {
      printf("Detect red loop\n");
    }
    printf("\n");
    PrintBoard();
    printf("\n");
  }

  void InitializeBoard() {
    memset(blocks_, 0, sizeof(char) * BOARD_MAX * BOARD_MAX);
    border_n_ = border_e_ = border_s_ = border_w_ = BOARD_CENTER;
  }
  
  /**
   * Test Detect{White,Red}Loop
   */
  void TestDetectLoop() {
    // Test LOOP_WHITE_2_S
    printf("Test LOOP_WHITE_2_S: ");
    InitializeBoard();
    SetMove(50, 50, '/');
    SetMove(51, 50, '\\');
    EvaluateLoop(50, 51);
    // Test LOOP_WHITE_2_N
    printf("Test LOOP_WHITE_2_N: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(51, 50, '\\');    
    SetMove(52, 50, '/');
    EvaluateLoop(51, 49);
    // Test LOOP_WHITE_2_E
    printf("Test LOOP_WHITE_2_E: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(51, 50, '/');
    SetMove(51, 51, '\\');
    EvaluateLoop(52, 50);
    // Test LOOP_WHITE_2_W
    printf("Test LOOP_WHITE_2_W: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(49, 50, '\\');
    SetMove(49, 51, '/');
    EvaluateLoop(48, 50);

    // Test LOOP_WHITE_3_S
    printf("Test LOOP_WHITE_3_S: ");
    InitializeBoard();
    SetMove(50, 50, '/');
    SetMove(51, 50, '+');
    SetMove(52, 50, '\\');
    EvaluateLoop(50, 51);
    // Test LOOP_WHITE_3_N
    printf("Test LOOP_WHITE_3_N: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(51, 50, '\\');
    SetMove(52, 50, '+');
    SetMove(53, 50, '/');
    EvaluateLoop(51, 49);
    // Test LOOP_WHITE_3_E
    printf("Test LOOP_WHITE_3_E: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(51, 50, '/');
    SetMove(51, 51, '+');
    SetMove(51, 52, '\\');
    EvaluateLoop(52, 50);
    // Test LOOP_WHITE_3_W
    printf("Test LOOP_WHITE_3_W: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(49, 50, '\\');
    SetMove(49, 51, '+');
    SetMove(49, 52, '/');
    EvaluateLoop(48, 50);

    // Test LOOP_RED_2_S
    printf("Test LOOP_RED_2_S: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(50, 51, '\\');
    SetMove(49, 51, '/');
    EvaluateLoop(49, 52);
    // Test LOOP_RED_2_N
    printf("Test LOOP_RED_2_N: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(50, 49, '\\');    
    SetMove(51, 49, '/');
    EvaluateLoop(50, 48);
    // Test LOOP_RED_2_E
    printf("Test LOOP_RED_2_E: ");
    InitializeBoard();
    SetMove(50, 50, '/');
    SetMove(51, 50, '/');
    SetMove(51, 51, '\\');
    EvaluateLoop(52, 50);
    // Test LOOP_RED_2_W
    printf("Test LOOP_RED_2_W: ");
    InitializeBoard();
    SetMove(50, 50, '/');
    SetMove(50, 49, '\\');
    EvaluateLoop(49, 49);

    // Test LOOP_RED_3_S
    printf("Test LOOP_RED_3_S: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(50, 51, '\\');
    SetMove(49, 51, '+');
    SetMove(48, 51, '/');
    EvaluateLoop(48, 52);
    // Test LOOP_RED_3_N
    printf("Test LOOP_RED_3_N: ");
    InitializeBoard();
    SetMove(50, 50, '+');
    SetMove(50, 49, '\\');
    SetMove(51, 49, '+');
    SetMove(52, 49, '/');
    EvaluateLoop(50, 48);
    // Test LOOP_RED_3_E
    printf("Test LOOP_RED_3_E: ");
    InitializeBoard();
    SetMove(50, 50, '/');
    SetMove(51, 50, '/');
    SetMove(51, 51, '+');
    SetMove(51, 52, '\\');
    EvaluateLoop(52, 50);
    // Test LOOP_RED_3_W
    printf("Test LOOP_RED_3_W: ");
    InitializeBoard();
    SetMove(50, 50, '/');
    SetMove(50, 49, '+');
    SetMove(50, 48, '\\');
    EvaluateLoop(49, 48);
  }

};


#endif  // end TEST_BOARD_HPP_
