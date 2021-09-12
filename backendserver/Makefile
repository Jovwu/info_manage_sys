.PHONY: all clean

#Add all source *.cc files
SRC=$(shell find -name '*.cc')

#Create all source *.cc Mapping source *.o files
OBJ=$(SRC:.cc=.o)

#Create the execute file name
ELF=backendserver

INCLUDES := -I. 
LIBS :=

#Add gdb Debug and library ncurses
CFLAG=-Wall -g $(INCLUDES) 
LDFLAG=-lpthread -lsqlite3 -lcrypto -std=c++11$(LIBS)
CC=GCC $(CFLAG)
CXX=g++ $(CFLAG)

all:$(ELF)

$(ELF):$(OBJ)
	$(CXX) $^ -o $@ $(LDFLAG)

.c.o:
	#@echo $(@D) $(<F)  $(@F)
	$(CXX) -c $(@D)/$(<F) -o $(@D)/$(@F)

#%.o:%.cc
.cc.o:
	$(CXX) -c $(@D)/$(<F) -o $(@D)/$(@F)

clean:
	-rm -f $(OBJ) *.o $(ELF)
