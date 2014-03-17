// board.c
// functions for scrabble board

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "board.h"
#include "trie.h"

const char *alphabet = "abcdefghijklmnopqrstuvwxyz";


// ========== board infrastructure functions ==========
board *make_board(trie *dictionary) {
  board *result = (board *) malloc(sizeof(board));
  assert (result != NULL);
  
  result->dict=dictionary;

  for (int i=0; i<17; i++) { // traverse horizontally
    for (int j=0; j<17; j++) { // traverse vertically
      
      // anchors are all false
      result->anchors[i][j] = false;

      // except for the middle of the board;
      result->anchors[8][8] = true;

      // set tile letters to empty
      result->space[i][j].tile_letter='-';

      // set crosschecks
      for (int k=0;k<26;k++) {
	// vertical and horizontal points set to 0
	
	// set all crosschecks unavailable for edges
	if (i==0 || i==BOARD_LENGTH || j==0 || j==BOARD_LENGTH) {
	  result->space[i][j].crosscheck_horiz[k]=false;
	  result->space[i][j].crosscheck_vert[k]=false;
	}
	// set the rest to available
	else {
	  result->space[i][j].crosscheck_horiz[k]=true;
	  result->space[i][j].crosscheck_vert[k]=true;
	}
      }
      result->space[i][j].points=0; // set points to 0

      result->space[i][j].special = 0; // default special is 0
      
      // set "special" spaces
      if (i==1 || i == 15) {
	if (j == 1 || j == 8 || j == 15)
	  result->space[i][j].special = 15; // triple word
	if (j==4 || j == 12)
	  result->space[i][j].special = 12; // double letter
      }
      if (i==2 || i==14) {
	if (j==2 || j==14)
	  result->space[i][j].special=14; // double word
	if (j==6 || j==10)
	  result->space[i][j].special=13; // triple letter
      }
      if (i==3 || i==13) {
	if (j==3 || j==13)
	  result->space[i][j].special=14; // double word
	if (j==7 || j==9)
	  result->space[i][j].special=13; //triple letter
      }
      if (i==4 || i==12) {
	if (j==1 || j==8 || j==15)
	  result->space[i][j].special=12; // double letter
	if (j==4 || j==12)
	  result->space[i][j].special=14; // double word
      }
      if (i==5 || i==11) {
	if (j==5 || j==11)
	  result->space[i][j].special=14; // double word
      }
      if (i==6 || i==10) {
	if (j==2 || j==6 || j==10 || j==14)
	  result->space[i][j].special=13; //triple letter
      }
      if (i==7 || i==9) {
	if (j==3 || j==7 || j==9 || j==13)
	  result->space[i][j].special=12; // double letter
      }
      if (i==8) {
	if (j==1 || j==15)
	  result->space[i][j].special=15; // triple word
	if (j==4 || j==12)
	  result->space[i][j].special=12; // double letter
	if (j==8)
	  result->space[i][j].special=14; // double word
      }
    }
  }
  return result;
}

void free_board(board *b) {
  free((void *) b);
}

// ========== determining things about spaces on the board ==========
bool space_isempty(b_space space) {
  if (space.tile_letter=='-')
    return true;
  else
    return false;
}

bool crosscheck_letter_vert(b_space space, char c) {
  if (space.crosscheck_vert[c-'a']==true)
    return true;
  else
    return false;
}

bool crosscheck_letter_horiz(b_space space, char c) {
  if (space.crosscheck_horiz[c-'a']==true)
    return true;
  else
    return false;
}

// return the biggest left-part possible from the anchor square
int leftpart_max_horiz(board *b, int horiz_i, int vert_i) {
  int max = 0; // max starts at zero
  while (max < 7) { // highest max is 6
    if (horiz_i-max==1)
      return max; // can't extend past the edge of the board
    if (b->space[horiz_i-max-1][vert_i].tile_letter!='-')
      return max; // can't extend past a filled tile
    if (b->anchors[horiz_i-max-1][vert_i]==true)
      return max; // can't extend leftwards to an anchor tile
    max++;
  }
  return max;
}

int leftpart_max_vert(board *b, int horiz_i, int vert_i) {
  int max = 0; // max starts at zero
  while (max < 7) { // highest max is 6
    if (vert_i-max==1)
      return max; // can't extend past the edge of the board
    if (b->space[horiz_i][vert_i-max-1].tile_letter!='-')
      return max; // can't extend past a filled tile
    for (int i=0;i<26;i++) {
      if (b->anchors[horiz_i][vert_i-max-1]==true)
	return max; // can't extend past an anchor
    }
    max++;
  }
  return max;
}

int count_left(board *b, int horiz_i, int vert_i){
  int i=0;
  while (b->space[horiz_i-1-i][vert_i].tile_letter!='-')
    i++;
  return i;
}

int count_top(board *b, int horiz_i, int vert_i){
  int i=0;
  while (b->space[horiz_i][vert_i-1-i].tile_letter!='-' && vert_i-i>0)
    i++;
  return i;
}

