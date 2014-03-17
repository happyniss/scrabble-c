scrabble-c
==========

An attempt at Scrabble with a solver. Buggy.


I attempted to “Take in a representation of a Scrabble board (include triple letter and word squares, etc) which may already have letters on the board, and return the best possible next move (adding just one word to the board)”

My sources included:
Research Papers:
Appel & Jacobson: “The World's Fastest Scrabble Program”
Gordon: “A Faster Scrabble Move Generation Algorithm”
Stack Overflow Questions:
Scrabble Algorithm: http://stackoverflow.com/questions/2497986/writing-an-algorithm-for-scrabble
Other Websites:
Perl Scrabble: http://www.dotnetperls.com/scrabble
Various Source Codes: http://www.gtoal.com/wordgames/scrabble.html


---------

The attempt was to break the program up like so:
Lexicon Representation (Trie)
Board Representation (2-D Arrays)
Determining playable words
Calculating word scores (“special” spaces, blank tiles, & the letter-point hash)

---------

There is a problem with transferring strings in which part of the string gest cut off; I thus need to debug it.

However, some of the functionality can be seen by running the various makefiles. Given the implementation, turning the code into a workable board game shouldn't be too difficult. However, I need to debug the string copying, etc., first. I severely overestimated the simplicity of creating such a program in C, given my skill level. 
