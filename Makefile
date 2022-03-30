TARGET = ymirdb

CC = gcc

CFLAGS = -c -Wall -Wvla -Werror -g -std=gnu11 -Werror=format-security
SRC = ymirdb.c
OBJ = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $<

test:
	bash test.sh

clean:
	rm -f *.o *.obj $(TARGET)
