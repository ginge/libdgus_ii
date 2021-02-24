CC=gcc
CFLAGS=-I. -g
DEPS = dgus_reg.h dgus.h dgus_util.h dgus_curve.h dgus_config.h dgus_control_text.h 
_OBJ = dgus_lcd.o dgus_util.c dgus_control_curve.o dgus_control_text.o main.o 
ODIR=.

LIBS=-l serialport

OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dgusmain: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

dgusmain-debug: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core dgusmain dgusmain-debug