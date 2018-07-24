# check libconfig/ncurses is installed

RM=rm -rf

all: list.o ui.o main.o
	gcc -Wall -o build list.o ui.o main.o -lncurses -lconfig
	mkdir --parents bin
	mv build bin/build

test_int.o: list.o
	gcc -c "src/test_int.c" -o test_int.o

test_int: test_int.o list.o
	gcc -Wall -o test_int test_int.o list.o
	./test_int
	make clean

list.o:
	gcc -c "src/list.c" -o list.o

main.o:
	gcc -c "src/main.c" -o main.o

ui.o:
	gcc -c "src/ui.c" -o ui.o

remake: clean all

clean:
	${RM} build
	${RM} test*
	${RM} bin
	${RM} *.o
	${RM} config.cfg
