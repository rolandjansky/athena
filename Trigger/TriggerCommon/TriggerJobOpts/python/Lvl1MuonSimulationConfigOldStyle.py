# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def TMDBSimulationSequence(flags):
    if not flags.Input.isMC:
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
            from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
            svcMgr += ByteStreamAddressProviderSvc()
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
            "TileRawChannelContainer/MuRcvRawChCnt"
        ]
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,"SGInputLoader"):
            topSequence.SGInputLoader.Load += [('TileRawChannelContainer','StoreGateSvc+MuRcvRawChCnt')]
    else:
        pass
    TileMuonReceiverDecision = CompFactory.TileMuonReceiverDecision('TileMuonReceiverDecision'
                                                                    , TileRawChannelContainer = "MuRcvRawChCnt" # input
                                                                    , TileMuonReceiverContainer = "rerunTileMuRcvCnt" # output
                                                                    , ManualRunPeriod = 2 # forcing Run 2 format (=2) for now, until TGC implements Run 3 format (=3)
                                                                    # run 2 thresholds
                                                                    , MuonReceiverEneThreshCellD6Low = 500
                                                                    , MuonReceiverEneThreshCellD6andD5Low = 500
                                                                    , MuonReceiverEneThreshCellD6High = 600
                                                                    , MuonReceiverEneThreshCellD6andD5High = 600
                                                                    # run 3 thresholds
                                                                    , MuonReceiverEneThreshCellD5 = 500
                                                                    , MuonReceiverEneThreshCellD6 = 500
                                                                    , MuonReceiverEneThreshCellD5andD6 = 500)
    from AthenaCommon.CFElements import seqAND
    return seqAND("TMDBSimSeq", [ TileMuonReceiverDecision ] )

def NSWTriggerSequence(flags):
    # OK, let's configure NSW trigger simulation
    nsw = CompFactory.NSWL1__NSWL1Simulation("NSWL1Simulation")
    nsw.DoNtuple=False

    PadTdsTool = CompFactory.NSWL1__PadTdsOfflineTool("NSWL1__PadTdsOfflineTool",DoNtuple=False)
    nsw.PadTdsTool = PadTdsTool

    PadTriggerLogicTool = CompFactory.NSWL1__PadTriggerLogicOfflineTool("NSWL1__PadTriggerLogicOfflineTool",DoNtuple=False)
    nsw.PadTriggerTool = PadTriggerLogicTool

    PadTriggerLookupTool = CompFactory.NSWL1__PadTriggerLookupTool("NSWL1__PadTriggerLookupTool")
    nsw.PadTriggerLookupTool = PadTriggerLookupTool

    StripTdsTool = CompFactory.NSWL1__StripTdsOfflineTool("NSWL1__StripTdsOfflineTool",DoNtuple=False)
    nsw.StripTdsTool = StripTdsTool

    MMTriggerProcessorTool = CompFactory.NSWL1__TriggerProcessorTool("NSWL1__TriggerProcessorTool")
    nsw.MMTriggerProcessorTool = MMTriggerProcessorTool

    # no MM trigger for this moment
    nsw.MMStripTdsTool = ""
    nsw.MMTriggerTool = ""
    nsw.DoMM=False
    nsw.DoMMDiamonds=True

    # sTGC pad trigger configuration
    nsw.DosTGC=True
    nsw.UseLookup=False #use lookup table for the pad trigger
    nsw.NSWTrigRDOContainerName="NSWTRGRDO"
    nsw.StripSegmentTool.rIndexScheme=0

    return nsw

