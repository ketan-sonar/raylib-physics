run: out
	./out

out: main.c
	gcc-14 -Wall -Werror `pkg-config --cflags raylib` -o out main.c `pkg-config --libs raylib`

clean:
	rm out
