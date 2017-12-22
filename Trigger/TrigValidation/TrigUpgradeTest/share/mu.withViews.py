# 
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
# 
#  OutputLevel: INFO < DEBUG < VERBOSE 
# 
  
include("TrigUpgradeTest/testHLT_MT.py") 

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# provide a minimal menu information
topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
topSequence.L1DecoderTest.roiUnpackers[1].OutputLevel=DEBUG
 
# for Desplaying StoreGate Dump
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.StoreGateSvc=Service("StoreGateSvc") 
ServiceMgr.StoreGateSvc.Dump=True 
 
# for Control Flow
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
 
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.CheckDependencies( True )
AlgScheduler.OutputLevel( DEBUG )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )


from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
from AthenaCommon import CfgMgr

doL2SA=True
doEFSA=False
 
if TriggerFlags.doMuon:
# ==================================================================================================================================
#               Setup PrepData 
# ==================================================================================================================================
  # Load data from Muon detectors
  #topSequence.SGInputLoader.Load = [ ('MdtCsmContainer','MDTCSM'), ('RpcPadContainer','RPCPAD'), ('TgcRdoContainer','TGCRDO'), ('CscRawDataContainer','CSCRDO')]
  import MuonRecExample.MuonRecStandaloneOnlySetup
  from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
  muonRecFlags.doTrackPerformance    = True
  muonRecFlags.TrackPerfSummaryLevel = 2
  muonRecFlags.TrackPerfDebugLevel   = 5
  muonRecFlags.doCSCs                = True
  muonRecFlags.doNSWNewThirdChain    = False
  muonCombinedRecFlags.doCaloTrkMuId = False
  muonCombinedRecFlags.printSummary = False
  from RecExConfig.RecFlags import rec

  from AthenaCommon.CfgGetter import getAlgorithm
  if muonRecFlags.doCSCs():
    CscDecoder = getAlgorithm("CscRdoToCscPrepData", tryDefaultConfigurable=True)
    CscDecoder.DoSeededDecoding=True
    CscDecoder.RoIs="MURoIs"
    topSequence += CscDecoder
 
  if muonRecFlags.doMDTs():
    MdtDecoder = getAlgorithm("MdtRdoToMdtPrepData", tryDefaultConfigurable=True)
    MdtDecoder.DoSeededDecoding=True
    MdtDecoder.RoIs="MURoIs"
    topSequence += MdtDecoder
 
  if muonRecFlags.doRPCs():
    RpcDecoder = getAlgorithm("RpcRdoToRpcPrepData", tryDefaultConfigurable=True)
    RpcDecoder.DoSeededDecoding=True
    RpcDecoder.RoIs="MURoIs"
    topSequence += RpcDecoder

  if muonRecFlags.doTGCs():
    TgcDecoder = getAlgorithm("TgcRdoToTgcPrepData", tryDefaultConfigurable=True)
    TgcDecoder.DoSeededDecoding=True
    TgcDecoder.RoIs="MURoIs"
    topSequence += TgcDecoder
    
  if muonRecFlags.doCSCs():
    topSequence += getAlgorithm("CscThresholdClusterBuilder")

  if muonRecFlags.doCreateClusters():
    getPublicTool("MuonClusterizationTool")
    topSequence += CfgMgr.MuonClusterizationAlg("MuonClusterizationAlg",TgcPrepDataContainer="TGC_MeasurementsAllBCs" )

  # muon thresholds
  testChains = ["HLT_mu6"]

  # set up L1RoIsFilter 
  from DecisionHandling.DecisionHandlingConf import RoRSeqFilter, DumpDecisions
  filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
  filterL1RoIsAlg.Input = ["MURoIDecisions"]
  filterL1RoIsAlg.Output = ["FilteredMURoIDecisions"]
  filterL1RoIsAlg.Chains = testChains
  filterL1RoIsAlg.OutputLevel = DEBUG

  from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm

  if doL2SA:
    # ==================================================================================================================================
    #               Setup L2MuonSA
    # ==================================================================================================================================
    # set up MuFastSteering
    from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
    muFastAlg = TrigL2MuonSAConfig("Muon")
    muFastAlg.OutputLevel = DEBUG

    svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection = False

    l2MuViewNode = AthSequencer("l2MuViewNode", Sequential=False, ModeOR=False, StopOverride=False)
    l2MuViewNode += muFastAlg
    l2MuViewsMaker = EventViewCreatorAlgorithm("l2MuViewsMaker", OutputLevel=DEBUG)
    l2MuViewsMaker.ViewFallThrough = True
 
    l2MuViewsMaker.Decisions = "MURoIDecisions"
    l2MuViewsMaker.RoIsLink = "initialRoI" # -||-
    l2MuViewsMaker.InViewRoIs = "MURoIs" # contract with the consumer
    l2MuViewsMaker.Views = "MUViewRoIs"
    l2MuViewsMaker.ViewNodeName = l2MuViewNode.name()

    muFastAlg.MuRoIs = l2MuViewsMaker.InViewRoIs
    muFastAlg.RecMuonRoI = "RecMURoIs"
    muFastAlg.MuFastDecisions = "MuFastAlg_Decisions"
    muFastAlg.MuFastComposite = "MuFastAlg_Composite"
    muFastAlg.MuFastForID = "MuFastAlg_IdDecisions"
    muFastAlg.MuFastForMS = "MuFastAlg_MsDecisions"

    # set up MuFastHypo
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMufastHypoConfig
    trigMufastHypo = TrigMufastHypoConfig("L2MufastHypoAlg")
    trigMufastHypo.OutputLevel = DEBUG

    trigMufastHypo.ViewRoIs = l2MuViewsMaker.Views
    trigMufastHypo.MuFastDecisions = muFastAlg.MuFastDecisions
    trigMufastHypo.RoIs = l2MuViewsMaker.InViewRoIs
    trigMufastHypo.Decisions = "L2MuonFastDecisions"
    trigMufastHypo.L1Decisions = l2MuViewsMaker.Decisions

    trigMufastHypo.HypoTools = [ trigMufastHypo.TrigMufastHypoToolFromName( "L2MufastHypoTool", c ) for c in testChains ] 

    muFastDecisionsDumper = DumpDecisions("muFastDecisionsDumper", OutputLevel=DEBUG, Decisions = trigMufastHypo.Decisions )
    muFastStep = seqAND("muFastStep", [filterL1RoIsAlg, l2MuViewsMaker, l2MuViewNode, trigMufastHypo, muFastDecisionsDumper])


  if doEFSA:
    from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
    ServiceMgr += Trk__TrackingVolumesSvc("TrackingVolumesSvc",BuildVolumesFromTagInfo = False)

    theSegmentFinder = CfgGetter.getPublicToolClone("MuonSegmentFinder","MooSegmentFinder")
    theSegmentFinder.DoSummary=True
    CfgGetter.getPublicTool("MuonLayerHoughTool").DoTruth=False
    theSegmentFinderAlg=CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker",
                                                    SegmentFinder=theSegmentFinder,
                                                    MuonSegmentOutputLocation = "MuonSegmentCollection",
                                                    UseCSC = muonRecFlags.doCSCs(),
                                                    UseMDT = muonRecFlags.doMDTs(),
                                                    UseRPC = muonRecFlags.doRPCs(),
                                                    UseTGC = muonRecFlags.doTGCs(),
                                                    doClusterTruth=False,
                                                    UseTGCPriorBC = False,
                                                    UseTGCNextBC  = False,
                                                    doTGCClust = muonRecFlags.doTGCClusterSegmentFinding(),
                                                    doRPCClust = muonRecFlags.doRPCClusterSegmentFinding(), OutputLevel=DEBUG )
    

 
    theNCBSegmentFinderAlg=CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker_NCB",
                                                       SegmentFinder = getPublicToolClone("MooSegmentFinder_NCB","MuonSegmentFinder",
                                                                                          DoSummary=False,
                                                                                          Csc2dSegmentMaker = getPublicToolClone("Csc2dSegmentMaker_NCB","Csc2dSegmentMaker",
                                                                                                                                 segmentTool = getPublicToolClone("CscSegmentUtilTool_NCB",
                                                                                                                                                                  "CscSegmentUtilTool",
                                                                                                                                                                  TightenChi2 = False, 
                                                                                                                                                                  IPconstraint=False)),
                                                                                          Csc4dSegmentMaker = getPublicToolClone("Csc4dSegmentMaker_NCB","Csc4dSegmentMaker",
                                                                                                                                 segmentTool = getPublicTool("CscSegmentUtilTool_NCB")),
                                                                                          DoMdtSegments=False,DoSegmentCombinations=False,DoSegmentCombinationCleaning=False),
                                                       MuonPatternCombinationLocation = "NCB_MuonHoughPatternCombinations", 
                                                       MuonSegmentOutputLocation = "NCB_MuonSegments", 
                                                       MuonSegmentCombinationOutputLocation = "NCB_MooreSegmentCombinations",
                                                       UseCSC = muonRecFlags.doCSCs(),
                                                       UseMDT = False,
                                                       UseRPC = False,
                                                       UseTGC = False,
                                                       UseTGCPriorBC = False,
                                                       UseTGCNextBC  = False,
                                                       doTGCClust = False,
                                                       doRPCClust = False)

    from MuonRecExample.MuonStandalone import MuonTrackSteering
    MuonTrackSteering.DoSummary=True
    MuonTrackSteering.DoSummary=DEBUG
    TrackBuilder = CfgMgr.MuPatTrackBuilder("MuPatTrackBuilder" )
    TrackBuilder.TrackSteering=CfgGetter.getPublicToolClone("MuonTrackSteering", "MuonTrackSteering")

    from AthenaCommon.Include import include
    include("InDetBeamSpotService/BeamCondSvc.py" )        
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg, xAODMaker__TrackCollectionCnvTool, xAODMaker__RecTrackParticleContainerCnvTool
  
    muonParticleCreatorTool = getPublicTool("MuonParticleCreatorTool")
  
    muonTrackCollectionCnvTool = xAODMaker__TrackCollectionCnvTool( name = "MuonTrackCollectionCnvTool", TrackParticleCreator = muonParticleCreatorTool )
  
    muonRecTrackParticleContainerCnvTool = xAODMaker__RecTrackParticleContainerCnvTool(name = "MuonRecTrackParticleContainerCnvTool", TrackParticleCreator = muonParticleCreatorTool )
 
    xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg( name = "MuonStandaloneTrackParticleCnvAlg", 
                                                              TrackParticleCreator = muonParticleCreatorTool,
                                                              TrackCollectionCnvTool=muonTrackCollectionCnvTool,
                                                              RecTrackParticleContainerCnvTool = muonRecTrackParticleContainerCnvTool,
                                                              TrackContainerName = "MuonSpectrometerTracks",
                                                              xAODTrackParticlesFromTracksContainerName = "MuonSpectrometerTrackParticles",
                                                              ConvertTrackParticles = False,
                                                              ConvertTracks = True)


    thetrkbuilder = getPublicToolClone("CombinedMuonTrackBuilder_SA", "CombinedMuonTrackBuilder", MuonHoleRecovery="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool')

    theCandidateTool = getPublicToolClone("MuonCandidateTool_SA", "MuonCandidateTool", TrackBuilder=thetrkbuilder)
    theMuonCandidateAlg=CfgMgr.MuonCombinedMuonCandidateAlg("MuonCandidateAlg",MuonCandidateTool=theCandidateTool)


    thecreatortool= getPublicToolClone("MuonCreatorTool_SA", "MuonCreatorTool", ScatteringAngleTool="", CaloMaterialProvider='TMEF_TrkMaterialProviderTool', MuonSelectionTool="", FillTimingInformation=False, OutputLevel=DEBUG)

    themuoncreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlg")
    themuoncreatoralg.MuonCreatorTool=thecreatortool
    themuoncreatoralg.CreateSAmuons=True



  # CF construction
  if doL2SA:
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg 
    summary = TriggerSummaryAlg( "TriggerSummaryAlg" ) 
    summary.L1Decision = "HLTChains" 
    summary.FinalDecisions = ["L2MuonFastDecisions"]
    summary.OutputLevel = DEBUG 
    step0 = parOR("step0", [ muFastStep ] )
    HLTsteps = seqAND("HLTsteps", [ step0, summary ]  ) 

    mon = TriggerSummaryAlg( "TriggerMonitoringAlg" ) 
    mon.L1Decision = "HLTChains" 
    mon.FinalDecisions = [ "L2MuonFastDecisions", "WhateverElse" ] 
    mon.HLTSummary = "MonitoringSummary" 
    mon.OutputLevel = DEBUG 
    hltTop = seqOR( "hltTop", [ HLTsteps, mon] )

    topSequence += hltTop   
    


def TMEF_TrkMaterialProviderTool(name='TMEF_TrkMaterialProviderTool',**kwargs):
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    kwargs.setdefault("UseCaloEnergyMeasurement", False)
    return Trk__TrkMaterialProviderTool(name,**kwargs)
