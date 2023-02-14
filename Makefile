# A makefile for smokers.c

smokers: smokers.c
	gcc -pthread -o smokers smokers.c