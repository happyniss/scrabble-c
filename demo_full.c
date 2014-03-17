// demo_full.c
// is broken

/* getline from text file is from http://www.daniweb.com/software-development/c/code/216411/reading-a-file-line-by-line */

#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <ctype.h>

#include "trie.h"
#include "board.h"
#include "scrabble_logic.h"

int main(){ 
  trie *dict;
  char *f = "word";
  dict = trie_new("hello");

  static const char filename[] = "grepdict.txt";
  FILE *file = fopen ( filename, "r" );
  if ( file != NULL )
    {
      char line [ 128 ]; /* or other suitable maximum line size */
      while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
	{
	  int i = 0;
	  while (line[i]!='\0') {
	    line[i] = tolower(line[i]);
	    i++;
	  }
	  trie_add(dict, line);
	}
      fclose ( file );
    }
  else
    {
      perror ( filename ); /* why didn't the file open? */
    }
  
  board *b;
  b = make_board(dict);
  
  // MAKE PLAYER AND TURN
  player *me;
  me = make_players(1);
  add_letter(me, 'r');
  add_letter(me, 'n');
  add_letter(me, 'g');
  add_letter(me,'a');
  add_letter(me,'p');
  add_letter(me,'0');
  printf("Tiles: ");

  turn *t;
  t = make_turn(me);
  make_move_horiz(b,8,8,"banana");
  make_move_vert(b,8,8,"ganana");
  if(crosscheck_letter_vert(b->space[9][7],'n')==false)
    printf("NOOOOOOOOO");
  if(crosscheck_letter_horiz(b->space[9][11],'n')==true)
    printf("YAY");
  if(crosscheck_letter_horiz(b->space[9][11],'f')==true)
    printf("NOOO");
  if(trie_ismember(dict->root,"balls")==true) 
    printf("It is\n");
  for (int i = 1;i<16;i++) {
    for (int j=1;j<16;j++) {
      if (check_anchor(b,i,j)==true) { // if is anchor
	printf("\n---------------\n");
	printf("At space: %d,%d\n", i,j);
	int maxleft = leftpart_max_horiz(b,i,j);
	int maxtop = leftpart_max_vert(b,i,j);
	printf("maxleft: %d\n",maxleft);
	leftpart(b,t,"",dict->root,i,j,maxleft);
	toppart(b,t,"",dict->root,i,j,maxtop); 
      }
    }
  }
  printf("---------------\n");
  printf("---------------\n");
  printf("BEST WORD\n");

  printf("%s\n", t->best_word);
  printf("%d\n",t->best_points);
  printf("--------------\n");

  print_board(b);
  free_board(b);
  trie_free(dict->root);
  free_turn(t);


  return 0;
}
