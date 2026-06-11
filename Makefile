all:
	gcc src/*.c -o server

clean:
	rm -f server
