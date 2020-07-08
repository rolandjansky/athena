# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
## Needed to correct ROOT behavior; see below
CWD = os.getcwd() # noqa

## MB: before starting, set ROOT to batch mode
from ROOT import gROOT

## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem

## Import the ROOT library for reading han results
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils # noqa

os.chdir(CWD)
gROOT.SetBatch(True)

MODVERSION = '$Id: DQHistogramMergeMod.py,v 1.8 2009-05-12 11:38:35 ponyisi Exp $'

DoProcMon=False
if DoProcMon:
    from . import DQProcMonitor
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
        from . import DQPostProcessMod
        DQPostProcessMod.DQPostProcess( outFileName, isIncremental )
