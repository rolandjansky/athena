#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

## #workaround to prevent online trigger folders to be enabled
## from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
## InDetTrigFlags.useConditionsClasses.set_Value_and_Lock(False)


## from InDetRecExample.InDetJobProperties import InDetFlags
## InDetFlags.doCaloSeededBrem = False


## from InDetRecExample.InDetJobProperties import InDetFlags
## InDetFlags.InDet25nsec = True 
## InDetFlags.doPrimaryVertex3DFinding = False 
## InDetFlags.doPrintConfigurables = False
## InDetFlags.doResolveBackTracks = True 
## InDetFlags.doSiSPSeededTrackFinder = True
## InDetFlags.doTRTPhaseCalculation = True
## InDetFlags.doTRTSeededTrackFinder = True
## InDetFlags.doTruth = False
## InDetFlags.init()

## # PixelLorentzAngleSvc and SCTLorentzAngleSvc
## include("InDetRecExample/InDetRecConditionsAccess.py")
## from InDetRecExample.InDetKeys import InDetKeys

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
   topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
   topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG


from TrigUpgradeTest.HLTCFConfig import *
from TrigUpgradeTest.MenuComponents import *
from ViewAlgs.ViewAlgsConf import TestEventViewCreatorAlgorithm
from AthenaCommon.AppMgr import ServiceMgr as svcMgr


#################################
# first step: calo
#################################
def InputMakerAlg(name):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, LinkName="initialRoI")
 

def FastCaloAlg(name):
    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
    theFastCaloAlgo=T2CaloEgamma_FastAlgo(name)
    theFastCaloAlgo.OutputLevel=VERBOSE
    theFastCaloAlgo.ClustersName="L2CaloClusters"
    svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False
    return theFastCaloAlgo

def FastCaloViewsMaker(name):
    testfastCaloViewsMaker = TestEventViewCreatorAlgorithm(name, OutputLevel=DEBUG)    
    testfastCaloViewsMaker.ViewFallThrough = True
    testfastCaloViewsMaker.RoIsLink = "initialRoI" # -||-
    testfastCaloViewsMaker.InViewRoIs = "EMCaloRoIs"
    testfastCaloViewsMaker.Views = "EMCaloViews"
    testfastCaloViewsMaker.ViewNodeName = "fastCaloInViewSubNode"
    return testfastCaloViewsMaker
  
def FastCaloHypo(name):
  from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlg
  theFastCaloHypo = TrigL2CaloHypoAlg(name)
  theFastCaloHypo.OutputLevel = DEBUG 
  return theFastCaloHypo

def TestFastCaloHypo(name):
  from TrigEgammaHypo.TrigEgammaHypoConf import TestTrigL2CaloHypoAlg
  theFastCaloHypo = TestTrigL2CaloHypoAlg(name)
  theFastCaloHypo.OutputLevel = DEBUG 
  return theFastCaloHypo

def TrigL2CaloHypoTool(name):
  from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
  hypotool= TrigL2CaloHypoToolFromName(name)
  hypotool.OutputLevel = DEBUG
  return hypotool


def TestTrigL2CaloHypoTool(name):
  #from TrigEgammaHypo.TrigEgammaHypoConf import TestTrigL2CaloHypoToolInc
  #hypotool= TestTrigL2CaloHypoToolInc(name)
  from TrigEgammaHypo.TestTrigL2CaloHypoTool import TestTrigL2CaloHypoToolFromName
  hypotool= TestTrigL2CaloHypoToolFromName(name)
  hypotool.OutputLevel = DEBUG
  return hypotool

def e3_etcut():
  return TestTrigL2CaloHypoTool("HLT_e3_etcut")
#  return TrigL2CaloHypoTool("HLT_e3_etcut")

def e5_etcut():
  return TestTrigL2CaloHypoTool("HLT_e5_etcut")
#  return TrigL2CaloHypoTool("HLT_e5_etcut")

def e7_etcut():
  return TestTrigL2CaloHypoTool("HLT_e7_etcut")

#def 2e3_etcut():
    #TrigL2CaloHypoToolMult
