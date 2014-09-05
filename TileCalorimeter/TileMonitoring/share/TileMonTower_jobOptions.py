#
# $Id: TileMonTower_jobOptions.py,v 1.1 2008-07-14 10:58:46 lfiorini Exp $
#
#
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloRec.CaloRecConf import CaloTowerAlgorithm
topSequence += CaloTowerAlgorithm("TileTowerBldr")
TileTowerBldr =  topSequence.TileTowerBldr
TileTowerBldr.TowerContainerName="TileTower"
TileTowerBldr.NumberOfPhiTowers=64
TileTowerBldr.NumberOfEtaTowers=100
TileTowerBldr.EtaMin=-5.0
TileTowerBldr.EtaMax=5.0
from TileRecUtils.TileRecUtilsConf import TileTowerBuilderTool
TileCmbTwrBldr = TileTowerBuilderTool( name = 'TileCmbTwrBldr',
                                       CellContainerName = "AllCalo",
                                       DumpTowers        = False,
                                       DumpWeightMap     = False);
TileTowerBldr +=  TileCmbTwrBldr
TileTowerBldr.TowerBuilderTools = [ TileCmbTwrBldr ]


