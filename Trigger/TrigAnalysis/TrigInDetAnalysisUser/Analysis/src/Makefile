# $Id: $
##########################################################################
#
#   File:         Makefile
#
#   Description:  builds the new egamma project including
#                 libraries...
#
#                  libeventstore.a   
#                  libtrig.a
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


ifeq ($(TestArea),)
    ATLASBASE = $(HOME)/atlas/TIDA
else
    ATLASBASE = $(TestArea)
#   ATLASBASE = /cvmfs/atlas-nightlies.cern.ch/repo/sw/21.1-dev/2018-04-09T2234/AthenaP1/21.1.24/InstallArea/x86_64-slc6-gcc62-opt/src/Trigger/TrigAnalysis
#   ATLASBASE = /cvmfs/atlas-nightlies.cern.ch/repo/sw/21.1-dev/2018-05-15T2242/AthenaP1/21.1.27/InstallArea/x86_64-slc6-gcc62-opt/src/Trigger/TrigAnalysis

#   ifeq ($(AtlasArea),)
#     ATLASBASE = /cvmfs/atlas-nightlies.cern.ch/repo/sw/21.1-dev/2018-07-31T2248/AthenaP1/21.1.33/InstallArea/x86_64-slc6-gcc62-opt/src/Trigger/TrigAnalysis
#      ATLASBASE = $TestArea
#   else
#     ATLASBASE = $(AtlasArea)/InstallArea/x86_64-slc6-gcc62-opt/src/Trigger/TrigAnalysis
#   endif

endif

CMTCONFIG=


USEATHENALIBS = 0       

TID   = $(ATLASBASE)/Trigger/TrigAnalysis/TrigInDetAnalysis
TIDU  = $(ATLASBASE)/Trigger/TrigAnalysis/TrigInDetAnalysisUtils
TIDE  = $(ATLASBASE)/Trigger/TrigAnalysis/TrigInDetAnalysisExample
TIDUB = $(ATLASBASE)/Trigger/TrigAnalysis/TrigInDetAnalysisUser

#      TIDUB = $(ATLASBASE)/TrigInDetAnalysisUser
#      TIDU  = $(TestArea)/Trigger/TrigAnalysis/TrigInDetAnalysisUtils
#      TIDUB = $(TestArea)/Trigger/TrigAnalysis/TrigInDetAnalysisUser

CXXFLAGS += -I$(TID) -I$(TIDU) -I$(TIDE) -fPIC  


ARCH_TYPE = $(ATLAS_ARCH)




SRCDIR = .
LIBDIR = ../lib/$(ARCH_TYPE)
OBJDIR = ../obj/$(ARCH_TYPE)
EXEDIR = ../exe/$(ARCH_TYPE)
VPATH += $(TID)/src 
VPATH += $(TIDU)/src 
VPATH += $(TIDE)/src 
VPATH += $(SRCDIR)


LIBSO = lib

ifeq ($(CMTCONFIG),)
    TIDLIB = -L$(LIBDIR) -lTIDA
else
#     TIDLIB = -L$(ATLASBASE)/$(ATLAS_ARCH)/lib -lTrigInDetAnalysis -lTrigInDetAnalysisUtils -lTrigInDetAnalysisExample 
    TIDLIB = -L$(ATLASBASE)/$(ATLAS_ARCH)/lib -lTrigInDetAnalysis -lTrigInDetAnalysisExample 
endif


# BOOSTDIR = /afs/cern.ch/sw/lcg/external/Boost/1.44.0_python2.6/$(ATLAS_ARCH)
# BOOSTINC = -I$(BOOSTDIR)/include/boost-1_44
# BOOSTLIB = -L$(BOOSTDIR)/lib -lboost_regex-gcc43-mt-1_44

# Root variables
ROOTCFLAGS   = $(shell root-config --cflags)
ROOTLIBS     = $(shell root-config --libs)
ROOTGLIBS    = $(shell root-config --libs)

CLHEPLIB    = $(shell clhep-config --libs)
CLHEPCFLAGS = $(shell clhep-config --include)

# BASEDIR   = $(shell ./cwd.sh -h)
BASEDIR   = $(CURDIR)


CXX = g++
CC  = gcc