#    return TestTrigL2CaloHypoTool("HLT_2e3_etcut")

# Make the calo sequence
########################
theFastCaloViewsMaker = FastCaloViewsMaker(name="fastCaloViewsMaker")
theFastCaloAlgo = FastCaloAlg(name="FastCaloAlgo")
fastCaloInViewSequence = theFastCaloViewsMaker.ViewNodeName
theFastCaloHypo = TestFastCaloHypo(name="L2CaloHypo")

# Nodes & sequence
fastCaloViewsMakerNode = AlgNode( Alg=theFastCaloViewsMaker,  inputProp='InputDecisions', outputProp='InViewRoIs')
fastCaloAlgoNode       = AlgNode( Alg=theFastCaloAlgo,     inputProp='RoIs',           outputProp='ClustersName')
fastCaloHypoNode       = HypoAlgNode( Alg=theFastCaloHypo, inputProp='CaloClusters',   outputProp='Decisions')

fastCaloViewVerify = CfgMgr.AthViews__ViewDataVerifier("FastCaloViewDataVerifier",\
            DataObjects = [('TrigRoiDescriptorCollection' , 'StoreGateSvc+'+fastCaloViewsMakerNode.getOutput())])

caloDecisionsDumper = DumpDecisions("caloDecisionsDumper", OutputLevel=DEBUG, Decisions = theFastCaloHypo.Decisions )  



fastCalo_NodeSequence = ViewNodeSequence("fastCalo_NodeSequence",
                                           Maker=fastCaloViewsMakerNode, 
                                           Algs=[fastCaloAlgoNode],
                                           Hypo=fastCaloHypoNode,
                                           OtherNodes=[AlgNode(Alg=fastCaloViewVerify), AlgNode(Alg=caloDecisionsDumper)],
                                           Seed="EMRoIDecisions")


#and then menu sequence
fastCaloSequence = MenuSequence("egammaCaloStep", nodeSeqList=[fastCalo_NodeSequence])
#print fastCaloSequence
#print theFastCaloHypo

    
#########################################
# second step:  tracking.....
#########################################
#workaround to prevent online trigger folders to be enabled
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
InDetTrigFlags.useConditionsClasses.set_Value_and_Lock(False)


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doCaloSeededBrem = False


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.InDet25nsec = True 
InDetFlags.doPrimaryVertex3DFinding = False 
InDetFlags.doPrintConfigurables = False
InDetFlags.doResolveBackTracks = True 
InDetFlags.doSiSPSeededTrackFinder = True
InDetFlags.doTRTPhaseCalculation = True
InDetFlags.doTRTSeededTrackFinder = True
InDetFlags.doTruth = False
InDetFlags.init()

# PixelLorentzAngleSvc and SCTLorentzAngleSvc
include("InDetRecExample/InDetRecConditionsAccess.py")
from InDetRecExample.InDetKeys import InDetKeys


def InDetCacheCreatorTrigViews(name="InDetCacheCreatorTrigViews"):
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__CacheCreator
    InDetCacheCreatorTrigViews = InDet__CacheCreator(name = name,
                                                    Pixel_ClusterKey = "PixelTrigClustersCache",
                                                    SCT_ClusterKey   = "SCT_ClustersCache",
                                                    SpacePointCachePix = "PixelSpacePointCache",
                                                    SpacePointCacheSCT   = "SctSpacePointCache",
                                                    SCTRDOCacheKey       = "SctRDOCache",
                                                    PixRDOCacheKey = "PixRDOCache",
                                                    OutputLevel=DEBUG)
    return InDetCacheCreatorTrigViews

