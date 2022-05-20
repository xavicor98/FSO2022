run : mur4
	./mur4 params.txt

run_test : mur4
	numactl --physcpubind=+1 ./mur4 params.txt

mur0 : mur0.c winsuport.o winsuport.h
	gcc -Wall -g mur0.c winsuport.o -o mur0 -lcurses


mur1 : mur1.c winsuport.o winsuport.h
	gcc -Wall -g mur1.c winsuport.o -o mur1 -lcurses -lpthread


mur2 : mur2.c winsuport.o winsuport.h
	gcc -Wall -g mur2.c winsuport.o -o mur2 -lcurses -lpthread



mur3 : pilota3 mur3.c mur3.h winsuport2.o winsuport2.h memoria.o
	gcc -Wall -g mur3.c winsuport2.o memoria.o -o mur3 -lcurses -lpthread

pilota3 : pilota3.c mur3.h winsuport2.o winsuport2.h memoria.o
	gcc -Wall -g pilota3.c winsuport2.o memoria.o  -o pilota3 -lcurses



mur4 : pilota4 mur4.c mur4.h winsuport2.o winsuport2.h memoria.o semafor.o missatge.o
	gcc -Wall -g mur4.c winsuport2.o memoria.o semafor.o missatge.o  -o mur4 -lcurses -lpthread

pilota4 : pilota4.c mur4.h winsuport2.o winsuport2.h memoria.o semafor.o missatge.o
	gcc -Wall -g pilota4.c winsuport2.o memoria.o semafor.o missatge.o -o pilota4 -lcurses


winsuport.o : winsuport.c winsuport.h
	gcc -Wall -g -c winsuport.c -o winsuport.o


memoria.o : memoria.c memoria.h
	gcc -Wall -g -c memoria.c -o memoria.o

winsuport2.o : winsuport2.c winsuport2.h
	gcc -Wall -g -c winsuport2.c -o winsuport2.o


semafor.o : semafor.c semafor.h
	gcc -c -Wall semafor.c -o semafor.o 

missatge.o : missatge.c missatge.h
	gcc -c -Wall missatge.c -o missatge.o


clean :
	rm -f winsuport.o winsuport2.o semafor.o missatge.o memoria.o mur0 mur1 mur2 mur3 pilota3 mur4 pilota4

