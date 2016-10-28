# Set up the file reading:


# Set up the file reading:
fileName = '/build1/schaffer/mc14_13TeV.167892.PowhegPythia8_AU2CT10_ggH125_ZZ4lep_noTau.merge.AOD.e3292_s1982_s2008_r5787_r5853_tid04606790_00/AOD.04606790._000004.pool.root.1'

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ fileName ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from ZMassConstraint.ZMassConstraintConf import TestAthenaConstraintFit
alg = TestAthenaConstraintFit(name="TestAthenaConstraintFit", OutputLevel=DEBUG)
theJob += alg

from ZMassConstraint.ZMassConstraintConf import ZMassConstraint__ConstraintFit
ToolSvc += ZMassConstraint__ConstraintFit("ConstraintFit", OutputLevel=DEBUG)

     
from ElectronPhotonFourMomentumCorrection.ElectronPhotonFourMomentumCorrectionConf import CP__EgammaCalibrationAndSmearingTool
ToolSvc += CP__EgammaCalibrationAndSmearingTool("EgammaCalibrationAndSmearingTool", OutputLevel=DEBUG)

from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__MuonCalibrationAndSmearingTool
ToolSvc += CP__MuonCalibrationAndSmearingTool("MuonCalibrationAndSmearingTool", OutputLevel=DEBUG)


# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 100 
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000


