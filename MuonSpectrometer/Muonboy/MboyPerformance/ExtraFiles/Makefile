# Makefile for the ntuple reader
# 
#copied from  makefile by A.Gaponenko 2005
#from InnerDetector/InDetValidation/InDetRTT/ExtraFiles/InDetRTT_Makefile

default: ntreader

TOPDIR = .
OBJDIR = $(TOPDIR)
DEPDIR = $(TOPDIR)

vpath %.o $(OBJDIR)
vpath %.d $(DEPDIR)

################################################################

CXX = g++
## CXXFLAGS = -g -O3 -Wall
#CXXFLAGS = -g -Wall
#LD  = g++
#LDFLAGS =
 
# Magic commandes suggusted by Krzysztof Ciba
# to solve the problem of root libraries on SLC4 in 32 and 64bit mode).
 
ifeq ($(findstring i686,$(CMTCONFIG)),)
CXXFLAGS = -g -Wall 
else
CXXFLAGS = -g -Wall -m32
endif
LD  = g++
LDFLAGS =

################################################################
NTREADERSRCS 	:= aantana.c AANTEvent.c Analysis.c HasEvt.c Assoc.c Fakes.c \
	Efficiency.c Resolution.c Staco.c  


NTREADEROBJS 	:= $(NTREADERSRCS:%.c=%.o)
NTREADERDEPENDS := $(NTREADERSRCS:%.c=$(DEPDIR)/%.d)

NTREADER_CXXFLAGS := $(CXXFLAGS) $(shell $(ROOTSYS)/bin/root-config --cflags)

NTREADER_LIBS := $(shell $(ROOTSYS)/bin/root-config --libs)

################################################################

ntreader: $(NTREADEROBJS)
	$(LD) -o $@ $(NTREADER_CXXFLAGS) $(LDFLAGS) \
	$(NTREADEROBJS:%.o=$(OBJDIR)/%.o) \
	$(NTREADER_LIBS) \
        -Xlinker -rpath -Xlinker $(shell $(ROOTSYS)/bin/root-config --libdir)

clean: cleanmain
.PHONY: cleanmain cleanall

cleanall: clean

cleanmain:
	$(RM) $(NTREADEROBJS:%.o=$(OBJDIR)/%.o) $(NTREADERDEPENDS) 
	$(RM) $(TOPDIR)/ntreader

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
include $(NTREADERDEPENDS)
endif

#----------------
$(NTREADERDEPENDS): $(DEPDIR)/%.d: %.c
	$(SHELL) -ec '$(CXX) -MM $(NTREADER_CXXFLAGS) $< \
                      | sed s#$*.o#$*.o\ $*.d# > $@ ;\
	[ -s $@ ] || rm -f $@; test -r $@ || exit 1'

$(NTREADEROBJS): %.o : %.c
	$(CXX) $(NTREADER_CXXFLAGS) -c $< -o $(OBJDIR)/$(*F).o

#----------------
.DELETE_ON_ERROR:
#


