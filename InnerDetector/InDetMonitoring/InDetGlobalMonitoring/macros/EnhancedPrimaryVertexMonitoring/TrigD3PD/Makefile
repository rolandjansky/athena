# $Id: Makefile 329568 2010-10-29 09:53:55Z krasznaa $
#
# Makefile for compiling the helper library for reading back the trigger
# information from the D3PD files.
#
# author: Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#

#
# Include the architecture definitions from the ROOT sources
#
#  Makefile.arch can be in a number of different locations depending on the system
#  you're compiling on. The Fink installed version of ROOT for instance has this file
#  in a different location than the "normally installed" ROOT versions...
#
ARCH_LOC_1 := $(wildcard $(shell root-config --prefix)/test/Makefile.arch)
ARCH_LOC_2 := $(wildcard $(shell root-config --prefix)/share/root/test/Makefile.arch)
ARCH_LOC_3 := $(wildcard $(shell root-config --prefix)/share/doc/root/test/Makefile.arch)
ifneq ($(strip $(ARCH_LOC_1)),)
  $(info Using $(ARCH_LOC_1))
  include $(ARCH_LOC_1)
else
  ifneq ($(strip $(ARCH_LOC_2)),)
    $(info Using $(ARCH_LOC_2))
    include $(ARCH_LOC_2)
  else
    ifneq ($(strip $(ARCH_LOC_3)),)
      $(info Using $(ARCH_LOC_3))
      include $(ARCH_LOC_3)
    else
      $(error Could not find Makefile.arch!)
    endif
  endif
endif

# Name of the library to create:
LIBRARY = TriggerD3PDHelpers

# Set the locations of some files
DICTHEAD  = $(LIBRARY)_Dict.h
DICTFILE  = $(LIBRARY)_Dict.$(SrcSuf)
DICTOBJ   = $(OBJDIR)/$(LIBRARY)_Dict.$(ObjSuf)
DICTLDEF  = LinkDef.h
SKIPCPPLIST = $(DICTFILE)
SKIPHLIST   = $(DICTHEAD) $(DICTLDEF)
SHLIBFILE   = lib$(LIBRARY).$(DllSuf)
LIBFILE     = lib$(LIBRARY).a
OBJDIR = obj
DEPDIR = $(OBJDIR)/dep
VPATH  += $(OBJDIR) ./

# List of all header and source files to build
HLIST   = $(filter-out $(SKIPHLIST),$(wildcard *.h))
CPPLIST = $(filter-out $(SKIPCPPLIST),$(wildcard *.$(SrcSuf)))

# List of all object files to build
OLIST = $(patsubst %.$(SrcSuf),%.o,$(notdir $(CPPLIST)))

# The default is to create the shared library:
default: $(SHLIBFILE) $(LIBFILE)

%.$(ObjSuf): %.$(SrcSuf)
		@echo "Compiling $<"
		@mkdir -p $(OBJDIR)
		@$(CXX) $(CXXFLAGS) -g -c $< -o $(OBJDIR)/$(notdir $@) $(INCLUDES)

# Rule to create the dictionary
$(DICTFILE): $(HLIST) $(DICTLDEF)
		@echo "Generating dictionary $@"
		@$(shell root-config --exec-prefix)/bin/rootcint -f $(DICTFILE) -c -p $(INCLUDES) $^

# Rule to comile the dictionary
$(DICTOBJ): $(DICTFILE)
		@echo "Compiling $<"
		@mkdir -p $(OBJDIR)
		@$(CXX) $(CXXFLAGS) -g -c $(INCLUDES) -o $@ $<

# Rule to link the shared library
$(SHLIBFILE): $(OLIST) $(DICTOBJ)
		@echo "Making shared library: $(SHLIBFILE)"
		@rm -f $(SHLIBFILE)
		@$(LD) $(LDFLAGS) $(SOFLAGS) -g $(addprefix $(OBJDIR)/,$(OLIST)) $(DICTOBJ) -o $(SHLIBFILE)

# Rule to create the static library
$(LIBFILE): $(OLIST) $(DICTOBJ)
		@echo "Making static library: $(LIBFILE)"
		@rm -f $(LIBFILE)
		@ar rcs $@ $(addprefix $(OBJDIR)/,$(OLIST)) $(DICTOBJ)

# Rule to clean the directory
clean:
		@rm -f $(OBJDIR)/*.$(ObjSuf)
		@rm -f $(DICTOBJ)
		@rm -f $(DICTFILE) $(DICTHEAD)
		@rm -f $(SHLIBFILE)

distclean: clean
		@rm -rf $(OBJDIR)

##############################
# The dependencies section   
# - the purpose of the .d files is to keep track of the
#   header file dependence
# - this can be achieved using the makedepend command 
##############################
# .d tries to pre-process .cc
-include $(foreach var,$(notdir $(CPPLIST:.$(SrcSuf)=.d)),$(DEPDIR)/$(var))

$(DEPDIR)/%.d: %.$(SrcSuf)
	@mkdir -p $(DEPDIR)
	@if test -f $< ; then \
		echo "Making $(@F)"; \
		$(SHELL) -ec '$(CPP) -MM $(CXXFLAGS) $(INCLUDES) $< | sed '\''/Cstd\/rw/d'\'' > $@'; \
	fi
