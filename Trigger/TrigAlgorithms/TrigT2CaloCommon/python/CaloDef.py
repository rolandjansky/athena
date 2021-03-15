# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import seqAND, parOR
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.CommonSequences.FullScanDefs import caloFSRoI

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
      from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
      inputEDM = mapThresholdToL1RoICollection("FSNOSEED")
   setMinimalCaloSetup()
   from AthenaCommon.AppMgr import ServiceMgr as svcMgr
   from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
   algo=HLTCaloCellMaker(name)
   #"HLTCaloCellMaker"
   algo.RoIs=inputEDM
   algo.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
   algo.CellsName=outputEDM
   return algo

def _algoHLTTopoCluster(inputEDM="CellsClusters", algSuffix="") :
   from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo
   algo = TrigCaloClusterMakerMT_topo(name="TrigCaloClusterMakerMT_topo"+algSuffix, doMoments=True, doLC=False, cells=inputEDM)
   from TrigEDMConfig.TriggerEDMRun3 import recordable
   algo.CaloClusters=recordable("HLT_TopoCaloClusters"+algSuffix)
   return algo

def _algoHLTTopoClusterLC(inputEDM="CellsClusters", algSuffix="") :
   from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo
   algo = TrigCaloClusterMakerMT_topo(name="TrigCaloClusterMakerMT_topo"+algSuffix, doMoments=True, doLC=True, cells=inputEDM)
   from TrigEDMConfig.TriggerEDMRun3 import recordable
   algo.CaloClusters=recordable("HLT_TopoCaloClusters"+algSuffix)
   return algo

def _algoL2Egamma(inputEDM="", doRinger=False, ClustersName="HLT_FastCaloEMClusters", RingerKey="HLT_FastCaloRinger"):
    if not inputEDM:
        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
        inputEDM = mapThresholdToL1RoICollection("EM")
    setMinimalCaloSetup()
    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_ReFastAlgo
    algo=T2CaloEgamma_ReFastAlgo("FastCaloL2EgammaAlg", doRinger=doRinger, RingerKey=RingerKey)
    algo.RoIs=inputEDM
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    algo.ClustersName=recordable("HLT_FastCaloEMClusters")
    return algo


####################################
##### SEQUENCES
####################################

def fastCaloRecoSequence(InViewRoIs, doRinger=False, ClustersName="HLT_FastCaloEMClusters", RingerKey="HLT_FastCaloRinger"):
    fastCaloAlg = _algoL2Egamma(inputEDM=InViewRoIs, doRinger=doRinger, ClustersName=ClustersName, RingerKey=RingerKey)

    import AthenaCommon.CfgMgr as CfgMgr
    fastCaloVDV = CfgMgr.AthViews__ViewDataVerifier("fastCaloVDV")
    fastCaloVDV.DataObjects = [( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+EMCaloRoIs' )]

    fastCaloInViewSequence = seqAND( 'fastCaloInViewSequence', [fastCaloVDV, fastCaloAlg] )
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


def HLTCellMaker(RoIs=caloFSRoI, outputName="CaloCells", algSuffix=""):
    cellMakerAlgo = _algoHLTCaloCell(name="HLTCaloCellMaker"+algSuffix, inputEDM=RoIs, outputEDM=outputName, RoIMode=True)
    return cellMakerAlgo

def HLTFSCellMakerRecoSequence(RoIs=caloFSRoI):
    cellMaker = HLTCellMaker(RoIs, outputName="CaloCellsFS", algSuffix="FS")
    RecoSequence = parOR("ClusterRecoSequenceFS", [cellMaker])
    return (RecoSequence, cellMaker.CellsName)


def HLTFSTopoRecoSequence(RoIs):
    cellMaker = HLTCellMaker(RoIs, outputName="CaloCellsFS", algSuffix="FS")
    topoClusterMaker = _algoHLTTopoCluster(inputEDM = cellMaker.CellsName, algSuffix="FS")
    RecoSequence = parOR("TopoClusterRecoSequenceFS", [cellMaker, topoClusterMaker])
    return (RecoSequence, topoClusterMaker.CaloClusters)

def HLTRoITopoRecoSequence(RoIs):
    import AthenaCommon.CfgMgr as CfgMgr
    HLTRoITopoRecoSequenceVDV = CfgMgr.AthViews__ViewDataVerifier("HLTRoITopoRecoSequenceVDV")
    HLTRoITopoRecoSequenceVDV.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+PrecisionCaloRoIs' ),
                                             ( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' )]

    cellMaker = HLTCellMaker(RoIs, algSuffix="RoI")
    topoClusterMaker = _algoHLTTopoCluster(inputEDM = cellMaker.CellsName, algSuffix="RoI")
    RecoSequence = parOR("RoITopoClusterRecoSequence", [HLTRoITopoRecoSequenceVDV, cellMaker, topoClusterMaker])
    return (RecoSequence, topoClusterMaker.CaloClusters)


def HLTLCTopoRecoSequence(RoIs='InViewRoIs'):
    cellMaker = HLTCellMaker(RoIs, outputName="CaloCellsLC", algSuffix="LC")
    topoClusterMaker = _algoHLTTopoClusterLC(inputEDM = cellMaker.CellsName, algSuffix="LC")
    RecoSequence = parOR("TopoClusterRecoSequenceLC",[cellMaker,topoClusterMaker])
    return (RecoSequence, topoClusterMaker.CaloClusters)
