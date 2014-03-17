# Makefile for demonstration 1

SRC=demo_board.c trie.c board.c scrabble_logic.c 
OBJ=demo_board.o trie.o board.o scrabble_logic.o 

CFLAGS= -std=c99

scrabble: ${OBJ} 
	gcc ${CFLAGS} ${OBJ} -o scrabble

depend:
	makedepend -Y ${SRC} &> /dev/null

clean:
	rm -f ${OBJ} scrabble

install: scrabble
	cp scrabble ~/bin

# DO NOT DELETE.

trie.o: trie.h

board.o: trie.h board.h

scrabble_logic.o: trie.h board.h scrabble_logic.h

demo_board.o: scrabble_logic.h
