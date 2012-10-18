# Author: Fabian Kohn (fkohn@cern.ch)
# ReweightUtils tuorial makefile (Part 4) (adapted from GoodRunLists package)

# --- External configuration ----------------------------------
include $(ROOTSYS)/test/Makefile.arch

# -------------------------------------------------------------
# General flags
# -------------------------------------------------------------
PACKAGE    = ReweightUtils
OUTPUTDIR  = ../StandAlone

# Get these from Makefile.arch above
#CC         = g++
#CCFLAGS    = -g -m32 -fPIC -Wall -W -Woverloaded-virtual -Wno-parentheses -Wno-unused-parameter -Wno-unused-variable
#LDFLAGS    = -g -m32 -fPIC 

INCLUDES   += -I${ROOTSYS}/include -I.. -I../../ReweightUtils

# Need these to avoid loading dependent libraries when ROOT starts
LINKLIBS = -L${ROOTSYS}/lib -L../../StandAlone -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lGui -pthread -lm -ldl -rdynamic -lReweightUtils

Tutorial4: Tutorial4.cxx
	@$(LD) $(CXXFLAGS) $(INCLUDES) $(LINKLIBS) Tutorial4.cxx -o Tutorial4

clean:
	@rm -f Tutorial4
