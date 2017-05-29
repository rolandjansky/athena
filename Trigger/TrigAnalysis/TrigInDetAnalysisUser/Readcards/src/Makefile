##########################################################################
#
#   File:         makefile
#
#   Description:  builds the cardreader libraries... 
#                 
#                  libReadcards.a
#                  
#                 and the test executable, 
#                 installs the headers in $(HOME)/include  
#                 and the library in $(HOME)/lib 
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


SRCDIR = .
LIBDIR = ../lib/$(ATLAS_ARCH)
OBJDIR = ../obj/$(ATLAS_ARCH)
EXEDIR = ../exe/$(ATLAS_ARCH)


CONFIGDIR = ../config
BASEDIR   = $(shell cwd.sh -p)
BASENAME  = $(shell cwd.sh -s)




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

# CSOURCES := $(wildcard *.cpp)
# DEPENDENCIES = $(patsubst %.cpp, %.d, $(CSOURCES))

CXXFLAGS = 
# CXXFLAGS += -Wno-long-double $(ROOTCFLAGS) $(CLHEPCFLAGS) -Wno-deprecated 
CXXFLAGS += -D_DIR_=$(CONFIGDIR) -I. $(ROOTCFLAGS) $(CLHEPCFLAGS) -Wno-deprecated 

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
RANFLAGS = 


RM  = rm
TAR = tar


OBJECTS  = $(OBJDIR)/main.o  
LOBJECTS = $(OBJDIR)/ReadCards.o \
           $(OBJDIR)/Value.o \
           $(OBJDIR)/utils.o \
           $(OBJDIR)/IReadCards.o


HEADERS  = $(SRCDIR)/ReadCards.h $(SRCDIR)/utils.h $(SRCDIR)/Value.h $(SRCDIR)/IReadCards.h 


LIBS += -L$(LIBDIR) -lReadcards

.SUFFIXES : .cc .cxx .c


# build executable: link



testparse : dirs $(EXEDIR)/testparse
$(EXEDIR)/testparse :  $(OBJECTS) lib
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS) $(LIBS)   $(GLIBS)



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

lib: $(LIBDIR)/libReadcards.a
$(LIBDIR)/libReadcards.a : $(LOBJECTS)
	$(AR) $(ARFLAGS)  $@ $(LOBJECTS)
	$(RANLIB) $@


clean :
	$(RM) -rf $(OBJDIR)/*.o  $(LIBDIR)/lib*.a 


archive : 
	cd .. ; tar -czf $(BASEDIR).tgz src 

all : lib testparse 

install : dirs all 
	cp $(HEADERS) $(HOME)/include
	cp $(LIBDIR)/libReadcards.a $(HOME)/lib

dirs : $(OBJDIR)/.d $(LIBDIR)/.d $(EXEDIR)/.d


