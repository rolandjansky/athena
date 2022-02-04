# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders

def TMDBConfig(flags):
    acc = ComponentAccumulator()
    if not flags.Input.isMC:
        from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
        acc.merge(ByteStreamReadCfg(flags, ["TileRawChannelContainer/MuRcvRawChCnt"]))

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
    emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )

    tmdbAlg = CompFactory.TileMuonReceiverDecision('TileMuonReceiverDecision'
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
                                                   , MuonReceiverEneThreshCellD5andD6 = 500
                                                   , TileCondToolEmscale = emScaleTool)
    acc.addEventAlgo(tmdbAlg)
    return acc

def MuonBytestream2RdoConfig(flags):
    acc = ComponentAccumulator()
    if flags.Input.isMC:
        return acc
    from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheNames
    MuonCacheCreator=CompFactory.MuonCacheCreator
    cacheCreator = MuonCacheCreator(MdtCsmCacheKey = MuonCacheNames.MdtCsmCache,
                                    CscCacheKey    = MuonCacheNames.CscCache if flags.Detector.GeometryCSC else "",
                                    RpcCacheKey    = MuonCacheNames.RpcCache,
                                    TgcCacheKey    = MuonCacheNames.TgcCache)
    acc.addEventAlgo(cacheCreator)
    postFix = "_L1MuonSim"
    # for MDT
    MDTRodDecoder = CompFactory.MdtROD_Decoder(name = "MdtROD_Decoder" + postFix)
    MuonMdtRawDataProviderTool = CompFactory.Muon.MDT_RawDataProviderToolMT(name = "MDT_RawDataProviderToolMT" + postFix,
                                                                             CsmContainerCacheKey = MuonCacheNames.MdtCsmCache,
                                                                             Decoder = MDTRodDecoder )
    MdtRawDataProvider = CompFactory.Muon.MdtRawDataProvider(name = "MdtRawDataProvider" + postFix,
                                                              ProviderTool = MuonMdtRawDataProviderTool)
    acc.addEventAlgo(MdtRawDataProvider)
    # for RPC
    RPCRodDecoder = CompFactory.Muon.RpcROD_Decoder(name = "RpcROD_Decoder" + postFix)
    MuonRpcRawDataProviderTool = CompFactory.Muon.RPC_RawDataProviderToolMT(name = "RPC_RawDataProviderToolMT" + postFix,
                                                                             RpcContainerCacheKey = MuonCacheNames.RpcCache,
                                                                             WriteOutRpcSectorLogic = False,
                                                                             Decoder = RPCRodDecoder )
    RpcRawDataProvider = CompFactory.Muon.RpcRawDataProvider(name = "RpcRawDataProvider" + postFix,
                                                              ProviderTool = MuonRpcRawDataProviderTool)
    acc.addEventAlgo(RpcRawDataProvider)
    # for TGC
    TGCRodDecoder = CompFactory.Muon.TGC_RodDecoderReadout(name = "TGC_RodDecoderReadout" + postFix)
    MuonTgcRawDataProviderTool = CompFactory.Muon.TGC_RawDataProviderToolMT(name = "TGC_RawDataProviderToolMT" + postFix,
                                                                             TgcContainerCacheKey = MuonCacheNames.TgcCache,
                                                                             Decoder = TGCRodDecoder )
    TgcRawDataProvider = CompFactory.Muon.TgcRawDataProvider(name = "TgcRawDataProvider" + postFix,
                                                              ProviderTool = MuonTgcRawDataProviderTool)
    acc.addEventAlgo(TgcRawDataProvider)
    # for CSC
    if flags.Detector.GeometryCSC:
        CSCRodDecoder = CompFactory.Muon.CscROD_Decoder(name = "CscROD_Decoder" + postFix,
                                                        IsCosmics = False,
                                                        IsOldCosmics = False )
        MuonCscRawDataProviderTool = CompFactory.Muon.CSC_RawDataProviderToolMT(name = "CSC_RawDataProviderToolMT" + postFix,
                                                                                CscContainerCacheKey = MuonCacheNames.CscCache,
                                                                                Decoder = CSCRodDecoder )
        CscRawDataProvider = CompFactory.Muon.CscRawDataProvider(name = "CscRawDataProvider" + postFix,
                                                                 ProviderTool = MuonCscRawDataProviderTool)
        acc.addEventAlgo(CscRawDataProvider)

    return acc


