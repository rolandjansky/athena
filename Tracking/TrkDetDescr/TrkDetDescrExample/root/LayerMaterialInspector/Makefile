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

$(LIBDIR)CylinderLayerMaterial.o:	$(SRCDIR)CylinderLayerMaterial.C	
	@echo -e [make] GCC: Generating CylinderLayerMaterial object file
	$(CXX) $(CXXOPTS) $(CXXFLAGS) $(MYINCL) $(INCLUDES) -c -o $(LIBDIR)CylinderLayerMaterial.o $(SRCDIR)CylinderLayerMaterial.C

$(LIBDIR)DiscLayerMaterial.o:	$(SRCDIR)DiscLayerMaterial.C	
	@echo -e [make] GCC: Generating DiscLayerMaterial object file
	$(CXX) $(CXXOPTS) $(CXXFLAGS) $(MYINCL) $(INCLUDES) -c -o $(LIBDIR)DiscLayerMaterial.o $(SRCDIR)DiscLayerMaterial.C

layval :	$(SRCDIR)LayerMaterialValidation.C $(LIBDIR)CylinderLayerMaterial.o  $(LIBDIR)DiscLayerMaterial.o
	@echo -e [make] GCC: Generating LayerMaterialValidation object file
	$(CXX) $(CXXOPTS) $(CXXFLAGS) $(MYINCL) $(INCLUDES) -I$(CINTSYSDIR) -c -o $(LIBDIR)LayerMaterialValidation.o $(SRCDIR)LayerMaterialValidation.C
	@echo -e [make] GCC: LayerMaterialValidation executable
	$(CXX) $(LDFLAGS) -o $(RUNDIR)LayerMaterialValidation $(ROOTLIBS) $(ROOTGLIBS) $(LIBDIR)LayerMaterialValidation.o $(LIBDIR)DiscLayerMaterial.o $(LIBDIR)CylinderLayerMaterial.o

clean:
	rm -f $(LIBDIR)*.o $(LIBDIR)core
distclean: clean
	rm -f $(LIBDIR)*.so
