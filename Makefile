test: DHT_func.o main.o
	gcc -g -Wall -Wextra -o test DHT_func.o main.o

DHT_func.o: DHT_func.c
	gcc -g -Wall -Wextra -c DHT_func.c

main.o: main.c
	gcc -g -Wall -Wextra -c main.c

clean:	
	rm -f *.o test