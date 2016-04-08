#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##-----------------------------------------------------------
##
## runAnalyzeChain.py
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

chain        = "EF_mu4_DiMu"        # name of the chain that should be analyzed

#chain        = "EF_mu4_DiMu_FS"        # name of the chain that should be analyzed

#chain        = "L2_2mu4_Jpsimumu"        # name of the chain that should be analyzed
#chain        = "L2_MU4_Trk_Jpsi"        # name of the chain that should be analyzed

#chain        = "L2_MU4_Jpsimumu_FS"        # name of the chain that should be analyzed
#chain        = "L2_2MU4_DiMu"        # name of the chain that should be analyzed

#chain        = "L2_MU4_DiMu_FS_noRoI"  # name of the chain that should be analyzed
#chain        = "L2_MU4_Trk_Jpsi_loose_FS"  # name of the chain that should be analyzed
#chain        = "L2_MU4_DiMu_FS_noOS"  # name of the chain that should be analyzed

outputLevel  = "INFO"                 # output level: "DEBUG", "INFO", "WARNING", "ERROR"
execute      = True                   # execute macro
savePlots    = True                   # save selected plots into the root file


period  = "D"

##-----------------------------------------------------------
## More settings
##-----------------------------------------------------------

tryNo     = "period" + period + ".02"
plotsFile = "plotsUpsi/" + chain + "." + tryNo + ".root"

## list of the plots to be saves
plots = []

plots += [ "jpsiPt0" ]
plots += [ "jpsiPt_hypo0" ]
plots += [ "jpsiEta0" ]
plots += [ "jpsiEta_hypo0" ]

if os.path.exists(plotsFile) :
  print "ERROR: file " + plotsFile + " already exists"
  execute = False

##-----------------------------------------------------------
## Even more settings
##-----------------------------------------------------------

settings    = ''
#settings    += ' gaus+pol1 fitMin=2.1 fitMax=4 '
settings    += ' massBinLo=19 massBinHi=27 '
#settings    += ' TrackCollName=\\"MuidMuonCollectionTracks\\" '
#settings    += ' MuonCollName=\\"MuidMuonCollection\\" '

## trigger cuts
perfCuts    = [ ]

## opposite charge check
if "_noOS" not in chain :
  perfCuts += [ "HypoCheckOS(1)" ]    # check that track have opposite sign

#######################################
## level-2 single muon triggers
#######################################
if "L2_MU" in chain or "L2_mu" in chain  :

  ##-----------------------
  ## TrigDiMuon chains:
  ##-----------------------
  if "_Trk_" not in chain :

    perfCuts += [ "HypoNMuHits(3)"   ]        # no. of associated muon hits requested in Hypo step (only for TrigDiMuon)

    ## mass cuts
    if "Jpsimumu" in chain :
      perfCuts += [ "HypoMass(2500,4300)" ]   # invariant mass cut at Hypo (Jpsi)
    elif "Upsimumu" in chain :
      perfCuts += [ "HypoMass(8000,12000)" ]  # invariant mass cut at Hypo (Upsi)
    else :
      perfCuts += [ "HypoMass(500,1e10)" ]    # invariant mass cut at Hypo (DiMu)

  ##-----------------------
  ## Muon+track chains:
  ##-----------------------

  ## all the cuts are applied at FEX level

#######################################
## level-2 dimuon triggers
#######################################
if "L2_2mu" in chain :

  ## vertex quality cut
  if "_noVtx" not in chain :
    perfCuts += [ "HypoChi2(20)" ]            # Hypo chi2 cut

  ## mass cuts
  if "Jpsimumu" in chain :
    perfCuts += [ "HypoMass(2500,4300)" ]   # invariant mass cut at Hypo (Jpsi)
  elif "Upsimumu" in chain :
    perfCuts += [ "HypoMass(8000,12000)" ]  # invariant mass cut at Hypo (Upsi)
  else :
    perfCuts += [ "HypoMass(500,1e10)" ]    # invariant mass cut at Hypo (DiMu)



#######################################
## Event Filter single muon triggers
#######################################
if "EF_MU" in chain or "EF_mu" in chain  :

  ##-----------------------
  ## TrigDiMuon chains:
  ##-----------------------
  if "_Trk_" not in chain :

    ## mass cuts
    if "Jpsimumu" in chain :
      perfCuts += [ "HypoMass(2500,4300)" ]   # invariant mass cut at Hypo (Jpsi)
    elif "Upsimumu" in chain :
      perfCuts += [ "HypoMass(8000,12000)" ]  # invariant mass cut at Hypo (Upsi)
    else :
      perfCuts += [ "HypoMass(500,1e10)" ]    # invariant mass cut at Hypo (DiMu)

  ##-----------------------
  ## Muon+track chains:
  ##-----------------------

  ## all the cuts are applied at FEX level


