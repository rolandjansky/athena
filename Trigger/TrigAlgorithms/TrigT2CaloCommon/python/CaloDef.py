# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import seqAND, parOR
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLT.CommonSequences.FullScanDefs import caloFSRoI

def setMinimalCaloSetup() :
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
    from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigCaloDataAccessSvc
    svcMgr+=TrigCaloDataAccessSvc()

########################
## ALGORITHMS
# defined as private within this module, so that they can be configured only in functions in this module
########################

def _algoHLTCaloCell(name="HLTCaloCellMaker", inputEDM='', outputEDM='CellsClusters', RoIMode=True) :
   if not inputEDM:
      from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
      inputEDM = mapThresholdToL1RoICollection("FSNOSEED")
   setMinimalCaloSetup()
   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
   algo=HLTCaloCellMaker(name)
   #"HLTCaloCellMaker"
   algo.RoIs=inputEDM
   algo.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
   algo.CellsName=outputEDM
   algo.ExtraInputs+=[  ( 'LArBadChannelCont', 'ConditionStore+LArBadChannel'), ( 'LArMCSym', 'ConditionStore+LArMCSym'), ('LArOnOffIdMapping' , 'ConditionStore+LArOnOffIdMap' ), ('LArFebRodMapping'  , 'ConditionStore+LArFebRodMap' ), ('CaloDetDescrManager', 'ConditionStore+CaloDetDescrManager') ]
   return algo

def _algoHLTHIEventShape(name='HLTEventShapeMaker', inputEDM='CellsClusters', outputEDM='HIEventShape'):
    from HIGlobal.HIGlobalConf import HIEventShapeMaker
    from HIGlobal.HIGlobalConf import HIEventShapeFillerTool

    algo = HIEventShapeMaker(name)
    algo.InputCellKey = inputEDM
    algo.InputTowerKey=""
    algo.NaviTowerKey=""
    algo.OutputContainerKey = outputEDM
    algo.HIEventShapeFillerTool = HIEventShapeFillerTool()

    return algo

def _algoHLTCaloCellCorrector(name='HLTCaloCellCorrector', inputEDM='CellsClusters', outputEDM='CorrectedCellsClusters', eventShape='HIEventShape'):
  from TrigCaloRec.TrigCaloRecConf import HLTCaloCellCorrector

  algo = HLTCaloCellCorrector(name)
  algo.EventShapeCollection = eventShape
  algo.InputCellKey = inputEDM
  algo.OutputCellKey = outputEDM

  return algo

def _algoHLTTopoCluster(inputEDM="CellsClusters", algSuffix="") :
   from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo
   algo = TrigCaloClusterMaker_topo(name="TrigCaloClusterMaker_topo"+algSuffix, doMoments=True, doLC=False, cells=inputEDM)
   from TrigEDMConfig.TriggerEDMRun3 import recordable
   algo.CaloClusters=recordable("HLT_TopoCaloClusters"+algSuffix)
   return algo

def _algoHLTTopoClusterLC(inputEDM="CellsClusters", algSuffix="") :
   from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo
   algo = TrigCaloClusterMaker_topo(name="TrigCaloClusterMaker_topo"+algSuffix, doMoments=True, doLC=True, cells=inputEDM)
   from TrigEDMConfig.TriggerEDMRun3 import recordable
   algo.CaloClusters=recordable("HLT_TopoCaloClusters"+algSuffix)
   return algo



