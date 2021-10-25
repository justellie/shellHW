IDIR =../include
CC= gcc 
CFLAGS=-g -W -fsanitize=address -I$(IDIR) 

ODIR=obj
#LDIR =../lib

_DEPS = list.h ejecutar.h lectura.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = shell.o list.o ejecutar.o lectura.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)	
	$(CC) $(CFLAGS) -c  $< -o $@
shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