# CSOURCES := $(wildcard *.cpp)
# DEPENDENCIES = $(patsubst %.cpp, %.d, $(CSOURCES))


CXXFLAGS += -g
CFLAGS   += -g



CXXFLAGS += -I. -I$(TID) -I$(TIDU) -I$(TIDE) $(ROOTCFLAGS) -Wno-deprecated   
CXXFLAGS += -I$(TIDUB)/Resplot/src -I$(TIDUB)/Readcards/src
CXXFLAGS += -I$(HOME)/include -Wall 

# CXXFLAGS += $(BOOSTINC)

# to get more output for debug compile code with: 
# make DEBUG=COREDBG : for the event building classes
# make DEBUG=TRIGDBG : for the signature classes
ifdef DEBUG
CXXFLAGS += -D$(DEBUG) -g
endif

GLIBS +=  $(ROOTLIBS) $(ROOTGLIBS)

LDFLAGS = $(shell root-config --ldflags)

AR      = ar
ARFLAGS = -r

RANLIB   = ranlib
RANFLAGS = 


RM  = rm
TAR = tar


DOBJECTS  = $(OBJDIR)/dependence.o $(OBJDIR)/AtlasStyle.o $(OBJDIR)/AtlasLabels.o   
COBJECTS  = $(OBJDIR)/comparitor.o $(OBJDIR)/computils.o $(OBJDIR)/AtlasStyle.o $(OBJDIR)/AtlasLabels.o 
CCOBJECTS = $(OBJDIR)/cpucost.o $(OBJDIR)/computils.o $(OBJDIR)/AtlasStyle.o $(OBJDIR)/AtlasLabels.o
CCHEADERS = $(SRCDIR)/computils.h 
CHOBJECTS = $(OBJDIR)/chains.o 
WOBJECTS  = $(OBJDIR)/wmain.o 
SOBJECTS  = $(OBJDIR)/skim.o 
ROBJECTS  = \
        $(OBJDIR)/rmain.o  \
        $(OBJDIR)/ConfAnalysis.o  \
        $(OBJDIR)/ConfVtxAnalysis.o  \
        $(OBJDIR)/PurityAnalysis.o \
        $(OBJDIR)/globals.o \
        $(OBJDIR)/computils.o

LOBJECTS  = \
        $(OBJDIR)/TIDAEvent.o \
        $(OBJDIR)/TagNProbe.o \
        $(OBJDIR)/Track.o \
        $(OBJDIR)/TIDAVertex.o   \
        $(OBJDIR)/TIDAChain.o   \
        $(OBJDIR)/TIDARoi.o   \
        $(OBJDIR)/TIDARoiParameters.o  \
        $(OBJDIR)/TIDARoiDescriptor.o  \
        $(OBJDIR)/TrackTrigObject.o \
        $(OBJDIR)/ChainString.o \
        $(OBJDIR)/TrigInDetAnalysisDict.o



DICTHEADERS = \
        $(TID)/TrigInDetAnalysis/TIDAEvent.h \
        $(TID)/TrigInDetAnalysis/TIDAChain.h \
        $(TID)/TrigInDetAnalysis/TIDARoi.h \
        $(TID)/TrigInDetAnalysis/TFileString.h \
        $(TID)/TrigInDetAnalysis/TIDARoiParameters.h \
        $(TID)/TrigInDetAnalysis/TIDARoiDescriptor.h \
        $(TID)/TrigInDetAnalysis/Track.h \
        $(TID)/TrigInDetAnalysis/TrackTrigObject.h \
        $(TID)/TrigInDetAnalysis/TrigInDetAnalysisLinkDef.h



.SUFFIXES : .cc .cxx .c


LIBS += $(TIDLIB) 

RLIBS += -L$(TIDUB)/Resplot/lib/$(ARCH_TYPE) -lResplot -L$(TIDUB)/Readcards/lib/$(ARCH_TYPE) -lReadcards 
RLIBS += -L$(TIDUB)/Resplot/lib/$(ARCH_TYPE) -lResplot -L$(TIDUB)/Readcards/lib/$(ARCH_TYPE) -lReadcards 


# LIBS += $(BOOSTLIB)

# build executable: link

