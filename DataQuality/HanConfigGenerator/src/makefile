


ifeq ($(AtlasEvent_PLATFORM),)
  ifeq ($(PLATFORM),)
    ATLAS_ARCH = $(shell uname)
  else 
    ATLAS_ARCH = $(PLATFORM)
  endif
else
  ATLAS_ARCH = $(AtlasEvent_PLATFORM)
endif



ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)

SOURCES = hcg.cxx addnode.cxx utils.cxx

INSTALLDIR=$(TestArea)/build/$(ATLAS_ARCH)/bin

hcg : $(SOURCES) 
	g++ $(ROOTCFLAGS) -Wall -O3 -o $@ $(SOURCES) $(ROOTLIBS) $(READLIB)

archive :  $(SOURCES) node.h addnode.h simpletimer.h spacer.h makefile README
	tar -czf hcg.tgz $?

install : hcg
	cp hcg $(HOME)/bin

ginstall : hcg
	cp hcg $(INSTALLDIR)

clean : 
	rm -f hcg
