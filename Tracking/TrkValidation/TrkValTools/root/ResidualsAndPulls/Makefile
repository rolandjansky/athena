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
ROOTLIBS    := $(shell root-config --libs)
ROOTGLIBS   := $(shell root-config --glibs)
ROOTFLAGS   := $(shell root-config --cflags)
#ROOTLIBS     = -L$(ROOTSYS)/lib -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -pthread -lm -ldl -rdynamic
#ROOTGLIBS    = -L$(ROOTSYS)/lib -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lGui -pthread -lm -ldl -rdynamic 
#ROOTFLAGS    = -pthread -I$(ROOTSYS)/include
ROOTINCS     = -I$(ROOTSYS)/include
ROOTCINT     = $(ROOTSYS)/bin/rootcint
CINTINCS     = $(CXXINCS) $(ROOTINCS)
INCLUDES     = $(CXXINCS) $(ROOTINCS)
LIBS         = $(CXXLIBS) $(ROOTLIBS)
GLIBS        = $(ROOTGLIBS) $(SYSXLIBS)
MYINCL       = -I./inc

# -L$(ROOTSYS)/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lGui -pthread -lm -ldl -rdynamic

#INCDIR       = pwd #/inc/
#LIBDIR       = pwd #/lib/
#RUNDIR       = pwd #/
#SRCDIR       = pwd #/src/

Pixel:	$(SRCDIR)PixelResidualsAndPulls.C 
	@echo -e [make] GCC: PixelResidualsAndPulls object
	$(CXX) $(CXXOPTS) $(CXXFLAGS) $(MYINCL) $(INCLUDES) -I$(CINTSYSDIR) -c -o $(LIBDIR)PixelResidualsAndPulls.o $(SRCDIR)PixelResidualsAndPulls.C
	@echo -e [make] GCC: PixelResidualsAndPulls executable
	$(CXX) $(LDFLAGS) -o $(RUNDIR)PixelResidualsAndPulls $(ROOTGLIBS) $(LIBDIR)PixelResidualsAndPulls.o

SCT:	$(SRCDIR)SCT_ResidualsAndPulls.C 
	@echo -e [make] GCC: SCT_ResidualsAndPulls object
	$(CXX) $(CXXOPTS) $(CXXFLAGS) $(MYINCL) $(INCLUDES) -I$(CINTSYSDIR) -c -o $(LIBDIR)SCT_ResidualsAndPulls.o $(SRCDIR)SCT_ResidualsAndPulls.C
	@echo -e [make] GCC: SCT_ResidualsAndPulls executable
	$(CXX) $(LDFLAGS) -o $(RUNDIR)SCT_ResidualsAndPulls $(ROOTGLIBS) $(LIBDIR)SCT_ResidualsAndPulls.o

TRT:	$(SRCDIR)TRT_ResidualsAndPulls.C 
	@echo -e [make] GCC: TRT_ResidualsAndPulls object
	$(CXX) $(CXXOPTS) $(CXXFLAGS) $(MYINCL) $(INCLUDES) -I$(CINTSYSDIR) -c -o $(LIBDIR)TRT_ResidualsAndPulls.o $(SRCDIR)TRT_ResidualsAndPulls.C
	@echo -e [make] GCC: TRT_ResidualsAndPulls executable
	$(CXX) $(LDFLAGS) -o $(RUNDIR)TRT_ResidualsAndPulls $(ROOTGLIBS) $(LIBDIR)TRT_ResidualsAndPulls.o

All:	$(SRCDIR)AllResidualsAndPulls.C
	@echo -e [make] GCC: AllResidualsAndPulls object
	$(CXX) $(CXXOPTS) $(CXXFLAGS) $(MYINCL) $(INCLUDES) -I$(CINTSYSDIR) -c -o $(LIBDIR)AllResidualsAndPulls.o $(SRCDIR)AllResidualsAndPulls.C
	@echo -e [make] GCC: AllResidualsAndPulls executable
	$(CXX) $(LDFLAGS) -o $(RUNDIR)AllResidualsAndPulls $(ROOTGLIBS) $(LIBDIR)AllResidualsAndPulls.o


clean:
	rm -f $(LIBDIR)*.o $(LIBDIR)core
distclean: clean
	rm -f $(LIBDIR)*.so