def MuonRdo2PrdConfig(flags):
    acc = ComponentAccumulator()
    if not flags.Trigger.L1MuonSim.EmulateNSW:
        return acc
    postFix = "_L1MuonSim"

    ### CSC RDO data ###
    if flags.Detector.GeometryCSC:
        CscRdoToCscPrepDataTool = CompFactory.Muon.CscRdoToCscPrepDataToolMT(name = "CscRdoToCscPrepDataToolMT" + postFix)
        CscRdoToCscPrepData = CompFactory.CscRdoToCscPrepData(name = "CscRdoToCscPrepData" + postFix,
                                                              CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool)
        acc.addEventAlgo(CscRdoToCscPrepData)
        CscClusterBuilderTool = CompFactory.CscThresholdClusterBuilderTool(name = "CscThresholdClusterBuilderTool" + postFix)
        CscClusterBuilder = CompFactory.CscThresholdClusterBuilder(name = "CscThresholdClusterBuilder"+postFix,
                                                                   cluster_builder = CscClusterBuilderTool)
        acc.addEventAlgo(CscClusterBuilder)
    ### MDT RDO data ###
    MdtRdoToMdtPrepDataTool = CompFactory.Muon.MdtRdoToPrepDataToolMT(name = "MdtRdoToPrepDataToolMT" + postFix)
    MdtRdoToMdtPrepData = CompFactory.MdtRdoToMdtPrepData(name = "MdtRdoToMdtPrepData" + postFix,
                                                          DecodingTool = MdtRdoToMdtPrepDataTool)
    acc.addEventAlgo(MdtRdoToMdtPrepData)
    ### RPC RDO data ###
    RpcRdoToRpcPrepDataTool = CompFactory.Muon.RpcRdoToPrepDataToolMT(name = "RpcRdoToPrepDataToolMT" + postFix)
    RpcRdoToRpcPrepData = CompFactory.RpcRdoToRpcPrepData(name = "RpcRdoToRpcPrepData" + postFix,
                                                          DecodingTool = RpcRdoToRpcPrepDataTool)
    acc.addEventAlgo(RpcRdoToRpcPrepData)
    ### TGC RDO data ###
    TgcRdoToTgcPrepDataTool = CompFactory.Muon.TgcRdoToPrepDataToolMT(name = "TgcRdoToPrepDataToolMT" + postFix)
    TgcRdoToTgcPrepData = CompFactory.TgcRdoToTgcPrepData(name = "TgcRdoToTgcPrepData" + postFix,
                                                          DecodingTool = TgcRdoToTgcPrepDataTool)
    acc.addEventAlgo(TgcRdoToTgcPrepData)

    return acc

def RecoMuonSegmentSequence(flags):
    acc = ComponentAccumulator()
    if not flags.Trigger.L1MuonSim.EmulateNSW:
        return acc
    postFix = "_L1MuonSim"
    theMuonLayerHough = CompFactory.MuonLayerHoughAlg("MuonLayerHoughAlg" + postFix,
                                                      TgcPrepDataContainer = "TGC_Measurements",
                                                      RpcPrepDataContainer = "RPC_Measurements",
                                                      CscPrepDataContainer = ("CSC_Clusters" if flags.Detector.GeometryCSC else ""),
                                                      MdtPrepDataContainer = "MDT_DriftCircles",
                                                      sTgcPrepDataContainer = ("STGC_Measurements" if flags.Detector.GeometrysTGC else ""),
                                                      MMPrepDataContainer = ("MM_Measurements" if flags.Detector.GeometryMM else "") )
    acc.addEventAlgo(theMuonLayerHough)
    theSegmentFinderAlg = CompFactory.MuonSegmentFinderAlg("MuonSegmentFinderAlg" + postFix)
    if not flags.Detector.GeometryCSC:
        theSegmentFinderAlg.CSC_clusterkey = ""
        theSegmentFinderAlg.Csc2dSegmentMaker = ""
        theSegmentFinderAlg.Csc4dSegmentMaker = ""
    acc.addEventAlgo(theSegmentFinderAlg)
    xAODMuonSegmentCnv = CompFactory.xAODMaker.MuonSegmentCnvAlg("MuonSegmentCnvAlg" + postFix)
    acc.addEventAlgo(xAODMuonSegmentCnv)
    return acc

def MuonRdo2DigitConfig(flags):
    acc = ComponentAccumulator()
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))
    MuonRdoToMuonDigitTool = CompFactory.MuonRdoToMuonDigitTool (DecodeMdtRDO = False,
                                                                 DecodeRpcRDO = True,
                                                                 DecodeTgcRDO = True,
                                                                 DecodeCscRDO = False,
                                                                 DecodeSTGC_RDO = False,
                                                                 DecodeMM_RDO = False,
                                                                 mdtRdoDecoderTool="",
                                                                 cscRdoDecoderTool="",
                                                                 cscCalibTool = "",
                                                                 stgcRdoDecoderTool="",
                                                                 mmRdoDecoderTool="",
                                                                 RpcDigitContainer = "RPC_DIGITS_L1",
                                                                 TgcDigitContainer = "TGC_DIGITS_L1")
    acc.addPublicTool(MuonRdoToMuonDigitTool)
    rdo2digit = CompFactory.MuonRdoToMuonDigit( "MuonRdoToMuonDigit",
                                                MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool)
    acc.addEventAlgo(rdo2digit)
    return acc

