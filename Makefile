compile:
	gcc -Wall -o run src/main.c -lwiringPi

run:
	./run

clean:
	rm run