#
# fast calo algorithm (central or forward regions)
#
def _algoL2Egamma(inputEDM="", doRinger=False, ClustersName="HLT_FastCaloEMClusters", RingerKey="HLT_FastCaloRinger", doForward=False, doAllEm=False, doAll=False):
    
    if not inputEDM:
        from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
        # using jet seeds for testing. we should use EM as soon as we have EM seeds into the L1
        inputEDM = mapThresholdToL1RoICollection("EM")
    setMinimalCaloSetup()
    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_ReFastAlgo
    algo=T2CaloEgamma_ReFastAlgo("FastCaloL2EgammaAlg", doRinger=doRinger, RingerKey=RingerKey)
    algo.RoIs=inputEDM
    if doForward:
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_ReFastFWDAlgo
        algo=T2CaloEgamma_ReFastFWDAlgo("FastCaloL2EgammaAlg_FWD", doRinger=doRinger, RingerKey=RingerKey)
        algo.RoIs=inputEDM
    else:
        if ( doAllEm or doAll ) :
          if ( doAllEm ):
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_AllEm
            algo=T2CaloEgamma_AllEm("L2CaloLayersEmFex")
            algo.RoIs=inputEDM
          else : # can only be doAll
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_All
            algo=T2CaloEgamma_All("L2CaloLayersFex")
            algo.RoIs=inputEDM
    algo.ExtraInputs+=[ ( 'LArBadChannelCont', 'ConditionStore+LArBadChannel'), ( 'LArMCSym', 'ConditionStore+LArMCSym'), ('LArOnOffIdMapping' , 'ConditionStore+LArOnOffIdMap' ), ('LArFebRodMapping'  , 'ConditionStore+LArFebRodMap' ), ('CaloDetDescrManager', 'ConditionStore+CaloDetDescrManager') ]
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    algo.ClustersName=recordable(ClustersName)
    return algo

####################################
##### SEQUENCES
####################################


