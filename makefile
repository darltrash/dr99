CC := clang
CFLAGS := `sdl2-config --libs --cflags` --std=c99 -Wall -lm

build:
	$(CC) -o daw src/*.c $(CFLAGS) -O3

debugbuild:
	$(CC) -o daw src/*.c $(CFLAGS) -DLOG_USE_COLOR=1 -O0 -ggdb

run: debugbuild
	./daw

clean:
	rm daw