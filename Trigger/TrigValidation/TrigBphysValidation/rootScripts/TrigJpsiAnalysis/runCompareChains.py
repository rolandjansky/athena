#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##-----------------------------------------------------------
##
## runCompareChains.py
##
## Configuration script for TriggerJpsiAnalysis
## Creates ROOT macro with desired configuration,
##
## author: Daniel Scheirich <scheiric@cern.ch>
## Part of TriggerJpsiAnalysis in TrigBphysValidation package
##
##-----------------------------------------------------------
import string, os

##-----------------------------------------------------------
## Basic settings
##-----------------------------------------------------------

outputLevel  = "INFO"                 # output level: "DEBUG", "INFO", "WARNING", "ERROR"
execute      = True                   # execute macro

# name of the chain that should be analyzed
chains       = [ ]

## TrigDiMuon chains
#chains      += [ "L2_MU4_DiMu_FS_noOS" ]

chains      += [ "L2_mu4_DiMu_FS" ]
chains      += [ "L2_mu4_DiMu" ]

##chains      += [ "L2_MU4_DiMu_SiTrk_FS" ]
#chains      += [ "L2_MU4_DiMu_noOS" ]
#chains      += [ "L2_MU4_DiMu" ]

## Muon+track chains
#chains      += [ "L2_MU4_Trk_Jpsi_loose_FS" ]
#chains      += [ "L2_MU4_Trk_Jpsi_loose" ]
#chains      += [ "L2_MU4_Trk_Jpsi_FS" ]
#chains      += [ "L2_MU4_Trk_Jpsi" ]

##-----------------------------------------------------------
## More settings
##-----------------------------------------------------------
## add flags to the settings string to activate features
##    gaus+const                        fit J/psi mass with Gaussian + constant
##    gaus+pol1                         fit J/psi mass with Gaussian + linear polynomial
##    fitMin=2000 fitMax=5000           defines fit range
##    massBinLo=8 massBinHi=13          bins berween which efficiency is calculated

settings     = "gaus+pol1 fitMin=2000 fitMax=5000 massBinLo=8 massBinHi=13"
#settings    += ' TrackCollName=\\"MuidMuonCollectionTracks\\" '
#settings    += ' MuonCollName=\\"MuidMuonCollection\\" '

##-----------------------------------------------------------
## Input files
##-----------------------------------------------------------
## You can either specify list of files explicitly using inFileNames
## or you can load entire directory using dirPath vatiable

## list of input files created by TriggerJpsiAnalysis algorithm
inFileNames  = [ ]
#inFileNames += [ "../../run/bphys.06.root" ]

inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00001.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00002.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00003.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00004.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00005.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00006.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00007.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00008.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00009.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00010.bphys.root" ]

inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00011.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00012.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00013.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00014.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00015.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00016.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00017.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00018.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00019.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00020.bphys.root" ]

## directory containing root files created by TriggerJpsiAnalysis algorithm
#dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-05-07/rootfiles"   ## min bias stream
#dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-05-08/rootfiles"   ## muon stream
#dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-05-10/rootfiles"   ## muon stream 2 runs with all events
#dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-05-11/rootfiles"   ## muon stream, fixed TrigDecisionToolBug
#dirPath       = "/net/s3_datac/scheiric/data/output_TrigDiMuonCalib/2010-06-25/rootfiles"
#dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/"

##-----------------------------------------
## Main script -- DO NOT EDIT !
##-----------------------------------------
## Jpsi selection cuts

import jpsiCuts

## create cuts string

cutStr = ""

for cut in jpsiCuts.cuts:
  cutStr += cut + " "

## generate root macro

macro   = ""
macro   += "// Generated by runAnalyzeChain.py\n"
macro   += "#include <vector>\n"
macro   += "#include <string>\n"
macro   += "#include \"Log.h\"\n"
macro   += "void runCompareChains() {\n"
macro   += "  gSystem->Load(\"TrigJpsiRootAnalysis_cpp.so\");\n"
if('dirPath' in dir()):
  macro   += "  TrigJpsiRootAnalysis analysis(" + outputLevel + ", \"" + dirPath + "\");\n"

else:
  macro   += "  std::vector<std::string> files;\n"
  for file in inFileNames :
    macro += "  files.push_back( \"" + file + "\" );\n"
  macro   += "  TrigJpsiRootAnalysis analysis(" + outputLevel + ", files);\n"

macro   += "  std::vector<std::string> chains;\n"
for chain in chains :
  macro += "  chains.push_back( \"" + chain + "\" );\n"

macro   += "  analysis.compareChains(chains, \""+ settings +"\", \""+ cutStr +"\");\n"
macro   += "}\n"

## create macro
f = open("runCompareChains.C","w")
f.write(macro)
f.close()

## execute root macro
if(execute) :
  os.system( "root -l runCompareChains.C" )

## end of script



