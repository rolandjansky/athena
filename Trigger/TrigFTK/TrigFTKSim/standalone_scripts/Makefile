ROOTCFLAGS=$(shell root-config --cflags)
ROOTLIBS=$(shell root-config --libs)
PYTHONCFLAGS=$(shell python-config --cflags)
PYTHONLIBS=$(shell python-config --ldflags)
PYTHONPATH=$(shell python-config --prefix)
CXX=$(shell root-config --cxx)

#CXXFLAGS=${ROOTCFLAGS} -g -I${AtlasBaseDir}/sw/lcg/external/pytools/1.8_python2.7/x86_64-slc6-gcc47-opt/include/python2.7 -I../TrigFTKSim -O3 -fPIC -D__USE_XOPEN2K8
#LIBS=-shared -L${AtlasBaseDir}/sw/lcg/external/Python/${PythonVERS}/${CMTCONFIG}/lib -L../standalone -lftk_classes -O3 -fPIC

CXXFLAGS=${ROOTCFLAGS} ${PYTHONCFLAGS} -g -I../TrigFTKSim -O3 -fPIC -D__USE_XOPEN2K8
LIBS=${ROOLIBS} -L${PYTHONPATH}/lib ${PYTHONLIBS} -shared -L../standalone -lftk_classes -O3 -fPIC

ifeq (i686,$(findstring i686,$(CMTCONFIG)))
	CXXFLAGS+=-m32
	LIBS+=-m32
else
	CXXFLAGS+=-m64
	LIBS+=-m64
endif

banksummaryplot: banksummaryplot.cc
	$(CXX) -g $(ROOTCFLAGS) $(ROOTLIBS) $< -o $@

SRCS=$(wildcard cftkutils_src/*.cxx)
OBJS=$(patsubst %.cxx, %.o, $(SRCS))

all: cftkutils.so

cftkutils.so : $(OBJS) ../standalone/libftk_classes.so
	g++ $(OBJS) -o $@ ${LIBS}

cftkutils_src/%.o: cftkutils_src/%.cxx  cftkutils_src/cftkutils.h
	g++ -c $< -o $@ ${CXXFLAGS} 

clean:
	rm -f cftkutils.so $(OBJS)

