CC=gcc
OBJECTS=disk.o fs.o testcase.o
EXTERN_OBJECT=validate.o
OUT_EXE=hw1

$(OUT_EXE) : disk.o fs.o testcase.o validate.o
	$(CC) -o hw1 disk.o fs.o testcase.o validate.o

disk.o : disk.c disk.h
	$(CC) -c disk.c

fs.o : fs.c disk.h fs.h
	$(CC) -c fs.c

testcase.o : testcase.c fs.h validate.h
	$(CC) -c testcase.c 
clean : 
	rm $(OBJECTS) $(OUT_EXE)