def RecoMuonSegmentSequence(flags):
    postFix = "_L1MuonSim"
    muonLayerHoughAlg = CompFactory.MuonLayerHoughAlg("MuonLayerHoughAlg" + postFix
                                                      ,TgcPrepDataContainer = "TGC_Measurements"
                                                      ,RpcPrepDataContainer = "RPC_Measurements"
                                                      ,CscPrepDataContainer = "CSC_Clusters"
                                                      ,MdtPrepDataContainer = "MDT_DriftCircles"
                                                      ,sTgcPrepDataContainer = "" # "STGC_Measurements"
                                                      ,MMPrepDataContainer = "" # "MM_Measurements"
                                                  )
    mdtCondDb = CompFactory.MdtCondDbAlg("MdtCondDbAlg" + postFix
                                         , isOnline = False
                                         , isData = not flags.Input.isMC
                                         , isRun1 = False
                                     )
    from IOVDbSvc.CondDB import conddb
    if flags.Input.isMC:
        conddb.addFolder("DCS_OFL", "/MDT/DCS/DROPPEDCH",className='CondAttrListCollection')
        conddb.addFolder("DCS_OFL", "/MDT/DCS/PSLVCHSTATE",className='CondAttrListCollection')
    else:
        conddb._SetAcc('DCS_OFL','COOLOFL_DCS')
        conddb.addFolder("DCS_OFL", "/MDT/DCS/HV",className='CondAttrListCollection')
        conddb.addFolder("DCS_OFL", "/MDT/DCS/LV",className='CondAttrListCollection')
    theSegmentFinderAlg = CompFactory.MuonSegmentFinderAlg("MuonSegmentFinderAlg" + postFix)
    xAODMuonSegmentCnv = CompFactory.xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg" + postFix)
    from AthenaCommon.CFElements import seqAND
    recoMuonSegment = seqAND( "MuSegRecSeqForL1Muon", [muonLayerHoughAlg,mdtCondDb,theSegmentFinderAlg, xAODMuonSegmentCnv] )
    return recoMuonSegment

def MuonBytestream2RdoSequence(flags):
    from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheNames
    MuonCacheCreator=CompFactory.MuonCacheCreator
    cacheCreator = MuonCacheCreator(MdtCsmCacheKey = MuonCacheNames.MdtCsmCache,
                                    CscCacheKey    = MuonCacheNames.CscCache if flags.Detector.GeometryCSC else "",
                                    RpcCacheKey    = MuonCacheNames.RpcCache,
                                    TgcCacheKey    = MuonCacheNames.TgcCache)
    
    postFix = "_L1MuonSim"
    # for MDT
    MDTRodDecoder = CompFactory.MdtROD_Decoder(name = "MdtROD_Decoder" + postFix)
    MuonMdtRawDataProviderTool = CompFactory.Muon__MDT_RawDataProviderToolMT(name = "MDT_RawDataProviderToolMT" + postFix,
                                                                             CsmContainerCacheKey = MuonCacheNames.MdtCsmCache,
                                                                             Decoder = MDTRodDecoder )
    MdtRawDataProvider = CompFactory.Muon__MdtRawDataProvider(name = "MdtRawDataProvider" + postFix,
                                                              ProviderTool = MuonMdtRawDataProviderTool)
    # for RPC
    RPCRodDecoder = CompFactory.Muon__RpcROD_Decoder(name = "RpcROD_Decoder" + postFix)
    MuonRpcRawDataProviderTool = CompFactory.Muon__RPC_RawDataProviderToolMT(name = "RPC_RawDataProviderToolMT" + postFix,
                                                                             RpcContainerCacheKey = MuonCacheNames.RpcCache,
                                                                             WriteOutRpcSectorLogic = False,
                                                                             Decoder = RPCRodDecoder )
    RpcRawDataProvider = CompFactory.Muon__RpcRawDataProvider(name = "RpcRawDataProvider" + postFix,
                                                              ProviderTool = MuonRpcRawDataProviderTool)
    # for TGC
    TGCRodDecoder = CompFactory.Muon__TGC_RodDecoderReadout(name = "TGC_RodDecoderReadout" + postFix)
    MuonTgcRawDataProviderTool = CompFactory.Muon__TGC_RawDataProviderToolMT(name = "TGC_RawDataProviderToolMT" + postFix,
                                                                             TgcContainerCacheKey = MuonCacheNames.TgcCache,
                                                                             Decoder = TGCRodDecoder )
    TgcRawDataProvider = CompFactory.Muon__TgcRawDataProvider(name = "TgcRawDataProvider" + postFix,
                                                              ProviderTool = MuonTgcRawDataProviderTool)

    bs2rdoSeq = [cacheCreator,
                 RpcRawDataProvider,
                 TgcRawDataProvider,
                 MdtRawDataProvider]
    # for CSC
    if flags.Detector.GeometryCSC:
        CSCRodDecoder = CompFactory.Muon__CscROD_Decoder(name = "CscROD_Decoder" + postFix,
                                                         IsCosmics = False,
                                                         IsOldCosmics = False )
        MuonCscRawDataProviderTool = CompFactory.Muon__CSC_RawDataProviderToolMT(name = "CSC_RawDataProviderToolMT" + postFix,
                                                                                 CscContainerCacheKey = MuonCacheNames.CscCache,
                                                                                 Decoder = CSCRodDecoder )
        CscRawDataProvider = CompFactory.Muon__CscRawDataProvider(name = "CscRawDataProvider" + postFix,
                                                                  ProviderTool = MuonCscRawDataProviderTool)
        bs2rdoSeq+=CscRawDataProvider

    from AthenaCommon.CFElements import seqAND
    muonBS2RDO = seqAND( "MuonBs2RdoSeqForL1Muon", bs2rdoSeq )
    return muonBS2RDO
    
