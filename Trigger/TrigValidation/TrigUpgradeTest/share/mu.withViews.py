# 
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
# 
#  OutputLevel: INFO < DEBUG < VERBOSE 
# 
  
include("TrigUpgradeTest/testHLT_MT.py") 

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

### If inputFile is BS(ByteStream), the bool is true. ###
isData = False 
if globalflags.InputFormat.is_bytestream():
  isData = True

### provide a minimal menu information ###
if isData:
  topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
  topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
  topSequence.L1DecoderTest.roiUnpackers[1].OutputLevel=DEBUG
else:
  pass
 
### for Desplaying StoreGate Dump ###
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.StoreGateSvc=Service("StoreGateSvc") 
ServiceMgr.StoreGateSvc.Dump=True 
 
### for Control Flow ###
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
 
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.CheckDependencies( True )
AlgScheduler.OutputLevel( DEBUG )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )


from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
from AthenaCommon import CfgMgr

doL2SA=True
domuComb=False
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

  if doL2SA:
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    l2MuViewNode = AthSequencer("l2MuViewNode", Sequential=False, ModeOR=False, StopOverride=False)
    l2MuViewsMaker = EventViewCreatorAlgorithm("l2MuViewsMaker", OutputLevel=DEBUG)
    l2MuViewsMaker.ViewFallThrough = True
 
    l2MuViewsMaker.Decisions = "MURoIDecisions"
    l2MuViewsMaker.RoIsLink = "initialRoI" # -||-
    l2MuViewsMaker.InViewRoIs = "MURoIs" # contract with the consumer
    l2MuViewsMaker.Views = "MUViewRoIs"
    l2MuViewsMaker.ViewNodeName = l2MuViewNode.name()

  if doL2SA or doEFSA:
    ### ==================== Input=BSfile ==================== ###
    if isData:
      ### CSC RDO date ###
      if muonRecFlags.doCSCs():
        from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscROD_Decoder
        CSCRodDecoder = Muon__CscROD_Decoder(name		= "CSCRodDecoder",
                                             IsCosmics		= False,
                                             IsOldCosmics 	= False,
                                             OutputLevel 	= INFO )
        ToolSvc += CSCRodDecoder
  
        from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderTool
        MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderTool(name        = "MuonCscRawDataProviderTool",
                                                                   Decoder     = ToolSvc.CSCRodDecoder,
                                                                   OutputLevel = INFO )
        ToolSvc += MuonCscRawDataProviderTool
  
        from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
        CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name                = "CscRdoToPrepDataTool",
                                                                OutputLevel         = INFO,
                                                                RawDataProviderTool = MuonCscRawDataProviderTool,
                                                                useBStoRdoTool	    = True)
        ToolSvc += CscRdoToCscPrepDataTool
  
        from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
        CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepDataProvider",
                                                  CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                                  PrintPrepData	    	  = False,
                                                  OutputLevel             = INFO)
        CscRdoToCscPrepData.DoSeededDecoding = True
        CscRdoToCscPrepData.RoIs = "MUViewRoIs"
   
        from MuonByteStream.MuonByteStreamConf import Muon__CscRawDataProvider
        CscRawDataProvider = Muon__CscRawDataProvider(name         = "CscRawDataProvider",
                                                      ProviderTool = MuonCscRawDataProviderTool,
                                                      OutputLevel  = INFO)
        l2MuViewNode += CscRawDataProvider  
   
      ### MDT RDO date ###
      if muonRecFlags.doMDTs():
        from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
        MDTRodDecoder = MdtROD_Decoder(name	   = "MDTRodDecoder",
                                       OutputLevel = INFO )
        ToolSvc += MDTRodDecoder
  
        from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderTool
        MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderTool(name        = "MuonMdtRawDataProviderTool",
                                                                   Decoder     = ToolSvc.MDTRodDecoder,
                                                                   OutputLevel = INFO )
        ToolSvc += MuonMdtRawDataProviderTool
  
        from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
        MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name                = "MdtRdoToPrepDataTool",
                                                             OutputLevel         = INFO,
                                                             RawDataProviderTool = MuonMdtRawDataProviderTool,
                                                             useBStoRdoTool      = True)
        ToolSvc += MdtRdoToMdtPrepDataTool
  
        from MuonRdoToPrepData.MuonRdoToPrepDataConf import MdtRdoToMdtPrepData
        MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name          = "MdtRdoToMdtPrepDataProvider",
                                                  DecodingTool  = MdtRdoToMdtPrepDataTool,
                                                  PrintPrepData = False,
                                                  OutputLevel   = INFO)  
        MdtRdoToMdtPrepData.DoSeededDecoding = True
        MdtRdoToMdtPrepData.RoIs = "MUViewRoIs"
  
        from MuonByteStream.MuonByteStreamConf import Muon__MdtRawDataProvider
        MdtRawDataProvider = Muon__MdtRawDataProvider(name         = "MdtRawDataProvider",
                                                      ProviderTool = MuonMdtRawDataProviderTool,
                                                      OutputLevel  = INFO)
        l2MuViewNode += MdtRawDataProvider 
  
      ### RPC RDO date ###
      if muonRecFlags.doRPCs():
        from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
        RPCRodDecoder = Muon__RpcROD_Decoder(name	 = "RPCRodDecoder",
                                             OutputLevel = INFO )
        ToolSvc += RPCRodDecoder
  
        from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderTool
        MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderTool(name    = "MuonRpcRawDataProviderTool",
                                                                   Decoder = RPCRodDecoder )
        ToolSvc += MuonRpcRawDataProviderTool
  
        from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
        RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name                = "RpcRdoToPrepDataTool",
                                                             OutputLevel         = INFO,
                                                             RawDataProviderTool = MuonRpcRawDataProviderTool,
                                                             useBStoRdoTool      = True)
        ToolSvc += RpcRdoToRpcPrepDataTool
  
        from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
        RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name          = "RpcRdoToRpcPrepDataProvider",
                                                  DecodingTool  = RpcRdoToRpcPrepDataTool,
                                                  PrintPrepData = False,
                                                  OutputLevel   = INFO)    
        RpcRdoToRpcPrepData.DoSeededDecoding = True
        RpcRdoToRpcPrepData.RoIs = "MUViewRoIs"
  
        from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
        RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider",
                                                      ProviderTool = MuonRpcRawDataProviderTool,
                                                      OutputLevel  = INFO)
        l2MuViewNode += RpcRawDataProvider 
    
      ### TGC RDO date ###
      if muonRecFlags.doTGCs():
        from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RodDecoderReadout
        TGCRodDecoder = Muon__TGC_RodDecoderReadout(name		= "TGCRodDecoder",
                                                    OutputLevel 	= INFO )
        ToolSvc += TGCRodDecoder
  
        from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderTool
        MuonTgcRawDataProviderTool = Muon__TGC_RawDataProviderTool(name    = "MuonTgcRawDataProviderTool",
                                                                   Decoder = TGCRodDecoder )
        ToolSvc += MuonTgcRawDataProviderTool
        
        from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
        TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name                = "TgcRdoToPrepDataTool",
                                                             OutputLevel         = INFO,
                                                             RawDataProviderTool = MuonTgcRawDataProviderTool,
                                                             useBStoRdoTool      = True)
        ToolSvc += TgcRdoToTgcPrepDataTool
  
        from MuonRdoToPrepData.MuonRdoToPrepDataConf import TgcRdoToTgcPrepData
        TgcRdoToTgcPrepData = TgcRdoToTgcPrepData(name          = "TgcRdoToTgcPrepDataProvider",
                                                  DecodingTool  = TgcRdoToTgcPrepDataTool,
                                                  PrintPrepData = False,
                                                  OutputLevel   = INFO)
        TgcRdoToTgcPrepData.DoSeededDecoding = True
        TgcRdoToTgcPrepData.RoIs = "MUViewRoIs"
   
        from MuonByteStream.MuonByteStreamConf import Muon__TgcRawDataProvider
        TgcRawDataProvider = Muon__TgcRawDataProvider(name         = "TgcRawDataProvider",
                                                      ProviderTool = MuonTgcRawDataProviderTool,
                                                      OutputLevel  = INFO)
        l2MuViewNode += TgcRawDataProvider 

    ### ==================== Input=RDOfile ==================== ###
    else:
      ### CSC RDO date ###
      if muonRecFlags.doCSCs():
        from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRDO_Decoder
        CscRdoDecoder = Muon__CscRDO_Decoder(name        = "CscRdoDecoder",
                                             OutputLevel = INFO)
        ToolSvc += CscRdoDecoder
   
        from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
        CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name              = "CscRdoToCscPrepDataTool",
                                                                OutputLevel       = INFO,
                                                                CscRdoDecoderTool = CscRdoDecoder,
                                                                useBStoRdoTool	  = False)
        ToolSvc += CscRdoToCscPrepDataTool
     
        from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
        CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepDataProvider",
                                                  CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                                  PrintPrepData	  	  = False,
                                                  OutputLevel             = INFO)
    
        CscRdoToCscPrepData.DoSeededDecoding = True
        CscRdoToCscPrepData.RoIs = "MUViewRoIs"
        l2MuViewNode += CscRdoToCscPrepData 
      
      ### MDT RDO date ###
      if muonRecFlags.doMDTs():
        from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRDO_Decoder
        MdtRdoDecoder = Muon__MdtRDO_Decoder(name        = "MdtRdoDecoder",
                                             OutputLevel = INFO)
        ToolSvc += MdtRdoDecoder
   
        from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
        MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name           = "MdtRdoToMdtPrepDataTool",
                                                             OutputLevel    = INFO,
                                                             useBStoRdoTool = False)
        ToolSvc += MdtRdoToMdtPrepDataTool
     
        from MuonRdoToPrepData.MuonRdoToPrepDataConf import MdtRdoToMdtPrepData
        MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name          = "MdtRdoToMdtPrepDataProvider",
                                                  DecodingTool  = MdtRdoToMdtPrepDataTool,
                                                  PrintPrepData = False,
                                                  OutputLevel   = INFO)
  
        MdtRdoToMdtPrepData.DoSeededDecoding = True
        MdtRdoToMdtPrepData.RoIs = "MUViewRoIs"
        l2MuViewNode += MdtRdoToMdtPrepData 
  
      ### RPC RDO date ###
      if muonRecFlags.doRPCs():
        from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRDO_Decoder
        RpcRdoDecoder = Muon__RpcRDO_Decoder(name        = "RpcRdoDecoder",
                                             OutputLevel = INFO)
        ToolSvc += RpcRdoDecoder
   
        from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
        RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name           = "RpcRdoToRpcPrepDataTool",
                                                             OutputLevel    = INFO,
                                                             RdoDecoderTool = RpcRdoDecoder,
                                                             useBStoRdoTool = False)
        ToolSvc += RpcRdoToRpcPrepDataTool
     
        from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
        RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name          = "RpcRdoToRpcPrepDataProvider",
                                                  DecodingTool  = RpcRdoToRpcPrepDataTool,
                                                  PrintPrepData = False,
                                                  OutputLevel   = INFO)
      
        RpcRdoToRpcPrepData.DoSeededDecoding = True
        RpcRdoToRpcPrepData.RoIs = "MUViewRoIs"
        l2MuViewNode += RpcRdoToRpcPrepData 
     
      ### TGC RDO date ###
      if muonRecFlags.doTGCs():
        from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRDO_Decoder
        TgcRdoDecoder = Muon__TgcRDO_Decoder(name        = "TgcRdoDecoder",
                                             OutputLevel = INFO)
        ToolSvc += TgcRdoDecoder
   
        from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
        TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name           = "TgcRdoToTgcPrepDataTool",
                                                             OutputLevel    = INFO,
                                                             useBStoRdoTool = False)
        ToolSvc += TgcRdoToTgcPrepDataTool
     
        from MuonRdoToPrepData.MuonRdoToPrepDataConf import TgcRdoToTgcPrepData
        TgcRdoToTgcPrepData = TgcRdoToTgcPrepData(name          = "TgcRdoToTgcPrepDataProvider",
                                                  DecodingTool  = TgcRdoToTgcPrepDataTool,
                                                  PrintPrepData = False,
                                                  OutputLevel   = INFO)
      
        TgcRdoToTgcPrepData.DoSeededDecoding = True
        TgcRdoToTgcPrepData.RoIs = "MUViewRoIs"
        l2MuViewNode += TgcRdoToTgcPrepData 
  
      ### CSC CscCluster date ###
      if muonRecFlags.doCSCs():
        from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
        CscClusterBuilderTool = CscThresholdClusterBuilderTool(name        = "CscThesholdClusterBuilderTool",
                                                               OutputLevel = INFO)
        ToolSvc += CscClusterBuilderTool
    
        from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilder
        CscClusterBuilder = CscThresholdClusterBuilder(name            = "CscThesholdClusterBuilder",
                                                       OutputLevel     = INFO,
                                                       cluster_builder = CscClusterBuilderTool)    
     
        l2MuViewNode += CscClusterBuilder 
    
      ### TGC TGC_MeasurementsAllBCs date ###
      if muonRecFlags.doCreateClusters():
        from MuonClusterization.MuonClusterizationConf import MuonClusterizationTool
        MuonClusterTool = MuonClusterizationTool(name        = "MuonClusterizationTool",
                                                 OutputLevel = INFO)
        ToolSvc += MuonClusterTool
    
        from MuonClusterization.MuonClusterizationConf import MuonClusterizationAlg
        MuonClusterAlg = MuonClusterizationAlg(name                 = "MuonClusterizationAlg",
                                               OutputLevel          = INFO,
                                               ClusterTool          = MuonClusterTool,
                                               TgcPrepDataContainer = "TGC_MeasurementsAllBCs")
    
        l2MuViewNode += MuonClusterAlg 


  ### muon thresholds ###
  testChains = ["HLT_mu6"]

  ### set up L1RoIsFilter ###
  from DecisionHandling.DecisionHandlingConf import RoRSeqFilter, DumpDecisions
  filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
  filterL1RoIsAlg.Input = ["MURoIDecisions"]
  filterL1RoIsAlg.Output = ["FilteredMURoIDecisions"]
  filterL1RoIsAlg.Chains = testChains
  filterL1RoIsAlg.OutputLevel = DEBUG

