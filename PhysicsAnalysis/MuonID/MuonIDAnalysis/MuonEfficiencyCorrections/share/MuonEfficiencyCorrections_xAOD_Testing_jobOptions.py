# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import sys
from MuonEfficiencyCorrections.CommonToolSetup import *


# a simple testing macro for the MuonEfficiencyCorrections_xAOD package in athena
#
# Usage: athena --filesInput <InputFile> MuonEfficiencyCorrections/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py
#  E.g.:  athena  --filesInput '/ptmp/mpp/junggjo9/Datasets/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.deriv.DAOD_MUON1.*/*root.1' MuonEfficiencyCorrections/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py
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
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyCorrections_TestAlg, CP__MuonCloseJetDecorationAlg
theJob += CP__MuonCloseJetDecorationAlg("JetDecorationAlg",
                                        MuonContainer = "CalibratedMuons")

alg = CP__MuonEfficiencyCorrections_TestAlg("EffiTestAlg")
alg.PileupReweightingTool = GetPRWTool()
alg.MuonSelectionTool = GetSelectionTool()
alg.DefaultRelease="cJan_2020"
alg.ValidationRelease="cApr_2020"
alg.SGKey = "CalibratedMuons"
## Select 30 GeV muons for the high-pt WP only
alg.MinPt = 3.e3
alg.MaxEta = 2.5
WPs = [
        # reconstruction WPs
        "LowPt",
        "Loose", 
        "Medium", 
        "Tight", 
        "HighPt", 
        "CaloTag",  
        "HighPt3Layers", 
        "LowPtMVA",    
        # track-to-vertex-association WPs
        "TTVA",
        # BadMuon veto SFs
        "BadMuonVeto_HighPt",            
        # isolation WPs
        "FCLooseIso",
        "FCTightIso",              
        "FCTightTrackOnlyIso",              
        "FCLoose_FixedRadIso",        
        "FCTight_FixedRadIso",
        "FCTightTrackOnly_FixedRadIso",
        "FCTight_FixedRadIso",           
        "FixedCutHighPtTrackOnlyIso",
        "FixedCutPflowLooseIso",            
        "FixedCutPflowTightIso",
       ]
for WP in WPs: 
    alg.EfficiencyTools += [GetMuonEfficiencyTool(WP,
                                                 Release="200202_Precision_r21",
                                                 BreakDownSystematics=False, 
                                                 UncorrelateSystematics=False)]
    alg.EfficiencyToolsForComparison += [GetMuonEfficiencyTool(WP, 
                                                CustomInput = "/ptmp/mpp/junggjo9/Cluster/SFFiles/Feb_2020_iso/",
                                                BreakDownSystematics=False, 
                                                UncorrelateSystematics=False)]

try: ToolSvc.MuonEfficiencyTool_CaloTag.ApplyKinematicSystematic = False
except: pass
try: ToolSvc.MuonEfficiencyTool_CaloTag_200202_Precision_r21.ApplyKinematicSystematic = False
except: pass

theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
#theApp.EvtMax = 200

ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.defaultLimit = 100
