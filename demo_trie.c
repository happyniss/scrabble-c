#include <stdio.h>
#include <stdlib.h>

#include "trie.h"

int main() {
  trie *my_tree;
  trie *ph;
  char *f = "word";
  ph = trie_new("he");
  printf("New trie created; contains word 'he'\n");

  trie_add(ph, "heal");
  printf("Added word 'heal'\n");
  trie_add(ph,"lol");
  printf("Added word 'lol'\n");
  trie_add(ph,"meep");
  printf("Added word 'meep'\n");
  printf("Checking whether the word 'meep' is in the dictionary.\n");
  if(trie_ismember(ph->root,"meep")==true) 
    printf("It is\n");
  printf("Checking whether 'lo' can start a word in the dictionary\n");
  if (trie_ispartial(ph->root,"lo")==true) 
    printf("It is\n");
  printf("Freeing trie\n");
  trie_free(ph->root);
  return 0;
}

