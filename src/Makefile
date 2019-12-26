GOAL = mbtan
CC = gcc
FLAGS = -lm -g -Wall
OGL = -lGL -lGLU -lglut

$(GOAL): main.o drawing.o image.o
	$(CC) -o $@ $^ $(FLAGS) $(OGL)

main.o: main.c drawing.h drawing.c image.h image.c
	$(CC) -o $@ $< -c

drawing.o: drawing.c drawing.h
	$(CC) -o $@ $< -c

func.o: image.c image.h
	$(CC) -o $@ $< -c


.PHONY:
	clean

clean:
	rm -rf $(GOAL) *.o
