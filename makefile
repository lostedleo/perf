FRAMECOMMON = ./yn_frame
SRVFRAME =
INCLUDE = -I/usr/local/include -I${FRAMECOMMON}/inc
LIBS = -lz -lpthread -lssl -L${FRAMECOMMON}/obj/ -lynframe

CFLAGS =-g3 -Wall -fPIC #-D USE_TCMALLOC=yes
BINARY = $(patsubst %.c,%,$(wildcard *.c))
BINARY_CPP = $(patsubst %.cpp,%,$(wildcard *.cpp))

all:$(BINARY) ${BINARY_CPP}

%:%.c
	$(CXX) $< -o $@  $(INCLUDE) $(LIBS) $(CFLAGS)

%:%.cpp
	$(CXX) $< -o $@  $(INCLUDE) $(LIBS) $(CFLAGS)

clean:
	@rm -rf ${BINARY} ${BINARY_CPP}
