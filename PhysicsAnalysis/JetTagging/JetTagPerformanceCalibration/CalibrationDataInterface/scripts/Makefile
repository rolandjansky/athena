 ######################################################################
 # Project: Gfitter                                                   #
 ###################################################################### 

MAKEFLAGS = --no-print-directory -r -s
#MAKEFLAGS = --warn-undefined-variables --debug

# for older ROOT versions, look in the 'test' subdirectory
#include ${ROOTSYS}/test/Makefile.arch
include ${ROOTSYS}/etc/Makefile.arch

# Internal configuration
PACKAGE=CalibrationDataInterface
LD_LIBRARY_PATH:=.:$(ROOTSYS)/lib:$(LD_LIBRARY_PATH)
OBJDIR=obj
DEPDIR=$(OBJDIR)/dep
VPATH= $(OBJDIR)
INCLUDES += -I../include/ -I$(ROOTSYS)/include 
ROOTSYS  ?= ERROR_RootSysIsNotDefined

DICTHEAD  = $(PACKAGE)_Dict.h
DICTFILE  = $(PACKAGE)_Dict.C
DICTOBJ   = $(PACKAGE)_Dict.o
DICTLDEF  = LinkDef.h
SKIPCPPLIST  = 
SKIPHLIST    =  $(DICTHEAD) $(DICTLDEF)
LIBFILE   = lib$(PACKAGE).a
SHLIBFILE = lib$(PACKAGE).$(DllSuf)
UNAME = $(shell uname)

#LINKLIBS = -L${ROOTSYS}/lib -lRIO -lNet -lCore -lGraf -lTree -lHist -lMatrix -lPhysics -lMathCore -lTMVA -lMinuit2 -ldl -lm
LINKLIBS = -L${ROOTSYS}/lib -lRIO -lNet -lCore -lGraf -lTree -lHist -lMatrix -lPhysics -lMathCore

default: shlib 

# List of all source files to build
HLIST   = $(filter-out $(SKIPHLIST),$(wildcard *.h))
CPPLIST = $(filter-out $(SKIPCPPLIST),$(wildcard *.$(SrcSuf)))
DICTHLIST = $(HLIST)

# List of all object files to build
OLIST=$(patsubst %.cxx,%.o,$(CPPLIST))

# Implicit rule to compile all classes
%.o : %.cxx
	@echo "Compiling $<"
	@mkdir -p $(OBJDIR)
	@$(CXX) $(CXXFLAGS) -g -c $< -o $(OBJDIR)/$(notdir $@) $(INCLUDES)


# Rule to make the dictionary
$(DICTFILE):  $(DICTHLIST) $(DICTLDEF)
	@echo "Generating dictionary $@" 
#	@echo rootcint -f $(DICTFILE) $^
	$(ROOTSYS)/bin/rootcint -f $(DICTFILE) -c $(INCLUDES) $^

$(OBJDIR)/$(DICTOBJ): $(DICTFILE)
	@echo "Compiling $<"
	@mkdir -p $(OBJDIR)
	@$(CXX) $(CXXFLAGS) -g -c $(INCLUDES) -o $@ $<


##############################
# The dependencies section   
# - the purpose of the .d files is to keep track of the
#   header file dependence
# - this can be achieved using the makedepend command 
##############################
# .d tries to pre-process .cc
ifneq ($(MAKECMDGOALS),clean)
-include $(foreach var,$(CPPLIST:.$(SrcSuf)=.d),$(DEPDIR)/$(var)) /dev/null
endif

$(DEPDIR)/%.d: %.$(SrcSuf)
	@mkdir -p $(DEPDIR)
	if test -f $< ; then \
		echo "Making $(@F)"; \
		$(SHELL) -ec '$(CPP) -MM $(CXXFLAGS) $(INCLUDES) $< | sed '\''/Cstd\/rw/d'\'' > $@'; \
	fi

# Rule to combine objects into a library
$(LIBFILE): $(OLIST) $(OBJDIR)/$(DICTOBJ)
	@echo "Making static library: $(LIBFILE)"
	@rm -f $(LIBFILE)
	@ar q $(LIBFILE) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ)
	@ranlib $(LIBFILE)

