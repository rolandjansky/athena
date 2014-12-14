# an example of how to run the MuonEfficiencyCorrections tool in athena

# Set up the file reading:/ptmp/mpp/goblirsc/datasets/xAOD/mc14_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.merge.AOD.e1852_s1896_s1912_r5591_r5625_tid01512429_00/AOD.01512429._000146.pool.root.1

if not "FNAME" in vars() and not "FNAME" in globals():
	FNAME ="/ptmp/mpp/goblirsc/datasets/xAOD/valid2.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2657_s1933_s1964_r5592_tid01495531_00/AOD.01495531._000181.pool.root.1"
	#FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5493.pool.root"
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
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