all : dirs resplot readcards rdict wdict reader comparitor cpucost chains skim listroot sb runtool dependence

$(OBJDIR)/computils.o : $(SRCDIR)/computils.h 
$(OBJDIR)/comparitor.o : $(SRCDIR)/computils.h 



rdict : $(EXEDIR)/rdict  
$(EXEDIR)/rdict : dirs resplot readcards $(ROBJECTS) $(LIBSO) 
	$(CXX) $(LDFLAGS) -o $@ $(ROBJECTS) $(LIBS) $(RLIBS) $(GLIBS)

wdict : $(EXEDIR)/wdict 
$(EXEDIR)/wdict : dirs $(WOBJECTS) $(LIBSO)     
	$(CXX) $(LDFLAGS) -o $@ $(WOBJECTS) $(LIBS) $(GLIBS)

reader : $(EXEDIR)/reader 
$(EXEDIR)/reader : dirs $(OBJDIR)/reader.o  $(LIBSO)    
	$(CXX) $(LDFLAGS) -o $@ $(OBJDIR)/reader.o $(LIBS) $(GLIBS)

dependence : $(EXEDIR)/dependence 
$(EXEDIR)/dependence : dirs readcards $(DOBJECTS)  $(LIBSO)  
	$(CXX) $(LDFLAGS) -o $@ $(DOBJECTS) $(LIBS) $(RLIBS) $(GLIBS)

skim : dirs $(EXEDIR)/skim 
$(EXEDIR)/skim : dirs $(OBJDIR)/skim.o  $(LIBSO)        
	$(CXX) $(LDFLAGS) -o $@ $(OBJDIR)/skim.o $(LIBS) $(GLIBS)

fastadd : dirs $(EXEDIR)/fastadd 
$(EXEDIR)/fastadd : dirs $(OBJDIR)/fastadd.o  $(LIBSO)  
	$(CXX) $(LDFLAGS) -o $@ $(OBJDIR)/fastadd.o $(LIBS) $(GLIBS)

refit : dirs $(EXEDIR)/refit 
$(EXEDIR)/refit : dirs $(OBJDIR)/refit.o  $(LIBSO)      
	$(CXX) $(LDFLAGS) -o $@ $(OBJDIR)/refit.o $(LIBS) $(RLIBS) $(GLIBS)

comparitor : $(EXEDIR)/comparitor  
$(EXEDIR)/comparitor : $(COBJECTS) $(CCHEADERS) $(LIBSO)
	$(CXX) $(LDFLAGS) -o $@ $(COBJECTS) $(LIBS) $(RLIBS) $(GLIBS)

cpucost : $(EXEDIR)/cpucost  
$(EXEDIR)/cpucost : $(CCOBJECTS) $(CCHEADERS) $(LIBSO)
	$(CXX) $(LDFLAGS) -o $@ $(CCOBJECTS) $(LIBS) $(RLIBS) $(GLIBS)

chains : $(EXEDIR)/chains  
$(EXEDIR)/chains : $(CHOBJECTS)  $(LIBSO)
	$(CXX) $(LDFLAGS) -o $@ $(CHOBJECTS) $(LIBS) $(RLIBS) $(GLIBS)


makeSmallRefFile : $(EXEDIR)/makeSmallRefFile
$(EXEDIR)/makeSmallRefFile : $(OBJDIR)/makeSmallRefFile.o
	$(CXX) $(LDFLAGS) -o $@ $(OBJDIR)/makeSmallRefFile.o  $(ROOTLIBS)

listroot : $(EXEDIR)/listroot  
$(EXEDIR)/listroot : $(OBJDIR)/listroot.o $(OBJDIR)/AtlasStyle.o $(OBJDIR)/AtlasLabels.o 
	$(CXX) $(LDFLAGS) -o $@ $(OBJDIR)/listroot.o $(OBJDIR)/AtlasStyle.o  $(RLIBS) $(ROOTLIBS)

runtool : dirs $(EXEDIR)/runtool
$(EXEDIR)/runtool : dirs $(OBJDIR)/runtool.o $(OBJDIR)/computils.o 
	$(CXX) $(LDFLAGS) -o $@ $(OBJDIR)/runtool.o $(OBJDIR)/computils.o $(GLIBS)

