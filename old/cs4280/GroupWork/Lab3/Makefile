PLAT:= $(shell uname)

ifeq ($(PLAT),Darwin)
CFLAGS= -framework OpenGL -framework GLUT -framework Foundation
else
CFLAGS= -IGL -IGLU -lglut
endif
CC=g++
SRCS=Project3

all : $(SRCS)

$(SRCS) : 
	$(CC) $(CFLAGS) -o $@ $@.cpp

clean:
	rm $(SRCS)
