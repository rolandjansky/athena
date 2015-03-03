# MuonTriggerTool root stand-alone makefile

include $(ROOTSYS)/test/Makefile.arch

# -------------------------------------------------------------
# General flags
# -------------------------------------------------------------
PACKAGE    = MuonTriggerTool
OUTPUTDIR  = ../lib

MFLAGS     = -MM -Wall -W -Woverloaded-virtual
INCLUDES   += -I${ROOTSYS}/include -I.. -I../src -I../MuonTriggerTool

# Need these to avoid loading dependent libraries when ROOT starts
LINKLIBS = -L${ROOTSYS}/lib

CCLIST     = $(wildcard ../src/*.cxx)

HLIST      = $(wildcard ../MuonTriggerTool/*.h)
OLIST      = $(patsubst %.cxx,%.o,$(CCLIST))
DLIST      = $(patsubst %.h,%.d,$(HLIST))

# -------------------------------------------------------------
# Libraries
# -------------------------------------------------------------
SHLIBFILE  = $(OUTPUTDIR)/libMuonTriggerTool.so

ifeq ($(PLATFORM),macosx)
EXTRALDFLAGS = -install_name @rpath/$(SHLIBFILE)
endif

# get libraries of ROOT
define ldlinksuffixROOT
   $(addsuffix $(LDLINKSUFFIX),$(Lib)) $(shell if [ "$(findstring -Ldlink2,$(OPTIONS))" ]; then echo $(addsuffix _pkgid_$(ROOTVER),$(Lib)); fi)
endef

# -------------------------------------------------------------
# Compilation
# -------------------------------------------------------------

default: shlib

# Implicit rule making all dependency Makefiles included at the end of this makefile
%.d: %.cxx $(HLIST)
	@echo "Making $@"
	@set -e; $(CC) $(MFLAGS) $(CXXFLAGS) $< \
		| awk '{ sub("^$(notdir $*).o:","$*.o $@:") ; print }' > $@ ;\
		[ -s $@ ] || rm -f $@

# Implicit rule to compile all classes
%.o : %.cxx
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $*.o 

# Rule to combine objects into a shared library
$(SHLIBFILE): $(OLIST)
	@echo "Linking $(SHLIBFILE)"
	@mkdir -p $(OUTPUTDIR)
	@rm -f $(SHLIBFILE)
	@$(LD) $(CXXFLAGS) $(SOFLAGS) $(LINKLIBS) $(EXTRALDFLAGS) $(OLIST) -o $(SHLIBFILE) 
	@rm -f $(OUTPUTDIR)/MuonTriggerToolLib.so
	@ln -s $(SHLIBFILE) $(OUTPUTDIR)/MuonTriggerToolLib.so 

-include $(DLIST)

shlib: $(SHLIBFILE)

clean:
	@rm -f ../*/*.o ../*/*.d
	@rm -f ../src/MuonTriggerToolDict.h
	@rm -f ../src/MuonTriggerToolDict.cxx
	@@rm -f $(SHLIBFILE)
	@rm -f $(OUTPUTDIR)/MuonTriggerToolLib.so
