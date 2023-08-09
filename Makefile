.RECIPEPREFIX = >
VPATH = src

CXX = /usr/bin/g++
FLAGS = -std=c++23 -Wall -Wextra -Wpedantic -Ofast -lcurses

SRCS = $(wildcard src/*.cc)
OBJS = $(patsubst src/%.cc,%.o,$(SRCS))

typr : $(OBJS)
>       $(CXX) $(FLAGS) -o typr $^
>       $(RM) *.o

%.o : %.cc
>       $(CXX) $(FLAGS) -c $<
