CXX 	 = g++
ROOTCFLAGS  = $(shell root-config --cflags)
ROOTLIBS    = $(shell root-config --libs)
ROOTGLIBS   = $(shell root-config --glibs)
#CXXFLAGS = $(shell root-config --cflags) -Wall -O3
CXXFLAGS   += $(ROOTCFLAGS)
LIBS	 = $(ROOTGLIBS) -lm -L/usr/X11R6 -L/usr/lib -L./ -lMinuit 
#LIBS	 = -lm -L/usr/X11R6 -L/usr/lib -L./

TARGET 	 = makeInactiveModuleList
OBJS	 = makeInactiveModuleList.o

all: ${TARGET}

${TARGET}: ${OBJS}
	@echo "Now make : $@"
	${CXX} ${CXXFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}
	@echo "... Compile done! "

.SUFFIXES: .cc .o
.cc.o:
	@echo "Start Compiling : $<"
	${CXX} ${CXXFLAGS} -c $<
	@echo "... Compiling Object Files $< -->done"
	@echo ""

.PHONY: clean
clean:
	@echo "-- Now Clean Up --"
	rm -f *.o *~ $(TARGET) 

ana.o: 