#Pixel
def InDetPixelRawDataProvider():
    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRodDecoder
    InDetPixelRodDecoder = PixelRodDecoder(name = "InDetPixelRodDecoder")
    svcMgr.ToolSvc += InDetPixelRodDecoder

    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProviderTool
    InDetPixelRawDataProviderTool = PixelRawDataProviderTool(name    = "InDetPixelRawDataProviderTool",
                                                            Decoder = InDetPixelRodDecoder)
    svcMgr.ToolSvc += InDetPixelRawDataProviderTool
    if (InDetFlags.doPrintConfigurables()):
        print InDetPixelRawDataProviderTool

    # load the PixelRawDataProvider
    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProvider
    InDetPixelRawDataProvider = PixelRawDataProvider(name         = "InDetPixelRawDataProvider",
                                                    RDOKey       = InDetKeys.PixelRDOs(),
                                                    ProviderTool = InDetPixelRawDataProviderTool,
#                                                    RDOCacheKey  = InDetCacheCreatorTrigViews.PixRDOCacheKey,
                                                    isRoI_Seeded = True,
                                                    OutputLevel = INFO )
    return InDetPixelRawDataProvider

#SCT
def InDetSCTRawDataProvider():
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCT_RodDecoder
    InDetSCTRodDecoder = SCT_RodDecoder(name        = "InDetSCTRodDecoder",
                                        TriggerMode = False)
    svcMgr.ToolSvc += InDetSCTRodDecoder

    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProviderTool
    InDetSCTRawDataProviderTool = SCTRawDataProviderTool(name    = "InDetSCTRawDataProviderTool",
                                                        Decoder = InDetSCTRodDecoder)
    svcMgr.ToolSvc += InDetSCTRawDataProviderTool
    if (InDetFlags.doPrintConfigurables()):
        print      InDetSCTRawDataProviderTool

        # load the SCTRawDataProvider
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProvider
    InDetSCTRawDataProvider = SCTRawDataProvider(name         = "InDetSCTRawDataProvider",
                                                RDOKey       = InDetKeys.SCT_RDOs(),
                                                ProviderTool = InDetSCTRawDataProviderTool,
                                                isRoI_Seeded = True )
    
#    InDetSCTRawDataProvider.RDOCacheKey = InDetCacheCreatorTrigViews.SCTRDOCacheKey
    return InDetSCTRawDataProvider

#TRT
def InDetTRTRawDataProvider():
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    InDetTRTCalDbSvc = TRT_CalDbSvc()
    svcMgr += InDetTRTCalDbSvc

    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
    InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc(name = "InDetTRTStrawStatusSummarySvc")
    svcMgr += InDetTRTStrawStatusSummarySvc

    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder
    InDetTRTRodDecoder = TRT_RodDecoder(name = "InDetTRTRodDecoder",
                                        LoadCompressTableDB = True)#(globalflags.DataSource() != 'geant4'))  
    svcMgr.ToolSvc += InDetTRTRodDecoder
  
    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProviderTool
    InDetTRTRawDataProviderTool = TRTRawDataProviderTool(name    = "InDetTRTRawDataProviderTool",
                                                        Decoder = InDetTRTRodDecoder)
    svcMgr.ToolSvc += InDetTRTRawDataProviderTool

  
    # load the TRTRawDataProvider
    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProvider
    InDetTRTRawDataProvider = TRTRawDataProvider(name         = "InDetTRTRawDataProvider",
                                                RDOKey       = "TRT_RDOs",
                                                ProviderTool = InDetTRTRawDataProviderTool,
                                                isRoI_Seeded = True )

    return InDetTRTRawDataProvider


#Pixel clusterisation
from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool",
                                                    PixelCalibSvc        = None,
                                                    PixelOfflineCalibSvc = None,
                                                    UsePixelCalibCondDB  = False)
svcMgr.ToolSvc += InDetClusterMakerTool


def InDetPixelClusterization():    
    from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
    InDetMergedPixelsTool = InDet__MergedPixelsTool(name                    = "InDetMergedPixelsTool",
                                                    globalPosAlg            = InDetClusterMakerTool,
                                                    MinimalSplitSize        = 0,
                                                    MaximalSplitSize        = 49,
                                                    MinimalSplitProbability = 0,
                                                    DoIBLSplitting = True,
                                                    SplitClusterAmbiguityMap= InDetKeys.SplitClusterAmbiguityMap())
    svcMgr.ToolSvc += InDetMergedPixelsTool

    from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
    InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder")
    svcMgr.ToolSvc += InDetPixelGangedAmbiguitiesFinder

    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
    InDetPixelClusterization = InDet__PixelClusterization(name                    = "InDetPixelClusterization",
                                                        clusteringTool          = InDetMergedPixelsTool,
                                                        gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                        DetectorManagerName     = InDetKeys.PixelManager(),
                                                        DataObjectName          = InDetKeys.PixelRDOs(),
                                                        ClustersName            = "PixelTrigClusters",
                                                        isRoI_Seeded            = True)

