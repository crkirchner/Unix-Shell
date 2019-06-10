# run make in terminal in listed folder
# run ./shell
CC=gcc
CFLAGS=-I. -ggdb
H = shell.h

# set flags
%.o: %.c $(H)
	$(CC) -c -o $@ $< $(CFLAGS)

#shell make launch
shell: main.o
	gcc -o shell main.o -I.

#cclean
clean: 
	rm *.o
	rm shell

#shell installation
install: shell
	cp ./shell $(HOME)/bin/shell
