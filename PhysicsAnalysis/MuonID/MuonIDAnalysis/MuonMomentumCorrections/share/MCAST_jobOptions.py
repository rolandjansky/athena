# Set up the file reading:
fileName = '/afs/cern.ch/work/g/gartoni/work/Files/test_files/test/AOD.01604396._000001.pool.root.1'

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ fileName ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__TestMCASTTool
alg = CP__TestMCASTTool()
alg.MuonCalibrationAndSmearingTool.OutputLevel = DEBUG
alg.OutputLevel = DEBUG 
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 100 
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
