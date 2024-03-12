main: main.c
	$(CC) *.c -o build/main -lSDL2 -lSDL2_Image -lSDL2_TTF

debug: main.c
	$(CC) -g *.c -o build/main -lSDL2 -lSDL2_Image
