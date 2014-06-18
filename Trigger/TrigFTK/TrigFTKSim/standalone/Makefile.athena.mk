# Main makefile was moved to Makefile.base.mk

BOOST_DIR=$(wildcard $(AtlasBaseDir)/sw/lcg/external/Boost/$(BoostVERS)_*/$(CMTCONFIG))
#BOOST_DIR=/$usr/lib
DCAP_DIR=
DCAP_CXXFLAGS=
DCAP_LDFLAGS=
BOOST_CXXFLAGS=-I$(wildcard $(BOOST_DIR)/include/boost-*)
#BOOST_DIR=/usr/include/boost
BOOST_SFX=-gcc43-mt
BOOST_LDFLAGS=-L$(BOOST_DIR)/lib -lboost_iostreams$(BOOST_SFX) -lboost_program_options$(BOOST_SFX) -lboost_filesystem$(BOOST_SFX)

ROOTCFLAGS=
CXXFLAGS=-O0 -pg -g -DHAVE_BOOST_IOSTREAMS $(BOOST_CXXFLAGS)
LIBS=-O0 -g -pg 

include Makefile.base.mk
