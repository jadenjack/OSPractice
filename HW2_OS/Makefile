CC=gcc
OBJECTS=disk.o fs.o testcase.o mount.o
EXTERN_OBJECT=validate.o
OUT_EXE=a.out

$(OUT_EXE) : disk.o fs.o mount.o testcase.o
	$(CC) -o a.out disk.o fs.o mount.o testcase.o

disk.o : disk.c disk.h
	$(CC) -c disk.c

fs.o : fs.c disk.h fs.h
	$(CC) -c fs.c

mount.o : mount.c fs.h
	$(CC) -c mount.c

testcase.o : testcase.c fs.h disk.h
	$(CC) -c testcase.c 
clean : 
	rm $(OBJECTS) $(OUT_EXE)
