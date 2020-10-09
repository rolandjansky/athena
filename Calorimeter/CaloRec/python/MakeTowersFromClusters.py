# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from CaloRec.CaloRecConf import CaloTopoTowerFromClusterMaker
from CaloRec.CaloRecConf import CaloTowerGeometrySvc

from   AthenaCommon.Logging import logging
import AthenaCommon.Constants as Lvl
#from AthenaCommon.GlobalFlags import globalflags

####################################
## Tower configuration dictionary ##
####################################

def TowersFromClustersDict(clusterBuilderName       = 'TowerFromClusterTool',
                           towerGeometrySvc         = CaloTowerGeometrySvc('CaloTowerGeometrySvc'),
                           cellContainerKey         = 'AllCalo',
                           buildTopoTowers          = True,
                           topoClusterContainerKey  = 'CaloTopoCluster',
                           orderClusterByPt         = False,
                           applyCellEnergyThreshold = False,
                           doCellIndexCheck         = False,
                           cellEnergyThreshold      = 0.,
                           applyLCW                 = False,
                           buildCombinedSignal      = False,
                           clusterRange             = 5.):
    ''' Configuration dictionary for tower-to-cluster converter 
    '''
    configDict = { 'ClusterBuilderName'          : clusterBuilderName,         ### name of the tower builder tool
                   'CaloTowerGeometrySvc'        : towerGeometrySvc,           ### tower geometry provider
                   'CaloCellContainerKey'        : cellContainerKey,           ### (input)   cell container key
                   'CaloTopoClusterContainerKey' : topoClusterContainerKey,    ### (input)   topo-cluster container key
                   'BuildTopoTowers'             : buildTopoTowers,            ### (control) form topo-towers
                   'OrderClusterByPt'            : orderClusterByPt,           ### (control) order final clusters by Pt
                   'ApplyCellEnergyThreshold'    : applyCellEnergyThreshold,   ### (control) apply energy thresholds to cells
                   'CellEnergyThreshold'         : cellEnergyThreshold,        ### (control) value of energy threshold
                   'PrepareLCW'                  : applyLCW,                   ### (control) prepare (and apply) LCW
                   'DoCellIndexCheck'            : doCellIndexCheck,           ### (control) check cell hash indices
                   'BuildCombinedTopoSignal'     : buildCombinedSignal,        ### (control) build combined topo-cluster/topo-tower container
                   'TopoClusterRange'            : clusterRange,               ### (control) range for topo-cluster in combined mode 
                   }
    return configDict

###################################
## Tower algorithm configuration ##
###################################