# Rule to combine objects into a unix shared library
$(SHLIBFILE): $(OLIST) $(OBJDIR)/$(DICTOBJ)
	@echo "Making shared library: $(SHLIBFILE)"
	@rm -f $(SHLIBFILE)
#ifeq ($(ARCH),aix)
#	/usr/ibmcxx/bin/makeC++SharedLib $(OutPutOpt) $@ $(LIBS) -p 0 $(OLIST) $(OBJDIR)/$(DICTOBJ)
#else
#ifeq ($(ARCH),aix5)
#	/usr/vacpp/bin/makeC++SharedLib $(OutPutOpt) $@ $(LIBS) -p 0 $(OLIST) $(OBJDIR)/$(DICTOBJ)
#else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so (this specialization copied from {$ROOTSYS}/test/Makefile)
	@echo "executing command: $(LD) $(SOFLAGS)$@ $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) $(OutPutOpt) $(SHLIBFILE)"
	$(LD) $(SOFLAGS)$@ $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) $(LINKLIBS) $(OutPutOpt) $(SHLIBFILE)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
	ln -sf $@ $(subst .$(DllSuf),.so,$@)
else
	$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) $(OutPutOpt) $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
	bindexplib $* $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) > $*.def
	lib -nologo -MACHINE:IX86 $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) -def:$*.def \
	   $(OutPutOpt)$(EVENTLIB)
	$(LD) $(SOFLAGS) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) $*.exp $(LIBS) \
	   $(OutPutOpt)$@
	$(MT_DLL)
else
	@echo "executing command: $(LD) $(SOFLAGS) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) $(OutPutOpt) $@ $(LINKLIBS)"
#	@$(LD) $(CXXFLAGS) $(SOFLAGS) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) $(OutPutOpt) $@ $(LINKLIBS)
	@$(LD) $(SOFLAGS) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) $(OutPutOpt) $@ $(LINKLIBS)
endif
endif
#endif
#endif
		@echo "$@ done"
#else
#	@echo "executing command: $(LD) $(CXXFLAGS) $(SOFLAGS) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) $(OutPutOpt) $(SHLIBFILE) $(LINKLIBS)"
#	@$(LD) $(CXXFLAGS) $(SOFLAGS) $(addprefix $(OBJDIR)/,$(OLIST)) $(OBJDIR)/$(DICTOBJ) $(OutPutOpt) $(SHLIBFILE) $(LINKLIBS)
#endif


## Rule to combine objects into a windows shared library
#$(DLLIBFILE): $(OLIST) $(OBJDIR)/$(DICTOBJ)
#	@echo "Making dll file: $(DLLIBFILE)"
#	@rm -f $(DLLIBFILE)
#	$(LD) -Wl,--export-all-symbols -Wl,--export-dynamic -Wl,--enable-auto-import -Wl,-Bdynamic -shared --enable-auto-image-base -Wl,-soname -o $(DLLIBFILE) -Wl,--whole-archive $(addprefix $(OBJDIR)/,$(OLIST) $(patsubst %.$(SrcSuf),%.o,$(DICTFILE))) -Wl,--no-whole-archive -L$(ROOTSYS)/lib -lCore -lTMVA -lCint -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lMinuit -lPhysics -lHtml -lm

# Useful build targets
lib: $(LIBFILE) 
shlib: $(SHLIBFILE)
#winlib: $(DLLIBFILE)
clean:
	rm -f $(DICTFILE) $(DICTHEAD)
	rm -f $(SHLIBFILE)
	rm -f $(OBJDIR)/*.o
	rm -f $(DEPDIR)/*.d
	rm -f $(LIBFILE)
	rm -f $(SHLIBFILE)
#	rm -f $(DLLIBFILE)

distclean:
	rm -rf obj 
	rm -f *~
	rm -f *_Dict*
	rm -f $(SHLIBFILE)
	rm -f $(LIBFILE)
	rm -f $(SHLIBFILE)
#	rm -f $(DLLIBFILE)

#.PHONY : winlib shlib lib default clean
.PHONY : shlib lib default clean

