include ../makefile.comm
SRC = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJPATH)/%.o,$(SRC))
DEPEND = ${OBJECTS:.o=.d}

all:$(OBJECTS)
	cd asm;make;cd ..

-include ${DEPEND}

$(OBJPATH)/%.o:%.cpp
	$(CXX) $< -o $@ -c $(INCLUDE) $(CFLAGS)

$(OBJPATH)/%.d:%.cpp
	@set -e; rm -f $@; \
	$(CXX) -M $< $(INCLUDE) $(CFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJPATH)//\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	@rm -rf ${OBJECTS} 
	@rm -rf ${DEPEND}

