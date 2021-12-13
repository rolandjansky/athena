# This is the job options file to get performance plots for local hadronic
# calibration on single pions


from CaloLocalHadCalib.CaloLocalHadCalibConf import GetLCSinglePionsPerf
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.AlgSequence import AlgSequence
from math import pi

lcPerf = GetLCSinglePionsPerf("LocalHadPerformance")

# collection name to study cluster moments
lcPerf.ClusterBasicCollName = "CaloTopoClusters"

# collections names to study engReco wrt Truth after different correction steps
lcPerf.ClusterCollectionNames = ["CaloTopoClusters", "CaloWTopoCluster", "CaloOOCTopoCluster", "CaloCalTopoClusters"]
#lcPerf.ClusterCollectionNames = ["CaloTopoCluster_EMAll", "CaloWTopoCluster_EMAll", "CaloOOCTopoCluster_EMAll", "CaloCalTopoCluster_EMAll"]
#lcPerf.ClusterCollectionNames = ["CaloTopoCluster_HadAll", "CaloWTopoCluster_HadAll", "CaloOOCTopoCluster_HadAll", "CaloCalTopoCluster_HadAll"]

lcPerf.CalibrationHitContainerNames= ["LArCalibrationHitInactive","LArCalibrationHitActive","TileCalibrationCellHitCnt"]
lcPerf.DMCalibrationHitContainerNames= ["LArCalibrationHitDeadMaterial", "TileCalibrationDMHitCnt"]

lcPerf.DistanceCut = 1.5
lcPerf.doEngRecOverTruth = True
lcPerf.doEngTag = True
lcPerf.doEngRecSpect = True
lcPerf.doEngNoiseClus = True
lcPerf.doClusMoments = True
lcPerf.doRecoEfficiency = True
lcPerf.etamin = 0.0
lcPerf.etamax = 5.0
lcPerf.netabin = 25
lcPerf.phimin = -pi
lcPerf.phimax = pi
lcPerf.nphibin = 1
lcPerf.logenermin = 2.0
lcPerf.logenermax = 6.4
lcPerf.nlogenerbin = 22
lcPerf.isTestbeam = False 
lcPerf.useGoodClus = False
lcPerf.usePionClustersOnly = False
lcPerf.useRecoEfficiency = False


if 'outFileName' in dir():
  lcPerf.OutputFileName = outFileName
else:
  lcPerf.OutputFileName = "LCSinglePionsPerformance.root"

topSequence += lcPerf

