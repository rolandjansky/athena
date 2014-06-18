CXXFLAGS=$(shell root-config --cflags) -g -Iinclude
LIBS=$(shell root-config --libs) -g

ifeq (i686,$(findstring i686,$(CMTCONFIG)))
	CXXFLAGS += -m32
	LIBS += -m32
else
	CXXFLAGS += -m64
	LIBS += -m64
endif

OBJS := FTKTimeSimDict.o src/FTKTimeSim.o src/DataOrganizer.o \
	src/SecDataOrganizer.o src/DataFormatter.o \
	src/TrackFitter.o src/SecStageSecDataOrganizer.o \
	src/SecStageTrackFitter.o src/AssociativeMemory.o \
	src/Region.o

all: libFTKTimeSim.a libFTKTimeSim.so

libFTKTimeSim.so : $(OBJS)
	g++ -shared -o $@ $(OBJS) $(LIBS)

libFTKTimeSim.a : $(OBJS)
	rm -f $@
	ar cru $@ $(OBJS)
	ranlib $@

FTKTimeSimDict.cxx : include/FTKTimeSim.hh include/FTKTimeSimLinkDef.h
	rootcint -f FTKTimeSimDict.cxx -c $^

%.o : %.cxx
	g++ -c -o $@ $< $(CXXFLAGS)

src/%.o: src/%.cxx
	g++ -c -o $@ $< $(CXXFLAGS)

clean: 
	rm -rf libFTKTimeSim.a libFTKTimeSim.so \
	$(OBJS) FTKTimeSimDict.cxx FTKTimeSimDict.h
