#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "trie.h"
#include "board.h"
#include "scrabble_logic.h"


turn *make_turn(player *p) {
  turn *result = (turn *) malloc(sizeof(turn));
  assert (result !=NULL);

  result->p1=p;
  result->curr=p;

  result->best_points=0;
  result->blank1 = '\0';
  result->blank2='\0';
  
}

void free_turn(turn *t) {
  free_players(t->p1);
  free((void *) t);
}

void *next_turn(turn *t) {
  // change player
  if (t->curr->next==NULL)
    t->curr=t->p1;
   else
     t->curr=t->curr->next;

   // reset counters
   t->best_points=0;
   t->blank1='\0';
   t->blank2='\0';
 }

void leftpart(board *b,turn *t, char *part,t_node *node,int horiz_i,int vert_i, int lim) {
  if(b->space[horiz_i-1][vert_i].tile_letter!='-') {
    char part2[BOARD_LENGTH];
    get_seg_left(b,horiz_i,vert_i,part2);
    if(trie_ispartial(node,part2)==true) {
      t_node *node2;
      node2=partial(node,part2);
      extendright(b,t,part2,node2,horiz_i,vert_i,&b->space[horiz_i][vert_i]);
    }
  }
  else {
    extendright(b,t,part,node,horiz_i,vert_i,&b->space[horiz_i][vert_i]);
    t_node *curr;
    char part2[BOARD_LENGTH];
    char ltr[2];
    if (lim > 0) {
      for (curr=node;curr!=NULL;curr=curr->next) {
	if (contains_letter(t->curr,curr->key)) {
	  hold_letter(t->curr,curr->key);
	  stringify(curr->key,ltr);
	  strcpy(part2,part);
	  strcat(part2,ltr);
	  
	  leftpart(b,t,part2,curr->children,horiz_i,vert_i,lim-1);
	  putback_letter(t->curr,curr->key);
	}
	else if (contains_blank(t->curr)) {
	  hold_letter(t->curr,'0');
	  if (t->blank1=='\0')
	    t->blank1=curr->key;
	  else
	    t->blank2=curr->key;
	  
	  stringify(curr->key,ltr);
	  strcpy(part2,part);
	  strcpy(part2,ltr);
	  
	  extendright(b,t,part2,curr->children,horiz_i,vert_i,&b->space[horiz_i][vert_i]);
	  putback_letter(t->curr,'0');
	}
      }
    }
  }
}

void toppart(board *b,turn *t, char *part,t_node *node,int horiz_i,int vert_i, int lim) {
  if (b->space[horiz_i][vert_i-1].tile_letter!='-') {
    char part2[BOARD_LENGTH];
    get_seg_up(b,horiz_i,vert_i,part2);
    if(trie_ispartial(node,part2)==true) {
      t_node *node2;
      node2=partial(node,part2);
      extenddown(b,t,part2,node2,horiz_i,vert_i,&b->space[horiz_i][vert_i]);
    }
  }
  else {
    extenddown(b,t,part,node,horiz_i,vert_i,&b->space[horiz_i][vert_i]);
    t_node *curr;
    char part2[BOARD_LENGTH];
    char ltr[2];
    if (lim > 0) {
      for (curr=node;curr!=NULL;curr=curr->next) {
	if (contains_letter(t->curr,curr->key)) {
	  hold_letter(t->curr,curr->key);
	  stringify(curr->key,ltr);
	  strcpy(part2,part);
	  strcat(part2,ltr);
	  
	  toppart(b,t,part2,curr->children,horiz_i,vert_i,lim-1);
	  putback_letter(t->curr,curr->key);
	}
	else if (contains_blank(t->curr)) {
	  hold_letter(t->curr,'0');
	  if (t->blank1=='\0')
	    t->blank1=curr->key;
	  else
	    t->blank2=curr->key;
	  
	  stringify(curr->key,ltr);
	  strcpy(part2,part);
	  strcpy(part2,ltr);
	  
	  extenddown(b,t,part2,curr->children,horiz_i,vert_i,&b->space[horiz_i][vert_i]);
	  putback_letter(t->curr,'0');
	}
      }
    }
  }
}

