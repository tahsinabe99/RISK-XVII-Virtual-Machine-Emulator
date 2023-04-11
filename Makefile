TARGET = vm_riskxvii

CC = gcc

CFLAGS     = -c -Wall -Wvla -Werror -Os -g -std=c11
LDFLAGS    = -Wl,-s
SRC        = vm_riskxvii.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $<

run:
	./$(TARGET)

test:
	echo what are we testing?!

clean:
	rm -f *.o *.obj $(TARGET)
