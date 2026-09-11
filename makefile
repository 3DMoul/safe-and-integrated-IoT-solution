CC=C:\avr\bin\avr-gcc
LD=C:\avr\bin\avr-ld
OBJCOPY="C:\avr\bin\avr-objcopy"
OBJDUMP="C:\avr\bin\avr-objdump"
AVRSIZE="C:\avr\bin\avr-size"

MCU=atmega328p

CFLAGS=-Wall -Wextra  -Wundef -pedantic \
		-Os -std=gnu99 -DF_CPU=16000000UL -mmcu=${MCU}
LDFLAGS=-mmcu=$(MCU)
PORT=\\\\.\\COM3

BIN=exefile

# OUT=${BIN}.elf ${BIN}.hex ${BIN}.lss
OUT=${BIN}.hex

SOURCES = src/main.c src/lcd.c src/millis.c src/companymanager.c src/uart.c

LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm

OBJS = $(SOURCES:.c=.o)

all: $(OUT)
$(OBJS): Makefile

#-include $(OBJS:.o=,P)
%.o:%.c
	$(COMPILE.c) -MD -o $@ $<

%.lss: %.elf
	$(OBJDUMP) -h -S -s $< > $@

%.elf: $(OBJS)
	$(CC) -Wl,-Map=$(@:.elf=.map) $(LDFLAGS) -o $@ $^
	$(AVRSIZE) $@

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .fuse -R .lock -R .user_signatures -R .comment $< $@

isp: ${BIN}.hex
	C:\avr\bin\avrdude -F -V -c arduino -p ${MCU} -P ${PORT} -U flash:w:$<

clean:
	rm -f $(OUT) $(OBJS) *.map *.P *.d