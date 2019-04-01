######################################
## Create standard 0.1 x 0.1 towers ##
######################################


from AthenaCommon.Logging import logging
mlog = logging.getLogger('CaloTopoTowerFragment.py:: ')

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from CaloRec.MakeClustersFromTowers import ClustersFromTowersDict, MakeClustersFromTowers
from CaloRec.CaloRecConf            import CaloTowerGeometrySvc

mlog.info(' ')
mlog.info('##################################')
mlog.info('## Standard Tower Configuration ##')
mlog.info('##################################')
mlog.info(' ')

#############################
## Tower Geometry Provider ##
#############################

mlog.info("setting up tower geometry provider")
caloTowerGeoSvc  = CaloTowerGeometrySvc('CaloTowerGeometryProvider')
caloTowerGeoSvc.TowerEtaBins = 100
caloTowerGeoSvc.TowerEtaMin  = -5.
caloTowerGeoSvc.TowerEtaMax  =  5.

svcMgr          += caloTowerGeoSvc

#############################
## CaloTopoTower Formation ##
#############################

caloTowerDict = ClustersFromTowersDict(clusterBuilderName='CaloTopoTowerBuilder',
                                       towerGeometrySvc=caloTowerGeoSvc,
                                       cellContainerKey='AllCalo',
                                       buildTopoTowers=True,
                                       topoClusterContainerKey='CaloCalTopoClusters',
                                       cellClusterWeightKey='CaloCalTopoTowerCellWeights',
                                       orderClusterByPt=False,
                                       applyCellEnergyThreshold=False,
                                       doCellIndexCheck=True,
                                       cellEnergyThreshold=0.,
                                       applyLCW=True)

caloTowerAlgo = MakeClustersFromTowers(clusterMakerName    = 'CaloTopoTowerMaker',
                                       clusterContainerKey = 'CaloCalTopoTowers',
                                       configDict          = caloTowerDict,
                                       debugOn             = False)

###################
## Tower Monitor ##
###################

from CaloRec.CaloRecConf import CaloTopoClusterFromTowerMonitor

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["CALOTOWERSTREAM DATAFILE='topotower.root' OPT='RECREATE'" ]

caloTowerMonAlgo = CaloTopoClusterFromTowerMonitor("CaloTopoTowerMonitor")
caloTowerMonAlgo.CaloTowerContainerKey = caloTowerAlgo.ClustersOutputName
caloTowerMonAlgo.CaloTowerGeometrySvc  = caloTowerGeoSvc
caloTowerMonAlgo.RootStreamName        = "CALOTOWERSTREAM"
caloTowerMonAlgo.RootDirName           = "StdTopoTower"
caloTowerMonAlgo.OutputLevel           = Lvl.INFO

topSequence+=caloTowerAlgo
topSequence+=caloTowerMonAlgo
