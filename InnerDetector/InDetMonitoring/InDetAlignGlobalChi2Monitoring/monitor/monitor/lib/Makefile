############################################
#                                          #
#  Makefile for library bigmatrix.so       #
#  loadable under ROOT                     #
#                                          #
############################################

include Makefile.arch
# CXX = g++
# CXXFLAGS = -g -O3 -Wall -I. $(shell root-config --cflags)

#
LIBNAME = libbigmatrix.so
LIBPATH = ../
#
OBJS = bigmatrix.o
HDRS    = $(patsubst %.o,%.h,$(OBJS))
#
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<
#
$(LIBNAME): $(OBJS) bigmatrixCint.o bigmatrix_LinkDef.h
	$(CXX) $(CXXFLAGS) -shared -o $@ $(OBJS) bigmatrixCint.o
#	cp $(LIBNAME) $(LIBPATH)
#
bigmatrixCint.cc : bigmatrix.h
	@echo "Generating dictionary ..."
	$(ROOTSYS)/bin/rootcint -f $@ -c $^ bigmatrix_LinkDef.h
#
bigmatrix.o: bigmatrix.cc bigmatrix.h
#
clean:
	rm -f $(OBJS)
	rm -f *Cint.*
	rm -f core*
	rm -f *~*
	rm -f $(LIBNAME)
