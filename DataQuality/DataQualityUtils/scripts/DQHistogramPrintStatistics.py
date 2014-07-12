#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## *****************************************************************************
VERSION = '$Id: DQHistogramPrintStatistics.py 354890 2011-03-28 16:30:59Z kama $'
## *****************************************************************************

import os
## Needed to correct ROOT behavior; see below
CWD = os.getcwd()

import sys

import ROOT
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem
os.chdir(CWD)

## Import the ROOT library for reading han results
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils


def DQHistogramPrintStatistics( inputFileName ):
  
  mf = dqutils.MonitoringFile(inputFileName)
  mf.printStatistics()


def usage():
  cmdi = sys.argv[0].rfind("/")
  cmd = sys.argv[0][cmdi+1:]
  print "Usage: ", cmd, "<input_file_name>"



########################################
## main()
########################################

if __name__ == "__main__":

  if len(sys.argv) != 2:
    usage()
    sys.exit(0)
  
  DQHistogramPrintStatistics( sys.argv[1] )
  
