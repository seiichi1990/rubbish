#include <iostream>
#include "trax.h"

bool trax::trace_loop(const int x, const int y, const int d, const int mark=0){
  // dir: up(1) down(2) left(3) right(4)
  int xx=x; int yy=y; int dir=d;

  do {
    int next_dir;

    switch(dir){
    case 1: // up
      yy--;
      next_dir = board[xx][yy]=='/' ? 4 : board[xx][yy]=='\\' ? 3 : 1;
      break;
    case 2: // down
      yy++;
      next_dir = board[xx][yy]=='/' ? 3 : board[xx][yy]=='\\' ? 4 : 2;
      break;
    case 3: // left
      xx--;
      next_dir = board[xx][yy]=='/' ? 2 : board[xx][yy]=='\\' ? 1 : 3;
      break;
    case 4: // right
      xx++;
      next_dir = board[xx][yy]=='/' ? 1 : board[xx][yy]=='\\' ? 2 : 4;
      break;
    }
    dir=next_dir;

    //    if (!(xx==x && yy==y) && mark!=0) board_marks[xx][yy]=mark;

    if (mark!=0) board_marks[xx][yy]=mark;

    if (xx==x && yy==y){ // Loop found !
      return true;
    }
  } while(board[xx][yy] != ' ');
  return false;
}

bool trax::trace_loop(const int x, const int y){
  bool found = false;
  for(int d=1; d<=4; d++)
    if (trace_loop(x, y, d)){
      trace_loop(x, y, d, 2); // mark

      int col = board_color[x][y];
      switch(d){
      case 1: // up
	if (board[x][y]!='\\') col = opposite_color(col);
	break;
      case 2: // down
	if (board[x][y]!='/') col = opposite_color(col);
	break;
      case 3: // left
	if (board[x][y]!='+') col = opposite_color(col);
	break;
      case 4: // right
	// nothing to do
	break;
      }

      if(col==1) red_loop = true;
      else white_loop = true;
      
      found = true;
    }

  // if (found){
  //   if(red_loop) std::cout << "[DEBUG] ---- RED LOOP! ----\n" ;
  //   if(white_loop) std::cout << "[DEBUG] ---- WHITE LOOP! ----\n" ;
  // }

  return found;
}



bool trax::trace_line(const int x, const int y, const int d, const int mark=0){
  // dir: up(1) down(2) left(3) right(4)
  int xx=x; int yy=y; int dir=d;


  if (mark!=0) board_marks[xx][yy]=mark;
  do {
    int next_dir;

    switch(dir){
    case 1: // up
      next_dir = board[xx][yy]=='/' ? 4 : board[xx][yy]=='\\' ? 3 : 1;
      break;
    case 2: // down
      next_dir = board[xx][yy]=='/' ? 3 : board[xx][yy]=='\\' ? 4 : 2;
      break;
    case 3: // left
      next_dir = board[xx][yy]=='/' ? 2 : board[xx][yy]=='\\' ? 1 : 3;
      break;
    case 4: // right
      next_dir = board[xx][yy]=='/' ? 1 : board[xx][yy]=='\\' ? 2 : 4;
      break;
    }

    dir=next_dir;
    switch(dir){
    case 1: yy--; break; // up
    case 2: yy++; break; // down
    case 3: xx--; break; // left
    case 4: xx++; break; // right
    }
    
    if (mark!=0) board_marks[xx][yy]=mark;

    if ((d==4 && xx==right +1 && (right-left)>=8) ||
	(d==2 && yy==bottom+1 && (bottom-top)>=8)){ // Line found !
      return true;
    }
  } while(board[xx][yy] != ' ');

   return false;
}


bool trax::trace_line(){
  bool found = false;

  for(int y=top; y<=bottom; y++){
    if(board[left+1][y]!=' '){ // horizontal candidate!
      if(trace_line(left+1, y, 4)){
	trace_line(left+1, y, 4, 2);  // mark the trace
	if (board_color[left+1][y] == 1) red_line = true;
	else white_line = true;
	found = true;
      }
    }
  }

  for(int x=left; x<=right; x++){
    if(board[x][top+1]!=' '){ // vertical candidate!
      if(trace_line(x, top+1, 2)){
	trace_line(x, top+1, 2, 2);   // mark the trace
	int col;
	col = board_color[x][top+1];
	if(board[x][top+1]=='+' || board[x][top+1]=='\\')
	  col = opposite_color(col);

	if(col==1) red_line = true;
	else white_line = true;
	found = true;   
      }
    }
  }

  // if (found)
  //   std::cout << "[DEBUG] ---- " << ( red_line ? "RED" : "WHITE" ) << " LINE! ----\n";

  return found;
}
