import sys
# Set up the file reading:
fileName = '/data/atlas/atlasdata/artoni/mcp/rel_20_7/valid1.147407.PowhegPythia8_AZNLO_Zmumu.recon.AOD.e3099_s2578_r7534_tid07542054_00/AOD.07542054._000068.pool.root.1'
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
