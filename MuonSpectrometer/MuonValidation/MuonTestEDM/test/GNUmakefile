#PROGNAME = progMakeHist
#SOURCEFILES = makeHistos.C

PROGNAME = comparePositions
SOURCEFILES = ComparePrdPositions.C

OBJS = $(patsubst %.C, %.o, $(SOURCEFILES))

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTLDFLAGS := $(shell root-config --ldflags) 

LDFLAGS	      = -O $(ROOTLDFLAGS) 
LIBS	     += $(ROOTLIBS)
CFLAGS	     += $(ROOTCFLAGS)

%.o: %.C
	g++  ${CFLAGS} -c  -g -o $@ $<

$(PROGNAME):	$(OBJS)
	g++  -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

test:
	@echo $(ROOTCFLAGS)

clean:	
	-rm -f ${PROGNAME} ${OBJS}
