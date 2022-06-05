TARGET    = vpr-extract

CC        = gcc
CFLAGS    = -O3 -Wall -Wextra -Werror -Wshadow -Wpedantic -Wconversion

LD        = gcc
LDFLAGS   = -s

BIN       = bin
BUILD     = build

SOURCE    = src
OBJECT    = $(BUILD)
SOURCES   = $(wildcard $(SOURCE)/*.c)
OBJECTS   = $(patsubst $(SOURCE)/%.c,$(OBJECT)/%.o,$(SOURCES))

INCLUDE   = include
INCLUDES  = $(addprefix -I,$(INCLUDE))

all: $(TARGET)

$(TARGET): $(BIN) $(BUILD) $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(BIN)/$(TARGET)

$(OBJECTS): $(OBJECT)/%.o : $(SOURCE)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN):
	mkdir -p $@

$(BUILD):
	mkdir -p $@

.PHONY: clean
clean:
	rm -fr $(BIN)/*
	rm -fr $(BUILD)/*

.PHONY: extra-clean
extra-clean:
	rm -fr $(BIN)
	rm -fr $(BUILD)