#######################################
## Event Filter dimuon triggers
#######################################
if "EF_2mu" in chain :

  ## vertex quality cut
  if "_noVtx" not in chain :
    perfCuts += [ "HypoChi2(20)" ]            # Hypo chi2 cut

  ## mass cuts
  if "Jpsimumu" in chain :
    perfCuts += [ "HypoMass(2500,4300)" ]   # invariant mass cut at Hypo (Jpsi)
  elif "Upsimumu" in chain :
    perfCuts += [ "HypoMass(8000,12000)" ]  # invariant mass cut at Hypo (Upsi)
  else :
    perfCuts += [ "HypoMass(500,1e10)" ]    # invariant mass cut at Hypo (DiMu)


#######################################
## Previous level trigger:
#######################################

if "EF_" in chain and "_passL2" not in chain :
  ## if we look at EF chain we want efficiencies w.r.t. Level-2

  perfCuts += [ "Level2(L2" + chain[2:len(chain)] + ")" ] # preceding level-2 chain

  settings += " UseEFPassThrough "                        # use EF pass-through istead of L2 ones

##-----------------------------------------------------------
## Input files
##-----------------------------------------------------------
## You can either specify list of files explicitly using inFileNames
## or you can load entire directory using dirPath vatiable

## list of input files created by TriggerJpsiAnalysis algorithm
#inFileNames  = [ ]

#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/user.scheiric.01.data10_7TeV_Muons_DetStatus-v03-pass1-analysis-2010F.periodF.TrigJpsiAnalysis.101012111203/user.scheiric.001618.EXT0._00002.bphys.root" ]
#inFileNames += [ "/net/s3_data_home/scheiric/workdir/TriggerValidation/AtlasOffline-15.8.0/WorkArea/run/bphys.04.root" ]

#inFileNames += [ "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-10-13/rootfiles/user.scheiric.001618.EXT0._00001.bphys.root.1" ]


## directory containing root files created by TriggerJpsiAnalysis algorithm
if "L2" in chain:
  if period == "D" :  dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-12-07/periodD_skimL2/"
  if period == "E" :  dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-12-07/periodE_skimL2/"
  if period == "F" :  dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-12-07/periodF_skimL2/"
  if period == "G" :  dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-12-07/periodG_skimL2/"

else :
  if period == "D" :  dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-12-07/periodD_skimEF/"
  if period == "E" :  dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-12-07/periodE_skimEF/"
  if period == "F" :  dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-12-07/periodF_skimEF/"
  if period == "G" :  dirPath       = "/net/s3_datac/scheiric/data/output_TriggerJpsiAnalysis/2010-12-07/periodG_skimEF/"

##-----------------------------------------
## Main script -- DO NOT EDIT !
##-----------------------------------------
## Jpsi selection cuts
import jpsiCuts

## add J/psi cuts
cutStr = ""
for cut in perfCuts:
  cutStr += cut + " "

for cut in jpsiCuts.cuts:
  cutStr += cut + " "

## generate root macro

macro   = ""
macro   += "// Generated by runAnalyzeChain.py\n"
macro   += "#include <vector>\n"
macro   += "#include <string>\n"
macro   += "#include \"Log.h\"\n"
macro   += "void runAnalyzeChain() {\n"
macro   += "  gSystem->Load(\"libGoodRunsLists.so\");\n"
macro   += "  gSystem->Load(\"TrigJpsiRootAnalysis_cpp.so\");\n"
if('dirPath' in dir()):
  macro   += "  TrigJpsiRootAnalysis analysis(" + outputLevel + ", \"" + dirPath + "\");\n"

else:
  macro   += "  std::vector<std::string> files;\n"
  for file in inFileNames :
    macro += "  files.push_back( \"" + file + "\" );\n"
  macro   += "  TrigJpsiRootAnalysis analysis(" + outputLevel + ", files);\n"

macro   += "  analysis.analyzeChain(\""+ chain +"\", \""+ settings +"\", \""+ cutStr +"\");\n"

if savePlots :
  macro   += "  TFile* fOut = TFile::Open(\"" + plotsFile + "\",\"RECREATE\") ;\n"
  macro   += "  fOut->cd();\n"
  for plot in plots :
    macro   += "  analysis.getHist(\"" + plot + "\")->Write();\n"
  macro   += "  fOut->Close();\n"


macro   += "}\n"

## create macro
f = open("runAnalyzeChain.C","w")
f.write(macro)
f.close()

## execute root macro
if(execute) :
  os.system( "root -l runAnalyzeChain.C" )

## end of script



