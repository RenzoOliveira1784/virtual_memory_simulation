# -*- Makefile -*-

bin: main.c
	gcc main.c -o vm

.PHONY: run clean 

run: 
	./vm addresses.txt lru fifo

clean: 
	rm vm