#    InDetPixelClusterization.ClusterContainerCacheKey = InDetCacheCreatorTrigViews.Pixel_ClusterKey
    return InDetPixelClusterization


def InDetSCT_Clusterization():
    #
    # --- SCT_ClusteringTool (public)
    #
    from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
    InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool",
                                                        globalPosAlg      = InDetClusterMakerTool,
                                                        conditionsService = InDetSCT_ConditionsSummarySvc)
    #
    # --- SCT_Clusterization algorithm
    #
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
    InDetSCT_Clusterization = InDet__SCT_Clusterization(name                    = "InDetSCT_Clusterization",
                                                        clusteringTool          = InDetSCT_ClusteringTool,
                                                        # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                        DetectorManagerName     = InDetKeys.SCT_Manager(),
                                                        DataObjectName          = InDetKeys.SCT_RDOs(),
                                                        ClustersName            = "SCT_TrigClusters",
                                                        conditionsService       = InDetSCT_ConditionsSummarySvc,
                       #                                 FlaggedConditionService = InDetSCT_FlaggedConditionSvc, 
                                                        isRoI_Seeded            = True )

#    InDetSCT_Clusterization.ClusterContainerCacheKey = InDetCacheCreatorTrigViews.SCT_ClusterKey
    return InDetSCT_Clusterization


def InDetSiTrackerSpacePointFinder():
    #Space points and FTF

    from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
    InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
    svcMgr.ToolSvc += InDetSiSpacePointMakerTool

    from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
    InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                 = "InDetSiTrackerSpacePointFinder",
                                                                    SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                    PixelsClustersName     = "PixelTrigClusters",
                                                                    SCT_ClustersName       = "SCT_TrigClusters",
                                                                    SpacePointsPixelName   = "PixelTrigSpacePoints",
                                                                    SpacePointsSCTName     = "SCT_TrigSpacePoints",
                                                                    SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                    ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                    ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                    ProcessOverlaps        = DetFlags.haveRIO.SCT_on(),
                                                                    OutputLevel=DEBUG)

#    InDetSiTrackerSpacePointFinder.SpacePointCacheSCT = InDetCacheCreatorTrigViews.SpacePointCacheSCT
#    InDetSiTrackerSpacePointFinder.SpacePointCachePix = InDetCacheCreatorTrigViews.SpacePointCachePix
    return InDetSiTrackerSpacePointFinder


def TrigFastTrackFinder_eGamma():
    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_eGamma
    theFTF = TrigFastTrackFinder_eGamma()
    theFTF.OutputLevel = DEBUG
    return theFTF

def InDetTrigTrackParticleCreatorAlg():
    from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
    from TrigInDetConf.TrigInDetPostTools import  InDetTrigParticleCreatorToolFTF

    from InDetTrigParticleCreation.InDetTrigParticleCreationConf import InDet__TrigTrackingxAODCnvMT
    theTrackParticleCreatorAlg = InDet__TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg",
                                                            doIBLresidual = False,
                                                            TrackName = "TrigFastTrackFinder_Tracks",
                                                            TrackParticlesName = "xAODTracks",
                                                            ParticleCreatorTool = InDetTrigParticleCreatorToolFTF)

    return theTrackParticleCreatorAlg

    
   
# New second step
def l2ElectronViewsMaker():
    l2ElectronViewsMaker = TestEventViewCreatorAlgorithm("l2ElectronViewsMaker", OutputLevel=DEBUG)
    l2ElectronViewsMaker.ViewFallThrough = True
    l2ElectronViewsMaker.RoIsLink = "roi" # -||-
    l2ElectronViewsMaker.InViewRoIs = "EMIDRoIs" # contract with the fastCalo
    l2ElectronViewsMaker.Views = "EMElectronViews"    
    #l2ElectronViewsMaker.Decisions = filterCaloRoIsAlg.Output[0] # output of L2CaloHypo
    l2ElectronViewsMaker.ViewNodeName = "electronInViewAlgs"
    return l2ElectronViewsMaker


