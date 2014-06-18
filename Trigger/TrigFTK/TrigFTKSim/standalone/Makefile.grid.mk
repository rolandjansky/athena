# This is a special version of makefile that enables dcap and boost by default.
# It expects BOOST_CXXFLAGS, BOOST_LDFLAGS, DCAP_CXXFLAGS, DCAP_LDFLAGS to be defined in shell
#  (you can set them up by sourcing (1) athena's setup.py, (2) TrigFTKSim/grid/dependencies.sh)

ROOTCFLAGS=
LIBS=-O2 -g
CXXFLAGS=-DHAVE_BOOST_IOSTREAMS -DHAVE_LIBDCAP -O2 -g

include Makefile.base.mk