void get_seg_left(board *b, int horiz_i, int vert_i, char *str) {
  // count tiles to the left
  int i;
  i = count_left(b,horiz_i,vert_i);
  
  // copy int string str
  int j = 0;
  while(i>0) {
    str[j]=b->space[horiz_i-i][vert_i].tile_letter;
    i--;
    j++;
  }
  
  str[j]='\0'; // add null character
}

// same as seg_left but transposed
void get_seg_up(board *b, int horiz_i, int vert_i, char *str) {
  int i;
  i = count_top(b,horiz_i,vert_i);
  int j=0;
  while(i>0) {
    str[j]=b->space[horiz_i][vert_i-i].tile_letter;
    i--;
    j++;
  }
  str[j]='\0';
}

void get_seg_right(board *b, int horiz_i, int vert_i, char *str){
  int i = 0;
  while (b->space[horiz_i+1+i][vert_i].tile_letter!='-') {
    str[i]=b->space[horiz_i+1+i][vert_i].tile_letter;
    i++;
  }
  str[i]='\0'; // add null
}

void get_seg_down(board *b, int horiz_i, int vert_i, char *str){
  int i = 0;
  while (b->space[horiz_i][vert_i+1+i].tile_letter!='-') {
    str[i]=b->space[horiz_i][vert_i+1+i].tile_letter;
    i++;
  }
  str[i]='\0'; // add null
  printf("%s",str);
}

// if adjacent squares have tiles and the square is not full, it is an anchor
bool check_anchor(board *b, int horiz_i, int vert_i) {
  if (b->space[horiz_i-1][vert_i].tile_letter!='-'||b->space[horiz_i+1][vert_i].tile_letter!='-'||b->space[horiz_i][vert_i-1].tile_letter!='-'||b->space[horiz_i][vert_i+1].tile_letter!='-') {
    if(space_isempty(b->space[horiz_i][vert_i])==true)
      return true;
  }
  return false;
}

// update anchors on board (indexes 1-15)
void update_anchors(board *b) {
  for (int i=1;i<BOARD_LENGTH;i++) {
    for (int j=1;j<BOARD_LENGTH;j++) {
      if (check_anchor(b,i,j)==true)
	b->anchors[i][j]=true;
      else
	b->anchors[i][j]=false;
    }
  }
}

// crosscheck a particular space
void update_crosscheck (board *b, int horiz_i, int vert_i) {
  char str[BOARD_LENGTH]; // array to hold strings being checked
  char buf[BOARD_LENGTH]; // array for strings found on board
  
  for (int i=0;i<26;i++) { // for every letter in the alphabet
    // reset crosscheck to true
    b->space[horiz_i][vert_i].crosscheck_vert[i]=true; 
    b->space[horiz_i][vert_i].crosscheck_horiz[i]=true; 
    
    // crosscheck horizontal if there are tiles to the left or right
    if (b->space[horiz_i-1][vert_i].tile_letter!='-' || b->space[horiz_i+1][vert_i].tile_letter!='-') {
      get_seg_left(b,horiz_i,vert_i,buf);
      strcpy(str,buf);
      
      stringify(alphabet[i],buf);
      strcat(str,buf);
      
      get_seg_right(b,horiz_i,vert_i,buf);
      strcat(str,buf);
      
      if (trie_ismember(b->dict->root,str)==false)
	b->space[horiz_i][vert_i].crosscheck_horiz[i]=false;
    }
    // crosscheck vertical if next to a tile
    if (b->space[horiz_i][vert_i-1].tile_letter!='-' || b->space[horiz_i][vert_i+1].tile_letter!='-') {
      get_seg_up(b,horiz_i,vert_i,buf);
      strcpy(str,buf);
      
      stringify(alphabet[i],buf);
      strcat(str,buf);
      
      get_seg_down(b,horiz_i,vert_i,buf);
      strcat(str,buf);
      //     printf("%s\n",str);
      
      // see if that's a word
      if (trie_ismember(b->dict->root,str)==false)
	b->space[horiz_i][vert_i].crosscheck_vert[i]=false; 
    }
  }
}


void board_crosschecks(board *b) {
  for (int i=1;i<15;i++) {
    for (int j=1;j<15;j++)
      update_crosscheck(b,i,j);
  }
}

void surrounding_crosschecks(board *b, int horiz_i, int vert_i) {
  if (b->space[horiz_i-1][vert_i].tile_letter=='-')
    update_crosscheck(b,horiz_i-1,vert_i);
  if (b->space[horiz_i+1][vert_i].tile_letter=='-')
    update_crosscheck(b,horiz_i+1,vert_i);
  if (b->space[horiz_i][vert_i-1].tile_letter=='-')
    update_crosscheck(b,horiz_i,vert_i-1);
  if (b->space[horiz_i][vert_i+1].tile_letter=='-')
    update_crosscheck(b,horiz_i,vert_i+1);
}

