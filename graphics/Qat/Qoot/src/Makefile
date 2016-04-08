prefix = /usr/local
rootsys= $(prefix)/root
targetdir = ../tmp


TMCPPFILES           =  $(shell find -name "*.cpp" )
MYCPPFILES           =  $(patsubst ./%,   %,   $(TMCPPFILES))
MYOFILES             =  $(patsubst %.cpp, %.o, $(MYCPPFILES))
DSOFILES             =  $(patsubst %.cpp, $(targetdir)/IODriver/%.so, $(MYCPPFILES))


override CXXFLAGS  +=  -g -O0 -fPIC -I../..   -I$(prefix)/include  -I$(rootsys)/include/ 

%.cpp.%.o:
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

all : $(targetdir)/IODriver/libQoot.so 
$(targetdir)/IODriver/libQoot.so : RootDriver.o
	mkdir -p $(targetdir)
	mkdir -p $(targetdir)/IODriver
	mkdir -p $(targetdir)/IODriver/`dirname $<`
	cp $< $(targetdir)/IODriver/`dirname $<`
	g++ --shared   $< -L$(prefix)/lib -L$(rootsys)/lib -lQatDataAnalysis   -lHist -lMatrix -lTree -lRIO -lNet -lMathCore -lCore -lCint -o $@
	rm -f $(targetdir)/IODriver/$<


all: $(DSOFILES)

$(DSOFILES):$(MYOFILES)


install: all
	mkdir -p $(prefix)/share
	cp -r $(targetdir)/IODriver/* $(prefix)/lib


uninstall:
	rm  -f $(prefix)/share/IODriver/libQoot.so

clean:
	rm -r -f $(targetdir)/IODriver RootDriver.o


bin:$(DSOFILES) 


distdir:
	mkdir -p  "$(distdir)"
	find . -name "*.cpp" -exec install -D {} $(distdir)/{} \;
	install Makefile.in $(distdir)

distclean:
	rm -r -f $(distdir)/IODriver





  
