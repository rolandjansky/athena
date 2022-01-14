# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders

def _TileMuonReceiverDecision(flags):
    acc = ComponentAccumulator()
    from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
    acc.merge(ByteStreamReadCfg(flags, ["TileRawChannelContainer/MuRcvRawChCnt"]))
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
    acc.addEventAlgo(TileMuonReceiverDecision)
    return acc


def _MuonBytestream2RdoSequence(flags):
    acc = ComponentAccumulator()
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

def Lvl1MuonSimulationCfg(flags):
    acc = ComponentAccumulator()

    if not flags.Input.isMC:
        acc.merge(_TileMuonReceiverDecision(flags))
        acc.merge(_MuonBytestream2RdoSequence(flags))

    #TODO make sure that the RdoToDigit does not not depend on all muon detectors
    #    acc.addCondAlgo(CompFactory.CscCondDbAlg())
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))
    rdoToDigitTool = CompFactory.MuonRdoToMuonDigitTool(DecodeMdtRDO = False,
                                                        DecodeRpcRDO = True,
                                                        DecodeTgcRDO = True,
                                                        DecodeCscRDO = False,
                                                        DecodeSTGC_RDO = False,
                                                        DecodeMM_RDO = False,
                                                        # for those subdetectors where the decoding is turned off, no need to create a RDO_Decoder ToolHandle
                                                        mdtRdoDecoderTool="",
                                                        cscRdoDecoderTool="",
                                                        stgcRdoDecoderTool="",
                                                        mmRdoDecoderTool="",
                                                        RpcDigitContainer = "RPC_DIGITS_L1",
                                                        TgcDigitContainer = "TGC_DIGITS_L1", 
                                                        cscCalibTool = ""
                                                        )
    acc.addPublicTool(rdoToDigitTool)
    rdoToDigitAlg = CompFactory.MuonRdoToMuonDigit( "MuonRdoToMuonDigit",
                                                    MuonRdoToMuonDigitTool = rdoToDigitTool)
    acc.addEventAlgo( rdoToDigitAlg)
    # RPC
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge( RPCCablingConfigCfg(flags) )

    rpcL1Alg = CompFactory.TrigT1RPC("TrigT1RPC",
                                     Hardware = True, # not sure if needed, not there in old config, present in JO
                                     DataDetail = False,
                                     RPCbytestream = False,
                                     RPCbytestreamFile = "",
                                     RPCDigitContainer = "RPC_DIGITS_L1",
                                     useRun3Config = True)
    acc.addEventAlgo(rpcL1Alg)

    #TGC
    from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
    acc.merge( TGCCablingConfigCfg(flags) )

    # only needed for MC
    if flags.Input.isMC:
        acc.merge(addFolders(flags, ["/TGC/TRIGGER/CW_EIFI", "/TGC/TRIGGER/CW_BW", "/TGC/TRIGGER/CW_TILE"],
                            "TGC_OFL", className="CondAttrListCollection"))
    else:
        tagName = flags.Trigger.L1MuonSim.CondDBOffline if flags.Trigger.L1MuonSim.CondDBOffline != '' else "OFLCOND-MC16-SDR-RUN2-04"

        acc.merge(addFolders(flags, ["<db>COOLOFL_TGC/OFLP200</db> /TGC/TRIGGER/CW_EIFI",
                                     "<db>COOLOFL_TGC/OFLP200</db> /TGC/TRIGGER/CW_BW",
                                     "<db>COOLOFL_TGC/OFLP200</db> /TGC/TRIGGER/CW_TILE"], 
                             tag=tagName, className="CondAttrListCollection"))
    #COOL DB will be used.
    from PathResolver import PathResolver
    bwCW_Run3_filePath=PathResolver.FindCalibFile("TrigT1TGC_CW/BW/CW_BW_Run3.v01.db")
    acc.merge(addFolders(flags, '<db>sqlite://;schema={0};dbname=OFLP200</db> /TGC/TRIGGER/CW_BW_RUN3'.format(bwCW_Run3_filePath),
                                tag='TgcTriggerCwBwRun3-01',
                                className='CondAttrListCollection'))

#    acc.merge(addFolders(flags, "/TGC/TRIGGER/CW_BW_RUN3 <tag>TgcTriggerCwBwRun3-01</tag>", db=bwCW_Run3_filePath, className="CondAttrListCollection"))
#    conddb.addFolder(bwCW_Run3_filePath,"/TGC/TRIGGER/CW_BW_RUN3 <tag>TgcTriggerCwBwRun3-01</tag>", className='CondAttrListCollection')

    acc.addCondAlgo(CompFactory.TGCTriggerDbAlg())
    acc.addCondAlgo(CompFactory.TGCTriggerCondAlg())


    tgcL1Alg = CompFactory.LVL1TGCTrigger.LVL1TGCTrigger("LVL1TGCTrigger",
                                                         InputData_perEvent  = "TGC_DIGITS_L1",
                                                         MuCTPIInput_TGC     = "L1MuctpiStoreTGC",
                                                         MaskFileName12      = "TrigT1TGCMaskedChannel._12.db",
                                                         useRun3Config = True)
    if not flags.Input.isMC:
        tgcL1Alg.TileMuRcv_Input = "rerunTileMuRcvCnt"

    acc.addEventAlgo(tgcL1Alg)
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    acc.merge(L1ConfigSvcCfg(flags))

    from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import MUCTPI_AthAlgCfg
    acc.merge(MUCTPI_AthAlgCfg(flags))

    return acc

if __name__ == "__main__":
    import sys
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    
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

    acc = MainServicesCfg(flags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))
    from AthenaCommon.CFElements import seqAND

    from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu
    generateL1Menu(flags)

    acc.addSequence(seqAND("L1MuonSim"))
    acc.merge(Lvl1MuonSimulationCfg(flags), sequenceName="L1MuonSim")
    from AthenaCommon.Constants import DEBUG
    acc.foreach_component("*/L1MuonSim/*").OutputLevel = DEBUG   # noqa: ATL900
    acc.printConfig(withDetails=True, summariseProps=True)

    sys.exit(acc.run().isFailure())