def NSWTriggerConfig(flags):
    acc = ComponentAccumulator()
    if not flags.Detector.GeometrysTGC and not flags.Detector.GeometryMM:
        return acc
    PadTdsTool = CompFactory.NSWL1.PadTdsOfflineTool("NSWL1__PadTdsOfflineTool",DoNtuple=False)
    PadTriggerLogicTool = CompFactory.NSWL1.PadTriggerLogicOfflineTool("NSWL1__PadTriggerLogicOfflineTool",DoNtuple=False)
    PadTriggerLookupTool = CompFactory.NSWL1.PadTriggerLookupTool("NSWL1__PadTriggerLookupTool")
    StripTdsTool = CompFactory.NSWL1.StripTdsOfflineTool("NSWL1__StripTdsOfflineTool",DoNtuple=False)
    MMStripTdsTool = CompFactory.NSWL1.MMStripTdsOfflineTool("NSWL1__MMStripTdsOfflineTool",DoNtuple=False)
    MMTriggerTool = CompFactory.NSWL1.MMTriggerTool("NSWL1__MMTriggerTool",DoNtuple=False)
    MMTriggerProcessorTool = CompFactory.NSWL1.TriggerProcessorTool("NSWL1__TriggerProcessorTool")
    nswAlg = CompFactory.NSWL1.NSWL1Simulation("NSWL1Simulation",
                                               DoOffline = True, # so far only offline simulation is available
                                               UseLookup = False,
                                               DoNtuple = False,
                                               DoMM = flags.Trigger.L1MuonSim.doMMTrigger,
                                               DoMMDiamonds = flags.Trigger.L1MuonSim.doMMTrigger,
                                               DosTGC = True, # sTGC pad-only trigger: default
                                               PadTdsTool = PadTdsTool,
                                               PadTriggerTool = PadTriggerLogicTool,
                                               PadTriggerLookupTool = PadTriggerLookupTool,
                                               StripTdsTool = StripTdsTool,
                                               MMStripTdsTool = MMStripTdsTool,
                                               MMTriggerTool = MMTriggerTool,
                                               MMTriggerProcessorTool = MMTriggerProcessorTool,
                                               NSWTrigRDOContainerName = "NSWTRGRDO" )
    acc.addEventAlgo(nswAlg)
    return acc

def RPCTriggerConfig(flags):
    acc = ComponentAccumulator()
    rpcAlg = CompFactory.TrigT1RPC("TrigT1RPC",
                                Hardware          = True,
                                DataDetail        = False,
                                RPCbytestream     = False,
                                RPCbytestreamFile = "",
                                RPCDigitContainer = "RPC_DIGITS_L1",
                                useRun3Config = True )
    acc.addEventAlgo(rpcAlg)
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge( RPCCablingConfigCfg(flags) ) # trigger roads
    return acc

def TGCTriggerConfig(flags):
    acc = ComponentAccumulator()
    tgcAlg = CompFactory.LVL1TGCTrigger.LVL1TGCTrigger("LVL1TGCTrigger",
                                                       InputData_perEvent  = "TGC_DIGITS_L1",
                                                       useRun3Config = True,
                                                       TileMuRcv_Input = "rerunTileMuRcvCnt",
                                                       TILEMU = True)
    if (flags.Detector.GeometrysTGC or flags.Detector.GeometryMM) and flags.Input.isMC:
        tgcAlg.MaskFileName12 = "TrigT1TGCMaskedChannel.noFI._12.db"
        tgcAlg.USENSW = True
        tgcAlg.NSWSideInfo = "AC"
        tgcAlg.NSWTrigger_Input = "NSWTRGRDO"
        tgcAlg.FORCENSWCOIN = not flags.Trigger.L1MuonSim.NSWVetoMode
    else:
        tgcAlg.MaskFileName12 = "TrigT1TGCMaskedChannel._12.db"

    if flags.Trigger.L1MuonSim.EmulateNSW:
        tgcAlg.MuctpiPhase1LocationTGC = "L1MuctpiStoreTGCint"

    acc.addEventAlgo(tgcAlg)
    from PathResolver import PathResolver
    bwCW_Run3_filePath=PathResolver.FindCalibFile("TrigT1TGC_CW/BW/CW_BW_Run3.v01.db")
    acc.merge(addFolders(flags, '<db>sqlite://;schema={0};dbname=OFLP200</db> /TGC/TRIGGER/CW_BW_RUN3'.format(bwCW_Run3_filePath),
                                tag='TgcTriggerCwBwRun3-01',
                                className='CondAttrListCollection'))
    acc.addCondAlgo(CompFactory.TGCTriggerCondAlg())
    from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
    acc.merge( TGCCablingConfigCfg(flags) )
    return acc

