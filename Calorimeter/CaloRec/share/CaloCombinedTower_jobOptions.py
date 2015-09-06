
#
# jobOptions file for Combined Tower Reconstruction
# (needed by jet and combined sliding window)
#

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloRec.CaloRecConf import CaloTowerAlgorithm


# -- switch on some monitoring
if not 'doCaloCombinedTowerMonitoring' in dir():
    doCaloCombinedTowerMonitoring = False

# -- do monitoring
if doCaloCombinedTowerMonitoring:

    from CaloRec.CaloRecConf import CaloTowerMonitor
    topSequence += CaloTowerAlgorithm("CmbTowerBldr")
    topSequence += CaloTowerMonitor("TowerSpy")

else:
    topSequence += CaloTowerAlgorithm("CmbTowerBldr")

# --------------------------------------------------------------
#  Algorithms Private Options
# --------------------------------------------------------------
# tower Maker:
# CmbTowerBldr.OutputLevel = 2
CmbTowerBldr =  topSequence.CmbTowerBldr

CmbTowerBldr.TowerContainerName="CombinedTower"
CmbTowerBldr.NumberOfPhiTowers=64
CmbTowerBldr.NumberOfEtaTowers=100
CmbTowerBldr.EtaMin=-5.0
CmbTowerBldr.EtaMax=5.0


# input to LArTowerBuilder:  cells in LArEM and LARHEC 
from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
LArCmbTwrBldr = LArTowerBuilderTool( name              = 'LArCmbTwrBldr',
                                     CellContainerName = "AllCalo",
                                     IncludedCalos     = [ "LAREM", 
                                                          "LARHEC" ])
#CmbTowerBldr.TowerBuilderTools+=[ LArCmbTwrBldr.getFullName() ]
#CmbTowerBldr +=  LArCmbTwrBldr #for private tools


# input to FCALTowerBuilder : cells in FCAL 
from LArRecUtils.LArRecUtilsConf import LArFCalTowerBuilderTool
FCalCmbTwrBldr = LArFCalTowerBuilderTool( name              = 'FCalCmbTwrBldr',
                                          CellContainerName = "AllCalo",
                                          MinimumEt         = 0.*MeV)
#CmbTowerBldr.TowerBuilderTools+=[ FCalCmbTwrBldr.getFullName() ]
#CmbTowerBldr +=  FCalCmbTwrBldr #for private tools


from TileRecUtils.TileRecUtilsConf import TileTowerBuilderTool
TileCmbTwrBldr = TileTowerBuilderTool( name = 'TileCmbTwrBldr',
                                       CellContainerName = "AllCalo",
                                       DumpTowers        = False,
                                       DumpWeightMap     = False);

#CmbTowerBldr.TowerBuilderTools+=[ TileCmbTwrBldr.getFullName() ]
CmbTowerBldr +=  TileCmbTwrBldr
CmbTowerBldr.TowerBuilderTools+=[ TileCmbTwrBldr ]
#for private tools
#for public tools use instead
#ToolSvc += TileCmbTwrBldr;

