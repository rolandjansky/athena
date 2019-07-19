# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import sys
from MuonEfficiencyCorrections.CommonToolSetup import *


# a simple testing macro for the MuonEfficiencyCorrections_xAOD package in athena
#
# Usage: athena --filesInput <InputFile> MuonEfficiencyCorrections/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py
#  E.g.:  athena --filesInput /ptmp/mpp/junggjo9/ClusterTP/Datasets/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.deriv.DAOD_MUON1.e3601_s3126_r9364_r9315_p3374/DAOD_MUON1.13055759._000989.pool.root.1 MuonEfficiencyCorrections/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py
# Access the algorithm sequence:
AssembleIO("MUONEFFTESTER")
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__CalibratedMuonsProvider, CP__MuonCalibrationPeriodTool
toolName = "MuonCalibrationTool"
calibTool = CP__MuonCalibrationPeriodTool(toolName)
ToolSvc += calibTool

theJob += CP__CalibratedMuonsProvider("CalibratedMuonsProvider",Tool=calibTool, prwTool = GetPRWTool(
 #               PRWLumiCalcFiles = ["GoodRunsLists/data18_13TeV/20180830/physics_13TeV_lowmu_2018.lumicalc.OflLumi-13TeV-001.root"],
 #               PRWMCConfigFiles = ["/ptmp/mpp/niko/TEMP/mc16_FULLSIM_r10244_NTUP_PILEUP.root"  ],
))

#ToolSvc.prwTool.PeriodAssignments = [ 304000, 348197,364485]
# Add the test algorithm:
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyCorrections_TestAlg
alg = CP__MuonEfficiencyCorrections_TestAlg("EffiTestAlg")
alg.PileupReweightingTool = GetPRWTool()
alg.MuonSelectionTool = GetSelectionTool()
alg.DefaultRelease="cFeb_2019"
alg.ValidationRelease="cMay_2019"
alg.SGKey = "CalibratedMuons"
## Select 30 GeV muons for the high-pt WP only
alg.MinPt = 4000
alg.MaxEta = 2.7
#alg.MinQualit = 1 #Medium
WPs = [
         # reconstruction WPs
      #  "LowPt",
        "Loose", 
        "Medium", 
       # "Tight", 
     #   "HighPt",       
         # track-to-vertex-association WPs
         "TTVA",
         # BadMuon veto SFs
     #   "BadMuonVeto_HighPt",
        #"GradientIso",
         # isolation WPs
        "FCLooseIso",                    
        "FCTight_FixedRadIso",
        "FCLoose_FixedRadIso",           
        "FixedCutHighPtTrackOnlyIso",
        "FCTightIso",                    
        "FixedCutPflowLooseIso",
        "FCTightTrackOnlyIso",           
        "FixedCutPflowTightIso",
        "FCTightTrackOnly_FixedRadIso",
        ]

for WP in WPs: 
    alg.EfficiencyTools += [GetMuonEfficiencyTool(WP, Release="190312_Winter_r21", 
                                                  #CustomInput = "/ptmp/mpp/junggjo9/ClusterTP/SFFiles/Winter_2019_coarsePtBinning/"
                                                  )]
    alg.EfficiencyToolsForComparison += [GetMuonEfficiencyTool(WP, 
                                                #CustomInput = "/ptmp/mpp/niko/TEMP/"
                                                Release = "190530_r21"
                                              )]
  
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
#theApp.EvtMax = 200

#ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.defaultLimit = 100