# A simple algorithm to confirm that data has been inherited from parent view
# Required to satisfy data dependencies
electronViewDataVerifier = CfgMgr.AthViews__ViewDataVerifier("electronViewDataVerifier")
electronViewDataVerifier.DataObjects = [('xAOD::TrigEMClusterContainer','StoreGateSvc+'+fastCaloAlgoNode.outputProp)]
#, ViewVerify,

def L2ElectronFex_1():
    from TrigEgammaHypo.TrigL2ElectronFexMTConfig import L2ElectronFex_1
    theElectronFex= L2ElectronFex_1()
#    theElectronFex.TrigEMClusterName = theFastCaloAlgo.ClustersName
#    theElectronFex.TrackParticlesName = InDetTrigTrackParticleCreatorAlg.TrackParticlesName
    theElectronFex.ElectronsName="Electrons"
    theElectronFex.OutputLevel=VERBOSE
    return theElectronFex


def TrigL2ElectronHypoAlg():
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronHypoAlg
    from TrigEgammaHypo.TrigL2ElectronHypoTool import TrigL2ElectronHypoToolFromName
    theElectronHypo = TrigL2ElectronHypoAlg()
    #theElectronHypo.Views = l2ElectronViewsMaker.Views
    #theElectronHypo.Electrons = theElectronFex.ElectronsName
    #theElectronHypo.ClusterDecisions = theFastCaloHypo.Decisions 
    theElectronHypo.ElectronDecisions = "ElectronL2Decisions"
    theElectronHypo.OutputLevel = VERBOSE
    #theElectronHypo.HypoTools = [ TrigL2ElectronHypoToolFromName( c ) for c in testChains ]
    return theElectronHypo


#############################
# create algorithms and nodes for second step
#############################


InDetCacheCreatorTrigViews = InDetCacheCreatorTrigViews()
l2ElectronViewsMaker = l2ElectronViewsMaker()
#ID sequence algorithms
InDetPixelRawDataProvider = InDetPixelRawDataProvider()
InDetSCTRawDataProvider = InDetSCTRawDataProvider()
InDetTRTRawDataProvider = InDetTRTRawDataProvider()
InDetPixelClusterization = InDetPixelClusterization()
InDetSCT_Clusterization = InDetSCT_Clusterization()
InDetSiTrackerSpacePointFinder = InDetSiTrackerSpacePointFinder()
TrigFastTrackFinder_eGamma = TrigFastTrackFinder_eGamma()
InDetTrigTrackParticleCreatorAlg = InDetTrigTrackParticleCreatorAlg()
#  alg electron:
L2ElectronFex_1 = L2ElectronFex_1()


# now make the seuqences

# rdo sequence: create RDOkeys
#InDetPixelRawDataProvider.RDOCacheKey  = InDetCacheCreatorTrigViews.PixRDOCacheKey,
#InDetSCTRawDataProvider.RDOCacheKey = InDetCacheCreatorTrigViews.SCTRDOCacheKey
NodePixelRDO_InDetCacheCreatorTrigViews = AlgNode(Alg=InDetCacheCreatorTrigViews, inputProp='', outputProp='PixRDOCacheKey')
NodeSCTRDO_InDetCacheCreatorTrigViews   = AlgNode(Alg=InDetCacheCreatorTrigViews, inputProp='', outputProp='SCTRDOCacheKey')

NodeRDO_InDetPixelRawDataProvider       = AlgNode( Alg=InDetPixelRawDataProvider,  inputProp='RDOCacheKey', outputProp='')
NodeRDO_InDetSCTRawDataProvider         = AlgNode( Alg=InDetSCTRawDataProvider,    inputProp='RDOCacheKey', outputProp='')


