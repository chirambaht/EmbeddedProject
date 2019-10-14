compile:
	gcc -Wall -o run src/main.c -lwiringPi -lpthread

run:
	./run

clean:
	rm run
