# Makefile version that allows to enable BOOST and DCAP

ROOTCFLAGS=
LIBS=-O2
CXXFLAGS=-DHAVE_BOOST_IOSTREAMS -O2 

BOOST_DIR=${BOOSTDIR}

ifndef BOOST_CXXFLAGS
	BOOST_CXXFLAGS=-I$(BOOST_DIR)/include/boost-${BOOST_VER}
endif
ifndef BOOST_LDFLAGS
	BOOST_LDFLAGS=-L$(BOOST_DIR)/lib -lboost_iostreams-gcc34-mt-${BOOST_VER} -lboost_program_options-gcc34-mt-${BOOST_VER} -lboost_filesystem-gcc34-mt-${BOOST_VER}
endif

include Makefile.base.mk
