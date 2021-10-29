IDIR =./include
CC= gcc 
CFLAGS=-g -Wall -Wvla -fsanitize=address -I$(IDIR) 

ODIR=obj
#LDIR =../lib

_DEPS = list.h execute.h read.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = shell.o list.o execute.o read.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)	
	$(CC) $(CFLAGS) -c  $< -o $@
shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
