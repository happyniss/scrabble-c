/* scrabble_logic.h
 * I implemented Appel & Jacobson's algorithm for finding
 * words, as described in "The World's Fastest Scrabble Program", published
 * in Communications of the ACM in 1988. Uses the structures I designed in
 * board.h and trie.h
 */


#include "board.h"
#include "trie.h"

#ifndef SCRABBLE_LOGIC_H
#define SCRABBLE_LOGIC_H

typedef struct turn_manage {
  player *p1; // player 1
  player *curr; // whose turn
  int best_points;
  char best_word[BOARD_LENGTH];
  
  char blank1; // keep track of what blank letters are being used as
  char blank2; 
  
} turn;

// ========== turn management ==========
turn *make_turn(player *p);
void *next_turn(turn *t);
void free_turn(turn *t);

// ========== finding a move ==========

void leftpart(board *b,turn *t, char *part, t_node *node,int horiz_i,int vert_i,int lim);
void extendright(board *b, turn *t, char *part,t_node *node, int horiz_i, int vert_i,b_space *anchor);
void legalmove_horiz(char *part,board *b,turn *t,int horiz_i,int vert_i);
void legalmove_vert(char *part,board *b,turn *t,int horiz_i,int vert_i);

// for vertical board

void toppart(board *b,turn *t, char *part, t_node *node,int horiz_i,int vert_i,int lim);
void extenddown(board *b, turn *t, char *part,t_node *node, int horiz_i, int vert_i,b_space *anchor);

#endif
