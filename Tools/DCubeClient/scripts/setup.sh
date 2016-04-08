#!/bin/bash
#
# enviroment setup script for dcube.py running on lxplus for (ba,z)sh shell
# 
# Aug 16, 2007 - cibak - switch to root 5.16/00
# Oct 23, 2007 - cibak - switch to root 5.17/04
# Apr 10, 2008 - cibak - switch to root 5.19/02 and python 2.5
#

export ROOT_VER=5.19.02
export BIN=slc4_amd64_gcc34

export ROOTSYS=/afs/cern.ch/sw/lcg/external/root/${ROOT_VER}/${BIN}/root
export PYTHONPATH=${ROOTSYS}/lib:/afs/cern.ch/sw/lcg/external/Python/2.5/${BIN}/lib/python2.5
export PATH=${ROOTSYS}/bin:/afs/cern.ch/sw/lcg/external/Python/2.5/${BIN}/bin:${PATH}
export LD_LIBRARY_PATH=${ROOTSYS}/lib:/afs/cern.ch/sw/lcg/external/Python/2.5/${BIN}/lib

