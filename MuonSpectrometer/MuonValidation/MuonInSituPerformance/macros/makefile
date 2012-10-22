NAME	= DO_method 

OBJECTS = DO_method.o 

CCFILES = $(OBJECTS:.o=.C)

ROOTLIBS      = $(shell root-config --libs)

ROOTGLIBS      +=  -lvectorDict

all:		$(NAME)

$(CCFILES):

$(OBJECTS):	$(HEADERS)

$(NAME):	$(OBJECTS)
	g++ -o $(NAME) -O $(OBJECTS) $(ROOTLIBS)   -lCintex -lMathCore 

.C.o:
	g++ -c -O -I$(ROOTSYS)/include $*.C
