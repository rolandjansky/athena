
CXX           = g++ 

FXX           = g77

FLINK         = -static \
		-Bstatic \
		-static-libcxa

FXXFLAGS = -v \
	-O3 \
        -I../. \
	-I../include \
	-c `cmt show macro_value new_fincludes`


ROOTCXXFLAGS  = $(shell root-config --cflags)
#CLHEPFLAGS    = $(shell clhep-config --include)


CXXFLAGS      = -g -O3 -Wall $(ROOTCXXFLAGS) -I./myMdtPlots -I./src

ROOTLIBS      = $(shell root-config --libs)

#PACKLIB       = /cern/pro/lib/libpacklib.a
CLHEPLIB      = -L/afs/cern.ch/sw/lcg/external/clhep/1.9.4.7/i686-slc5-gcc43-opt/lib -lCLHEP-Vector-1.9.4.7 -lCLHEP-Matrix-1.9.4.7 -lCLHEP-Geometry-1.9.4.7
CLHEPFLAGS    = -I/afs/cern.ch/sw/lcg/external/clhep/1.9.4.7/i686-slc5-gcc43-opt/include

TOOLSLIB      = 
CDIR          = ../cpplib
BINDIR        = bin
LIBDIR        = bin

CURRENTDIR    = $(shell `echo $PWD`)
LIBS          = $(ROOTLIBS)

SRC           = myMdtPlots_x.cpp

OBJS          = ChamberDerivs.o ChamberNames.o

APP           = 

all: selcuts_x 

ChamberDerivs.o: ChamberDerivs.cpp
	$(CXX) $(CXXFLAGS) $(CLHEPFLAGS) -c ChamberDerivs.cpp

ChamberNames.o: ChamberNames.cpp
	$(CXX) $(CXXFLAGS) -c ChamberNames.cpp

level1Plots_x: level1Plots_x.cpp
	$(CXX) $(CXXFLAGS) level1Plots_x.cpp \
	$(LIBS) -o level1Plots_x

fixSDTree_x: $(OBJS) fixSDTree_x.cpp
	$(CXX) $(CXXFLAGS) $(CLHEPFLAGS) fixSDTree_x.cpp \
	$(OBJS) $(LIBS) $(CLHEPLIB) -o fixSDTree_x

simplePlots_x: $(OBJS) simplePlots.cpp
	$(CXX) $(CXXFLAGS) simplePlots.cpp \
	$(OBJS) $(LIBS) -o simplePlots_x

plots_x: $(OBJS) globalchi2_v_alignparam.cpp
	$(CXX) $(CXXFLAGS) globalchi2_v_alignparam.cpp  \
	$(OBJS) $(LIBS) -o plots_x

calcErrorScaling_x: $(OBJS) calcErrorScaling.cpp
	$(CXX) $(CXXFLAGS) calcErrorScaling.cpp  \
	$(OBJS) $(LIBS) -o calcErrorScaling_x

plottrend_x: $(OBJS) plottrend_x.cpp
	$(CXX) $(CXXFLAGS) plottrend_x.cpp \
	$(OBJS) $(LIBS) -o plottrend_x

plottrendEE_x: ChamberNames.o plottrendEE_x.cpp
	$(CXX) $(CXXFLAGS) plottrendEE_x.cpp \
	ChamberNames.o $(LIBS) -o plottrendEE_x

muontrackplots_x: $(OBJS) muontrackplots_x.cpp
	$(CXX) $(CXXFLAGS) muontrackplots_x.cpp \
	$(OBJS) $(LIBS) -o muontrackplots_x

chi2validation_x: $(OBJS) chi2validation_x.cpp
	$(CXX) $(CXXFLAGS) chi2validation_x.cpp \
	$(OBJS) $(LIBS) -o chi2validation_x

plotres_x: $(OBJS) plotres.cpp
	$(CXX) $(CXXFLAGS) plotres.cpp  \
	$(OBJS) $(LIBS) -o plotres_x

selcuts_x: $(OBJS) selcuts_x.cpp
	$(CXX) $(CXXFLAGS) $(CLHEPFLAGS) selcuts_x.cpp \
	$(OBJS) $(LIBS) $(CLHEPLIB) -o selcuts_x

compare_x: $(OBJS) compare_x.cpp
	$(CXX) $(CXXFLAGS) compare_x.cpp  \
	$(OBJS) $(LIBS) -o compare_x

unbiasedResPlots_x: $(OBJS) unbiasedResPlots_x.cpp
	$(CXX) $(CXXFLAGS) unbiasedResPlots_x.cpp  \
	$(OBJS) $(LIBS) -o unbiasedResPlots_x

eel_x: $(OBJS) eel_x.cpp
	$(CXX) $(CXXFLAGS) eel_x.cpp \
	$(OBJS) $(LIBS) -o eel_x

pulls_x: $(OBJS) pulls_x.cpp
	$(CXX) $(CXXFLAGS) pulls_x.cpp  \
	$(OBJS) $(LIBS) -o pulls_x

bee_x: $(OBJS) bee_x.cpp
	$(CXX) $(CXXFLAGS) bee_x.cpp  \
	$(OBJS) $(LIBS) -o bee_x

combineUnbiasedResPlots_x: $(OBJS) combineUnbiasedResPlots_x.cpp
	$(CXX) $(CXXFLAGS) combineUnbiasedResPlots_x.cpp  \
	$(OBJS) $(LIBS) -o combineUnbiasedResPlots_x

plotCorr_x: $(OBJS) plotCorr.C
	$(CXX) $(CXXFLAGS) plotCorr.C  \
	$(OBJS) $(LIBS) -o plotCorr_x

chamberFits_x: $(OBJS) chamberFits_x.cpp
	$(CXX) $(CXXFLAGS) chamberFits_x.cpp  \
	$(OBJS) $(LIBS) -o chamberFits_x

plotBEESagitta_x: ChamberNames.o plotBEESagitta_x.cpp
	$(CXX) $(CXXFLAGS) plotBEESagitta_x.cpp  \
	ChamberNames.o $(LIBS) -o plotBEESagitta_x

install:
	mv $(OBJS) $(BINDIR)
	mv -f $(APP) $(BINDIR)

clean:
	rm -f $(OBJS) $(APP) $(BINDIR)/$(APP)

ChamberData.o: src/ChamberData.cxx myMdtPlots/ChamberData.h
	$(CXX) $(CXXFLAGS) -c src/ChamberData.cxx -o ChamberData.o