# cluster seuqence:
#InDetPixelClusterization.ClusterContainerCacheKey = InDetCacheCreatorTrigViews.Pixel_ClusterKey
#InDetSCT_Clusterization.ClusterContainerCacheKey = InDetCacheCreatorTrigViews.SCT_ClusterKey

NodePixelCluster_InDetCacheCreatorTrigViews = AlgNode(Alg=InDetCacheCreatorTrigViews, inputProp='', outputProp='Pixel_ClusterKey')
NodeSCTCluster_InDetCacheCreatorTrigViews   = AlgNode(Alg=InDetCacheCreatorTrigViews, inputProp='', outputProp='SCT_ClusterKey')

NodeCluster_InDetPixelClusterization  = AlgNode( Alg=InDetPixelClusterization,  inputProp='ClusterContainerCacheKey', outputProp='')
NodeCluster_InDetSCT_Clusterization   = AlgNode( Alg=InDetSCT_Clusterization,   inputProp='ClusterContainerCacheKey', outputProp='')

# space point sequence
#InDetSiTrackerSpacePointFinder.SpacePointCacheSCT = InDetCacheCreatorTrigViews.SpacePointCacheSCT
#InDetSiTrackerSpacePointFinder.SpacePointCachePix = InDetCacheCreatorTrigViews.SpacePointCachePix
NodePixelSP_InDetCacheCreatorTrigViews     = AlgNode(Alg=InDetCacheCreatorTrigViews, inputProp='', outputProp='SpacePointCachePix')
NodeSCTSP_InDetCacheCreatorTrigViews       = AlgNode(Alg=InDetCacheCreatorTrigViews, inputProp='', outputProp='SpacePointCacheSCT')

NodePixelSP_InDetSiTrackerSpacePointFinder = AlgNode(Alg=InDetSiTrackerSpacePointFinder, inputProp='SpacePointCachePix', outputProp='')
NodeSCTSP_InDetSiTrackerSpacePointFinder   = AlgNode(Alg=InDetSiTrackerSpacePointFinder, inputProp='SpacePointCacheSCT', outputProp='')


# view sequence
Node_l2ElectronViewsMaker = AlgNode(Alg=l2ElectronViewsMaker, inputProp='InputDecisions', outputProp='InViewRoIs')

# algorithms in view:
#IDSequence = [  InDetPixelRawDataProvider, InDetSCTRawDataProvider, InDetTRTRawDataProvider, InDetPixelClusterization, InDetSCT_Clusterization, InDetSiTrackerSpacePointFinder, theFTF, theTrackParticleCreatorAlg ]
# : take rois from viewCreator
NodeRoI_InDetPixelRawDataProvider    = AlgNode( Alg=InDetPixelRawDataProvider,  inputProp='RoIs', outputProp='')
NodeRoI_InDetSCTRawDataProvider      = AlgNode( Alg=InDetSCTRawDataProvider, inputProp='RoIs', outputProp='')
NodeRoI_InDetTRTRawDataProvider      = AlgNode( Alg=InDetTRTRawDataProvider,  inputProp='RoIs', outputProp='')
NodeRoI_InDetPixelClusterization     = AlgNode( Alg=InDetPixelClusterization,  inputProp='RoIs', outputProp='')
NodeRoI_InDetSCT_Clusterization      = AlgNode( Alg=InDetSCT_Clusterization,  inputProp='RoIs', outputProp='')
NodeRoI_InDetSiTrackerSpacePointFinder   = AlgNode( Alg=InDetSiTrackerSpacePointFinder,  inputProp='RoIs', outputProp='')
NodeRoI_TrigFastTrackFinder_eGamma       = AlgNode( Alg= TrigFastTrackFinder_eGamma,  inputProp='RoIs', outputProp='')
NodeRoI_InDetTrigTrackParticleCreatorAlg = AlgNode( Alg=InDetTrigTrackParticleCreatorAlg,
                                                     inputProp='roiCollectionName',
                                                     outputProp='TrackParticlesName')
