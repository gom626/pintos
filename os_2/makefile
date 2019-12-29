target=testlib
ob=main.o bitmap.o hash.o list.o
CC=gcc
$(target):$(ob)
	$(CC) -lm -o $@ $(ob)
main.o: list.h bitmap.h hash.h main.c
	gcc -c main.c
bitmap.o: bitmap.c bitmap.h round.h limits.h stdio.c debug.h
	gcc -c bitmap.c
hash.o: hash.h hash.c debug.h
	gcc -c hash.c
list.o: list.c list.h debug.h
	gcc -c list.c
clean:
	rm $(ob) $(target)
