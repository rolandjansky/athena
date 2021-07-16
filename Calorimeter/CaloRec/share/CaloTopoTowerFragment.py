######################################
## Create standard 0.1 x 0.1 towers ##
######################################


from AthenaCommon.Logging import logging
mlog = logging.getLogger('CaloTopoTowerFragment.py:: ')

import AthenaCommon.Constants as Lvl
from CaloDetDescr.CaloDetDescrConf  import CaloTowerGeometryCondAlg

from CaloRec.MakeTowersFromClusters import TowersFromClustersDict, MakeTowersFromClusters


mlog.info(' ')
mlog.info('##################################')
mlog.info('## Standard Tower Configuration ##')
mlog.info('##################################')
mlog.info(' ')

#############################
## Tower Geometry Provider ##
#############################
if not hasattr(condSeq,'CaloTowerGeometryCondAlg'):
    mlog.info("setting up tower geometry provider")
    caloTowerGeoAlg              = CaloTowerGeometryCondAlg()
    caloTowerGeoAlg.TowerEtaBins = 100
    caloTowerGeoAlg.TowerEtaMin  = -5.
    caloTowerGeoAlg.TowerEtaMax  =  5.
    condSeq                      += caloTowerGeoAlg

#############################
## CaloTopoTower Formation ##
#############################

caloTowerDict = TowersFromClustersDict(clusterBuilderName='CaloTopoTowerBuilder',
                                       cellContainerKey='AllCalo',
                                       buildTopoTowers=True,
                                       topoClusterContainerKey='CaloCalTopoClusters',
#                                       cellClusterWeightKey='CaloCalTopoTowerCellWeights',
                                       orderClusterByPt=False,
                                       applyCellEnergyThreshold=False,
                                       doCellIndexCheck=False,
                                       cellEnergyThreshold=0.,
                                       applyLCW=True,
                                       buildCombinedSignal=False,
                                       clusterRange=5.)

caloTowerAlgo = MakeTowersFromClusters(towerMakerName      = 'CaloTopoTowerMaker',
                                       towerContainerKey = 'CaloCalTopoTowers',
                                       configDict          = caloTowerDict,
                                       debugOn             = False)

topSequence+=caloTowerAlgo
