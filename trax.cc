/*
   Trax Design Competition test (and host) program

   Platform:
     - Developed and tested on FreeBSD 9.3 (amd64) 
     - Will work on other platforms with C++ compiler.
     
   Usage:
     See http://lut.eee.u-ryukyu.ac.jp/traxjp/ (written in Japanese)

   License:
     - Yasunori Osana <osana@eee.u-ryukyu.ac.jp> wrote this file.
     - This file is provided "AS IS" in the beerware license rev 42.
       (see http://people.freebsd.org/~phk/)

 */

#include <iostream>
#include <iomanip>

#include "trax.h"

const std::string esc_ul = "\033[4m";
const std::string esc_rev = "\033[7m";
const std::string esc_red = "\033[31m";
const std::string esc_normal = "\033[0m";

// const std::string esc_ul = "_";
// const std::string esc_rev = "=";
// const std::string esc_red = "*";
// const std::string esc_normal = "";


void trax::clear_marks(){
  for(int x=0; x<BOARD_MAX; x++)
    for(int y=0; y<BOARD_MAX; y++)
      board_marks[x][y] = 0; 
}

void trax::clear_board(){

  for(int x=0; x<BOARD_MAX; x++){
    for(int y=0; y<BOARD_MAX; y++){
      board[x][y] = ' ';
      board_color[x][y] = 0;  // right hand side is 1: RED, 2: WHITE
      board_marks[x][y] = 0; 
    }
  }

  left = right = top = bottom = BOARD_MAX/2;

  red_loop = white_loop = false;
  red_line = white_line = false;
}


std::ostream& operator<<(std::ostream& stream, const trax& t){
  std::cout << "   |";
  for(int x=t.left; x<=t.right; x++)
    stream << (char)(x-t.left+'@') << ' ';
    stream << "|\n";
  
  for(int y=t.top; y<=t.bottom; y++){
    stream << std::setw(3) << y-t.top;
    for(int x=t.left; x<=t.right; x++){
      stream << (x==t.left ? "|" : "")
		<< (t.board_marks[x][y] == 1 ? esc_rev : "")
		<< (t.board_marks[x][y] == 2 ? esc_ul  : "")
		<< (t.board_color[x][y] == 1 ? esc_red : "")
		<< t.board[x][y] << esc_normal << ' '
		<< (x==t.right ? "|" : "");
    }
    stream << std::endl;
  }
  return stream;
}


int trax::opposite_color(const int c){
  if (c==0) return 0;
  if (c==1) return 2;
  return 1;
}

void trax::get_around_colors(const int x, const int y,
		       int& lc, int& rc, int& uc, int& dc){
  // left color
  lc = board_color[x-1][y]; 

  // right color
  rc = board[x+1][y]=='+' ? board_color[x+1][y] :
    opposite_color(board_color[x+1][y]);

  // up color
  uc = board[x][y-1]=='+' ? opposite_color(board_color[x][y-1]) :
    board[x][y-1]=='/' ? board_color[x][y-1] :
    opposite_color(board_color[x][y-1]);

  // down color
   dc = board[x][y+1]=='+' ? opposite_color(board_color[x][y+1]) :
    board[x][y+1]=='\\' ? board_color[x][y+1] :
    opposite_color(board_color[x][y+1]);
}


bool trax::place(move mo){
  int x = left +mo.x;
  int y = top  +mo.y;
  if (mo.x == 0){ left--; }
  if (mo.y == 0){ top--;   }

  if (board[x][y] != ' '){
    std::cout << "**** ALREADY OCCUPIED! ****\n";
    return false;
  }

  board_marks[x][y] = 1;
  
  if (right < x) right = x;
  bottom = (bottom < y) ? y : bottom;
  return place(x, y, mo.tile);
}