void extenddown(board *b,turn *t,char *part,t_node *node,int horiz_i,int vert_i,b_space *anchor) {
  t_node *curr;
  char part2[BOARD_LENGTH];
  char ltr[2];
  
  // if the space is empty try to build with letters on the rack
  if (space_isempty(b->space[horiz_i][vert_i])==true){
    
    for(curr=node;curr!=NULL;curr=curr->next) {
      if (node->key=='\0'&& &b->space[horiz_i][vert_i]!=anchor) // if node is the end of a word
	legalmove_vert(part,b,t,horiz_i,vert_i); // the string part is a word
      
      // if the letter is in the crosscheck
      else if (crosscheck_letter_vert(b->space[horiz_i][vert_i],curr->key)==true) {
	if (contains_letter(t->curr,curr->key)) {
	  hold_letter(t->curr,curr->key);
	  stringify(curr->key,ltr);
	  strcpy(part2,part);
	  strcat(part2,ltr);
	  
	  extenddown(b,t,part2,curr->children,horiz_i,vert_i+1,anchor);
	  putback_letter(t->curr,curr->key);
	}
	else if (contains_blank(t->curr)) {
	  hold_letter(t->curr,'0');
	  if (t->blank1=='\0')
	    t->blank1=curr->key;
	  else
	    t->blank2=curr->key;
	  stringify(curr->key,ltr);
	  strcpy(part2,part);
	  strcpy(part2,ltr);
	  extenddown(b,t,part2,curr->children,horiz_i,vert_i+1,anchor);
	  putback_letter(t->curr,'0');
	}
      }
    }
  }
  else { // if the space is occupied, see if it is possible to contin
    for (curr=node;curr!=NULL;curr=curr->next) {
      if (curr->key==b->space[horiz_i][vert_i].tile_letter) {
	strcpy(part2,part);
	stringify(b->space[horiz_i][vert_i].tile_letter,ltr);
	strcat(part2,ltr);
	
	extenddown(b,t,part2,curr->children,horiz_i,vert_i+1,anchor);
	break;
      }
    }
  }
}

void extendright(board *b,turn *t,char *part,t_node *node,int horiz_i,int vert_i,b_space *anchor) {
  t_node *curr;
  char part2[BOARD_LENGTH];
  char ltr[2];
  
  // if the space is empty try to build with letters on the rack
  if (space_isempty(b->space[horiz_i][vert_i])==true){
    
    for(curr=node;curr!=NULL;curr=curr->next) {
      if (node->key=='\0'&& &b->space[horiz_i][vert_i]!=anchor) // if node is the end of a word
	legalmove_horiz(part,b,t,horiz_i,vert_i); // the string part is a word
      
      // if the letter is in the crosscheck
      else if (crosscheck_letter_vert(b->space[horiz_i][vert_i],curr->key)==true) {
	if (contains_letter(t->curr,curr->key)) {
	  hold_letter(t->curr,curr->key);
	  stringify(curr->key,ltr);
	  strcpy(part2,part);
	  strcat(part2,ltr);
	  
	  extendright(b,t,part2,curr->children,horiz_i+1,vert_i,anchor);
	  putback_letter(t->curr,curr->key);
	}
	else if (contains_blank(t->curr)) {
	  hold_letter(t->curr,'0');
	  if (t->blank1=='\0')
	    t->blank1=curr->key;
	  else
	    t->blank2=curr->key;
	  stringify(curr->key,ltr);
	  strcpy(part2,part);
	  strcpy(part2,ltr);
	  extendright(b,t,part2,curr->children,horiz_i+1,vert_i,anchor);
	  putback_letter(t->curr,'0');
	}
      }
    }
  }
  else { // if the space is occupied, see if it is possible to contin
    for (curr=node;curr!=NULL;curr=curr->next) {
      if (curr->key==b->space[horiz_i][vert_i].tile_letter) {
	strcpy(part2,part);
	stringify(b->space[horiz_i][vert_i].tile_letter,ltr);
	strcat(part2,ltr);
	
	extendright(b,t,part2,curr->children,horiz_i+1,vert_i,anchor);
	break;
      }
    }
  }
}


