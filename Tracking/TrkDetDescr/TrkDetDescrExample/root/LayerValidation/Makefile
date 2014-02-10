CXX          = g++
#CXXOPTS      = -O2
CXXOPTS      = -g #-O
CXXFLAGS     = -Wall -fPIC
CXXINCS      = 
SOFLAGS      = -shared
LDFLAGS      = -g #-O 
CXXLIBS      = -lm -ldl -rdynamic
SYSXLIBS     = -L/usr/X11R6/lib -lXpm -lX11

CINTSYSDIR   = $(ROOTSYS)/cint
#ROOTLIBS    := $(shell root-config --libs)
#ROOTGLIBS   := $(shell root-config --glibs)
#ROOTFLAGS   := $(shell root-config --cflags)
ROOTLIBS     = -L$(ROOTSYS)/lib -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -pthread -lm -ldl -rdynamic
ROOTGLIBS    = -L$(ROOTSYS)/lib -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lGui -pthread -lm -ldl -rdynamic
ROOTFLAGS    = -pthread -I$(ROOTSYS)/include
ROOTINCS     = -I$(ROOTSYS)/include
ROOTCINT     = $(ROOTSYS)/bin/rootcint
CINTINCS     = $(CXXINCS) $(ROOTINCS)
INCLUDES     = $(CXXINCS) $(ROOTINCS)
LIBS         = $(CXXLIBS) $(ROOTLIBS)
GLIBS        = $(ROOTGLIBS) $(SYSXLIBS)
MYINCL       = -I./inc

#INCDIR       = pwd #/inc/
#LIBDIR       = pwd #/lib/
#RUNDIR       = pwd #/
#SRCDIR       = pwd #/src/

$(LIBDIR)LayerValidationTree.o:	$(SRCDIR)LayerValidationTree.C	
	@echo -e [make] GCC: Generating LayerValidationTree object file
	$(CXX) $(CXXOPTS) $(CXXFLAGS) $(MYINCL) $(INCLUDES) -c -o $(LIBDIR)LayerValidationTree.o $(SRCDIR)LayerValidationTree.C

layval :	$(SRCDIR)LayerValidation.C $(LIBDIR)LayerValidationTree.o 
	@echo -e [make] GCC: Generating LayerValidation object file
	$(CXX) $(CXXOPTS) $(CXXFLAGS) $(MYINCL) $(INCLUDES) -I$(CINTSYSDIR) -c -o $(LIBDIR)LayerValidation.o $(SRCDIR)LayerValidation.C
	@echo -e [make] GCC: LayerValidation executable
	$(CXX) $(LDFLAGS) -o $(RUNDIR)LayerValidation $(ROOTLIBS) $(ROOTGLIBS) $(LIBDIR)LayerValidation.o $(LIBDIR)LayerValidationTree.o

clean:
	rm -f $(LIBDIR)*.o $(LIBDIR)core
distclean: clean
	rm -f $(LIBDIR)*.so
