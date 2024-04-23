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
	./floating  0.5 1000000 -134 -23134 0.00001>  testOutput
	@echo The following should be empty if there are no problems
	diff sampleOutput testOutput 2> /dev/null
	@echo Testing complete
