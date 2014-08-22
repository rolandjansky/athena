SHELL = /bin/sh
UNAME = $(shell uname)

# Root variables
ROOTCFLAGS   = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS     = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS    = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs)

################
##linux
CXX          = g++
RM           = rm -f
AR           = ar cru

##Flags
CXXFLAGS     = -O0 -g -fPIC -funroll-loops # -Wall

PROGNAME = histogramCompare
SRCEXT   = cpp
SRCDIR   = src
INCDIR   = include
OBJDIR   = build
EXEDIR   = bin
UTILSSRC = utils/src
SRCS    := $(shell find $(SRCDIR) -name '*.$(SRCEXT)')
OBJS    := $(patsubst $(SRCDIR)/%.$(SRCEXT),$(OBJDIR)/%.o,$(SRCS))

GARBAGE  = $(OBJDIR)/*.o $(EXEDIR)/$(PROGNAME) 

#################
##Dependencies
# Linux
ifeq "$(UNAME)" "Linux"
RANLIB       = ranlib
CXXFLAGS    += -I$(INCDIR) -I$(INCPDFDIR) $(ROOTCFLAGS) #-I$(GSLINC)
LINKFLAGS    = -g $(shell root-config --nonew) $(shell root-config --ldflags)
endif

# OS X
ifeq "$(UNAME)" "Darwin"
RANLIB       = ranlib
CXXFLAGS    += -I$(INCDIR) -I$(INCPDFDIR) $(ROOTCFLAGS) #-I$(GSLINC)
LINKFLAGS    =
endif

##Libraries
LIBS       += $(ROOTLIBS) #-lHtml -lThread -lMinuit -lMathCore -lMinuit2 -lRooFit -lRooFitCore -lFoam -lProof -lMathMore

all : $(EXEDIR)/$(PROGNAME)

$(EXEDIR)/$(PROGNAME) : $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LINKFLAGS) $(LIBS)

$(OBJDIR)/%.o : $(SRCDIR)/%.$(SRCEXT)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean   :
	$(RM) $(GARBAGE)

cleanall:
	$(RM) $(GARBAGE)
