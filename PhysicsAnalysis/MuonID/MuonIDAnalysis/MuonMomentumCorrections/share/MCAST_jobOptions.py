import sys
# Set up the file reading:
fileName = '/afs/cern.ch/work/g/gartoni/work/FilesDiLepton/mc15_13TeV.301223.Pythia8EvtGen_A14NNPDF23LO_Zprime_NoInt_mumu_E6Chi5000.merge.DAOD_EXOT0.e3795_s2608_s2183_r6630_r6264_p2353_tid05541666_00/DAOD_EXOT0.05541666._000001.pool.root.1'
print 'File Name: ', fileName

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ fileName ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__TestMCASTTool
alg = CP__TestMCASTTool()
#alg.MuonCalibrationAndSmearingTool.Release = 'PreRecs'
alg.MuonCalibrationAndSmearingTool.OutputLevel = WARNING #VERBOSE 
alg.MuonCalibrationAndSmearingTool.ToroidOff = False 
alg.OutputLevel = VERBOSE 
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10 
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
