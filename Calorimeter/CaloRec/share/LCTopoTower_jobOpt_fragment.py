# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This example demonstrates how to schedule the algorithms to build LCTopoTower as CaloCluster objects
# 

################
## CaloTowers ##
################

from CaloRec.CaloRecConf import CaloTowerxAODFromCells

from AthenaCommon.SystemOfUnits import deg, GeV, MeV

towersCoarseIncl = CaloTowerxAODFromCells("CombinedInclCoarseTowerMaker",CaloTowerContainer="CmbTowerInclCoarse",registerLinks=True)
towersFineIncl   = CaloTowerxAODFromCells("CombinedInclFineTowerMaker",CaloTowerContainer="CmbTowerInclFine",registerLinks=True,nEta=200,nPhi=128);

topSequence   += towersCoarseIncl
topSequence   += towersFineIncl

################################
## Tower-to-cluster Converter ##
################################

from CaloRec.MakeClustersFromTowers import ClustersFromTowersDict, MakeClustersFromTowers

mlog.info(' ')
mlog.info('#############################################################')
mlog.info('## Configure inclusive TopoCluster from fine grid towers   ##')
mlog.info('#############################################################')
mlog.info(' ')

fineInclTowerDict = ClustersFromTowersDict(clusterBuilderName = 'FineInclTowerToClusterConverter',
                                           towerBuilderAlgo   = towersFineIncl)
fineincltowers    = MakeClustersFromTowers(clusterMakerName    = 'FineInclTowerClusterMaker',
                                           clusterContainerKey = 'EMTowerClusterFine',
                                           configDict          = fineInclTowerDict)

mlog.info(' ')
mlog.info('#############################################################')
mlog.info('## Configure inclusive TopoCluster from coarse grid towers ##')
mlog.info('#############################################################')
mlog.info(' ')

coarseInclTowerDict = ClustersFromTowersDict(clusterBuilderName = 'CoarseInclTowerToClusterConverter',
                                             towerBuilderAlgo   = towersCoarseIncl)
coarseincltowers    = MakeClustersFromTowers(clusterMakerName    = 'CoarseInclTowerClusterMaker',
                                             clusterContainerKey = 'EMTowerClusterCoarse',
                                             configDict          = coarseInclTowerDict)

mlog.info(' ')
mlog.info('################################################')
mlog.info('## Configure TopoTowers from fine grid towers ##')
mlog.info('################################################')
mlog.info(' ')

fineTopoTowerDict = ClustersFromTowersDict(clusterBuilderName   = 'FineTowerToTopoClusterConverter',
                                           towerBuilderAlgo     = towersFineIncl)
fineTopoTowerDict['CaloTopoClusterContainerKey'] = 'CaloCalTopoClusters'
fineTopoTowerDict['CellClusterWeightKey']    = 'CaloCellWeightsFine'
finetopotowers    = MakeClustersFromTowers(clusterMakerName     = 'FineTopoTowerClusterMaker',
                                           clusterContainerKey  = 'LCWTowerTopoClusterFine',
                                           configDict           = fineTopoTowerDict)

mlog.info(' ')
mlog.info('##################################################')
mlog.info('## Configure TopoTowers from coarse grid towers ##')
mlog.info('##################################################')
mlog.info(' ')

coarseTopoTowerDict = ClustersFromTowersDict(clusterBuilderName   = 'CoarseTowerToTopoClusterConverter',
                                             towerBuilderAlgo     = towersCoarseIncl)
coarseTopoTowerDict['CaloTopoClusterContainerKey'] = 'CaloCalTopoClusters'
coarseTopoTowerDict['CellClusterWeightKey']    = 'CaloCellWeightsCoarse'
coarsetopotowers    = MakeClustersFromTowers(clusterMakerName     = 'CoarseTopoTowerClusterMaker',
                                             clusterContainerKey  = 'LCWTowerTopoClusterCoarse',
                                             configDict           = coarseTopoTowerDict)

# schedule the tower builders
#topSequence += fineincltowers
#topSequence += coarseincltowers
topSequence += finetopotowers
topSequence += coarsetopotowers
