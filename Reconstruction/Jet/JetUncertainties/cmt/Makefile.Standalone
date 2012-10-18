# ARCH_LOC_1 := $(wildcard $(shell root-config --prefix)/etc/Makefile.arch)
# ARCH_LOC_2 := $(wildcard $(shell root-config --prefix)/share/root/test/Makefile.arch)
# ARCH_LOC_3 := $(wildcard $(shell root-config --prefix)/share/doc/root/test/Makefile.arch)
# ifneq ($(strip $(ARCH_LOC_1)),)
#   $(info Using $(ARCH_LOC_1))
#   include $(ARCH_LOC_1)
# else
#   ifneq ($(strip $(ARCH_LOC_2)),)
#     $(info Using $(ARCH_LOC_2))
#     include $(ARCH_LOC_2)
#   else
#     ifneq ($(strip $(ARCH_LOC_3)),)
#       $(info Using $(ARCH_LOC_3))
#       include $(ARCH_LOC_3)
#     else
#       $(error Could not find Makefile.arch! ROOT not set up?)
#     endif
#   endif
# endif

ifeq ($(wildcard $(ROOTSYS)/test/Makefile.arch),) 
    include $(ROOTSYS)/etc/Makefile.arch 
else
    include $(ROOTSYS)/test/Makefile.arch 
endif

ROOTBIN := $(shell root-config --bindir)
ROOTLIB := $(shell root-config --libdir)
ROOTINC := $(shell root-config --incdir)

# -------------------------------------------------------------
# General flags
# -------------------------------------------------------------
PACKAGE    = JetUncertainties
OUTPUTDIR  = ../StandAlone

# These warnings bother me, so I turn them off.
CXXFLAGS += -DJES_STANDALONE

# add utils subdirectories if needed
INCLUDES   += -I.. -I${ROOTINC}

# Need these to avoid loading dependent libraries when ROOT starts
LINKLIBS = -L${ROOTLIB} -lHist -lMatrix -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic

# -------------------------------------------------------------
# ROOT Cint
# -------------------------------------------------------------
CINT       = JetUncertaintiesCint
LDEFFILE   = ../Root/LinkDef.h
CINTFILE   = $(OUTPUTDIR)/JetUncertaintiesCint.cxx
CINTOBJ    = $(patsubst %.cxx,%.o,$(CINTFILE))

# -------------------------------------------------------------
# ProofAna source
# -------------------------------------------------------------
# add util subdirectories if needed
CXXLIST = ../Root/UJUncertaintyProvider.cxx ../Root/JESUncertaintyProvider.cxx ../Root/MultijetJESUncertaintyProvider.cxx ../Root/JESUtils.cxx $(CINTFILE)
HLIST   = ../JetUncertainties/UJUncertaintyProvider.h ../JetUncertainties/JESUncertaintyProvider.h ../JetUncertainties/MultijetJESUncertaintyProvider.h
OLIST   = $(patsubst %.cxx,%.o,$(CXXLIST))
DLIST   = $(patsubst %.h,%.d,$(HLIST))


# -------------------------------------------------------------
# Libraries
# -------------------------------------------------------------
LIBFILE  = $(OUTPUTDIR)/libJetUncertainties.so

ifeq ($(PLATFORM),macosx)
EXTRALDFLAGS = -install_name $(LIBFILE)
endif

# get libraries of ROOT
define ldlinksuffixROOT
   $(addsuffix $(LDLINKSUFFIX),$(Lib)) $(shell if [ "$(findstring -Ldlink2,$(OPTIONS))" ]; then echo $(addsuffix _pkgid_$(ROOTVER),$(Lib)); fi)
endef

# -------------------------------------------------------------
# Compilation
# -------------------------------------------------------------

default: shlib

# Implicit rule to compile all classes
%.o : %.cxx
	@echo "Compiling $<"
	@$(CXX) -MD $(CXXFLAGS) $(INCLUDES) -c $< -o $*.o 

# Rule to make ROOTCINT output file
$(CINTOBJ) : $(HLIST) $(LDEFFILE)
	@echo "Running rootcint"
	@$(ROOTBIN)/rootcint -f $(CINTFILE) -c -p  $(CXXFLAGS) $(INCLUDES) $(HLIST) $(LDEFFILE)
	@echo "Compiling $(CINTFILE)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(CINTFILE) -o $@

rootcint : $(HLIST) $(LDEFFILE)
	@echo "Running rootcint"
	$(ROOTBIN)/rootcint -f $(CINTFILE) -c -p  $(CXXFLAGS) $(INCLUDES) $(HLIST) $(LDEFFILE)

# Rule to combine objects into a shared library
$(LIBFILE): $(OLIST)
	@echo "Linking $(LIBFILE)"
	@rm -f $(LIBFILE)
	@$(LD) $(CXXFLAGS) $(SOFLAGS) $(LINKLIBS) $(EXTRALDFLAGS) $(OLIST) -o $(LIBFILE) 

-include $(DLIST)

JetUncertaintiesCint: $(CINTOBJ)
shlib: $(LIBFILE)

clean:
	@rm -f ../*/*.o ../*/*.d
	



