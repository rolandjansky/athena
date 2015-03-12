

ROOTINC  = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)

hanconfig : hanconfig.cxx
	g++ $(ROOTINC) $< -o $@ $(ROOTLIBS)


