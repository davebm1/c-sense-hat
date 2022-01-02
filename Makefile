CC		= gcc
LIBS	= -li2c
WARN	= -Wall

#################################################

all: pressure humidity led_matrix

pressure:	pressure.o
	$(CC) $(WARN) $? -o $@ $(LIBS)

humidity:	humidity.o
	$(CC) $(WARN) $? -o $@ $(LIBS)

led_matrix:	led_matrix.o
	$(CC) $(WARN) $? -o $@ $(LIBS)

%.o:	%.c
	$(CC) -c $(WARN) $? -o $@

clean:
	rm -f *.o pressure  humidity led_matrix *.c~
