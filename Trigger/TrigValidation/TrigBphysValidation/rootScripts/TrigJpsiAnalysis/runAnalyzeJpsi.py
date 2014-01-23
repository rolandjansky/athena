#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##-----------------------------------------------------------
##
## runAnalyzeJpsi.py
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

##-----------------------------------------------------------
## More settings
##-----------------------------------------------------------
## add flags to the settings string to activate features
##    gaus+const                        fit J/psi mass with Gaussian + constant
##    gaus+pol1                         fit J/psi mass with Gaussian + linear polynomial
##    fitMin=2000 fitMax=5000           defines fit range
##    massBinLo=8 massBinHi=13          bins berween which efficiency is calculated
##    DoEventList                       create ".py" file with the list of selected events for each run. Used for AOD skimming

settings     = "gaus+pol1 fitMin=2.2 fitMax=4.8 massBinLo=8 massBinHi=13 DoEventList"
#settings    += ' TrackCollName=\\"MuidMuonCollectionTracks\\" '
#settings    += ' MuonCollName=\\"MuidMuonCollection\\" '

##-----------------------------------------------------------
## Input files
##-----------------------------------------------------------
## You can either specify list of files explicitly using inFileNames
## or you can load entire directory using dirPath vatiable

## list of input files created by TriggerJpsiAnalysis algorithm
inFileNames  = [ ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00001.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00002.bphys.root" ]
inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00003.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00004.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00005.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00006.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00007.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00008.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00009.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00010.bphys.root" ]

#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00011.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00012.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00013.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00014.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00015.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00016.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00017.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00018.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00019.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-08-26/user.yiyang.data10_7TeV_Muons.E1_E7.TriggerJpsiAnalysis.1.100821165959/user.yiyang.000479.EXT0._00020.bphys.root" ]

#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/user.scheiric.01.data10_7TeV_Muons_DetStatus-v03-pass1-analysis-2010F.periodF.TrigJpsiAnalysis.101012111203/user.scheiric.001618.EXT0._00001.bphys.root.1" ]
#inFileNames += [ "/net/s3_data_home/scheiric/workdir/TriggerValidation/AtlasOffline-15.8.0/WorkArea/run/bphys.04.root" ]


#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00001.bphys.root.1" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00002.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00003.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00004.bphys.root.1" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00005.bphys.root.1" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00006.bphys.root.1" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00007.bphys.root.1" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00008.bphys.root.1" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00009.bphys.root.1" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00010.bphys.root.1" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00107.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00108.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00109.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00110.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00111.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00112.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00113.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00114.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00115.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00116.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00214.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00215.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00216.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00217.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00218.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00219.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00220.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00221.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00222.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00224.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00420.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00421.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00422.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00423.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00424.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00425.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00426.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00427.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00428.bphys.root" ]
#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00429.bphys.root" ]

## directory containing root files created by TriggerJpsiAnalysis algorithm
#dirPath       = "/net/s3_datac/scheiric/data/output_TrigDiMuonCalib/2010-06-22/rootfiles"
#dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/"

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
macro   += "// Generated by runAnalyzeJpsi.py\n"
macro   += "#include <vector>\n"
macro   += "#include <string>\n"
macro   += "#include \"Log.h\"\n"
macro   += "void runAnalyzeJpsi() {\n"
macro   += "  gSystem->Load(\"TrigJpsiRootAnalysis_cpp.so\");\n"
if('dirPath' in dir()):
  macro   += "  TrigJpsiRootAnalysis analysis(" + outputLevel + ", \"" + dirPath + "\");\n"

else:
  macro   += "  std::vector<std::string> files;\n"
  for file in inFileNames :
    macro += "  files.push_back( \"" + file + "\" );\n"
  macro   += "  TrigJpsiRootAnalysis analysis(" + outputLevel + ", files);\n"

macro   += "  analysis.analyzeJpsi(\""+ settings +"\", \""+ cutStr +"\");\n"
macro   += "}\n"

## create macro
f = open("runAnalyzeJpsi.C","w")
f.write(macro)
f.close()

## execute root macro
if(execute) :
  os.system( "root -l runAnalyzeJpsi.C" )

## end of script



