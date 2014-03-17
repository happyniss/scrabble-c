/* board.h
 * Interface for Scrabble board.
 * Scrabble board designed by Hashbro.
 */

/* For the part of the program that determines potential moves on the board, 
 * I utilize a "backtracking method" described by Appel and Jacobson in
 * the paper "The World's Fastest Scrabble Program" as published in 
 * "Communications of the ACM." The implementation as well as other aspects
 * of the board (data representation the board and tiles, saving, loading
 * boards, determining the "best" move and playing it, are my own.
 */

#include <stdbool.h>
#include <stdlib.h>
#include "trie.h"

#ifndef BOARD_LENGTH
#define BOARD_LENGTH 16
#endif

#ifndef TILES
#define TILES 27
#endif

#ifndef BOARD_H
#define BOARD_H

typedef struct board_space {
  bool crosscheck_horiz[26]; // indexed letters possible are true, otherwise false
  bool crosscheck_vert[26];
  char tile_letter; // if '-', empty, else letter on tile
  int points; // number of points on tile 
  int special;/* 12 = double letter
		 13 = triple letter
		 14 = double word
		 15 = triple word */
} b_space;

typedef struct board_rep {
  b_space space[17][17];
  bool anchors[17][17];

  trie *dict;
} board;

typedef struct player_rep {
  int points; // number of points player has
  int rack[TILES]; // in order of a-z, blank: number of each tile
  int off_rack[TILES]; // tiles taken off the rack temporarily

  struct player_rep *next; // point to next player
} player;

// ========== board infrastructure functions ==========
board *make_board(trie *dict); // makes a scrabble board
void free_board(board *b); // delete a board

// ========== determining things about spaces on the board ==========
bool space_isempty(b_space space);
// not if going horiz use vert crosscheck
bool crosscheck_letter_vert(b_space space, char c); // check vertical xc
bool crosscheck_letter_horiz(b_space space, char c);

// returns the limit size of the leftpart
int leftpart_max_horiz(board *b, int horiz_i, int vert_i); 
int leftpart_max_vert(board *b, int hoiz_i, int vert_i);

// returning strings touching a tile
int count_left(board *b, int horiz_i, int vert_i);
int count_top(board *b, int horiz_i, int vert_i);
void get_seg_left(board *b, int horiz_i, int vert_i, char *str);
void get_seg_right(board *b, int horiz_i, int vert_i, char *str);
void get_seg_up(board *b, int horiz_i, int vert_i, char *str);
void get_seg_down(board *b, int horiz_i, int vert_i, char *str);

// ========== updating data about the board ==========
// anchors
bool check_anchor(board *b, int horiz_i, int vert_i);
void update_anchors(board *b);

// crosschecks: one should update squares surrounding tiles modified
void update_crosscheck(board *b, int horiz_i, int vert_i);
void board_crosschecks(board *b); // update new crosschecks 
void surrounding_crosschecks(board *b, int horiz_i, int vert_i);

// ========== player data ==========
player *make_players(int num_of);
void free_players(player *p);
int player_points(player *p); 

// === obtaining and modifying rack data === 

bool contains_letter(player *p, char c);
bool contains_blank(player *p);

void remove_letter(player *p, char c);
void add_letter(player *p, char c);

// for temporarily removing letters from the rack
void hold_letter(player *p, char c);
void putback_letter(player *p, char c);

// ========== etc ==========
// useful functions
void stringify(char c, char *buf); // turns a character into a string
int tilescore(char c); // returns the default tile points for a char
void print_board(board *b);
void print_rack(player *p);

// modifying the board
void put_tile(board *b, int horiz_i, int vert_i, char c);

void make_move_horiz(board *b, int horiz_i, int vert_i, char *c); // IMPLEMENT ME. INCLUDE UPDATE CROSSCHECKING
void make_move_vert(board *b, int horiz_i, int vert_i, char *c);

#endif