bool trax::place(const int x, const int y, const char tile){
  int color = 0;

  board[x][y] = tile;

  // check left, right, up, down
  int lc, rc, uc, dc;
  get_around_colors(x, y, lc, rc, uc, dc);

  // Isolated
  if ( lc==rc && rc==uc && uc==dc && dc == 0 ){
    std::cout << "**** ISOLATED ****\n";
    color = 1; // This should happen only at 1st move only. 
  }


  // Tiles around
  std::cout << (lc!=0 ? ((lc==1 ? esc_red : "") + "L" + esc_normal) : "")
	    << (rc!=0 ? ((rc==1 ? esc_red : "") + "R" + esc_normal) : "")
	    << (uc!=0 ? ((uc==1 ? esc_red : "") + "U" + esc_normal) : "")
	    << (dc!=0 ? ((dc==1 ? esc_red : "") + "D" + esc_normal) : "")
	    << "\n";
  
  // 3 same color check
  if(is_prohibited_3(x, y)) return false; // if true, it's prohibited pattern
  
  // tile requirement check
  if(!is_consistent_placement(x, y, tile)) return false;

  // color the tile
  if (color==0){ // if not already colored
    if (tile=='+')  color = (lc==1 || rc==1 || uc==2 || dc==2) ? 1 : 2;
    if (tile=='/')  color = (lc==1 || uc==1 || dc==2 || rc==2) ? 2 : 1;
    if (tile=='\\') color = (lc==1 || dc==1 || uc==2 || rc==2) ? 2 : 1;
  }

  if (color==0) std::cout << "**** SOMETHING IS WRONG ****\n";
  board_color[x][y]=color;

  scan_forced();
  trace_loop(x, y);
  trace_line();
  
  return true;
}

bool trax::scan_forced(){
  for(int y=top; y<=bottom; y++){
    for(int x=left; x<=right; x++){
      if(board_color[x][y] != 0) continue;

      int lc, rc, uc, dc;
      get_around_colors(x, y, lc, rc, uc, dc);

      char forced = ' ';
      if ( (lc==uc && uc!=0 ) || (dc==rc && rc!=0 ) ) forced = '/'; 
      if ( (rc==uc && uc!=0 ) || (dc==lc && lc!=0 ) ) forced = '\\';
      if ( (lc==rc && rc!=0 && rc!=dc && rc!=uc) ||
	   (uc==dc && dc!=0 && dc!=lc && dc!=lc) ) forced = '+';

      if(forced != ' '){
	std::cout << "Forced play: [X:" << x-left << ", Y:" << y-top
		  << ", Tile:" << forced << "] ";
	place(x, y, forced);
	return true; 
      }
    }
  }
  // No more forced play here
  return false;
}

// ----------------------------------------------------------------------

std::string player(int p){
  switch(p){
  case 1:
    return "white";
  case 2:
   return "red";
  }
  return "someone unknown";
}

#include "solver.hpp"
#include "recorder.hpp"

int main(){
  trax t;
  
  t.clear_board();

  // first move is either "@0/" or "@0+"
  std::string m;
  bool preamble = true;

  int turn = 0;
  int p = 1;

  Recorder rec(0);
  TraxSolver p1_solver(1);
  TraxSolver p2_solver(2);
  move mo(""), opp_mo("");
  
  //  std::cout << t;
  // while ( std::cin >> m ){
  while (true){
    TraxSolver *solver_ptr;
    if (p == 1) {
      solver_ptr = &p1_solver;
    } else {
      solver_ptr = &p2_solver;
    }
    solver_ptr->MyTurn(turn, opp_mo, mo);
    opp_mo = mo;
    m = solver_ptr->GetMoveString(mo);
    rec.Record(m);

    if (preamble && m[0]=='@') preamble=false;

    if (!preamble){
      std::cout << "Turn " << ++turn
		<< " (player " << p << ": " << player(p) << "): ";
      
      // move mo = move(m);
      std::cout << m << "[" << mo << "] ";


      bool violation = false;
      
      // place a move
      if(!t.place(mo)) violation = true;

      std::cout << t;
      if (t.is_board_consistent()){
	if(t.loop() || t.line()){
	  int winner = p;

	  switch(winner){
	  case 1:
	    if (t.red() && !t.white()) winner=2;
	    break;
	  case 2:
	    if (t.white() && !t.red()) winner=1;
	    break;
	  }
	  std::cout << "---- Player " << winner << "(" << player(winner)
		    << ") GOT A " << (t.loop() ? "LOOP" : "LINE")
		    << " in player " << p << "(" << player(p)
		    << ")'s turn ! ----\n";
	  std::cout << "==== Player " << winner << " won the game.\n";
	  return 0;
	}
      }
      else {
	violation = true;
      }

      if (violation){
	std::cout << "---- VIOLATION ! ----\n";
	std::cout << "==== Player " << p << " lost the game by violation.\n";
	return -1;
      }

      std::cout << "Going to next turn." << std::endl;
      
      t.clear_marks();
      p = (p==2) ? 1 : 2;
    }
  }
}