sb : $(EXEDIR)/sb  
$(EXEDIR)/sb : $(OBJDIR)/chainparser.o 
	$(CXX) $(LDFLAGS) -o $@ $(OBJDIR)/chainparser.o $(RLIBS) $(ROOTLIBS)


# compile
# $(OBJDIR)/%.o : $(SRCDIR)/%.cxx 
$(OBJDIR)/%.o : %.cxx 
	echo $(CXXFLAGS)
	$(CXX) $(CXXFLAGS) -c $<  -o $@

# $(OBJDIR)/%.o : $(SRCDIR)/%.c 
$(OBJDIR)/%.o : %.c 
	$(CC) $(CFLAGS) -c $<  -o $@


%/.d:
	mkdir -p $(@D)
	touch $@

rootdep : $(DICTHEADERS)
	cp $(DICTHEADERS) . 

$(SRCDIR)/TrigInDetAnalysisDict.cxx: $(DICTHEADERS)
	@echo rootcint  -f $@ -c -I$(TID) $?
	@rootcint       -f $@ -c -I$(TID) $?


$(OBJECTS) : $(HEADERS)

.PHONY : 



# libraries - only build these if not already built in athena

ifeq ($(CMTCONFIG),)

lib: $(LIBDIR)/libTIDA.a 
$(LIBDIR)/libTIDA.a : $(LOBJECTS)
	$(AR) $(ARFLAGS)  $@ $(LOBJECTS)
	$(RANLIB) $@

libso: $(LIBDIR)/libTIDA.so 
$(LIBDIR)/libTIDA.so : $(LOBJECTS) 
	$(CXX) $(LDFLAGS) -O3 -dynamiclib -shared -o  $@ $(LOBJECTS) $(RSSOLIB)$(GLIBS)

else
# if libraries are already build inside athena, do nothing 
lib:
libso:

endif


clean :
	$(RM) -rf $(OBJDIR)/*.o  $(LIBDIR)/lib*.{a,so}
	$(RM) -f $(OBJDIR)/*.o  $(LIBDIR)/lib*.{a,so} $(SRCDIR)/*Dict.{h,cxx}
	make -C $(TIDUB)/Resplot/src clean
	make -C $(TIDUB)/Readcards/src clean


libclean :
	$(RM) -f $(LIBDIR)/lib*.{a,so} 


resplot: $(TIDUB)/Resplot/lib/$(ARCH_TYPE)/.d
	make -C $(TIDUB)/Resplot/src install

readcards:  $(TIDUB)/Readcards/lib/$(ARCH_TYPE)/.d
	make -C $(TIDUB)/Readcards/src install

archive : 
	cd ../.. ; tar -czf $(BASEDIR).tgz $(BASEDIR)/src $(BASEDIR)/config 


ifeq ($(WorkDir_DIR),)
   INSTALLDIR=$(TestArea)/build/$(ATLAS_ARCH)/bin
else
   INSTALLDIR=$(WorkDir_DIR)/bin
endif

$(INSTALLDIR)/TIDA% : $(EXEDIR)/%
	cp $< $@
	cp $< $@.exe


INSTALLFILES=comparitor chains cpucost runtool sb rdict reader dependence

ginstall : install $(patsubst %,$(INSTALLDIR)/TIDA%,$(INSTALLFILES))

install : all 
	cp $(EXEDIR)/comparitor $(HOME)/bin
	cp $(EXEDIR)/chains     $(HOME)/bin
	cp $(EXEDIR)/reader     $(HOME)/bin
	cp $(EXEDIR)/rdict      $(HOME)/bin
	cp $(EXEDIR)/cpucost    $(HOME)/bin
	cp $(EXEDIR)/skim       $(HOME)/bin
	cp $(EXEDIR)/listroot   $(HOME)/bin
	cp $(EXEDIR)/runtool    $(HOME)/bin
	cp $(EXEDIR)/sb         $(HOME)/bin
	cp $(EXEDIR)/dependence $(HOME)/bin
	cp $(SRCDIR)/*.pcm      $(HOME)/bin

dirs :  $(LIBDIR)/.d $(OBJDIR)/.d $(EXEDIR)/.d

