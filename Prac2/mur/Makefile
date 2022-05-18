run : mur3
	./mur3 params.txt

run_test : mur3
	numactl --physcpubind=+1 ./mur3 params.txt

mur0 : mur0.c winsuport.o winsuport.h
	gcc -Wall -g mur0.c winsuport.o -o mur0 -lcurses


mur1 : mur1.c winsuport.o winsuport.h
	gcc -Wall -g mur1.c winsuport.o -o mur1 -lcurses -lpthread


mur2 : mur2.c winsuport.o winsuport.h
	gcc -Wall -g mur2.c winsuport.o -o mur2 -lcurses -lpthread



mur3 : pilota3 mur3.c winsuport2.o winsuport2.h memoria.o
	gcc -Wall -g mur3.c winsuport2.o memoria.o -o mur3 -lcurses -lpthread

pilota3 : pilota3.c winsuport2.o winsuport2.h memoria.o
	gcc -Wall -g pilota3.c winsuport2.o memoria.o  -o pilota3 -lcurses -lpthread


winsuport.o : winsuport.c winsuport.h
	gcc -Wall -g -c winsuport.c -o winsuport.o


memoria.o : memoria.c memoria.h
	gcc -Wall -g -c memoria.c -o memoria.o

winsuport2.o : winsuport2.c winsuport2.h
	gcc -Wall -g -c winsuport2.c -o winsuport2.o


clean : 
	rm -f winsuport.o winsuport2.o memoria.o mur0 mur1 mur2 mur3 pilota3

