# Makefile for ROOT - Jetnet Interface classes and test applications 
# Author: Vassil Verguilov
# Created: 22.01.2005 @ 17:52
#

#HOST := $(shell echo $HOSTNAME)

#JETNET_LIB=libJetNet.$(HOSTNAME).o
JETNET_LIB=jetnet.o
#JETNET_SRC=jetnet34.f
JETNET_SRC=jetnet_35.f

ROOTLIBS=$(shell root-config --libs)
ROOTGLIBS=$(shell root-config --glibs)

SUBDIRS = mbase mastro \
          mmain mfbase \
          mfilter mdata \
          mhbase mhvstime \
          mhist manalysis \
          msignal \
          mbadpixels \
          mhcalib \
          mhflux \
          mflux \
          mcalib \
          mpedestal \
          mfileio \
          mreflector \
          mgeom \
          msql \
          mimage \
          mmontecarlo \
          mhft \
          mmc \
          mraw \
          mcamera \
          mtrigger \
	  mpointing \
          mreport \
          mgui \
          mranforest \
          mhistmc \
          mjobs \
          mtools \
          mstarcam \
          msupercuts \
          mdisp

PROJECT_S  := $(wildcard *.$(SrcSuf))
INCLUDES = -I. $(SUBDIRS:%=-I$(MARSSYS)/%)
CINTLIST_H := $(addsuffix .h, $(basename $(PROJECT_S)))
CINTLIST_O := $(addsuffix .$(ObjSuf), $(basename $(CINTLIST_H)))

SOLIBS = -lgfortran -L/home/vasoto/Projects/cern/root_v3.10.2/lib/root -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lpthread -lm -ldl -L/home/vasoto/Projects/cern/root_v3.10.2/lib/root -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lGui -lpthread -lm -ldl

SRCS=trainNN.cxx

EXE=test.exe
LIB=libTJetNet.so

OBJS=$(JETNET_LIB) TNeuralDataSet.o TJetNet.o
SOCMD=ld
FC=gfortran
CC=g++ -m32
#CC=gcc
INC=-I$(ROOTSYS)/include $(INCLUDES)
LIBS=-lgfortran $(ROOTLIBS) $(ROOTGLIBS) -lstdc++ -lgcc -lc 
LIBS_SO=-lgfortran $(ROOTLIBS) -lstdc++ -lgcc -lc
COPT=-O3 -fPIC
# -g
FOPT=-ffixed-form -ffixed-line-length-none -u -Wall -fPIC -O
# -g

all: lib

TNeuralDataSetDict.h TNeuralDataSetDict.cxx : TNeuralDataSet.cxx TNeuralDataSet.h
	$(RM) TNeuralDataSetDict.*
	@echo Making Dictionary for TNeuralDataSet...
	rootcint TNeuralDataSetDict.cxx -c TNeuralDataSet.h

TJetNetDict.h TJetNetDict.cxx : TJetNet.cxx TJetNet.h
	$(RM) TJetNetDict.*
	@echo Making Dictionary for TJetNet
	rootcint TJetNetDict.cxx -c TJetNet.h

TTrainedNetworkDict.h TTrainedNetworkDict.cxx : TTrainedNetwork.cxx TTrainedNetwork.h
	$(RM) TTrainedNetworkDict.*
	@echo Making Dictionary for TTrainedNetwork
	rootcint TTrainedNetworkDict.cxx -c TTrainedNetwork.h

TNetworkToHistoToolDict.h TNetworkToHistoToolDict.cxx : TNetworkToHistoTool.cxx TNetworkToHistoTool.h
	$(RM) TNetworkToHistoToolDict.*
	@echo Making Dictionary for TNetworkToHistoTool
	rootcint TNetworkToHistoToolDict.cxx -c TNetworkToHistoTool.h


$(OBJS): $(JETNET_SRC) 
#	@echo Building JetNet Library
	$(FC) $(FOPT) -c $(JETNET_SRC) -o $(JETNET_LIB)
	$(RM) TNeuralDataSetDict.*
	@echo Making Dictionary for TNeuralDataSet...
	rootcint TNeuralDataSetDict.cxx -c TNeuralDataSet.h
	$(CC) $(COPT) -c TNeuralDataSetDict.cxx $(INC) -o TNeuralDataSetDict.o
	$(CC) $(COPT) -c TNeuralDataSet.cxx $(INC) -o TNeuralDataSet.o
	$(RM) TJetNetDict.*
	@echo Making Dictionary for TJetNet
	rootcint TJetNetDict.cxx -c TJetNet.h
	$(CC) $(COPT) -c TJetNetDict.cxx $(INC) -o TJetNetDict.o
	$(CC) $(COPT) -c TJetNet.cxx $(INC) -o TJetNet.o
	$(CC) -c -o $@ $<
	@echo Making Dictionary for TTrainedNetwork
	rootcint TTrainedNetworkDict.cxx -c TTrainedNetwork.h
	$(CC) $(COPT) -c TTrainedNetworkDict.cxx $(INC) -o TTrainedNetworkDict.o
	$(CC) $(COPT) -c TTrainedNetwork.cxx $(INC) -o TTrainedNetwork.o
	$(CC) -c -o $@ $<
	@echo Making Dictionary for TNetworkToHistoTool
	rootcint TNetworkToHistoToolDict.cxx -c TNetworkToHistoTool.h
	$(CC) $(COPT) -c TNetworkToHistoToolDict.cxx $(INC) -o TNetworkToHistoToolDict.o
	$(CC) $(COPT) -c TNetworkToHistoTool.cxx $(INC) -o TNetworkToHistoTool.o
	$(CC) $(COPT) -c -o $@ $<

$(EXE): $(OBJS) $(SRCS)
	$(CC) $(COPT) $(SRCS) $(JETNET_LIB) TNeuralDataSetDict.o TNeuralDataSet.o TJetNetDict.o TJetNet.o TTrainedNetwork.o TTrainedNetworkDict.o TNetworkToHistoTool.o TNetworkToHistoToolDict.o $(INC) $(LIBS) -o $(EXE)

lib: $(OBJS) 
	$(CC) -shared -Wl,-soname,libTJetNet.so.0 -o libTJetNet.so $(JETNET_LIB) TNeuralDataSetDict.o TNeuralDataSet.o TJetNetDict.o TJetNet.o TTrainedNetwork.o TTrainedNetworkDict.o TNetworkToHistoTool.o TNetworkToHistoToolDict.o $(INC) $(LIBS_SO)
#-g

cint : TNeuralDataSetDict.h TJetNetDict.h TTrainedNetworkDict.hT NetworkToHistoToolDict.h

$(OBJS1) : %.o : %.cxx $(JETNET_SRC)
	$(FC) $(FOPT) -c $(JETNET_SRC) -o $(JETNET_LIB)
	$(CC) -c -o $@ $<

jetnet: $(OBJS1)
	$(CC) $(INC) $(LIBS) -s -o $@ $(OBJS1)

clean:
	$(RM) $(EXE) *~ $(JETNET_LIB) libTJetNet.so *.o *.bak core *Dict.h *Dict.cxx gauss.exe
backup:
	$(RM) backup.tar.gz
	@tar czvf backup.tar.gz *.cxx *.h *.c *.C Makefile* *.f

