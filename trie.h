/* trie.h -- Interface for trie implementation
 * Make tries from dictionary, add member, search tries,
 * Implemented with guidance from www.cs.bu.edu/teaching/tree/trie, which
 * explained trie and included sample structures.
 * There are other implementations of trie online, but I thought making
 * one would be a good learning experience (I did not look at the others).
 */

#include <stdbool.h>

#ifndef TRIE_H
#define TRIE_H

typedef struct trie_node 
{ 
  char key;
  struct trie_node *next, *children;
} t_node;

typedef struct trie {
  t_node *root;
} trie;

// make and free tries

trie *trie_new(char s[]);
t_node *t_node_new(char c);
void trie_free(t_node *t);

// find out if a word is a member of a trie
bool trie_ismember(t_node *root, char s[]);
bool trie_ispartial(t_node *root, char s[]);

// add a word to a trie
void trie_add(trie *t, char s[]);

/* Finds a key at a given level (must give first node of level):
 * returns the node with the key if it is found, otherwise returns the last.
 * non-NULL node of the level,
 */
t_node *find_key(t_node *level, char c);
t_node *partial(t_node *root,char s[]); // returns the node given a string

#endif
