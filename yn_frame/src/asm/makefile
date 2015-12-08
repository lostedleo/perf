include ../../makefile.comm

SRC = $(wildcard *.asm)
OBJECTS = $(patsubst %.asm,$(OBJPATH)/%.o,$(SRC))

all:$(OBJECTS)

$(OBJPATH)/%.o:%.asm
	$(AS) $< -o $@ $(ASFLAGS)

clean:
	cd $(OBJPATH)
	@rm -rf ${OBJECTS} 

