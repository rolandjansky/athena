
ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

CLHEP_LIB     = $(SITEROOT)/LCGCMT/LCGCMT_$(LCGCMTVERS)/InstallArea/$(CMTCONFIG)/lib
CLHEP_LIBFILE = $(shell readlink $(CLHEP_LIB)/libCLHEP.so)
CLHEP_LIBDIR  = $(shell dirname ${SITEROOT}/LCGCMT/LCGCMT_${LCGCMTVERS}/InstallArea/${CMTCONFIG}/lib/$(CLHEP_LIBFILE))
CLHEP_INC     = $(CLHEP_LIBDIR)/../include

CXX           = g++
LD            = g++

TMNUTIL_INC   = -I$(TestArea)/InstallArea/include/TriggerMenuNtuple
TMNUTIL_LIB   = -L$(TestArea)/InstallArea/$(CMTCONFIG)/lib -lTriggerMenuNtupleDict

ROOTCINT      = rootcint

CXXFLAGS      = -I$(ROOTSYS)/include -g -Wall -fPIC
CXXFLAGS     += $(ROOTCFLAGS)
CXXFLAGS     += -I. -I.. -I../src
CXXFLAGS     += -DVALIDATION_TOOL -DPACKAGE_VERSION=0
CXXFLAGS     += $(TMNUTIL_INC)
CXXFLAGS     += -I$(CLHEP_INC)

LDFLAGS=
SOFLAGS       = -shared
LIBS          = $(ROOTLIBS) -L.
LIBS         += -lPyROOT
LIBS         += -L$(CLHEP_LIB) -lCLHEP

OBJS          = MsgStream.o FexAlgo.o TrigBjetFexStandalone.o TrigTrackJetFinderTool.o
OBJS         += TuningLikelihood.o TaggerHelper.o TrigBjetDataHelper.o TrigBjetTagger.o

%.o: %.cxx
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: ../src/%.cxx
	$(CXX) -c $(CXXFLAGS) -o $@ $<

all: libValidationTool.so libMyDict.so

libValidationTool.so: $(OBJS)
	$(LD) $(SOFLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

MyDict.cxx:  Linkdef.h
	$(ROOTCINT) -f $@ -c $(CXXFLAGS) -p BugCint.h $^

libMyDict.so: MyDict.cxx $(DICT_SOURCES)
	$(LD) -shared -o$@ `root-config --ldflags` $(CXXFLAGS) -I$(ROOTSYS)/include $^


# clean
clean:
	rm -f *~ *.o *.o~ core *.so *.d MyDict.cxx MyDict.h test

