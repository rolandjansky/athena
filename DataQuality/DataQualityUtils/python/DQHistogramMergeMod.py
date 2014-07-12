# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## *****************************************************************************
MODVERSION = '$Id: DQHistogramMergeMod.py,v 1.8 2009-05-12 11:38:35 ponyisi Exp $'
## *****************************************************************************

import os
## Needed to correct ROOT behavior; see below
CWD = os.getcwd()

import sys
import ROOT

## MB: before starting, set ROOT to batch mode
from ROOT import gROOT
gROOT.SetBatch(True)

## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem
os.chdir(CWD)

## Import the ROOT library for reading han results
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils
DoProcMon=False
if DoProcMon:
  import DQProcMonitor
  DQProcMonitor.startProcMonThread()
  
def DQHistogramMerge( listFileName, outFileName, runPostProcessing, directoryRegularExpression=".*", histogramRegularExpression=".*", isIncremental=False, compressionLevel=1,debugLevel=0 ):
  
  mf = dqutils.MonitoringFile()
  if directoryRegularExpression!=".*" or histogramRegularExpression!=".*":
    mf.setDirectoryRegEx(directoryRegularExpression)
    mf.setHistogramRegEx(histogramRegularExpression)
  mf.setDebugLevel(debugLevel)
  mf.setCompressionLevel(compressionLevel)
  mf.mergeFiles( outFileName, listFileName )
  mf.mergeLBintervals( outFileName )
  
  if runPostProcessing:
    import DQPostProcessMod
    DQPostProcessMod.DQPostProcess( outFileName, isIncremental )
