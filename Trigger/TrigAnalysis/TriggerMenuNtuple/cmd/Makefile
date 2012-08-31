#---------------------------------------------------------------------------
# Standalone makefile to compile the package which depends only on the ROOT 
#---------------------------------------------------------------------------

PROJECT_ROOT := $(shell (cd ..; pwd))
PROJECT_NAME := $(shell basename $(PROJECT_ROOT))

CXX := g++
LD  := g++ 

ROOTCFLAGS := $(shell root-config --cflags)
ROOTLIBS   := $(shell root-config --libs)
ROOTGLIBS  := $(shell root-config --glibs)

CXXFLAGS := -O \
	-I$(PROJECT_ROOT) -I$(PROJECT_ROOT)/$(PROJECT_NAME) $(ROOTCFLAGS)
INCFLAGS := -I$(PROJECT_ROOT)/$(PROJECT_NAME) -I$(shell root-config --incdir)
LDFLAGS  := -O
SOFLAGS  := -shared
LIBS     := $(ROOTLIBS)	

EXCLUDE_OBJS := RoIData.o RoIData_Electron.o RoIData_Jet.o \
	RoIData_MissingET.o RoIData_Muon.o RoIData_Tau.o RoIData_TileMu.o \
	TriggerMenuNtupleDict.o

DICT_HDRS := ChainEntry.h CombLinks.h RoILinks.h FeatureIndex.h \
	ElectronRoILinks.h JetRoILinks.h MuonRoILinks.h TauRoILinks.h \
	TMNUtil.h TrigDecSummary.h
DICT_OBJS := $(patsubst ../$(PROJECT_NAME)/%LinkDef.h,%Dict.o, \
	$(wildcard ../$(PROJECT_NAME)/*LinkDef.h))
DICT_SRCS := $(patsubst %.o,../src/%.cxx, $(DICT_OBJS))

SRC_OBJS  := $(patsubst ../src/%.cxx,%.o, $(wildcard ../src/*.cxx))
# LIB_OBJS  := $(patsubst %.h,%.o, $(DICT_HDRS)) $(DICT_OBJS)
SRC_OBJS  := $(filter-out $(EXCLUDE_OBJS),$(SRC_OBJS))
LIB_OBJS  := $(DICT_OBJS) $(SRC_OBJS)
LIB_OBJS2 := $(addprefix ../obj/,$(LIB_OBJS))

vpath %.cxx ../src
vpath %Dict.cxx ../src
vpath %.o   ../obj
vpath %.so  ../lib
vpath %.h   ../$(PROJECT_NAME)
VPATH = ../src:../obj:../lib:../$(PROJECT_NAME):.

all: lib$(PROJECT_NAME).so

$(DICT_OBJS): %Dict.o: ../src/%Dict.cxx
	@mkdir -p ../obj
	$(CXX) -fPIC $(CXXFLAGS) -c $< -o ../obj/$(@F)

$(SRC_OBJS): %.o: %.cxx %.h
	@mkdir -p ../obj
	$(CXX) -fPIC $(CXXFLAGS) -c $< -o ../obj/$(@F)

CPPPARSER = 
$(DICT_SRCS): ../src/%Dict.cxx: %LinkDef.h $(DICT_HDRS)
	(cd ../$(PROJECT_NAME); \
	rootcint $(CPPPARSER) -f ../src/$(@F) -c -I.. -I$(INCFLAGS) \
	$(DICT_HDRS) ../$(PROJECT_NAME)/$*LinkDef.h)

lib$(PROJECT_NAME).so: $(LIB_OBJS)
	@mkdir -p ../lib
	$(LD) $(SOFLAGS) $(LIB_OBJS2) $(LIBS) -o ../lib/$(@F)

.PHONY: all clean cleanall cleanobj cleanlib show

clean: cleanall

cleanall: cleanobj cleanlib

cleanobj:
	@rm -fr ../obj/*.o

cleanlib: 
	@rm -fr ../lib/lib*.so

show:
	@echo "pwd: $(PWD)"
	@echo "PROJECT_NAME: $(PROJECT_NAME)"
	@echo "DICT_HDRS: $(DICT_HDRS)"
	@echo "DICT_SRCS: $(DICT_SRCS)"
	@echo "DICT_OBJS: $(DICT_OBJS)"
	@echo "LIB_OBJS: $(LIB_OBJS)"