def TGCModifierConfig(flags):
    acc = ComponentAccumulator()
    if not flags.Trigger.L1MuonSim.EmulateNSW:
        return acc
    recTool = CompFactory.LVL1.TrigT1TGCRecRoiTool("TrigT1TGCRecRoiToolLegacy")
    recTool.UseRun3Config=False # this is intentional
    tgcModifier = CompFactory.LVL1TGCTrigger.TGCOutputModifier("TGCOutputModifier",
                                                                TrigT1TGCRecRoiTool=recTool,
                                                                InputMuctpiLocation = "L1MuctpiStoreTGCint",
                                                                OutputMuctpiLocation = "L1MuctpiStoreTGC",
                                                                EmulateA = True,
                                                                EmulateC = True,
                                                                NSWVetoMode = flags.Trigger.L1MuonSim.NSWVetoMode )
    acc.addEventAlgo(tgcModifier)
    return acc

def MuctpiConfig(flags):
    acc = ComponentAccumulator()
    rpcRecRoiTool = CompFactory.LVL1.TrigT1RPCRecRoiTool("TrigT1RPCRecRoiTool", UseRun3Config=True)
    tgcRecRoiTool = CompFactory.LVL1.TrigT1TGCRecRoiTool("TrigT1TGCRecRoiTool", UseRun3Config=True)
    trigThresholdDecTool = CompFactory.LVL1.TrigThresholdDecisionTool(name="TrigThresholdDecisionTool",
                                                                       RPCRecRoiTool = rpcRecRoiTool,
                                                                       TGCRecRoiTool = tgcRecRoiTool)
    muctpiTool = CompFactory.LVL1MUCTPIPHASE1.MUCTPI_AthTool(name="MUCTPI_AthTool",
                                                              MuCTPICTPLocation = 'L1MuCTPItoCTPLocation',
                                                              OverlapStrategyName = 'LUT',
                                                              LUTXMLFile = 'TrigConfMuctpi/overlapRun3_20201214.xml',
                                                              InputSource = 'DIGITIZATION',
                                                              RPCRecRoiTool = rpcRecRoiTool,
                                                              TGCRecRoiTool = tgcRecRoiTool,
                                                              TrigThresholdDecisionTool = trigThresholdDecTool)
    muctpiAlg = CompFactory.LVL1MUCTPIPHASE1.MUCTPI_AthAlg(name="MUCTPI_AthAlg",
                                                         MUCTPI_AthTool = muctpiTool)
    acc.addEventAlgo(muctpiAlg)
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    acc.merge(L1ConfigSvcCfg(flags))
    return acc


def Lvl1MuonSimulationCfg(flags):
    acc = ComponentAccumulator()

    acc.merge(MuonBytestream2RdoConfig(flags)) # data prep for muon bytestream data
    acc.merge(MuonRdo2DigitConfig(flags)) # input for rpc/tgc trigger simulation
    acc.merge(RPCTriggerConfig(flags)) # rpc trigger simulation, including bis78 to prepare for bis78-tgc coincidence
    acc.merge(TMDBConfig(flags)) # for tmdb decision to prepare for tile-muon coincidence
    acc.merge(NSWTriggerConfig(flags)) # nsw trigger simulation to prepare input for nsw-tgc coincidence
    acc.merge(TGCTriggerConfig(flags)) # tgc trigger simulation
    acc.merge(MuonRdo2PrdConfig(flags)) # data prep for nsw-tgc coincidence emulator
    acc.merge(RecoMuonSegmentSequence(flags)) # segment reco for nsw-tgc coincidence emulator
    acc.merge(TGCModifierConfig(flags)) # overwrite output from tgc by nsw-tgc coincidence emulator
    acc.merge(MuctpiConfig(flags)) # muctpi simulation

    return acc

if __name__ == "__main__":
    import sys
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    flags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1']
    flags.Common.isOnline=False
    flags.Exec.MaxEvents=25
    flags.Concurrency.NumThreads = 1
    flags.Concurrency.NumConcurrentEvents=1
    flags.Scheduler.ShowDataDeps=True
    flags.Scheduler.CheckDependencies=True
    flags.Scheduler.ShowDataFlow=True
    flags.Trigger.enableL1MuonPhase1=True
    flags.Trigger.triggerMenuSetup='Dev_pp_run3_v1'
    flags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))

    from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu
    generateL1Menu(flags)

    acc.merge(Lvl1MuonSimulationCfg(flags))

    acc.printConfig(withDetails=True, summariseProps=True, printDefaults=True)

    sys.exit(acc.run().isFailure())