// ========== player data ==========
player *make_players(int num_of) {
  player *curr = NULL;

  while (num_of > 0) {
    player *p = (player *) malloc(sizeof(player));
    assert (p !=NULL);
    
    p->points=0;
    for (int i=0;i<27;i++)
      p->rack[i]=0;

    if (curr==NULL) { // first time
      p->next=NULL;
    }
    else
      p->next=curr;
    
    curr = p;
    num_of--;
  }
  return curr;
}

void free_players(player *p) {
  player *temp;
  
  while (p!=NULL) {
    temp = p;
    p=p->next;
    free((void *) temp);
  }
}

int player_points(player *p) {
  return p->points;
}

// rack

bool contains_letter(player *p, char c) {
  int i;
  i = c-'a';
  
  if (p->rack[i]>0)
    return true;
  else
    return false;
}

bool contains_blank(player *p) {
  if (p->rack[26]>0)
    return true;
  else
    return false;
}

void remove_letter(player *p, char c) {
  if (c=='0')
    p->rack[26]--;
  else {
    int i = c-'a';
    p->rack[i]--;
  }
}

void add_letter(player *p, char c) {
  if (c == '0') // if blank, add blank
    p->rack[26]++;
  else { // else add letter
    int i;
    i = c-'a';
    p->rack[i]++;
  }
}

void hold_letter(player *p, char c) {
  if (c=='0') {
    p->rack[26]--;
    p->off_rack[26]++;
  }
  else {
    int i=c-'a';
    p->rack[i]--;
    p->off_rack[i]++;
  }
}

void putback_letter(player *p, char c) {
  if (c=='0') {
    p->off_rack[26]--;
    p->rack[26]++;
  }
  else {
    p->off_rack[c-'a']--;
    p->rack[c-'a']++;
  }
}

void stringify(char c, char *buf) {
  buf[0] = c;
  buf[1] = '\0';
}

void print_board(board *b) {
  printf("\n");
  printf("   ");
  for (int i=1;i<16;i++) {
    printf("%3d",i);
  }
  printf("\n");
  for (int j=1;j<16;j++) {
    printf("%3d ",j);
    for (int i=1; i<16;i++) {
      b_space curspace = b->space[i][j];
      if(curspace.tile_letter!='-') {
	printf("(");
	printf("%c",curspace.tile_letter);
	printf(")");
      }
      else if (curspace.special==12)
	printf("DLS");
      else if (curspace.special==13)
	printf("TLS");
      else if (curspace.special == 14)
	printf("DWS");
      else if (curspace.special == 15)
	printf("TWS");
      else
	printf("[ ]");
    } printf("\n");
  }
  printf("\n");
}

void print_rack(player *p) {
  for (int i=0;i<TILES;i++)
    if (p->rack[i]>0)
      printf("[%c] x%d\n",alphabet[i], p->rack[i]);
}

int tilescore(char c) {
  int score;
  if (c=='a'||c=='e'||c=='h'||c=='l'||c=='n'||c=='o'||c=='r'||c=='s'||c=='t'||c=='u')
    score = 1;
  else if (c=='d'||c=='g')
    score = 2;
  else if (c=='b'||c=='c'||c=='m'||c=='p')
    score = 3;
  else if (c=='f'||c=='h'||c=='v'||c=='w'||c=='y')
    score = 4;
  else if (c=='k')
    score = 5;
  else if (c=='j'||c=='x')
    score = 8;
  else if (c=='q'||c=='z')
    score = 10;
  return score;
}

// modify board
void put_tile(board *b, int horiz_i, int vert_i, char c) {
  b->space[horiz_i][vert_i].points = tilescore(c);
  b->space[horiz_i][vert_i].tile_letter=c;
}

void make_move_horiz(board *b, int horiz_i, int vert_i, char *c) {
  int i = 0;
  while (c[i]!='\0' && horiz_i+i <16) {
    b_space *curspace = &b->space[horiz_i+i][vert_i];
    // if space is empty:
    if (curspace->tile_letter=='-') {
      // put tile on it
      curspace->tile_letter=c[i];
      curspace->points=tilescore(c[i]);
    }
    i++;
  }
  // update anchor tiles
  update_anchors(b);

  // update crosschecks
  while (i>0) {
    i--;
    surrounding_crosschecks(b,horiz_i+i,vert_i);
  }
}

void make_move_vert(board *b, int horiz_i, int vert_i, char *c) {
  int i = 0;
  while (c[i]!='\0' && vert_i+i < 16) {
    b_space *curspace = &b->space[horiz_i][vert_i+i];
    // if space is empty:
    if (curspace->tile_letter=='-') {
      // put tile on it
      curspace->tile_letter=c[i];
      curspace->points=tilescore(c[i]);
    }
    i++;
  }
  // update anchor tiles
  update_anchors(b);

  // update crosschecks
  while (i>0) {
    i--;
    surrounding_crosschecks(b,horiz_i,vert_i+i);
  }
}
