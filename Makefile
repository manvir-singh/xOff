all:
	gcc xoff.c -ggdb -o xoff_debug
release:
	gcc xoff.c -flto -O3 -mwindows -o xoff
clean:
	rm *.exe
