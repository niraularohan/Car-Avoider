all:
	g++ -I src/include -L src/lib -o car-avoider main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -lSDL2_ttf