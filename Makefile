CC=gcc
FLAGS=-Wall
MAIN=queue.o
DEPENDECIES=
BIN=queue

all: run

run: build
	@./main

build: 
	$(CC) -c queue.c $(FLAGS)