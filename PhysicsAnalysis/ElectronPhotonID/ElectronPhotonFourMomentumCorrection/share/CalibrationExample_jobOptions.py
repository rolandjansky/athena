# Set up the file reading:
import os
FNAME = os.environ['ASG_TEST_FILE_MC']

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from ElectronPhotonFourMomentumCorrection.ElectronPhotonFourMomentumCorrectionConf import testAthenaEgammaCalibTool
alg = testAthenaEgammaCalibTool()
alg.EgammaCalibrationAndSmearingTool.ESModel = "es2015cPRE"
alg.EgammaCalibrationAndSmearingTool.OutputLevel = DEBUG
theJob += alg
print alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
