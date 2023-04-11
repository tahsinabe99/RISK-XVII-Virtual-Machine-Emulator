TARGET = vm_riskxvii

CC = gcc

CFLAGS     = -c -Wall -Wvla -Werror -Os -g -std=c11
LDFLAGS    = -W1,-s 
SRC        = vm_riskxvii.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(LDFAGS) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $<

run:
	./$(TARGET)

test:
	echo what are we testing?!

clean:
	rm -f *.o *.obj $(TARGET)
