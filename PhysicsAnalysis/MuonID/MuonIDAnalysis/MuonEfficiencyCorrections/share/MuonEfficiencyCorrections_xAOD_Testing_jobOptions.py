# a simple testing macro for the MuonEfficiencyCorrections_xAOD package in athena
#
# Usage: athena -c "FNAME='<input file>'" MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py

if not "FNAME" in vars() and not "FNAME" in globals():
	FNAME = "/afs/cern.ch/work/k/kokasaha/public/DAOD_HIGG2D1.05621480._000019.pool.root.1"
	#FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5493.pool.root"
	#FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.devval_rel_5.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

#the ToolHandle constructor should be given "CP::PileupReweightingTool/myTool" as its string argument
ToolSvc += CfgMgr.CP__PileupReweightingTool("MyPRWTool",
                ConfigFiles=["/afs/cern.ch/user/n/nkoehler/public/MCP/share/160425_MEC/pileup_rw.mc15b.root"],
                DefaultChannel=410000,
                DataScaleFactor=1./1.09,
                DataScaleFactorUP=1./1.11, DataScaleFactorDOWN=1./1.07,
                LumiCalcFiles=["/afs/cern.ch/user/n/nkoehler/public/MCP/share/160425_MEC/ilumicalc_histograms_None_276262-284484.root"])

# Add the MCP tool
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
tool = CP__MuonEfficiencyScaleFactors("MyMCPTool")
tool.WorkingPoint = "Loose"
tool.PileupReweightingTool = ToolSvc.MyPRWTool
# tool.CustomInputFolder="/afs/cern.ch/user/n/nkoehler/public/MCP/ScaleFactorFiles/160520/"

# Add an MCP tool for TTVA SFs
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
TTVAtool = CP__MuonEfficiencyScaleFactors("MyMCPTTVATool")
TTVAtool.WorkingPoint = "TTVA"
TTVAtool.PileupReweightingTool = ToolSvc.MyPRWTool
# TTVAtool.CustomInputFolder="/afs/cern.ch/user/n/nkoehler/public/MCP/ScaleFactorFiles/160520/"

# Add Isolaiton tool
isotool = CP__MuonEfficiencyScaleFactors("TestIsolationSF")
isotool.WorkingPoint = "GradientLooseIso"
isotool.PileupReweightingTool = ToolSvc.MyPRWTool
isotool.OutputLevel = VERBOSE
ToolSvc += isotool

from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonTriggerScaleFactors
trigsftool = CP__MuonTriggerScaleFactors("TriggerSFTool");
trigsftool.MuonQuality = "Medium"
# trigsftool.CustomInputFolder="/afs/cern.ch/atlas/project/muon/mcp/ScaleFactorFiles/Data15_allPeriods_161115/"

# Add the test algorithm:
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyCorrections_TestAlg
alg = CP__MuonEfficiencyCorrections_TestAlg()
alg.ScaleFactorTool = tool
alg.IsolationScaleFactorTool = isotool
alg.TrigScaleFactorTool = trigsftool
alg.TTVAScaleFactorTool = TTVAtool
alg.OutputLevel = DEBUG
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