def MuonRdo2PrdSequence(flags):
    postFix = "_L1MuonSim"
    ### CSC RDO data ###
    CscRdoToCscPrepDataTool = CompFactory.Muon__CscRdoToCscPrepDataTool(name = "CscRdoToCscPrepDataTool" + postFix)
    CscRdoToCscPrepData = CompFactory.CscRdoToCscPrepData(name = "CscRdoToCscPrepData" + postFix,
                                                          CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool)
    CscClusterBuilderTool = CompFactory.CscThresholdClusterBuilderTool(name = "CscThresholdClusterBuilderTool" + postFix)
    CscClusterBuilder = CompFactory.CscThresholdClusterBuilder(name = "CscThresholdClusterBuilder"+postFix,
                                                               cluster_builder = CscClusterBuilderTool)
    ### MDT RDO data ###
    MdtRdoToMdtPrepDataTool = CompFactory.Muon__MdtRdoToPrepDataTool(name = "MdtRdoToPrepDataTool" + postFix)
    MdtRdoToMdtPrepData = CompFactory.MdtRdoToMdtPrepData(name = "MdtRdoToMdtPrepData" + postFix,
                                                          DecodingTool = MdtRdoToMdtPrepDataTool)
    ### RPC RDO data ###
    RpcRdoToRpcPrepDataTool = CompFactory.Muon__RpcRdoToPrepDataTool(name = "RpcRdoToPrepDataTool" + postFix)
    RpcRdoToRpcPrepData = CompFactory.RpcRdoToRpcPrepData(name = "RpcRdoToRpcPrepData" + postFix,
                                                          DecodingTool = RpcRdoToRpcPrepDataTool)
    
    ### TGC RDO data ###
    TgcRdoToTgcPrepDataTool = CompFactory.Muon__TgcRdoToPrepDataTool(name = "TgcRdoToPrepDataTool" + postFix)
    TgcRdoToTgcPrepData = CompFactory.TgcRdoToTgcPrepData(name = "TgcRdoToTgcPrepData" + postFix,
                                                          DecodingTool = TgcRdoToTgcPrepDataTool)
    
    from AthenaCommon.CFElements import seqAND
    muonRDO2PRD = seqAND( "MuonRdo2PrdSeqForL1Muon", [
        CscRdoToCscPrepData,
        CscClusterBuilder,
        MdtRdoToMdtPrepData,
        RpcRdoToRpcPrepData,
        TgcRdoToTgcPrepData ])

    return muonRDO2PRD

