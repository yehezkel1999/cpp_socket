#
#	Author: Tal Ben Yehezkel
#
# make: make shared library with debug flags
# make release: make shared library with optimazation flags
#
# make staticd: make static library with debug flags
# make staticr: make static library with optimazation flags
# 


# compiler:
CC=g++
# compiler flags: 
CFLAGS=-g -Wall
# linker flags:
LDFLAGS=
# remove command (predefined as rm -f):
RM=rm -f
# zip command:
ZIP=zip

# .cpp and .h files directory:
SRC=src
# object files directory:
OBJ=obj

LIBCPP=$(SRC)/Socket.cpp
LIBH=$(SRC)/Socket.h

# all of the source files: 
ALL_SRCS=$(wildcard $(SRC)/*.cpp)
# all of the object files:
ALL_OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(ALL_SRCS))
# all of the source files, apart from socket.cpp: 
SRCS=$(filter-out $(LIBCPP), $(wildcard $(SRC)/*.cpp))
# all of the object files, apart from socket.o:
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

# output name:
OUT=Socket.so
# static output name:
STATIC=Socket.a
# zipped file name in case the code needs to be submitted as a zipped file
SUBMIT=Socket.zip

# the target to be executed when the makefile is executed (command: make)
all: $(OUT)


# create static library with optimazation flags
staticr: CFLAGS=-O2 -DNDEBUG
staticr: clean
staticr: $(STATIC)

# create static library with debug flags
staticd: $(STATIC)

$(STATIC): $(ALL_OBJS) $(OBJ)
	ar rcs $@ $<


# release build: cleans the build and rebuilds it with optimazation flags 
# rather than debug flags (command: make release)
release: CFLAGS=-O2 -DNDEBUG
release: clean
release: $(OUT)

# make the shared object file with the given compiler, flags and all obj files.
$(OUT): $(OBJS) $(OBJ) $(LIBCPP) $(LIBH)
	$(CC) $(CFLAGS) $(OBJS) -fPIC -shared -o $@ $(LIBCPP) -lc $(LDFLAGS)

# make every obj file with the corresponding cpp file.
$(OBJ)/%.o: $(SRC)/%.cpp $(OBJ) 
	$(CC) $(CFLAGS) -c $< -o $@

# creates obj files directory (if it does not already exist).
$(OBJ):
	mkdir -p $@

# deletes the binary, all the files in the obj files directory, and 
# debug symbols (command: make clean)
clean:
	$(RM) -r $(OBJ)
	$(RM) $(OUT)
	$(RM) $(STATIC)
	$(RM) *.dSYM

# deletes the previously made zipped file and zips the binary, and
# the source files directory (command: make submit) 
zip:
	$(RM) $(SUBMIT)
	$(ZIP) $(SUBMIT) $(OUT) $(SRC)