# ==================================================================================================================================
#               Setup L2MuonSA
# ==================================================================================================================================
  if doL2SA:
    ### set up MuFastSteering ###
    from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
    muFastAlg = TrigL2MuonSAConfig("Muon")
    muFastAlg.OutputLevel = DEBUG

    svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection = False

    l2MuViewNode += muFastAlg

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


# ==================================================================================================================================
#               Setup muComb
# ==================================================================================================================================
  if domuComb:
    ### please read out TrigmuCombMTConfig file ###
    ### and set up to run muCombMT algorithm    ###

 
    ### if we wold like to run muComb chain after muFast chain, ###
    ### please try the bellow lines.                            ###
   
    ### set up muCombHypo algorithm ###
    #from TrigMuonHypo.TrigMuonHypoConfig import TrigmuCombHypoConfig
    #trigmuCombHypo = TrigmuCombHypoConfig()
    #trigmuCombHypo.OutputLevel = DEBUG
  
    #trigmuCombHypo.Decisions = "L2muCombDecisions"
    #trigmuCombHypo.MuonSADecisions = trigMufastHypo.Decisions
    #trigmuCombHypo.ViewRoIs = l2muCombViewsMaker.Views
    #trigmuCombHypo.MuCombContainer = muCombAlg.Decisions
  
    #trigmuCombHypo.HypoTools = [ trigmuCombHypo.TrigmuCombHypoToolFromName( name = "L2muCombHypoTool", nath = c ) for c in testChains ] 
  
    #muCombDecisionsDumper = DumpDecisions("muCombDecisionsDumper", OutputLevel=DEBUG, Decisions = trigmuCombHypo.Decisions )
 
    ### need to use ViewNode ### 
    #muCombStep = stepSeq("muCombStep", filterL2MuonSARoIsAlg, [ l2muCombViewsMaker, trigmuCombHypo, muCombDecisionsDumper ])

    pass 

# ==================================================================================================================================
#               Setup EFMuonSA
# ==================================================================================================================================
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


# ==================================================================================================================================
#               Setup CF(Control Flow)
# ==================================================================================================================================
  ### CF construction ###
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
