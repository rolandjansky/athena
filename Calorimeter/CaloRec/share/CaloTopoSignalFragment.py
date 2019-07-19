######################################
## Create standard 0.1 x 0.1 towers ##
######################################


from AthenaCommon.Logging import logging
mlog = logging.getLogger('CaloTopoSognalFragment.py:: ')

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from CaloRec.MakeClustersFromTowers import ClustersFromTowersDict, MakeClustersFromTowers
from CaloRec.CaloRecConf            import CaloTowerGeometrySvc, CaloTopoClusterTowerMerger

mlog.info(' ')
mlog.info('##################################')
mlog.info('## Topological Signal Formation ##')
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

caloTowerDict = ClustersFromTowersDict(clusterBuilderName='CaloFwdTopoTowerBuilder',
                                       towerGeometrySvc=svcMgr.CaloTowerGeometryProvider,
                                       cellContainerKey='AllCalo',
                                       buildTopoTowers=True,
                                       topoClusterContainerKey='CaloCalTopoClusters',
                                       cellClusterWeightKey='CaloCalFwdTopoTowerCellWeights',
                                       orderClusterByPt=False,
                                       applyCellEnergyThreshold=False,
                                       doCellIndexCheck=False,
                                       cellEnergyThreshold=0.,
                                       applyLCW=True,
                                       buildCombinedSignal=True,
                                       clusterRange=2.5)

caloTowerAlgo = MakeClustersFromTowers(towerMakerName      = 'CaloFwdTopoTowerMaker',
                                       towerContainerKey = 'CaloCalFwdTopoTowers',
                                       configDict          = caloTowerDict,
                                       debugOn             = False)
#merging
caloTowerMerger                         = CaloTopoClusterTowerMerger("CaloTopoSignalMaker")
caloTowerMerger.TopoClusterRange        = caloTowerAlgo.CaloFwdTopoTowerBuilder.TopoClusterRange
caloTowerMerger.TopoClusterContainerKey = caloTowerAlgo.CaloFwdTopoTowerBuilder.CaloTopoClusterContainerKey
caloTowerMerger.TopoTowerContainerKey   = caloTowerAlgo.ClustersOutputName
caloTowerMerger.TopoSignalContainerKey  = 'CaloCalTopoSignals'
caloTowerMerger.OutputLevel             = Lvl.DEBUG

topSequence+=caloTowerAlgo
topSequence+=caloTowerMerger