def MakeTowersFromClusters(towerMakerName      = 'CaloTowerBuilderAlg',        ### name of tower builder algorithm
                           towerContainerKey   = 'CaloTowerTopoCluster',       ### output container key
                           configDict          = TowersFromClustersDict(),     ### tower builder tool configuration
                           debugOn             = True): #False
    ''' This function generates an instance of a cluster algorithm configuration producting clusters trom towers with or without moments. 
    '''
    mlog = logging.getLogger('MakeTowersFromClusters.py:: ')
    mlog.info('TowerMakerName    = "'+towerMakerName+'"')
    mlog.info('TowerContainerKey = <'+towerContainerKey+'>')

    ########################################
    ## Configuring the tower builder tool ##
    ########################################

    ''' collect properties from dictionary and set correct dependencies
    ''' 
    mlog.info('Converter properties: ',configDict)
    excludedKeys = [ 'ClusterBuilderName' ]
    if configDict['PrepareLCW']: 
        towerBuilder  = CaloTopoTowerFromClusterMaker(configDict['ClusterBuilderName'],OrderClusterByPt=False) ### order by pt after LCW calibration!
        excludedKeys += [ 'OrderClusterByPt' ]
    else:
        towerBuilder  = CaloTopoTowerFromClusterMaker(configDict['ClusterBuilderName'])

    ''' Copy properties from dictionary
    '''
    for key,value in configDict.items():
        if key not in excludedKeys:
            setattr(towerBuilder,key,value)

    ''' Check basic consistency of configuration
    '''
    mlog.info('Consistency check')
    if towerBuilder.PrepareLCW and not towerBuilder.BuildTopoTowers:
        raise RuntimeError('{0}[inconsistent configuration] applying LCW requires to build topo-towers'.format(towerBuilder.name()))
    if towerBuilder.BuildCombinedTopoSignal and not towerBuilder.BuildTopoTowers:
        raise RuntimeError('{0}[inconsistent configuration] building combined topo-cluster/topo-tower signals requires to build topo-towers'.format(towerBuilder.name()))
    if towerBuilder.ApplyCellEnergyThreshold and towerBuilder.BuildTopoTowers:
        raise RuntimeError('{0}[inconsistent configuration] applying cell energy thresholds for topo-towers not yet implemented'.format(towerBuilder.name()))

    ''' Tower converter configuration summary
    '''
    if towerBuilder.BuildTopoTowers:
        if towerBuilder.PrepareLCW:
            ''' LCW topo-towers
            '''
            mlog.info('################################################')
            mlog.info('## Produce LCW calibrated topo-tower clusters ##')
            mlog.info('################################################')
            mlog.info('CaloTopoClusterContainerKey .. {0}'.format(towerBuilder.CaloTopoClusterContainerKey))
            #mlog.info('CellClusterWeightKey ......... {0}'.format(towerBuilder.CellClusterWeightKey))
        else:
            ''' EM topo-towers
            '''
            mlog.info('###############################################')
            mlog.info('## Produce EM calibrated topo-tower clusters ##')
            mlog.info('###############################################')
            mlog.info('CaloTopoClusterContainerKey .. {0}'.format(towerBuilder.CaloTopoClusterContainerKey))

        if towerBuilder.BuildCombinedTopoSignal:
            mlog.info(' ')
            mlog.info('Combined topo-cluster/topo-towermode with y_max = {0}'.format(towerBuilder.TopoClusterRange))
    else:    
        ''' EM towers
        '''
        mlog.info('########################################')
        mlog.info('## Produce EM standard tower clusters ##')
        mlog.info('########################################')

    ''' Set debug flag (not a property in the dictionary)
    '''
    if debugOn:
        towerBuilder.OutputLevel  = Lvl.DEBUG

    towerCoreName = towerMakerName
    if towerCoreName.find('Builder') > 0:
        (towerCoreName.replace('Builder',' ')).rstrip(' ')
    elif towerCoreName.find('Maker') > 0:
        (towerCoreName.replace('Maker',' ')).rstrip(' ')

    ############################
    ## Setting up the moments ##
    ############################
    
    ''' External tools for moment calculation
    '''
    ##from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    #from AthenaCommon.AppMgr import ToolSvc
    #caloNoiseTool  = CaloNoiseToolDefault()
    #ToolSvc       += caloNoiseTool
        
    from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
    CaloNoiseCondAlg ()
    CaloNoiseCondAlg(noisetype="totalNoise") # "electronicNoise","pileupNoise","totalNoise"
    
    ''' Cluster moment maker (all general moments)
    '''
    from CaloRec.CaloTopoClusterFlags import jobproperties
    from AthenaCommon.SystemOfUnits   import deg #, GeV, MeV
    from CaloRec.CaloRecConf          import CaloClusterMomentsMaker
    clusterMoments                  = CaloClusterMomentsMaker (towerMakerName+'MomentMaker')
    clusterMoments.MaxAxisAngle     = 20*deg
    #clusterMoments.CaloNoiseTool    = caloNoiseTool
    #clusterMoments.UsePileUpNoise   = True
    clusterMoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
    clusterMoments.MinBadLArQuality = 4000
    clusterMoments.MomentsNames     = [
        "FIRST_PHI" 
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

    from IOVDbSvc.CondDB import conddb
    if not conddb.isOnline:
        #from LArRecUtils.LArHVScaleRetrieverDefault import LArHVScaleRetrieverDefault
        #clusterMoments.LArHVScaleRetriever = LArHVScaleRetrieverDefault()
        clusterMoments.MomentsNames       += ["ENG_BAD_HV_CELLS","N_BAD_HV_CELLS"]
    
    ###############################################################
    ## Set up the tower builder algorithm - as a cluster builder ##
    ###############################################################

    ''' Basic algorithm properties
    '''
    #from CaloRec.CaloRecConf import CaloClusterMaker
    from CaloRec.CaloRecConf import CaloTopoTowerMaker
    towerMaker                    = CaloTopoTowerMaker(towerMakerName)
    towerMaker.TowersOutputName   = towerContainerKey
    towerMaker.TowerMakerTool     = towerBuilder
    mlog.info('instantiated CaloTopoTowerMaker "{0}" configuration'.format(towerMaker.name()))

    ''' Set up bad cell corrections
    '''
    from CaloClusterCorrection.CaloClusterBadChannelListCorr import CaloClusterBadChannelListCorr
    badChannelCorr = CaloClusterBadChannelListCorr()

    ''' Register correction and moment tools
    '''
    towerMaker.TowerCorrectionTools += [ badChannelCorr ]
    towerMaker.TowerCorrectionTools += [ clusterMoments ]
    towerMaker                      += clusterMoments

    ####################################
    ## Configure LCW calibration tool ##
    ####################################

    if towerBuilder.PrepareLCW:
        from CaloRec.CaloRecConf import CaloTopoTowerFromClusterCalibrator
        ''' Configure name for calibration tool
        '''
        towerCalName    = towerCoreName+'Calibrator'
        towerCalibrator = CaloTopoTowerFromClusterCalibrator(towerCalName)
        mlog.info('add LCW calibration tool <'+towerCalName+'>')
        #mlog.info('TopoTowers: '+towerCalName+'.CellClusterWeightKey = "'+towerBuilder.CellClusterWeightKey+'"')
        #towerCalibrator.CellClusterWeightKey = towerBuilder.CellClusterWeightKey
        towerCalibrator.OrderClusterByPt     = configDict['OrderClusterByPt']
        if debugOn:
            towerCalibrator.OutputLevel = Lvl.DEBUG
        ''' Schedule calibration tool
        '''
        towerMaker.TowerCalibratorTool = towerCalibrator

    #######################
    # Configuration done ##
    #######################

    return towerMaker

##
##    toolname       = configDict['ClusterBuilderName']     ### name of the tower builder tool
##    cellkey        = configDict['CaloCellContainerKey']   ### cell container key
##    buildtopotower = configDict['BuildTopoTowers']        ### controls if topo-towers or inclusive towers are built
##    towergeosvc    = configDict['CaloTowerGeometrySvc']   ### tower geometry provider 
##    if ( buildtopotower ):
##        topoclusterkey = configDict['CaloTopoClusterContainerKey']
##    else:
##        topoclusterkey = 'N/A'
##
##    cellweightkey  = configDict['CellClusterWeightKey']
##
##    mlog.info('(input) CaloCellContainer        <'+cellkey+'>')
##    mlog.info('(input) CaloTopoClusterContainer <'+topoclusterkey+'>')
##    mlog.info('(input) CellClusterWeightKey     <'+cellweightkey+'>')
