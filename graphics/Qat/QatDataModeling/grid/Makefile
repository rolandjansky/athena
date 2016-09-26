THISDIR = QatDataModeling
XTRA_INCLUDES = -I../../../local/include
include ../../../sw/grid/Makefile.common 

LDFLAGS += -L../../../local/lib
UNAME_S := $(shell uname -s)
LOADLIBES+=-lgfortran
ifeq ($(UNAME_S),Darwin)
  LOADLIBES+=-lQatDataAnalysis
  LOADLIBES+=-lCLHEP-GenericFunctions-2.1.3.1
  LOADLIBES+=-lCLHEP-Matrix-2.1.3.1
  LOADLIBES+=-lCLHEP-Vector-2.1.3.1     
endif


