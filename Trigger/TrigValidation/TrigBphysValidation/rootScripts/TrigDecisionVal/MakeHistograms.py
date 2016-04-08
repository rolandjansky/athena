#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##-----------------------------------------------------------
## For Monte Carlo Physics Validation : B-Trigger 
## author: Yi Yang <yi.yang@cern.ch>
##-----------------------------------------------------------
import string, os
##-----------------------------------------------------------
## Basic settings
##-----------------------------------------------------------
execute   = True   # execute macro
LoadGRL_package = True  # If no GRL package ==> False and also comment out the lines in BTrigger.C (search GRLs) 
useGRL    = "false"     # Using GRL 
GRL_file = "GRLs_2011/data11_7TeV.periodAllYear_DetStatus-v31-pro09_CoolRunQuery-00-04-00_Muon.xml"

# ------------------------------------------------------------- 
# Output Files Name
# ------------------------------------------------------------- 
outputFile = "Histograms_Data_test.root"

# Input Files
Directory = True      # use the path of directory for input files ==> Set dirPath
#Directory = False    # use single file for input file  ==> Set dirPath_file

##################################
# For using many files as inputs
##################################
dirPath = "/d00/yiyang/work/17.0.1.2/DQ2_Download/IU_B_Ntuple/MonteCarlo/user.yiyang.mc10b.108491.Pythia_Upsilon1S_mu2p5mu2p5.IU_Upsilon_Ntuple.r2300.r17_0_1_2.test.2.110716091501/"

##################################
# For using single file as input
##################################
dirPath_file = "/d00/yiyang/work/16.2.0.4/Trigger_PhysVal/input/"
inFileNames  = [ ]
inFileNames += [ ""+dirPath_file+"ntuple_directJpsimu4mu4_r1621.root" ]

## directory containing root files created by TriggerJpsiAnalysis algorithm
input_file = ""
if (Directory) :
    import glob
    for filename in glob.glob( dirPath+"/"'*.root*') :
        input_file += "" + filename + "\n"
else :
    for filename in inFileNames :
        input_file += "" + filename + "\n"
## create macro
f_in = open( "input.txt", "w" )
f_in.write( input_file )
f_in.close()

## ==========================================================
##
## name of the chain that should be analyzed
##
## ==========================================================
## Denominators for efficiency calculation
denominators  = [ ]
denominators += [ "L1_MU0" ]
denominators += [ "L1_2MU0" ]
denominators += [ "L2_mu4" ]
denominators += [ "L2_2mu4" ]
denominators += [ "EF_mu4" ]
denominators += [ "EF_2mu4" ]
denominators += [ "EF_mu4_DiMu" ]
denominators += [ "L2_mu4_Trk_Jpsi" ]
denominators += [ "EF_mu4_Trk_Jpsi" ]
denominators += [ "L2_mu4_Trk_Upsi_FS" ]
denominators += [ "EF_mu4_Trk_Upsi_FS" ]
denominators += [ "L2_mu6_Trk_Jpsi" ]
denominators += [ "EF_mu6_Trk_Jpsi" ]
denominators += [ "L2_mu6_Trk_Upsi_FS" ]
denominators += [ "EF_mu6_Trk_Upsi_FS" ]


chains = [ ]

chains += [ "L1_MU0" ]
chains += [ "L1_2MU0" ]
chains += [ "L2_mu4" ]
chains += [ "EF_mu4" ]
chains += [ "EF_mu6" ]

chains += [ "L2_mu4_DiMu" ]
chains += [ "L2_mu4_DiMu_FS" ]
chains += [ "L2_mu4_DiMu_MG" ]
chains += [ "L2_mu4_DiMu_MG_FS" ]
chains += [ "L2_mu4_DiMu_SiTrk" ]
chains += [ "L2_mu4_DiMu_SiTrk_FS" ]
chains += [ "L2_MU4_DiMu" ]
chains += [ "L2_MU4_DiMu_FS" ]
chains += [ "L2_mu4_Jpsimumu"]
chains += [ "L2_mu6_Jpsimumu"]
chains += [ "L2_mu6_Jpsimumu_tight" ]
chains += [ "L2_mu6_Jpsimumu_SiTrk" ]
chains += [ "L2_mu10_Jpsimumu"]
chains += [ "L2_mu4_Trk_Jpsi" ]
chains += [ "L2_mu4_Upsimumu_FS"]
chains += [ "L2_mu4_Upsimumu_tight_FS" ]
chains += [ "L2_mu6_Upsimumu_FS"]
chains += [ "L2_mu4_Upsimumu_SiTrk_FS" ]
chains += [ "L2_mu6_Upsimumu_SiTrk_FS" ]
chains += [ "L2_mu10_Upsimumu_FS"]
chains += [ "L2_mu4_Trk_Upsi_FS" ]

