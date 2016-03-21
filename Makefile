CFLAGS = -g -O3 -ansi -pedantic -Wall -Wextra -Wno-unused-parameter
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include

HOEDOWN_CFLAGS = $(CFLAGS) -Isrc
ifneq ($(OS),Windows_NT)
	HOEDOWN_CFLAGS += -fPIC
endif

RFCDOWN_SRC=\
	src/autolink.o \
	src/buffer.o \
	src/document.o \
	src/escape.o \
	src/html.o \
	src/html_blocks.o \
	src/html_smartypants.o \
	src/stack.o \
	src/version.o

.PHONY:		all test test-pl clean

all:		librfcdown.so librfcdown.a rfcdown

# Libraries
librfcdown.so: librfcdown.so.1
	ln -f -s $^ $@

librfcdown.so.1: $(RFCDOWN_SRC)
	$(CC) -shared $^ $(LDFLAGS) -o $@
#	$(CC) -Wl,-soname,$(@F) -shared $^ $(LDFLAGS) -o $@

librfcdown.a: $(RFCDOWN_SRC)
	$(AR) rcs librfcdown.a $^

# Executables
rfcdown: bin/rfcdown.o $(RFCDOWN_SRC)
	$(CC) $^ $(LDFLAGS) -o $@

# Perfect hashing
src/html_blocks.c: html_block_names.gperf
	gperf -L ANSI-C -N rfcdown_find_block_tag -c -C -E -S 1 --ignore-case -m100 $^ > $@

# Testing
test: rfcdown
	python test/runner.py

test-pl: rfcdown
	perl test/MarkdownTest_1.0.3/MarkdownTest.pl \
		--script=./rfcdown --testdir=test/MarkdownTest_1.0.3/Tests --tidy

# Housekeeping
clean:
	$(RM) src/*.o bin/*.o
	$(RM) librfcdown.so librfcdown.so.1 librfcdown.a
	$(RM) rfcdown rfcdown.exe

# Installing
install:
	install -m755 -d $(DESTDIR)$(LIBDIR)
	install -m755 -d $(DESTDIR)$(BINDIR)
	install -m755 -d $(DESTDIR)$(INCLUDEDIR)

	install -m644 librfcdown.a $(DESTDIR)$(LIBDIR)
	install -m755 librfcdown.so.1 $(DESTDIR)$(LIBDIR)
	ln -f -s librfcdown.so.1 $(DESTDIR)$(LIBDIR)/librfcdown.so

	install -m755 rfcdown $(DESTDIR)$(PREFIX)/bin

	install -m755 -d $(DESTDIR)$(PREFIX)/include/rfcdown
	install -m644 src/*.h $(DESTDIR)$(PREFIX)/include/rfcdown


# Generic object compilations
%.o: %.c
	$(CC) $(HOEDOWN_CFLAGS) -c -o $@ $<

src/html_blocks.o: src/html_blocks.c
	$(CC) $(HOEDOWN_CFLAGS) -Wno-static-in-inline -c -o $@ $<
