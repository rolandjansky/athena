
#
# Include the architecture definitions from the ROOT sources
#
#  Makefile.arch can be in a number of different locations depending on the system
#  you're compiling on. The Fink installed version of ROOT for instance has this file
#  in a different location than the "normally installed" ROOT versions...
#
#  The library doesn't depend on ROOT at the moment, but the plan is to make it
#  depend on it later on.
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
      $(error Could not find Makefile.arch! ROOT not set up?)
    endif
  endif
endif

test: EventInfoD3PDObject.o MuonROID3PDObject.o VarHandle.o testDict.o test.o
	@echo "Linking $@"
	$(LD) -g $(LDFLAGS) $^ $(LIBS) -o $@

new_test: Class2.o testDict.o new_test.o
	@echo "Linking $@"
	$(LD) -g $(LDFLAGS) $^ $(LIBS) -o $@

testDict.cxx: EventInfoD3PDObject.h MuonROID3PDObject.h LinkDef.h
	@echo "Generating $@"
	$(shell root-config --exec-prefix)/bin/rootcint -f $@ -c -p $(INCLUDES) $^

clean:
	rm -f *.o
	rm -f testDict.*

.SUFFIXES: .cxx .o

.cxx.o:
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -g -c $< -o $@ $(INCLUDES)