# particle creator
#theTrackParticleCreatorAlg.roiCollectionName = l2ElectronViewsMaker.InViewRoIs
#theElectronFex.TrackParticlesName = theTrackParticleCreatorAlg.TrackParticlesName
NodeTrack_L2ElectronFex_1      = AlgNode( Alg=L2ElectronFex_1,  inputProp='TrackParticlesName', outputProp='ElectronsName')

#take rois from viewCreator
Node_electronViewDataVerifier  = AlgNode( Alg=electronViewDataVerifier)
NodeCluster_L2ElectronFex_1    = AlgNode( Alg=L2ElectronFex_1,  inputProp='TrigEMClusterName', outputProp='ElectronsName')


# hypo

## Node_TrigL2ElectronHypoAlg   = HypoAlgNode( Alg=TrigL2ElectronHypoAlg(), inputProp='Electrons',   outputProp='ElectronDecisions')

## electron_NodeSequence = ViewNodeSequence("electron_NodeSequence",
##                                            Maker=Node_l2ElectronViewsMaker, 
##                                            Algs=[NodeRoI_InDetPixelRawDataProvider,
##                                                      NodeRoI_InDetSCTRawDataProvider,
##                                                      NodeRoI_InDetTRTRawDataProvider,
##                                                      NodeRoI_InDetPixelClusterization ],
##                                            Hypo=Node_TrigL2ElectronHypoAlg,
##                                            Seed="")









##########################################
# menu
##########################################
#testChains = ["HLT_e3_etcut", "HLT_e5_etcut", "HLT_e7_etcut", "HLT_2e3_etcut", "HLT_e3e5_etcut"]


testChains  = [
    Chain(name='HLT_e3_etcut', Seed="L1_EM3",   \
            ChainSteps=[ChainStep("Step1_e3_etcut", [SequenceHypoTool(fastCaloSequence,e3_etcut() )])]),
    Chain(name='HLT_e5_etcut', Seed="L1_EM3",   \
              ChainSteps=[ChainStep("Step1_e5_etcut", [SequenceHypoTool(fastCaloSequence,e5_etcut() )])]),
    Chain(name='HLT_e7_etcut', Seed="L1_EM3",   \
              ChainSteps=[ChainStep("Step1_e7_etcut", [SequenceHypoTool(fastCaloSequence,e7_etcut() )])]),
  #  Chain(name='HLT_2e3_etcut', Seed="L1_EM3",   \
  #            ChainSteps=[ChainStep("Step1_2e3_etcut", [SequenceHypoTool(fastCaloSequence, 2e3_etcut() )])])
    ]
    
topSequence.L1DecoderTest.prescaler.Prescales = ["HLT_e3_etcut:2", "HLT_2e3_etcut:2.5"]
  
## for t in theElectronHypo.HypoTools:
##   t.OutputLevel = VERBOSE
## # topSequence += theElectronHypo
## # InDetCacheCreatorTrigViews,
## electronSequence = seqAND("electronSequence", [ InDetCacheCreatorTrigViews, l2ElectronViewsMaker, electronInViewAlgs, theElectronHypo ] )

## electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", OutputLevel=DEBUG, Decisions = theElectronHypo.ElectronDecisions )    
## egammaIDStep = stepSeq("egammaIDStep", filterCaloRoIsAlg, [ electronSequence,  electronDecisionsDumper ] )



##########################################
# CF construction
##########################################
from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq

from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
summary = TriggerSummaryAlg( "TriggerSummaryAlg" )
summary.InputDecision = "HLTChains"
summary.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions" ]
summary.OutputLevel = DEBUG

steps = seqAND("EgammaMenu_HLTSteps"  )
decisionTree_From_Chains(steps, testChains, NSTEPS=1)
steps += summary

mon = TriggerSummaryAlg( "TriggerMonitoringAlg" )
mon.InputDecision = "HLTChains"
mon.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions", "WhateverElse" ]
mon.HLTSummary = "MonitoringSummary"
mon.OutputLevel = DEBUG


hltTop = seqOR( "hltTop", [ steps, mon] )
topSequence += hltTop


#print fastCaloViewsMaker
#print theFastCaloHypo


##########################################  


  
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
