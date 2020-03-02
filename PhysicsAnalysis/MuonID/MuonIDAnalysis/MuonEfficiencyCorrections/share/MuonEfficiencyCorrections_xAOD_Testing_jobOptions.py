# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import sys
from MuonEfficiencyCorrections.CommonToolSetup import *


# a simple testing macro for the MuonEfficiencyCorrections_xAOD package in athena
#
# Usage: athena --filesInput <InputFile> MuonEfficiencyCorrections/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py
#  E.g.:  athena --filesInput /ptmp/mpp/junggjo9/Datasets/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.deriv.DAOD_MUON1.e3601_s3126_r10724_p3629/DAOD_MUON1.15437076._000069.pool.root.1 MuonEfficiencyCorrections/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py
# Access the algorithm sequence:
AssembleIO("MUONEFFTESTER")
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__CalibratedMuonsProvider, CP__MuonCalibrationPeriodTool
toolName = "MuonCalibrationTool"
calibTool = CP__MuonCalibrationPeriodTool(toolName)
ToolSvc += calibTool

theJob += CP__CalibratedMuonsProvider("CalibratedMuonsProvider",Tool=calibTool, prwTool = GetPRWTool())

# Add the test algorithm:
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyCorrections_TestAlg
alg = CP__MuonEfficiencyCorrections_TestAlg("EffiTestAlg")
alg.PileupReweightingTool = GetPRWTool()
alg.MuonSelectionTool = GetSelectionTool()
alg.DefaultRelease="cFeb_2019"
alg.ValidationRelease="cOct_2019"
alg.SGKey = "CalibratedMuons"
## Select 30 GeV muons for the high-pt WP only
alg.MinPt = 100.e3
alg.MaxEta = 2.5
WPs = [
         # reconstruction WPs
        "LowPt",
        "Loose", 
        "Medium", 
        "Tight", 
        "HighPt", 
        "CaloTag",      
         # track-to-vertex-association WPs
         "TTVA",
         # BadMuon veto SFs
        "BadMuonVeto_HighPt",        
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
    alg.EfficiencyTools += [GetMuonEfficiencyTool(WP, Release="190530_r21")]
    alg.EfficiencyToolsForComparison += [GetMuonEfficiencyTool(WP, 
                                                CustomInput = "/eos/user/j/jojungge/AutumunRecommendations/to_copy/")]
ToolSvc.MuonEfficiencyTool_CaloTag.ApplyKinematicSystematic = False
ToolSvc.MuonEfficiencyTool_CaloTag_190530_r21.ApplyKinematicSystematic = False

### New working points
for WP in["HighPt3Layers", "LowPtMVA"]:    
    alg.EfficiencyTools += [GetMuonEfficiencyTool(WP, 
                                                CustomInput = "/eos/user/j/jojungge/AutumunRecommendations/to_copy/")]
    alg.EfficiencyToolsForComparison += [GetMuonEfficiencyTool(WP, 
                                                CustomInput = "/eos/user/j/jojungge/AutumunRecommendations/to_copy/")]
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
#theApp.EvtMax = 200

#ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.defaultLimit = 100
