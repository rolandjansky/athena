# arch-tag: fdf6c710-9e73-40f6-863a-d5c9026923a9

VPATH = .

PROGRAM  = MakeEff

CXXFLAGS = $(shell root-config --cflags) -I. -Iinclude -I../../MonteCarloReactUtils/MonteCarloReactUtils/

ROOTLIBS = $(shell root-config --libs)

LIBS     = $(ROOTLIBS) -I. -L../../../../InstallArea/i686-slc4-gcc34-opt/lib/ -lMonteCarloReactUtils 

OPT      =

GCC      = gcc

OBJECTS  = MakeEff.o



all:: $(PROGRAM)


$(PROGRAM): $(OBJECTS)
	$(GCC) $(OPT) $(CXXFLAGS) -o $(PROGRAM) -g $(OBJECTS) $(LIBS)

main.run: MakeEff.o
	$(GCC) $(OPT) $(CXXFLAGS) -o $(PROGRAM) -g MakeEff.o

MakeEff.o: MakeEff.cpp
	$(GCC) $(OPT) $(CXXFLAGS) -c MakeEff.cpp

relink:
	rm $(PROGRAM)
	make


clean:
	rm -f EMUAna
	rm -f *~
	rm -f *core*;
	rm -f *.o
