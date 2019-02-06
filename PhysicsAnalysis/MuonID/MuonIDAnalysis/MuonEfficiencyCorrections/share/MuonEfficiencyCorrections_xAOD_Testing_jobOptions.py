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

# Add the test algorithm:
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyCorrections_TestAlg
alg = CP__MuonEfficiencyCorrections_TestAlg("EffiTestAlg")
alg.PileupReweightingTool = GetPRWTool()
alg.DefaultRelease="cSummer2018"
alg.ValidationRelease="cMoriond2019"

WPs = [
         # reconstruction WPs
       # "LowPt",
       # "Loose", 
       # "Medium", 
       # "Tight", 
       # "HighPt",
         # track-to-vertex-association WPs
       #  "TTVA",
         # BadMuon veto SFs
        # "BadMuonVeto_HighPt",
         # isolation WPs
        "FCLoose_FixedRadIso",
        "FCTightIso",
        "FCTightTrackOnlyIso",        
        "FCTightTrackOnly_FixedRadIso",
        "FCTight_FixedRadIso",
        "FixedCutHighPtTrackOnlyIso",
        "FixedCutPflowLooseIso",
        "FixedCutPflowTightIso",
     
        ]

for WP in WPs: 
  #  alg.EfficiencyTools += [GetMuonEfficiencyTool(WP, Release = "180905_TriggerUpdate", useLowPtMap = False )]
    #alg.EfficiencyToolsForComparison += [GetMuonEfficiencyTool(WP, Release="Moriond_2019", CustomInput = "/ptmp/mpp/junggjo9/ClusterTP/SFFiles/Moriond_2019_HybridID_noSC/", )]
    alg.EfficiencyTools += [GetMuonEfficiencyTool(WP, Release="Moriond_2019", CustomInput = "/ptmp/mpp/junggjo9/ClusterTP/SFFiles/Moriond_2019_HybridID_noSC/", )]

theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
#theApp.EvtMax = 200

#ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.defaultLimit = 100
