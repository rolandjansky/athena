# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CaloRec.CaloRecConf import CaloTowerxAODFromCells
from CaloRec.CaloRecConf import CaloTopoClusterFromTowerMaker

from AthenaCommon.Logging import logging

import AthenaCommon.Constants as Lvl

def ClustersFromTowersDict(clusterBuilderName='TowerFromClusterMaker',
                           towerBuilderAlgo=CaloTowerxAODFromCells('CmbTowerAlgo'),
                           orderedClusterByPt=False,
                           doLCW=False,                      #### don't apply LCW by default 
                           caloTopoClusterKey='NONE',        #### all cells is default
                           caloCellClusterWeightKey='NONE'): #### no cell weight data object needed
    ''' Configuration dictionary for tower-to-cluster converter 
    '''
    configDict = { 'ClusterBuilderName'          : clusterBuilderName,
                   'CaloTowerBuilder'            : towerBuilderAlgo,
                   'OrderClusterByPt'            : orderedClusterByPt,
                   'CaloTopoClusterContainerKey' : caloTopoClusterKey,
                   'CellClusterWeightKey'        : caloCellClusterWeightKey,
                   'ApplyLCW'                    : doLCW
                   }
    return configDict

def MakeClustersFromTowers(clusterMakerName='CaloClusterMaker',clusterContainerKey='TowerTopoCluster',configDict=ClustersFromTowersDict(),applyEnergyThreshold=False,debugOn=False):
    ''' This function generates an instance of a cluster algorithm producting clusters trom towers with or without moments 
    '''
    # collect inputs
    mlog = logging.getLogger('MakeClustersFromTowers.py:: ')
    mlog.info('ClusterMakerName    = "'+clusterMakerName+'"')
    mlog.info('ClusterContainerKey = <'+clusterContainerKey+'>')
    mlog.info('Converter parameters: ',configDict)

    # configure cluster builder
    cnvname  = configDict['ClusterBuilderName']
    twralgo  = configDict['CaloTowerBuilder']
    towerkey = twralgo.CaloTowerContainer
    mlog.info('(input) CaloTowerContainer <'+towerkey+'>')
    cellkey  = 'AllCalo' ### twralgo.InputCellContainer --> this does not work, why?
    mlog.info('(input) CaloCellContainer  <'+cellkey+'>')
    ptorder  = configDict['OrderClusterByPt']
    tcluskey = configDict['CaloTopoClusterContainerKey']
    tcwgtkey = configDict['CellClusterWeightKey'] 
    #### buildtt  = ( tcluskey != 'NONE' and tcwgtkey != 'NONE' )
    buildtt  = configDict['ApplyLCW']
    ''' Configuration module for the tower converter
    '''
    towerConverter = CaloTopoClusterFromTowerMaker(cnvname,CaloTowerContainerKey=towerkey,CaloCellContainerKey=cellkey,OrderClusterByPt=ptorder)
    ''' Refinement of converter configuration
    '''
    mlog.info(' ')
    if buildtt:
        mlog.info('################################################')
        mlog.info('## Produce LCW calibrated topo-tower clusters ##')
        mlog.info('################################################')
        mlog.info('CaloTopoClusterContainerKey .. {0}'.format(tcluskey))
        mlog.info('CellClusterWeightKey ......... {0}'.format(tcwgtkey))
        towerConverter.CaloTopoClusterContainerKey = tcluskey
        towerConverter.CellClusterWeightKey        = tcwgtkey
        towerConverter.ApplyLCW                    = True
    else:
        mlog.info('####################################################')
        mlog.info('## Produce EM calibrated inclusive tower clusters ##')
        mlog.info('####################################################')
    mlog.info(' ')
    if applyEnergyThreshold:
        towerConverter.CellEnergyThreshold = twralgo.CellEnergyThreshold 
    if debugOn:
        towerConverter.OutputLevel = Lvl.DEBUG
    # setting up the moments: external tools
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    caloNoiseTool = CaloNoiseToolDefault()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += caloNoiseTool

    # moment maker
    from CaloRec.CaloTopoClusterFlags import jobproperties
    from AthenaCommon.SystemOfUnits import deg, GeV, MeV
    from CaloRec.CaloRecConf import CaloClusterMomentsMaker
    clusterMoments = CaloClusterMomentsMaker (clusterMakerName+'MomentMaker')
    clusterMoments.MaxAxisAngle = 20*deg
    clusterMoments.CaloNoiseTool = caloNoiseTool
    clusterMoments.UsePileUpNoise = True
    clusterMoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
    clusterMoments.MinBadLArQuality = 4000
    clusterMoments.MomentsNames = ["FIRST_PHI" 
                                   ,"FIRST_ETA"
                                   ,"SECOND_R" 
                                   ,"SECOND_LAMBDA"
                                   ,"DELTA_PHI"
                                   ,"DELTA_THETA"
                                   ,"DELTA_ALPHA" 
                                   ,"CENTER_X"
                                   ,"CENTER_Y"
                                   ,"CENTER_Z"
                                   ,"CENTER_MAG"
                                   ,"CENTER_LAMBDA"
                                   ,"LATERAL"
                                   ,"LONGITUDINAL"
                                   ,"FIRST_ENG_DENS" 
                                   ,"ENG_FRAC_EM" 
                                   ,"ENG_FRAC_MAX" 
                                   ,"ENG_FRAC_CORE" 
                                   ,"SECOND_ENG_DENS" 
                                   ,"ISOLATION"
                                   ,"ENG_BAD_CELLS"
                                   ,"N_BAD_CELLS"
                                   ,"N_BAD_CELLS_CORR"
                                   ,"BAD_CELLS_CORR_E"
                                   ,"BADLARQ_FRAC"
                                   ,"ENG_POS"
                                   ,"SIGNIFICANCE"
                                   ,"CELL_SIGNIFICANCE"
                                   ,"CELL_SIG_SAMPLING"
                                   ,"AVG_LAR_Q"
                                   ,"AVG_TILE_Q"
                                   ,"PTD"
                                   ,"MASS"
                                   ]

    # only add HV related moments if it is offline.
    from IOVDbSvc.CondDB import conddb
    if not conddb.isOnline:
        from LArRecUtils.LArHVScaleRetrieverDefault import LArHVScaleRetrieverDefault
        clusterMoments.LArHVScaleRetriever=LArHVScaleRetrieverDefault()
        clusterMoments.MomentsNames += ["ENG_BAD_HV_CELLS"
                                        ,"N_BAD_HV_CELLS"
                                        ]

    # cluster maker
    from CaloRec.CaloRecConf import CaloClusterMaker
    clusterMaker = CaloClusterMaker(clusterMakerName)
    clusterMaker.ClustersOutputName = clusterContainerKey
    clusterMaker.ClusterMakerTools  = [ towerConverter ]
    mlog.info('instantiated CaloClusterMaker "{0}"'.format(clusterMaker.name()))

    # bad cell corrections          
##    from CaloClusterCorrection.CaloClusterBadChannelListCorr import CaloClusterBadChannelListCorr
##    badChannelCorr = CaloClusterBadChannelListCorr()

    # Correction tools
##    clusterMaker.ClusterCorrectionTools += [ badChannelCorr ]
    clusterMaker.ClusterCorrectionTools += [ clusterMoments ]

    # configuring the algorithm
    clusterMaker += towerConverter
##    clusterMaker += badChannelCorr
    clusterMaker += clusterMoments

    if buildtt:
        from CaloRec.CaloRecConf import CaloTopoClusterFromTowerCalibrator
        calgname = clusterMakerName+'Calibrator'
        mlog.info('TopoTowers: add LCW calibration tool <'+calgname+'>')
        clusterCalibrator = CaloTopoClusterFromTowerCalibrator(calgname)
        mlog.info('TopoTowers: '+calgname+'.CellClusterWeightKey = "'+tcwgtkey+'"')
        clusterCalibrator.CellClusterWeightKey     = tcwgtkey
        clusterCalibrator.OrderClusterByPt         = ptorder
        clusterMaker.ClusterCorrectionTools       += [ clusterCalibrator ]
        clusterMaker                              += clusterCalibrator

    # done
    return clusterMaker
