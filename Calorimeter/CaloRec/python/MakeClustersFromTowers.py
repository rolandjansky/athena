# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from CaloRec.CaloRecConf import CaloTopoClusterFromTowerMaker
from CaloRec.CaloRecConf import CaloTowerGeometrySvc

from AthenaCommon.Logging import logging

import AthenaCommon.Constants as Lvl

def ClustersFromTowersDict(clusterBuilderName='TowerFromClusterTool',
                           towerGeometrySvc=CaloTowerGeometrySvc('CaloTowerGeometrySvc'),
                           cellContainerKey='AllCalo',
                           buildTopoTowers=True,
                           topoClusterContainerKey='CaloTopoCluster',
                           cellClusterWeightKey='CaloCellClusterWeightKey',
                           orderClusterByPt=False,
                           applyCellEnergyThreshold=False,
                           cellEnergyThreshold=0.,
                           applyLCW=False):
    ''' Configuration dictionary for tower-to-cluster converter 
    '''
    configDict = { 'ClusterBuilderName'          : clusterBuilderName,         ### name of the tower builder tool
                   'CaloTowerGeometrySvc'        : towerGeometrySvc,           ### tower geometry provider
                   'CaloCellContainerKey'        : cellContainerKey,           ### (input)   cell container key
                   'CaloTopoClusterContainerKey' : topoClusterContainerKey,    ### (input)   topo-cluster container key
                   'CellClusterWeightKey'        : cellClusterWeightKey,       ### (output)  cell weights in topo-clusters
                   'BuildTopoTowers'             : buildTopoTowers,            ### (control) form topo-towers
                   'OrderClusterByPt'            : orderClusterByPt,           ### (control) order final clusters by Pt
                   'ApplyCellEnergyThreshold'    : applyCellEnergyThreshold,   ### (control) apply energy thresholds to cells
                   'CellEnergyThreshold'         : cellEnergyThreshold,        ### (control) value of energy threshold
                   'PrepareLCW'                  : applyLCW                    ### (control) prepare (and apply) LCW
                   }
    return configDict

def MakeClustersFromTowers(clusterMakerName='TowerBuilderAlg',                 ### name of tower builder algorithm
                           clusterContainerKey='CaloTowerTopoCluster',         ### output container key
                           configDict=ClustersFromTowersDict(),
                           debugOn=False):
    ''' This function generates an instance of a cluster algorithm configuration producting clusters trom towers with or without moments. 
    '''
    mlog = logging.getLogger('MakeClustersFromTowers.py:: ')
    mlog.info('ClusterMakerName    = "'+clusterMakerName+'"')
    mlog.info('ClusterContainerKey = <'+clusterContainerKey+'>')

    ''' collect properties from dictionary
    ''' 
    mlog.info('Converter properties: ',configDict)
    toolname       = configDict['ClusterBuilderName']     ### name of the tower builder tool
    cellkey        = configDict['CaloCellContainerKey']   ### cell container key
    buildtopotower = configDict['BuildTopoTowers']        ### controls if topo-towers or inclusive towers are built
    towergeosvc    = configDict['CaloTowerGeometrySvc']   ### tower geometry provider 
    if ( buildtopotower ):
        topoclusterkey = configDict['CaloTopoClusterContainerKey']
    else:
        topoclusterkey = 'N/A'

    cellweightkey  = configDict['CellClusterWeightKey']

    mlog.info('(input) CaloCellContainer        <'+cellkey+'>')
    mlog.info('(input) CaloTopoClusterContainer <'+topoclusterkey+'>')
    mlog.info('(input) CellClusterWeightKey     <'+cellweightkey+'>')

    ''' other configuration control
    '''
    ptordered  = configDict['OrderClusterByPt']
    doLCW      = configDict['PrepareLCW']

    ''' Tower converter configuration
    '''
    mlog.info(' ')
    if doLCW:
        mlog.info('################################################')
        mlog.info('## Produce LCW calibrated topo-tower clusters ##')
        mlog.info('################################################')
        mlog.info('CaloTopoClusterContainerKey .. {0}'.format(topoclusterkey))
        mlog.info('CellClusterWeightKey ......... {0}'.format(cellweightkey))
        towerConverter                             = CaloTopoClusterFromTowerMaker(toolname,CaloCellContainerKey=cellkey,OrderClusterByPt=False) ### order by pt after LCW calibration!
        towerConverter.CaloTowerGeometrySvc        = towergeosvc
        towerConverter.CaloTopoClusterContainerKey = topoclusterkey 
        towerConverter.CellClusterWeightKey        = cellweightkey
        towerConverter.PrepareLCW                  = True

    else:
        mlog.info('###############################################')
        mlog.info('## Produce EM calibrated topo-tower clusters ##')
        mlog.info('###############################################')
        towerConverter                      = CaloTopoClusterFromTowerMaker(toolname,CaloCellContainerKey=cellkey,OrderClusterByPt=ptorder)
        towerConverter.CaloTowerGeometrySvc = towergeosvc
        towerConverter.PrepareLCW           = False
 
    mlog.info(' ')
    if configDict['ApplyCellEnergyThreshold']:
        towerConverter.CellEnergyThreshold = configDict['CellEnergyThreshold']

    import AthenaCommon.Constants as Lvl

    if debugOn:
        towerConverter.OutputLevel  = Lvl.DEBUG

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
    clusterMaker                   += towerConverter
    mlog.info('instantiated CaloClusterMaker "{0}"'.format(clusterMaker.name()))

    # bad cell corrections          
    from CaloClusterCorrection.CaloClusterBadChannelListCorr import CaloClusterBadChannelListCorr
    badChannelCorr = CaloClusterBadChannelListCorr()

    # Correction tools
    clusterMaker.ClusterCorrectionTools += [ badChannelCorr ]
    clusterMaker.ClusterCorrectionTools += [ clusterMoments ]
    clusterMaker                        += clusterMoments

    if doLCW:
        from CaloRec.CaloRecConf import CaloTopoClusterFromTowerCalibrator
        calgname = clusterMakerName+'Calibrator'
        mlog.info('TopoTowers: add LCW calibration tool <'+calgname+'>')
        towerCalibrator = CaloTopoClusterFromTowerCalibrator(calgname)
        mlog.info('TopoTowers: '+calgname+'.CellClusterWeightKey = "'+cellweightkey+'"')
        towerCalibrator.CellClusterWeightKey     = cellweightkey
        towerCalibrator.OrderClusterByPt         = ptordered
        if debugOn:
            towerCalibrator.OutputLevel = Lvl.DEBUG
        clusterMaker.ClusterCorrectionTools     += [ towerCalibrator ]
        clusterMaker                            += towerCalibrator


    # done
    return clusterMaker
