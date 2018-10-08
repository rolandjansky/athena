#
# $Id: CaloClusterTopoGetter.py,v 1.10 2009-05-19 09:41:18 menke Exp $
#
# File: CaloRec/python/CaloClusterTopoGetter.py
# Created: September 2008, S.Menke
# Purpose: Define default calibrated topo cluster algo and corrections
#
# Modified: May 4, 2009, P.Loch
# Purpose: added H1-style cell calibration to TopoClusters (3rd signal state)
#
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from RecExConfig.ObjKeyStore import objKeyStore
from AthenaCommon.JobProperties import jobproperties as jp
import traceback

#from CaloUtils.CaloUtilsConf import H1ClusterCellWeightTool, EMFracClusterClassificationTool, OutOfClusterCorrectionTool, DeadMaterialCorrectionTool2
#from CaloUtils.CaloUtilsConf import CaloLCClassificationTool, CaloLCWeightTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool

#from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
#>> new PL May 4, 2009
#from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterCellWeightCalib
#<<

from CaloRec.CaloRecConf import CaloTopoClusterFromTowerMaker, CaloClusterMomentsMaker, CaloClusterMaker #, CaloClusterLockVars, CaloClusterPrinter
from CaloRec import CaloRecFlags
from CaloRec.CaloTopoClusterFlags import jobproperties          #### jobproperties is a like a singleton.
from CaloRec.CaloTopoClusterFromTowerFlags import jobproperties
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += theCaloNoiseTool

