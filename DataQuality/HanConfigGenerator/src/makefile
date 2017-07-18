

ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)

SOURCES = hcg.cxx addnode.cxx

# install : hcg
# 	cp hcg $(HOME)/bin 

hcg : $(SOURCES) 
	g++ $(ROOTCFLAGS) -O3 -o $@ $(SOURCES) $(ROOTLIBS) $(READLIB)

archive :  $(SOURCES) node.h addnode.h simpletimer.h spacer.h makefile README
	tar -czf hcg.tgz $?

install : hcg
	cp hcg $(HOME)/bin

clean : 
	rm hcg
