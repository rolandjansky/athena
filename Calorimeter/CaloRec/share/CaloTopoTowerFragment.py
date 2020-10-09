######################################
## Create standard 0.1 x 0.1 towers ##
######################################


from AthenaCommon.Logging import logging
mlog = logging.getLogger('CaloTopoTowerFragment.py:: ')

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from CaloRec.MakeTowersFromClusters import TowersFromClustersDict, MakeTowersFromClusters
from CaloRec.CaloRecConf            import CaloTowerGeometrySvc

mlog.info(' ')
mlog.info('##################################')
mlog.info('## Standard Tower Configuration ##')
mlog.info('##################################')
mlog.info(' ')

#############################
## Tower Geometry Provider ##
#############################

if not hasattr(svcMgr,'CaloTowerGeometryProvider'):
    mlog.info("setting up tower geometry provider")
    caloTowerGeoSvc              = CaloTowerGeometrySvc('CaloTowerGeometryProvider')
    caloTowerGeoSvc.TowerEtaBins = 100
    caloTowerGeoSvc.TowerEtaMin  = -5.
    caloTowerGeoSvc.TowerEtaMax  =  5.
    svcMgr                      += caloTowerGeoSvc

#############################
## CaloTopoTower Formation ##
#############################

caloTowerDict = TowersFromClustersDict(clusterBuilderName='CaloTopoTowerBuilder',
                                       towerGeometrySvc=svcMgr.CaloTowerGeometryProvider,
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
