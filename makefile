TARGET     = vpr-extract

CC         = g++
CFLAGS     = -O2 -Wall -Wextra -Werror -Wshadow -Wpedantic -Wconversion \
             -ffunction-sections -ffast-math -funroll-loops -fPIC

LD         = g++
LDFLAGS    = -s

BIN        = Bin
BUILD      = Build

SOURCE     = Sources
OBJECT     = $(BUILD)
SOURCES    = $(wildcard $(SOURCE)/*.cpp)
OBJECTS    = $(patsubst $(SOURCE)/%.cpp,$(OBJECT)/%.obj,$(SOURCES))

ifeq ($(PREFIX),)
PREFIX     = /usr/local
endif

MAKEFLAGS += $(prefix -j,$(shell nproc))

all: $(TARGET)
$(TARGET): $(BIN)/$(PROJECT)

$(BIN)/$(PROJECT): $(BIN) $(BUILD) $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(BIN)/$(TARGET)

$(OBJECT)/%.obj: $(SOURCE)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN):
	mkdir -p $@

$(BUILD):
	mkdir -p $@

.PHONY: tests
tests:
	make -C Tests/PE
	./bin/$(TARGET) Tests/PE/x86-pe.o Tests/PE/x64-pe.o
	#make -C Tests/ELF
	#./bin/$(TARGET) Tests/PE/x86-elf.o Tests/PE/x64-elf.o

.PHONY: install
install: $(BIN)/$(PROJECT)
	install -d $(PREFIX)/bin
	install -m 555 $(BIN)/$(TARGET) $(PREFIX)/bin

.PHONY: clean
clean:
	rm -fr `find . -name "*.o"`
	rm -fr `find . -name "*.obj"`
	rm -fr `find . -name "*.bin"`
	rm -fr `find . -name "*.exe"`
	rm -fr ./bin/*
	rm -fr ./build/*

.PHONY: extra-clean
extra-clean:
	rm -fr ./bin
	rm -fr ./build
