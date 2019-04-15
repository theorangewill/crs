CC = gcc
SRCEXT := c

BUILDDIR := obj
SRCDIR := src
BINDIR := bin
TARGET := bin/crs

SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -O3 -Wall -std=c99 -pedantic -lm -fopenmp 
LIB := -L lib
INC := -I include
DEFINE := -D U_OPENMP


$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $(TARGET) $(CFLAGS) $(INC) $(DEFINE)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) $(DEFINE) -c -o $@ $<

clean:
	@echo Cleaning...
	rm -rf bin/
	rm -rf obj/

.PHONY: clean
