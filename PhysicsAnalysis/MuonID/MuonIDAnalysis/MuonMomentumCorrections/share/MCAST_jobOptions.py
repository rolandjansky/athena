path = '/data/atlas/atlasdata/artoni/mcp/tests/mc16_13TeV.301026.PowhegPythia8EvtGen_AZNLOCTEQ6L1_DYmumu_1000M1250.deriv.DAOD_EXOT0.e3649_s3126_r9364_p3654'
files = [os.path.join(path, f) for f in os.listdir(path)]

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = files 

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__TestMCASTTool
alg = CP__TestMCASTTool()
#alg.MuonCalibrationAndSmearingTool.FilesPath = '/home/artoni/MuonCombinedPerformance/atlas-mcp/MaintainMuonMomentumCorrections/run/temporary_config_files/'
#alg.MuonCalibrationAndSmearingTool.Release = 'Recs2019_05_30'
alg.MuonCalibrationAndSmearingTool.OutputLevel = INFO 
alg.MuonCalibrationAndSmearingTool.AddExtraDecorations = True
alg.MuonCalibrationAndSmearingTool.doExtraSmearing = True 
alg.MuonCalibrationAndSmearingTool.do2StationsHighPt = False 
alg.OutputLevel = INFO 
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
