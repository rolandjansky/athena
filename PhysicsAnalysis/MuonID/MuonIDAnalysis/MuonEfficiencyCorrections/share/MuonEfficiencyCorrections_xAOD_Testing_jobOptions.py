# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import sys
include("MuonEfficiencyCorrections/CommonToolSetup.py")

# a simple testing macro for the MuonEfficiencyCorrections_xAOD package in athena
#
# Usage: athena -c "inputFile='<input file>'" MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py

# Access the algorithm sequence:
AssembleIO("MUONEFFTESTER")
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyCorrections_TestAlg
alg = CP__MuonEfficiencyCorrections_TestAlg("EffiTestAlg")
alg.PileupReweightingTool = GetPRWTool()
alg.DefaultRelease="cRecommendationsSep17"
alg.ValidationRelease="cRecommendationsToday"

WPs = [
         # reconstruction WPs
        # "LowPt",
         "Loose", 
         "Medium", 
         "Tight", 
         "HighPt",
         # track-to-vertex-association WPs
         "TTVA",
         # BadMuon veto SFs
       #  "BadMuonVeto_HighPt",
         # isolation WPs
        # "FixedCutLooseIso", "LooseTrackOnlyIso", "LooseIso", "GradientIso", "GradientLooseIso",
        # "FixedCutTightTrackOnlyIso", "FixedCutHighPtTrackOnlyIso", "FixedCutTightIso"
        ]

for WP in WPs: 
    alg.EfficiencyTools += [GetMuonEfficiencyTool(WP)]
    alg.EfficiencyToolsForComparison += [GetMuonEfficiencyTool(WP, Release="MoriondTest", CustomInput = "/afs/cern.ch/user/j/jojungge/public/MCP/ScaleFactorFiles/180214_Moriond21")]
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
#theApp.EvtMax = 200

#ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.defaultLimit = 100
