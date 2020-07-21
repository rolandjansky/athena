path = '/data/atlassmallfiles/users/artoni/mcp/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.deriv.DAOD_MUON6.e3601_s3126_r10201_p4029'
files = [os.path.join(path, f) for f in os.listdir(path)]

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = files 

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__TestMCASTTool
alg = CP__TestMCASTTool()
alg.MuonCalibrationAndSmearingTool.OutputLevel = INFO 
alg.MuonCalibrationAndSmearingTool.AddExtraDecorations = True
alg.MuonCalibrationAndSmearingTool.doExtraSmearing = False 
alg.MuonCalibrationAndSmearingTool.do2StationsHighPt = False 
alg.MuonCalibrationAndSmearingTool.SagittaRelease = 'sagittaBiasDataAll_03_02_19_Data17'
alg.MuonCalibrationAndSmearingTool.Year = 'Data17'
alg.MuonCalibrationAndSmearingTool.StatComb = False
alg.MuonCalibrationAndSmearingTool.SagittaCorr = False 
alg.MuonCalibrationAndSmearingTool.doSagittaMCDistortion = True
alg.MuonCalibrationAndSmearingTool.SagittaCorrPhaseSpace = False
alg.OutputLevel = INFO 
alg.Output = 'Output_Setup2.root'
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10 
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