class CaloClusterTopoFromTowerGetter ( Configured )  :
    mlog = logging.getLogger('CaloClusterTopoFromTowerGetter::instance :' )
    # default value dict
    _defaults       = { 'OutputType'     : 'CaloClusterContainer',                                  ##########
                        'OutputAuxType'  : 'CaloClusterAuxContainer',                               ## output container types
                        'OutputLinkType' : 'CaloClusterCellLinkContainer',                          ##########
                        }
    # output specs
    _outputType     = _defaults['OutputType']
    _outputAuxType  = _defaults['OutputAuxType']
    _outputLinkType = _defaults['OutputLinkType']
    # pre-defined settings
    _finesettings   = { } 
    _coarsesettings = { } #_outputType : ' ', _outputAuxType : ' ', _outputLinkType : ' ' }
    _finesettings[_outputType]       = 'TowerClusterFine'
    _finesettings[_outputAuxType]    = 'TowerClusterFineAux.'
    _finesettings[_outputLinkType]   = 'TowerClusterFine_links'
    _coarsesettings[_outputType]     = 'TowerClusterCoarse'
    _coarsesettings[_outputAuxType]  = 'TowerClusterCoarseAux.'
    _coarsesettings[_outputLinkType] = 'TowerClusterCoarse_links'
    # dynamic settings
    _status = { 'Initialize' : True }
    jobproperties.CaloTopoClusterFromTowerFlags.print_JobProperties()
    if jobproperties.CaloTopoClusterFromTowerFlags.towerGrid() == 'Coarse':
        _output = _coarsesettings
        mlog.info( 'configured for tower grid <{0}>'.format(jobproperties.CaloTopoClusterFromTowerFlags.towerGrid.get_Value()))
    elif jobproperties.CaloTopoClusterFromTowerFlags.towerGrid() == 'Fine':
        _output = _finesettings
        mlog.info( 'configured for tower grid <{0}>'.format(jobproperties.CaloTopoClusterFromTowerFlags.towerGrid.get_Value()))
    else:
        mlog.error( 'invalid tower grid spec {0}'.format(jobproperties.CaloTopoClusterFromTowerFlags.towerGrid.get_Value()))
        mlog.info('%s',_output)
        _status['Initialize'] = False
    # processing status
    if _status['Initialize']:
        mlog.info('configuration module instantiated successfully')
    else:
        mlog.error('failed to instantiate configuration module')
    mlog.info('done with creating object')

    def configure(self):

        mlog = logging.getLogger( 'CaloClusterTopoFromTowerGetter::configure :' )
        mlog.info ('scheduled to output %s',self._output)

        if not self._status['Initialize']: 
            mlog.error('module is disabled due to initialization problems')
            return False

        # get handle to upstream object
        theCaloCellGetter = self.getInputGetter(jp.CaloRecFlags.clusterCellGetterName())
        cellContKey       = theCaloCellGetter.outputKey()

        # configure cluster maker
        TopoBuilder       = CaloTopoClusterFromTowerMaker(jobproperties.CaloTopoClusterFromTowerFlags.towerConverterName.get_Value(),
                                                          CaloTowerContainerKey=jobproperties.CaloTopoClusterFromTowerFlags.inputTowerContainerKey.get_Value(),
                                                          CaloCellContainerKey=cellContKey,
                                                          OrderClusterByPt=jobproperties.CaloTopoClusterFromTowerFlags.orderByPt.get_Value())

        # moment makers
        TopoMoments = CaloClusterMomentsMaker ("TopoMoments")
        TopoMoments.MaxAxisAngle = 20*deg
        TopoMoments.CaloNoiseTool = theCaloNoiseTool
        TopoMoments.UsePileUpNoise = True
        TopoMoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
        TopoMoments.MinBadLArQuality = 4000
        TopoMoments.MomentsNames = ["AVG_LAR_Q"
                                    ,"AVG_TILE_Q"
                                    ,"BAD_CELLS_CORR_E"
                                    ,"BADLARQ_FRAC"
                                    ,"CELL_SIGNIFICANCE"
                                    ,"CELL_SIG_SAMPLING"
                                    ,"CENTER_LAMBDA"
                                    ,"CENTER_MAG"
                                    ,"CENTER_X"
                                    ,"CENTER_Y"
                                    ,"CENTER_Z"
                                    ,"DELTA_ALPHA" 
                                    ,"DELTA_PHI"
                                    ,"DELTA_THETA"
                                    ,"ENG_BAD_CELLS"
                                    ,"ENG_FRAC_CORE" 
                                    ,"ENG_FRAC_EM" 
                                    ,"ENG_FRAC_MAX" 
                                    ,"ENG_POS"
                                    ,"FIRST_ENG_DENS" 
                                    ,"FIRST_ETA"
                                    ,"FIRST_PHI" 
                                    ,"ISOLATION"
                                    ,"LATERAL"
                                    ,"LONGITUDINAL"
                                    ,"MASS"
                                    ,"N_BAD_CELLS"
                                    ,"N_BAD_CELLS_CORR"
                                    ,"PTD"
                                    ,"SECOND_ENG_DENS" 
                                    ,"SECOND_LAMBDA"
                                    ,"SECOND_R" 
                                    ,"SIGNIFICANCE"]

        # only add HV related moments if it is offline.
        from IOVDbSvc.CondDB import conddb
        if not conddb.isOnline:
            from LArRecUtils.LArHVScaleRetrieverDefault import LArHVScaleRetrieverDefault
            TopoMoments.LArHVScaleRetriever=LArHVScaleRetrieverDefault()
            TopoMoments.MomentsNames += ["ENG_BAD_HV_CELLS"
                                         ,"N_BAD_HV_CELLS"
                                         ]

        # cluster maker
        CaloTopoCluster = CaloClusterMaker(jobproperties.CaloTopoClusterFromTowerFlags.clusterMakerName.get_Value())
        mlog.info('instantiated CaloClusterMaker "{0}"'.format(CaloTopoCluster.name()))
        CaloTopoCluster.ClustersOutputName = self._output[self._outputType]
        CaloTopoCluster.ClusterMakerTools  = [ TopoBuilder ]

        # bad cell corrections          
        from CaloClusterCorrection.CaloClusterBadChannelListCorr import CaloClusterBadChannelListCorr
        BadChannelListCorr = CaloClusterBadChannelListCorr()

        # Correction tools
        CaloTopoCluster.ClusterCorrectionTools += [BadChannelListCorr]
        CaloTopoCluster.ClusterCorrectionTools += [TopoMoments]

        # configuring the algorithm
        CaloTopoCluster += TopoBuilder
        CaloTopoCluster += BadChannelListCorr
        CaloTopoCluster += TopoMoments

        objKeyStore.addManyTypesTransient(self.output())
        mlog.info('add output %s',self.output())
        # only write main object in AOD 
        # 2014-01-15 W.L. Remove objs from output streams  b/c of xAOD migration
        #objKeyStore.addStreamESD(self.outputType(),self.outputKey())
        #objKeyStore.addStreamESD("CaloShowerContainer",self.outputKey()+"_Data")
        #objKeyStore.addStreamESD("CaloCellLinkContainer",self.outputKey()+"_Link")
        #objKeyStore.addStreamAOD(self.outputType(),self.outputKey())

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += CaloTopoCluster

        self._handle = CaloTopoCluster

        return True
 
    def handle(self):
        return self._handle

    def output(self):
        return self._output

    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType
                                                       


                                                            