def Lvl1MuRdo2Digit(flags):
    MuonRdoToMuonDigitTool = CompFactory.MuonRdoToMuonDigitTool (DecodeMdtRDO = False,
                                                                 DecodeRpcRDO = True,
                                                                 DecodeTgcRDO = True,
                                                                 DecodeCscRDO = False,
                                                                 DecodeSTGC_RDO = False,
                                                                 DecodeMM_RDO = False,
                                                                 mdtRdoDecoderTool="",
                                                                 cscRdoDecoderTool="",
                                                                 stgcRdoDecoderTool="",
                                                                 mmRdoDecoderTool="",
                                                                 RpcDigitContainer = "RPC_DIGITS_L1",
                                                                 TgcDigitContainer = "TGC_DIGITS_L1")
    MuonRdoToMuonDigitTool.cscCalibTool = ""
    rdo2digit = CompFactory.MuonRdoToMuonDigit( "MuonRdoToMuonDigit",
                                                MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool)
    return rdo2digit
    
def TGCTriggerConfig(flags):
    tmdbInput = "rerunTileMuRcvCnt"
    tgc = CompFactory.LVL1TGCTrigger__LVL1TGCTrigger("LVL1TGCTrigger",
                                                     InputData_perEvent  = "TGC_DIGITS_L1",
                                                     useRun3Config = True,
                                                     TileMuRcv_Input = tmdbInput )

    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    if (MuonGeometryFlags.hasSTGC() or MuonGeometryFlags.hasMM()) and flags.Input.isMC:
        tgc.MaskFileName12 = "TrigT1TGCMaskedChannel.noFI._12.db"
        tgc.USENSW = True
        tgc.NSWSideInfo = "AC"
        tgc.NSWTrigger_Input = "NSWTRGRDO"
        tgc.FORCENSWCOIN = not flags.Trigger.L1MuonSim.NSWVetoMode
    else:
        tgc.MaskFileName12 = "TrigT1TGCMaskedChannel._12.db"

    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if flags.Trigger.L1MuonSim.EmulateNSWA or flags.Trigger.L1MuonSim.EmulateNSWC:
        tgc.MuctpiPhase1LocationTGC = "L1MuctpiStoreTGCint"
    tgc.TILEMU = True
    condSeq += CompFactory.TGCTriggerCondAlg()
    from PathResolver import PathResolver
    bwCW_Run3_filePath=PathResolver.FindCalibFile("TrigT1TGC_CW/BW/CW_BW_Run3.v01.db")
    conddb.blockFolder("/TGC/TRIGGER/CW_BW_RUN3")
    conddb.addFolder(bwCW_Run3_filePath,"/TGC/TRIGGER/CW_BW_RUN3 <tag>TgcTriggerCwBwRun3-01</tag>",forceMC=True,force=True, className='CondAttrListCollection')

    return tgc

def TGCModifierConfig(flags):
    recTool = CompFactory.LVL1__TrigT1TGCRecRoiTool("TrigT1TGCRecRoiToolLegacy")
    recTool.UseRun3Config=False # this is intentional
    tgcModifier = CompFactory.LVL1TGCTrigger__TGCOutputModifier("TGCOutputModifier",
                                                                TrigT1TGCRecRoiTool=recTool,
                                                                InputMuctpiLocation = "L1MuctpiStoreTGCint",
                                                                OutputMuctpiLocation = "L1MuctpiStoreTGC",
                                                                EmulateA = flags.Trigger.L1MuonSim.EmulateNSWA,
                                                                EmulateC = flags.Trigger.L1MuonSim.EmulateNSWC,
                                                                NSWVetoMode = flags.Trigger.L1MuonSim.NSWVetoMode )
    return tgcModifier

