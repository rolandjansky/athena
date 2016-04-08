#!/bin/csh
# enviroment setup for dcube.py running on lxplus for (t)csh shell
# 
# Aug 16, 2007 - cibak - switch to root 5.16/00
# Oct 23, 2007 - cibak - switch to root 5.17/04
# Apr 10, 2008 - cibak - switch to root 5.19/02 and python 2.5
#

setenv ROOT_VER 5.19.02 
setenv BIN  slc4_amd64_gcc34

setenv ROOTSYS /afs/cern.ch/sw/lcg/external/root/${ROOT_VER}/${BIN}/root
setenv PYTHONPATH ${ROOTSYS}/lib:/afs/cern.ch/sw/lcg/external/Python/2.5/${BIN}/lib/python2.5
setenv PATH ${ROOTSYS}/bin:/afs/cern.ch/sw/lcg/external/Python/2.5/${BIN}/bin:${PATH}
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:/afs/cern.ch/sw/lcg/external/Python/2.5/${BIN}/lib

