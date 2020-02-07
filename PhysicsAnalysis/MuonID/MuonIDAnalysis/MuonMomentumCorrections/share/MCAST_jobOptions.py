
fileName = '/data/atlassmallfiles/users/artoni/mcp/mc16_13TeV.301030.PowhegPythia8EvtGen_AZNLOCTEQ6L1_DYmumu_2000M2250.deriv.DAOD_MUON1.e3649_s3126_r10751_p4062/DAOD_MUON1.20298065._000005.pool.root.1'
print 'File Name: ', fileName

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ fileName ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__TestMCASTTool
alg = CP__TestMCASTTool()
alg.MuonCalibrationAndSmearingTool.FilesPath = '/home/artoni/MuonCombinedPerformance/atlas-mcp/MaintainMuonMomentumCorrections/run/temporary_config_files/'
#alg.MuonCalibrationAndSmearingTool.Release = 'Recs2019_05_30'
#alg.MuonCalibrationAndSmearingTool.OutputLevel = VERBOSE 
alg.OutputLevel = INFO 
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
