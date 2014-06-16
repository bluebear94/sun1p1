objects = $(patsubst %.c,%.o,$(wildcard *.c))

sun1p1 : $(objects)
	cc -o sun1p1 $(objects) -lncurses -lm

main.o slp.o stackops.o renderer.o defs.o gen.o combat.o menu.o : defs.h
main.o : menu.h
gen.o : blocks.h
#slp.c : stackops.c

.PHONY : clean
clean : 
	rm sun1p1 $(objects)
.PHONY : print
print: *.c
	lpr -p $?
	touch print
