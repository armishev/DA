CC=clang++
CFLAGS=-c -Wall -Werror -Wextra -std=c++17
LDFLAGS=
SOURCES=LZ77.cpp Huffman.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=myzip

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
