#OBJS = $(patsubst %.c,%.o,$(shell find -name "*.c"))
OBJS = main.o
LIBS = 
OUT = ./vcpu
CFLAGS = -O3 -Wall -fgnu89-inline
PREFIX = /usr/bin

# link
$(OUT): $(OBJS)
	@echo "[INFO] linking"
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(OUT)
	@echo

install: $(OUT)
	install $(OUT) $(PREFIX)

.PHONY: assembler

assembler: 
	$(CC) $(CFLAGS) assembler.c -o assembler

debug: CFLAGS += -ggdb -DYDEBUG
debug: $(OUT)

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.c
	@echo "[INFO] generating deps for $*.o"
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	@echo

# remove compilation products
clean:
	$(RM) $(OBJS) $%.d

