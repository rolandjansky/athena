# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import sys
include("MuonEfficiencyCorrections/CommonToolSetup.py")

# a simple testing macro for the MuonEfficiencyCorrections_xAOD package in athena
#
# Usage: athena -c "FNAME='<input file>'" MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py


# Access the algorithm sequence:
AssembleIO("MUONEFFTESTER")
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()





#athena -c "inputFile='/ptmp/mpp/zenon/DirectStau/mc16_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.deriv.DAOD_SUSY3.e3601_s3126_r9364_r9315_p3179/DAOD_SUSY3.11544886._000178.pool.root.1'" MuonEfficiencyCorrections/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py


# Add the test algorithm:
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyCorrections_TestAlg
alg = CP__MuonEfficiencyCorrections_TestAlg("EffiTestAlg")
alg.PileupReweightingTool = GetPRWTool()

WPs = [
    "Loose", "Medium", "Tight" , "TTVA", "HighPt", "BadMuon", 
#    "GradientIso", "LooseIso",
#    "TightIso", "FixedCutLooseIso", "FixedCutTightTrackOnlyIso",
]
for WP in WPs: alg.EfficiencyTools += [GetMuonEfficiencyTool(WP)]
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200

#ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.defaultLimit = 100
