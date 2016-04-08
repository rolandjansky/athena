SRC           = EVmva
ARCH          = linux
CXX           =
ObjSuf        = o
SrcSuf        = C
ExeSuf        = exe
DllSuf        = so
OutPutOpt     = -o 

EVENTLIB      = $(EVENTSO)

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)



ifeq ($(ARCH),solaris)
# Solaris CC
CXX           = /opt/SUNWspro/bin/CC
CXXFLAGS      = -O -KPIC
LD            = /opt/SUNWspro/bin/CC
LDFLAGS       = -O
SOFLAGS       = -G
endif

ifeq ($(ARCH),solarisCC5)
# Solaris CC 5.0
CXX           = CC
CXXFLAGS      = -O -KPIC -D__SunOS_5_6 -library=iostream,no%Cstd
LD            = CC
LDFLAGS       = -O -library=iostream,no%Cstd -o
SOFLAGS       = -G
endif

ifeq ($(ARCH),solarisegcs)
# Solaris egcs
CXX           = g++
CXXFLAGS      = -O -fPIC
LD            = CC
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),solarisx86)
# Solaris CC on Intel
CXX           = CC
CXXFLAGS      = -O -KPIC
LD            = CC
LDFLAGS       = -O
SOFLAGS       = -G
endif

ifeq ($(ARCH),sgicc)
# SGI
CXX           = CC -n32  -I/usr/include/CC.sgi
CXXFLAGS      = -O
LD            = CC -n32  -I/usr/include/CC.sgi
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),sgiegcs)
# SGI 6.x with EGCS
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O -Wl,-u,__builtin_new -Wl,-u,__builtin_delete -Wl,-u,__nw__FUiPv
SOFLAGS       = -shared
endif

ifeq ($(ARCH),sgin32egcs)
# SGI 6.x with EGCS for n32 ABI
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O -L/usr/lib32 -Wl,-woff,134
SOFLAGS       = -shared
endif

ifeq ($(ARCH),sgigcc)
# SGI with GCC
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O -Wl,-u,__builtin_new -Wl,-u,__builtin_delete -Wl,-u,__nw__FUiPv
SOFLAGS       = -shared
endif

ifeq ($(ARCH),sgikcc)
# SGI with KCC
CXX           = KCC -n32 --no_exceptions
CXXFLAGS      = -O
LD            = KCC -n32 --no_exceptions
LDFLAGS       = -O
SOFLAGS       =
endif


ifeq ($(ARCH),linux)
# Linux with gcc 2.7.2.x
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif


ifeq ($(ARCH),linuxdeb2)
# Linux with gcc 2.7.2.x
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxsuse6)
# Linux with gcc 2.7.2.x
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxSLC4)
# Linux with gcc 3.2.3.x
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O -L/usr/lib64
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxegcs)
# Linux with egcs (>= RedHat 5.2)
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxkcc)
# Linux with the KAI compiler
CXX           = KCC
CXXFLAGS      = -fPIC +K0
LD            = KCC
LDFLAGS       = -O
SOFLAGS       =
endif



ifeq ($(CXX),)
$(error $(ARCH) invalid architecture)
endif

CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

#------------------------------------------------------------------------------
EO       = $(SRC).$(ObjSuf) 
#LIBO     = $(SRC1).$(ObjSuf)
ES       = $(SRC).$(SrcSuf)
E        = $(SRC).$(ExeSuf)


OBJS          = $(EO)  

PROGRAMS      = $(E) 

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(LIBO) $(PROGRAMS)

#$(LIBO):   $(SRC1).$(SrcSuf) 
#		@echo "Compiling $(SRC1).$(SrcSuf)"
#		$(CXX) -Wall -c  $(SRC1).$(SrcSuf)
#		@echo "$@ done"

$(E):      $(EO)    
		$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
		@echo "$@ done"
		@echo "Starting the executable"
		./$@

clean:
		@rm -f $(OBJS) core

distclean:      clean
		@rm -f $(PROGRAMS) $(EVENTSO) $(EVENTLIB) *Dict.* *.def *.exp \
		   *.root *.ps .def so_locations

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

###

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -c $<