def Lvl1EndcapMuonSequence(flags):
    tmdb = TMDBSimulationSequence(flags)
    tgc = TGCTriggerConfig(flags)
    from AthenaCommon.CFElements import seqAND
    if flags.Trigger.L1MuonSim.EmulateNSWA or flags.Trigger.L1MuonSim.EmulateNSWC:
        if flags.Trigger.L1MuonSim.NSWVetoMode:
            rdo2prd = MuonRdo2PrdSequence(flags)
            recoSegment = RecoMuonSegmentSequence(flags)
            tgcmod = TGCModifierConfig(flags)
            l1MuEndcapSim = seqAND("L1MuonEndcapSim", [tmdb,tgc,rdo2prd,recoSegment,tgcmod] )
        else:
            tgcmod = TGCModifierConfig(flags)
            l1MuEndcapSim = seqAND("L1MuonEndcapSim", [tmdb,tgc,tgcmod] )
    else:
        from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
        if (MuonGeometryFlags.hasSTGC() or MuonGeometryFlags.hasMM()) and flags.Input.isMC:
            nsw = NSWTriggerSequence(flags)
            l1MuEndcapSim = seqAND("L1MuonEndcapSim", [tmdb,nsw,tgc] )
        else:
            l1MuEndcapSim = seqAND("L1MuonEndcapSim", [tmdb,tgc] )
    return l1MuEndcapSim

def Lvl1BarrelMuonSequence(flags):
    rpc = CompFactory.TrigT1RPC("TrigT1RPC",
                                Hardware          = True,
                                DataDetail        = False,
                                RPCbytestream     = False,
                                RPCbytestreamFile = "",
                                RPCDigitContainer = "RPC_DIGITS_L1",
                                useRun3Config = True )

    # trigger roads setting is configured in either MuonCnvExample.MuonCablingConfig or MuonConfig.MuonCablingConfig

    from AthenaCommon.CFElements import seqAND
    l1MuBarrelSim = seqAND("L1MuonBarrelSim", [rpc] )
    return l1MuBarrelSim

def Lvl1MuctpiConfig(flags):
    rpcRecRoiTool = CompFactory.LVL1__TrigT1RPCRecRoiTool("TrigT1RPCRecRoiTool", UseRun3Config=True)
    tgcRecRoiTool = CompFactory.LVL1__TrigT1TGCRecRoiTool("TrigT1TGCRecRoiTool", UseRun3Config=True)
    muctpiTool = CompFactory.LVL1MUCTPIPHASE1__MUCTPI_AthTool(name="MUCTPI_AthTool")
    muctpiTool.MuCTPICTPLocation = 'L1MuCTPItoCTPLocation'
    muctpiTool.OverlapStrategyName = 'LUT'
    muctpiTool.LUTXMLFile = 'TrigConfMuctpi/overlapRun3_20201214.xml'
    muctpiTool.InputSource = 'DIGITIZATION'
    muctpiTool.RPCRecRoiTool = rpcRecRoiTool
    muctpiTool.TGCRecRoiTool = tgcRecRoiTool
    muctpiTool.TrigThresholdDecisionTool = CompFactory.LVL1__TrigThresholdDecisionTool(name="TrigThresholdDecisionTool")
    muctpiTool.TrigThresholdDecisionTool.RPCRecRoiTool = rpcRecRoiTool
    muctpiTool.TrigThresholdDecisionTool.TGCRecRoiTool = tgcRecRoiTool
    muctpi = CompFactory.LVL1MUCTPIPHASE1__MUCTPI_AthAlg(name="MUCTPI_AthAlg",
                                                         MUCTPI_AthTool = muctpiTool)
    return muctpi

def Lvl1MuonSimulationSequence(flags):
    rdo2digit = Lvl1MuRdo2Digit(flags)
    l1Barrel = Lvl1BarrelMuonSequence(flags)
    l1Endcap = Lvl1EndcapMuonSequence(flags)
    l1Muctpi = Lvl1MuctpiConfig(flags)
    from AthenaCommon.CFElements import seqAND
    if not flags.Input.isMC:
        bs2rdo = MuonBytestream2RdoSequence(flags)
        l1MuonSim = seqAND("L1MuonSimSeq", [bs2rdo, rdo2digit, l1Barrel, l1Endcap, l1Muctpi] )
    else:
        l1MuonSim = seqAND("L1MuonSimSeq", [rdo2digit, l1Barrel, l1Endcap, l1Muctpi] )
    return l1MuonSim
