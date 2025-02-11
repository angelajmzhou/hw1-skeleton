all: floating

floating : floating.o main.o
	gcc -g -Wall -o floating floating.o main.o

floating.o : floating.c floating.h 
	gcc -g -c -Wall floating.c

main.o : main.c floating.h 
	gcc -g -c -Wall main.c

clean :
	rm *.o

test : clean floating
	touch testOutput
	./floating  0x33c00000  0x387fe000>  testOutput
	@echo The following should be empty if there are no problems
	diff sampleOutput testOutput 2> /dev/null
	@echo Testing complete
