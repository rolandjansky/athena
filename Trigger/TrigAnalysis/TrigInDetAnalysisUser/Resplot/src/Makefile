##########################################################################
#
#   File:         makefile
#
#   Description:  builds the new egamma project including
#                 libraries...
#
#                  libResplot.a   
#                  
#                 and the executable
#
#   Created:      Mon Apr 18 23:33:55 BST 2005
#   Modified:     
#                   
#                   
######################################################################### 



ifeq ($(AtlasEvent_PLATFORM),)
  ifeq ($(PLATFORM),)
    ATLAS_ARCH = $(shell uname)
  else 
    ATLAS_ARCH = $(PLATFORM)
  endif
else
  ATLAS_ARCH = $(AtlasEvent_PLATFORM)
endif


CMTCONFIG=


CMDDIR = .
SRCDIR = ../src
INCDIR = ../src
LIBDIR = ../lib/$(ATLAS_ARCH)
OBJDIR = ../obj/$(ATLAS_ARCH)
EXEDIR = ../exe/$(ATLAS_ARCH)


# Root variables
ROOTCFLAGS   = $(shell root-config --cflags)
ROOTLIBS     = $(shell root-config --libs)
ROOTGLIBS    = $(shell root-config --glibs)

# CLHEPLIB    = $(shell clhep-config --libs)
# CLHEPCFLAGS = $(shell clhep-config --include)

# CLHEPLIB    = -L/afs/cern.ch/sw/lcg/external/clhep/2.0.2.3/slc3_ia32_gcc323/lib-lCLHEP-2.0.2.3
# CLHEPCFLAGS = -I/afs/cern.ch/sw/lcg/external/clhep/2.0.2.3/slc3_ia32_gcc323/include



BASEDIR   = $(shell ./cwd.sh -h)

CXX = g++
CC  = gcc

CSOURCES := $(wildcard *.cpp)
DEPENDENCIES = $(patsubst %.cpp, %.d, $(CSOURCES))

CXXFLAGS = $(shell root-config --ldflags) -O2
# CXXFLAGS += -Wno-long-double $(ROOTCFLAGS) $(CLHEPCFLAGS) -Wno-deprecated 
CXXFLAGS += -Wall -fPIC  -I.. -I$(HOME)/include $(ROOTCFLAGS) $(CLHEPCFLAGS) -Wno-deprecated 
# CXXFLAGS +=   -I. -I$(HOME)/include $(ROOTCFLAGS) $(CLHEPCFLAGS) -Wno-deprecated 
# CXXFLAGS +=  -I$(HOME)/include $(ROOTCFLAGS) $(CLHEPCFLAGS) -Wno-deprecated 

# to get more output for debug compile code with: 
# make DEBUG=COREDBG : for the event building classes
# make DEBUG=TRIGDBG : for the signature classes
ifdef DEBUG
CXXFLAGS += -D$(DEBUG) -g
endif


GLIBS += $(ROOTGLIBS) $(ROOTLIBS)

LDFLAGS = $(shell root-config --ldflags)

AR      = ar
ARFLAGS = -r

RANLIB   = ranlib


RM  = rm
TAR = tar




OBJECTS  = $(OBJDIR)/main.o  
ROBJECTS = $(OBJDIR)/rmain.o $(OBJDIR)/Resplot.o 

LOBJECTS = $(OBJDIR)/Resplot.o  $(OBJDIR)/generate.o  \
	   $(OBJDIR)/rmsFrac.o  $(OBJDIR)/ResplotDict.o 

HEADERS  = $(INCDIR)/Resplot.h $(INCDIR)/tagname.h \
	   $(INCDIR)/StatVal.h $(INCDIR)/Directory.h 


LIBS += -L$(LIBDIR) -lResplot 

.SUFFIXES : .cc .cxx .c


# build executable: link


all : dirs res
exe : dirs res

res : $(EXEDIR)/res
$(EXEDIR)/res :   $(OBJECTS) lib
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS) $(LIBS)   $(GLIBS)



# $(SRCDIR)/*Dict.cxx : $(SRCDIR)/Resplot.cxx  $(INCDIR)/Resplot.h
#	@echo rootcint -f $@ -c -I.. $(INCDIR)/Resplot.h
#	@rootcint -f $@ -c -I.. $(INCDIR)/Resplot.h


$(SRCDIR)/%Dict.cxx :  $(INCDIR)/%.h 
	@echo rootcint  -f $@ -c -I.. -I$(INCDIR) $<
	@rootcint       -f $@ -c -I.. -I$(INCDIR) $< 


# compile
$(OBJDIR)/%.o : $(SRCDIR)/%.cxx 
	$(CXX) $(CXXFLAGS) -c $<  -o $@

$(OBJDIR)/%.o : $(SRCDIR)/%.c 
	$(CC) $(CFLAGS) -c $<  -o $@


%/.d:
	mkdir -p $(@D)
	touch $@


$(OBJECTS) : $(HEADERS)

.PHONY : 

# libraries



all : dirs lib libso res 

lib: dirs $(LIBDIR)/libResplot.a
$(LIBDIR)/libResplot.a : $(LOBJECTS)
	echo "LIBDIR " $(LIBDIR)
	echo "ATLAS_ARCH " $(ATLAS_ARCH)
	echo "AtlasEvent_PLATFORM "  $(AtlasEvent_PLATFORM)
	$(AR) $(ARFLAGS)  $@ $(LOBJECTS)
	$(RANLIB) $@


libso: dirs $(LIBDIR)/libResplot.so
$(LIBDIR)/libResplot.so : $(LOBJECTS)
	$(CXX) $(CXXFLAGS) -dynamiclib -shared -o $@ $(LOBJECTS) $(ROOTLIBS)
#	$(CXX) $(CXXFLAGS) -shared -o $@ $(LOBJECTS) $(ROOTLIBS)
#	$(RANLIB) $@


clean :
	$(RM) -rf $(OBJDIR)/*.o  $(LIBDIR)/lib*.{a,so}  $(EXEDIR)/res
	$(RM) -rf $(SRCDIR)/*LinkDef.{h,cxx} $(SRCDIR)/*~ $(INCDIR)/*~  $(SRCDIR)/*Dict.{h,cxx} $(INCDIR)/*Dict.{h,cxx}


archive : 
	cd ../.. ; tar -czf $(BASEDIR)/$(BASEDIR).tgz $(BASEDIR)/src 

install : dirs all 
	cp $(HEADERS) $(HOME)/include
	cp $(LIBDIR)/libResplot.{a,so} $(HOME)/lib

dirs : $(OBJDIR)/.d $(LIBDIR)/.d $(EXEDIR)/.d 



