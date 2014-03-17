// trie.c
// Implementation of trie:

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "trie.h"

t_node *t_node_new(char c) {
  t_node *result = (t_node *) malloc(sizeof(t_node));
  assert (result !=NULL);
  
  result->key = c;
  result->next = NULL;
  result->children = NULL;
  
  return result;
}

trie *trie_new(char c[]) {
  int i;
  trie *result = (trie *) malloc(sizeof(trie));
  assert(result != NULL);
  
  t_node *curr;
  curr = t_node_new(c[0]);
  result->root = curr;
  
  for (i=1;c[i];i++) {
    curr->children = t_node_new(c[i]);
    curr = curr->children;
  }
  
  curr->children = t_node_new('\0'); // add null character to signal end word
  return result;
}

void trie_free(t_node *root) {
  if (root == NULL)
    return;
  
  trie_free(root->next);
  trie_free(root->children);

  free((void *) root);
}


t_node *find_key(t_node *level, char c) {
  t_node *curr;
  for (curr=level; curr->next != NULL; curr = curr->next) {
    if (curr->key == c)
      return curr;
  }
  return curr;
}

bool trie_ismember(t_node *root, char s[]) {
  t_node *lvl = root;
  int i = 0;  // start at beginning of string
  t_node * curr;

  while(1) {
    curr = find_key(lvl,s[i]); // find key on level
    if (curr->key != s[i]) // if key isn't present, string not member
      return false;
    if (s[i] == '\0') // matched end of key; string is member
      return true;
    // look for next letter in the string
      lvl = curr->children;
      i++;
  }
}

// same as trie_ismember but doesn't have to contain full word

t_node *partial(t_node *root, char s[]) {

  t_node *lvl = root;
  int i = 0;// start at beginning of string
  t_node *curr;

  while(1){
    // check if end of string being checked has been reached first
    if (s[i]=='\0')
      return lvl;

    curr = find_key(lvl,s[i]);
      
    lvl = curr->children;
    i++;
  }
}

bool trie_ispartial(t_node *root, char s[]) {

  t_node *lvl = root;
  int i = 0;// start at beginning of string
  t_node *curr;

  while(1){
    // check if end of string being checked has been reached first
    if (s[i]=='\0')
      return true;

    curr = find_key(lvl,s[i]);
    if (curr->key!=s[i]) // key isn't present
      return false;
    lvl = curr->children;
    i++;
  }
}

void trie_add(trie *t, char s[]) { // add if not already in dictionary
  t_node *lvl = t->root; // top level
  int i = 0; // beginning of s
  
  while(1) {
    t_node *curr;
    curr = find_key(lvl, s[i]);
    
    // if the letter is not on that level
    if (curr->key != s[i]) {
      curr->next = t_node_new(s[i]); // add it
      curr = curr->next; // move to the newly-added letter
    }
    
    // if end of word to add has been reached
    if (s[i] == '\0') 
      break;
    
    // otherwise, move to the next level and the next letter:
    // if there are children of the current node, search for next letter
    // by repeating the loop with the next level
    if (curr->children != NULL) {
      lvl = curr->children;
      i++;
    }
    // but if the current node has no children, add the rest of the word
    else
      while (1) {
	i++;
	curr->children = t_node_new(s[i]);
	curr = curr->children;
	if (s[i] == '\0') // if we have added the end of the word
	  break;
      }
  }   
}