void legalmove_horiz(char *part,board *b,turn *t, int horiz_i, int vert_i) {
  printf("Legal move: %s\n",part);
  
  // letters_played
  int pointcount = 0;
  int crosscheck_total=0;
  int multiplier = 1;
  int letters_used[TILES]={0};

  // get word score
  for (int i=0;i<strlen(part);i++) {
    b_space curspace = b->space[horiz_i-strlen(part)+i][vert_i];
    char placing = part[strlen(part)-1-i]; // current letter
    
    if(space_isempty(curspace)==true) { // if space is empty
      
      // get crosscheck points
      
      int crosscheck_pts = 0;
      int j;
      j = count_top(b,horiz_i,vert_i);
      while(j>0){
	crosscheck_pts+=b->space[horiz_i][vert_i-i].points;
	j--;
      }
      while(b->space[horiz_i][vert_i+1+j].tile_letter!='-'){
	crosscheck_pts+=b->space[horiz_i][vert_i+1+j].tile_letter;
	j++;
      }
      
      
      if (letters_used[placing-'a']<t->curr->off_rack[i]) { // if character tile left
	letters_used[placing-'a']++; // show that using char
	
	// get points for word placed
	if (curspace.special==12) { // double letter
	  pointcount+=(tilescore(placing)*2);
	  if (b->space[horiz_i-strlen(part)+i][vert_i+1].tile_letter!='-' || b->space[horiz_i-strlen(part)+i][vert_i-1].tile_letter!='-') // if crosscheck
	    crosscheck_pts+=(tilescore(placing)*2); // get ltr pts 2x
	}
	
	else if (curspace.special==13){ // triple letter
	  pointcount+=(tilescore(placing)*3);
	  
	  if (b->space[horiz_i-strlen(part)+i][vert_i+1].tile_letter!='-' || b->space[horiz_i-strlen(part)+i][vert_i-1].tile_letter!='-') // if crosscheck
	    crosscheck_pts+=(tilescore(placing)*3); // get crosscheck
	}
	
	else { // not special
	  pointcount+=tilescore(placing);
	  if (crosscheck_letter_vert(curspace,placing)==true)
	    pointcount+=tilescore(placing);
	}
      }
      else if (letters_used[26]<t->curr->off_rack[26]){ // use blank;
	letters_used[26]++;
      }

      
      // get multiplier, if any
      if(curspace.special==14) {
	multiplier*=2; // double word
	crosscheck_pts*=2; // double word for crosscheck points
      }
      else if(curspace.special==15) {
	multiplier*=3; // triple word
	crosscheck_pts*=3;
      }
      crosscheck_total+=crosscheck_pts; // add crosscheck points to total
    }
    else {// if there is already a tile on the board
      pointcount+=curspace.points; // add the value of the tile
    }

    pointcount*=multiplier; // multiply points by any double or triple WS
    
    pointcount+=crosscheck_total; // add all crosscheck points
    
  }
  
  printf("Points: %d\n",pointcount);
  if (pointcount>t->best_points) {
    t->best_points=pointcount;
    strcpy(t->best_word,part);
  }
}

void legalmove_vert(char *part,board *b,turn *t, int horiz_i, int vert_i) {
  printf("Legal move: %s\n",part);
  
  // letters_played
  int pointcount = 0;
  int crosscheck_total=0;
  int multiplier = 1;
  int letters_used[TILES]={0};

  // get word score
  for (int i=0;i<strlen(part);i++) {
    b_space curspace = b->space[horiz_i][vert_i-strlen(part)+1];
    char placing = part[strlen(part)-1-i]; // current letter
    
    if(space_isempty(curspace)==true) { // if space is empty
      
      // get crosscheck points
      
      int crosscheck_pts = 0;
      int j;
      j = count_top(b,horiz_i,vert_i);
      while(j>0){
	crosscheck_pts+=b->space[horiz_i-i][vert_i].points;
	j--;
      }
      while(b->space[horiz_i+1+j][vert_i].tile_letter!='-'){
	crosscheck_pts+=b->space[horiz_i+1+j][vert_i].tile_letter;
	j++;
      }
      
      
      if (letters_used[placing-'a']<t->curr->off_rack[i]) { // if character tile left
	letters_used[placing-'a']++; // show that using char
	
	// get points for word placed
	if (curspace.special==12) { // double letter
	  pointcount+=(tilescore(placing)*2);
	  if (b->space[horiz_i+1][vert_i-strlen(part)+i].tile_letter!='-' || b->space[horiz_i-1][vert_i-strlen(part)+i].tile_letter!='-') // if crosscheck
	    crosscheck_pts+=(tilescore(placing)*2); // get ltr pts 2x
	}
	
	else if (curspace.special==13){ // triple letter
	  pointcount+=(tilescore(placing)*3);
	  
	  if (b->space[horiz_i+1][vert_i-strlen(part)+i].tile_letter!='-' || b->space[horiz_i-1][vert_i-strlen(part)+i].tile_letter!='-') // if crosscheck
	    crosscheck_pts+=(tilescore(placing)*3); // get crosscheck
	}
	
	else { // not special
	  pointcount+=tilescore(placing);
	  if (crosscheck_letter_horiz(curspace,placing)==true)
	    pointcount+=tilescore(placing);
	}
      }
      else if (letters_used[26]<t->curr->off_rack[26]){ // use blank;
	letters_used[26]++;
      }

      
      // get multiplier, if any
      if(curspace.special==14) {
	multiplier*=2; // double word
	crosscheck_pts*=2; // double word for crosscheck points
      }
      else if(curspace.special==15) {
	multiplier*=3; // triple word
	crosscheck_pts*=3;
      }
      crosscheck_total+=crosscheck_pts; // add crosscheck points to total
    }
    else {// if there is already a tile on the board
      pointcount+=curspace.points; // add the value of the tile
    }

    pointcount*=multiplier; // multiply points by any double or triple WS
    
    pointcount+=crosscheck_total; // add all crosscheck points
    
  }
  
  printf("Points: %d\n",pointcount);
  if (pointcount>t->best_points) {
    t->best_points=pointcount;
    strcpy(t->best_word,part);
  }
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. Designed by Ben Pfaff */

// I can use this for implementing a shuffled stack of tiles to draw

void shuffle(char *array, size_t n)
{
  if (n > 1) {
    size_t i;
    for (i = 0; i < n - 1; i++) {
      size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
      int t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}
