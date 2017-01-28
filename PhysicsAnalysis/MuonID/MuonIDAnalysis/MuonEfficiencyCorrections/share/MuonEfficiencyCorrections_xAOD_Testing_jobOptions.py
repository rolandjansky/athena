import sys
# a simple testing macro for the MuonEfficiencyCorrections_xAOD package in athena
#
# Usage: athena -c "FNAME='<input file>'" MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py

if not "FNAME" in vars() and not "FNAME" in globals():
    print 'Usage: athena -c "FNAME=\'<input file>\'" MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py'
    sys.exit(1)

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

#the ToolHandle constructor should be given "CP::PileupReweightingTool/myTool" as its string argument
PRWTool = CfgMgr.CP__PileupReweightingTool("MyPRWTool",
                DataScaleFactor=1.0/1.09,
                DataScaleFactorUP=1., 
                DataScaleFactorDOWN=1.0/1.18,
                ConfigFiles=["/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"],
                LumiCalcFiles=["/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.root", "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root"])
ToolSvc += PRWTool

# Add the MCP tool
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
tool = CP__MuonEfficiencyScaleFactors("MyMCPTool")
tool.WorkingPoint = "Loose"
# tool.CustomInputFolder="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/MuonEfficiencyCorrections/160928_PostICHEP/"
ToolSvc += tool

# Add an MCP tool for TTVA SFs
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
TTVAtool = CP__MuonEfficiencyScaleFactors("MyMCPTTVATool")
TTVAtool.WorkingPoint = "TTVA"
# TTVAtool.CustomInputFolder="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/MuonEfficiencyCorrections/160928_PostICHEP/"
ToolSvc += TTVAtool

# Add Isolaiton tool
isotool = CP__MuonEfficiencyScaleFactors("TestIsolationSF")
isotool.WorkingPoint = "GradientLooseIso"
isotool.OutputLevel = VERBOSE
# isotool.CustomInputFolder="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/MuonEfficiencyCorrections/160928_PostICHEP/"
ToolSvc += isotool

from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonTriggerScaleFactors
trigsftool = CP__MuonTriggerScaleFactors("TriggerSFTool");
trigsftool.MuonQuality = "Medium"
# trigsftool.CustomInputFolder="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/MuonEfficiencyCorrections/160928_PostICHEP/"
ToolSvc += trigsftool

# Add the test algorithm:
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyCorrections_TestAlg
alg = CP__MuonEfficiencyCorrections_TestAlg()
alg.ScaleFactorTool = tool
alg.IsolationScaleFactorTool = isotool
alg.TrigScaleFactorTool = trigsftool
alg.TTVAScaleFactorTool = TTVAtool
alg.PileupReweightingTool = PRWTool
alg.OutputLevel = DEBUG
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
