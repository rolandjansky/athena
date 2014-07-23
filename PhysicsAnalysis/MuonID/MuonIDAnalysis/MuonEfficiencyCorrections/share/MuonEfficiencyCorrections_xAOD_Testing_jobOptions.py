# an example of how to run the MuonEfficiencyCorrections tool in athena

# Set up the file reading:
FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5493.pool.root"
#FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.devval_rel_5.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the MCP tool

from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
tool = CP__MuonEfficiencyScaleFactors("MyMCPTool")
tool.WorkingPoint = "CBandST"
tool.DataPeriod = "2012"

# Add the test algorithm:
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyCorrections_TestAlg
alg = CP__MuonEfficiencyCorrections_TestAlg()
alg.ScaleFactorTool = tool
alg.OutputLevel = DEBUG
theJob += alg



# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 500
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
