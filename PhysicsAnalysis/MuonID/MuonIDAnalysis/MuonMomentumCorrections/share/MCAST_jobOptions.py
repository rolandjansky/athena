# Set up the file reading:
fName = "/afs/cern.ch/atlas/project/PAT/data/xAOD/valid2.117050.PowhegPythia_P2011C_ttbar.digit.AOD.e2657_s1933_s1964_r5493.pool.root"
#fName = "/afs/cern.ch/user/g/gartoni/public/AOD.01466650._000081.pool.root.1"
#fName = "/afs/cern.ch/user/g/gartoni/public/AOD.01466650._000176.pool.root.1"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ fName ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__TestMCASTTool
alg = CP__TestMCASTTool()
alg.MuonCalibrationAndSmearingTool.OutputLevel = DEBUG
#alg.MuonCalibrationAndSmearingTool.Year = 'Data12'
#alg.MuonCalibrationAndSmearingTool.Release = 'Rel17.2Sum13'
#alg.MuonCalibrationAndSmearingTool.FilesPath = '/afs/cern.ch/work/g/gartoni/work/ParallelTest/MuonCalibrationAndSmearing/share/'
alg.OutputLevel = WARNING 
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 3 
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