def fastCaloRecoSequence(InViewRoIs, doRinger=False, ClustersName="HLT_FastCaloEMClusters", RingerKey="HLT_FastCaloRinger",doAllEm=False,doAll=False):
    
    fastCaloAlg = _algoL2Egamma(inputEDM=InViewRoIs, doRinger=doRinger, ClustersName=ClustersName, RingerKey=RingerKey, doAllEm=doAllEm, doAll=doAll)
    
    name = 'fastCaloInViewSequence'
    import AthenaCommon.CfgMgr as CfgMgr

    fastCaloVDV = CfgMgr.AthViews__ViewDataVerifier("fastCaloVDV")
    fastCaloVDV.DataObjects = [( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s'%InViewRoIs  )]

    if doAllEm:
        name = 'fastCaloInViewSequenceAllEM'
          
    elif doAll:
        name = 'fastCaloInViewSequenceAll'    

    fastCaloInViewSequence = seqAND( name, [fastCaloVDV, fastCaloAlg] )
    sequenceOut = fastCaloAlg.ClustersName
    return (fastCaloInViewSequence, sequenceOut)



def fastCaloRecoFWDSequence(InViewRoIs, doRinger=False, ClustersName="HLT_FastCaloEMClusters_FWD", RingerKey="HLT_FastCaloRinger_FWD"):
    # create alg
    fastCaloAlg = _algoL2Egamma(inputEDM=InViewRoIs, doRinger=doRinger, ClustersName=ClustersName, RingerKey=RingerKey,
                                doForward=True)
    import AthenaCommon.CfgMgr as CfgMgr
    fastCaloVDV = CfgMgr.AthViews__ViewDataVerifier("fastCaloVDV_FWD")
    fastCaloVDV.DataObjects = [( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+FSJETMETCaloRoI' )]
    fastCaloInViewSequence = seqAND('fastCaloInViewSequence_FWD' , [fastCaloVDV, fastCaloAlg] )
    sequenceOut = fastCaloAlg.ClustersName
    return (fastCaloInViewSequence, sequenceOut)



def fastCaloEVCreator():
    InViewRoIs="EMCaloRoIs"
    fastCaloViewsMaker = CompFactory.EventViewCreatorAlgorithm( "IMfastCalo" )
    fastCaloViewsMaker.ViewFallThrough = True
    fastCaloViewsMaker.RoIsLink = "initialRoI"
    fastCaloViewsMaker.RoITool = CompFactory.ViewCreatorInitialROITool()
    fastCaloViewsMaker.InViewRoIs = InViewRoIs
    fastCaloViewsMaker.Views = "EMCaloViews"
    fastCaloViewsMaker.ViewNodeName = "fastCaloInViewSequence"
    return (fastCaloViewsMaker, InViewRoIs)



def fastCaloEVFWDCreator():
    #InViewRoIs="EMCaloRoIs"
    InViewRoIs = "FSJETMETCaloRoI"
    fastCaloViewsMaker = CompFactory.EventViewCreatorAlgorithm( "IMfastCalo_FWD" )
    fastCaloViewsMaker.ViewFallThrough = True
    fastCaloViewsMaker.RoIsLink = "initialRoI"
    fastCaloViewsMaker.RoITool = CompFactory.ViewCreatorInitialROITool()
    fastCaloViewsMaker.InViewRoIs = InViewRoIs
    fastCaloViewsMaker.Views = "EMCaloViews_FWD"
    fastCaloViewsMaker.ViewNodeName = "fastCaloInViewSequence_FWD"

    return (fastCaloViewsMaker, InViewRoIs)


def fastCalo_All_EVCreator():
    InViewRoIs = "EMCaloRoIs"
    fastCaloViewsMaker = CompFactory.EventViewCreatorAlgorithm( "IM_LArPS_All" )
    fastCaloViewsMaker.ViewFallThrough = True
    fastCaloViewsMaker.RoIsLink = "initialRoI"
    fastCaloViewsMaker.RoITool = CompFactory.ViewCreatorInitialROITool()
    fastCaloViewsMaker.InViewRoIs = InViewRoIs
    fastCaloViewsMaker.Views = "LArPS_All_Views"

    return (fastCaloViewsMaker, InViewRoIs)

def fastCalo_AllEM_EVCreator():
    InViewRoIs = "EMCaloRoIs"
    fastCaloViewsMaker = CompFactory.EventViewCreatorAlgorithm( "IM_LArPS_AllEM" )
    fastCaloViewsMaker.ViewFallThrough = True
    fastCaloViewsMaker.RoIsLink = "initialRoI"
    fastCaloViewsMaker.RoITool = CompFactory.ViewCreatorInitialROITool()
    fastCaloViewsMaker.InViewRoIs = InViewRoIs
    fastCaloViewsMaker.Views = "LArPS_AllEM_Views"

    return (fastCaloViewsMaker, InViewRoIs)

##################################
# standalone testing
###################################

def createFastCaloSequence(EMRoIDecisions, doRinger=False, ClustersName="HLT_FastCaloEMClusters", RingerKey="HLT_FastCaloRinger"):
    """Used for standalone testing"""
    (fastCaloViewsMaker, InViewRoIs) = fastCaloEVCreator()
    # connect to RoIs

    fastCaloViewsMaker.InputMakerInputDecisions  = [ EMRoIDecisions ]
    fastCaloViewsMaker.InputMakerOutputDecisions =  EMRoIDecisions + "IMOUTPUT"

    (fastCaloInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs, doRinger=doRinger, ClustersName=ClustersName, RingerKey=RingerKey)

    fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewSequence ])
    return (fastCaloSequence, sequenceOut)

##################################
# cluster maker functions
###################################

def clusterFSInputMaker( ):
  """Creates the inputMaker for FS in menu"""
  RoIs = caloFSRoI
  from AthenaConfiguration.ComponentFactory import CompFactory
  InputMakerAlg = CompFactory.InputMakerForRoI("IMclusterFS", RoIsLink="initialRoI")
  InputMakerAlg.RoITool = CompFactory.ViewCreatorInitialROITool()
  InputMakerAlg.RoIs=RoIs
  return InputMakerAlg


def HLTCellMaker(ConfigFlags,RoIs=caloFSRoI, outputName="CaloCells", algSuffix=""):
    cellMakerAlgo = _algoHLTCaloCell(name="HLTCaloCellMaker"+algSuffix, inputEDM=RoIs, outputEDM=outputName, RoIMode=True)
    return cellMakerAlgo


def HLTFSCellMakerRecoSequence(ConfigFlags,RoIs=caloFSRoI):
    cellMaker = HLTCellMaker(ConfigFlags, RoIs, outputName="CaloCellsFS", algSuffix="FS")
    RecoSequence = parOR("ClusterRecoSequenceFS", [cellMaker])
    return (RecoSequence, cellMaker.CellsName)


def HLTFSTopoRecoSequence(ConfigFlags,RoIs):
    cellMaker = HLTCellMaker(ConfigFlags, RoIs, outputName="CaloCellsFS", algSuffix="FS")
    topoClusterMaker = _algoHLTTopoCluster(inputEDM = cellMaker.CellsName, algSuffix="FS")
    RecoSequence = parOR("TopoClusterRecoSequenceFS", [cellMaker, topoClusterMaker])
    return (RecoSequence, topoClusterMaker.CaloClusters)


def HLTRoITopoRecoSequence(ConfigFlags, RoIs, algSuffix=''):
    import AthenaCommon.CfgMgr as CfgMgr
    HLTRoITopoRecoSequenceVDV = CfgMgr.AthViews__ViewDataVerifier("HLTRoITopoRecoSequenceVDV%s"%algSuffix)
    HLTRoITopoRecoSequenceVDV.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+PrecisionCaloRoIs%s'%algSuffix ),
                                             ( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' )]

    cellMaker = HLTCellMaker(ConfigFlags, RoIs, algSuffix="RoI%s"%algSuffix)
    topoClusterMaker = _algoHLTTopoCluster(inputEDM = cellMaker.CellsName, algSuffix="RoI%s"%algSuffix)
    RecoSequence = parOR("RoITopoClusterRecoSequence%s"%algSuffix, [HLTRoITopoRecoSequenceVDV, cellMaker, topoClusterMaker])
    return (RecoSequence, topoClusterMaker.CaloClusters)


def HLTHIRoITopoRecoSequence(ConfigFlags, RoIs, algSuffix=''):

    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import  getTrigEgammaKeys
    TrigEgammaKeys = getTrigEgammaKeys()
    eventShape = TrigEgammaKeys.egEventShape

    import AthenaCommon.CfgMgr as CfgMgr
    HLTRoITopoRecoSequenceVDV = CfgMgr.AthViews__ViewDataVerifier("HLTHIRoITopoRecoSequenceVDV")
    HLTRoITopoRecoSequenceVDV.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+PrecisionCaloRoIs' ),
                                             ( 'xAOD::HIEventShapeContainer' , 'StoreGateSvc+' + eventShape ),
                                             ( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' )]

    cellMaker = HLTCellMaker(ConfigFlags, RoIs, algSuffix="HIRoI")
    cellCorrector = _algoHLTCaloCellCorrector(
        name='HLTRoICaloCellCorrector',
        inputEDM=cellMaker.CellsName,
        outputEDM='CorrectedRoICaloCells',
        eventShape=eventShape)

    topoClusterMaker = _algoHLTTopoCluster(inputEDM = cellCorrector.OutputCellKey, algSuffix="HIRoI")
    RecoSequence = parOR("HIRoITopoClusterRecoSequence", [HLTRoITopoRecoSequenceVDV, cellMaker, cellCorrector, topoClusterMaker])
    return (RecoSequence, topoClusterMaker.CaloClusters)


def HLTLCTopoRecoSequence(ConfigFlags, RoIs='InViewRoIs'):
    cellMaker = HLTCellMaker(ConfigFlags, RoIs, outputName="CaloCellsLC", algSuffix="LC")
    cellMaker.TileCellsInROI = True
    topoClusterMaker = _algoHLTTopoClusterLC(inputEDM = cellMaker.CellsName, algSuffix="LC")
    RecoSequence = parOR("TopoClusterRecoSequenceLC",[cellMaker,topoClusterMaker])
    return (RecoSequence, topoClusterMaker.CaloClusters)