chains += [ "EF_mu4_DiMu" ]
chains += [ "EF_mu4_DiMu_FS" ]
chains += [ "EF_mu4_DiMu_MG" ]
chains += [ "EF_mu4_DiMu_MG_FS" ]
chains += [ "EF_mu4_DiMu_SiTrk" ]
chains += [ "EF_mu4_DiMu_SiTrk_FS" ]
chains += [ "EF_MU4_DiMu" ]
chains += [ "EF_MU4_DiMu_FS" ]
chains += [ "EF_mu4_Jpsimumu" ]
chains += [ "EF_mu6_Jpsimumu" ]
chains += [ "EF_mu6_Jpsimumu_tight" ]
chains += [ "EF_mu6_Jpsimumu_SiTrk" ]
chains += [ "EF_mu10_Jpsimumu" ]
chains += [ "EF_mu4_Trk_Jpsi" ]
chains += [ "EF_mu4_Upsimumu_FS" ]
chains += [ "EF_mu4_Upsimumu_tight_FS" ]
chains += [ "EF_mu6_Upsimumu_FS" ]
chains += [ "EF_mu4_Upsimumu_SiTrk_FS" ]
chains += [ "EF_mu6_Upsimumu_SiTrk_FS" ]
chains += [ "EF_mu10_Upsimumu_FS" ]
chains += [ "EF_mu4_Trk_Upsi_FS" ]

chains += ["L2_2mu4"]
chains += ["L2_2mu4_DiMu"]
chains += ["L2_2mu4_DiMu_noVtx"]
chains += ["L2_2mu4_Jpsimumu"]
chains += ["L2_2mu4_Jpsimumu_noVtx"]
chains += ["L2_2mu4_Upsimumu"]
chains += ["L2_2mu4_Bmumu"]
chains += ["L2_2mu4_BmumuX"]

chains += ["EF_2mu4"]
chains += ["EF_2mu4_DiMu"]
chains += ["EF_2mu4_DiMu_noVtx"]
chains += ["EF_2mu4_Jpsimumu"]
chains += ["EF_2mu4_Jpsimumu_noVtx"]
chains += ["EF_2mu4_Upsimumu"]
chains += ["EF_2mu4_Bmumu"]
chains += ["EF_2mu4_BmumuX"]


## ==========================================================
##
## Cuts on the Candidates
##
## ==========================================================
cuts = [ ]
# Number of SCT
cuts += [ "6.0" ]
# Number of Pixel
cuts += [ "1.0" ]
# Number of TRT
cuts += [ "0.0" ]
# Number of Combined Muon
cuts += [ "2.0" ]
# pT of first muon
cuts += [ "4.0" ]
# pT of second muon
cuts += [ "4.0" ]
# eta of muons
cuts += [ "2.5" ]


##-----------------------------------------
## Main script -- DO NOT EDIT !
##-----------------------------------------
## generate root macro
macro   = ""
macro   += "#include \"TROOT.h\" \n"
macro   += "#include \"TChain.h\" \n"
macro   += "#include \"TSystem.h\" \n"
macro   += "#include \"<vector>\" \n"
macro   += "void MakeHistograms() { \n"
if ( LoadGRL_package ) :
   macro   += "    gSystem->Load(\"libPhysics\"); \n"
   macro   += "    gSystem->Load(\"./GoodRunsLists-00-00-91/StandAlone/libGoodRunsLists.so\"); \n"
   macro   += "    gSystem->AddIncludePath(\"-IGoodRunsLists-00-00-91\"); \n"

macro   += " // ==============================\n"
macro   += "    std::vector<double> cuts;\n"
for cut in cuts :
  macro += "    cuts.push_back( " + cut + " ); \n"

macro   += " // ==============================\n"
macro   += "    std::vector<std::string> denominators;\n"
for denominator in denominators :
  macro += "    denominators.push_back( \"" + denominator + "\" );\n"


macro   += " // ==============================\n"
macro   += "    std::vector<std::string> chains;\n"
for chain in chains :
  macro += "    chains.push_back( \"" + chain + "\" );\n"

macro   += "//  ============================== \n"
macro   += "\n"
macro   += "    gROOT->LoadMacro(\"BTrigger.C+\"); \n"

macro   += "    StartAnalysis( denominators, chains, cuts, "+useGRL+", \""+GRL_file+"\" ); \n" 

macro   += " } \n" 

## create macro
f = open( "MakeHistograms.C", "w" )
f.write( macro )
f.close()

## execute root macro
if( execute ) :
  os.system( "root -l -q MakeHistograms.C" )
  os.system( "mv Histograms_MC_.root " + outputFile)

## end of script



