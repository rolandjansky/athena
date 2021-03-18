# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def Lvl1MCMuonSimulationCfg(flags):
    acc = ComponentAccumulator()

    #TODO make sure that the RdoToDigit does not not depend on all muon detectors
    #    acc.addCondAlgo(CompFactory.CscCondDbAlg())

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
                                        RPCDigitContainer = "RPC_DIGITS_L1")
    acc.addEventAlgo(rpcL1Alg)

    #TGC
    from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
    acc.merge( TGCCablingConfigCfg(flags) )

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    # only needed for MC
    acc.merge(addFolders(flags, ["/TGC/TRIGGER/CW_EIFI", "/TGC/TRIGGER/CW_BW", "/TGC/TRIGGER/CW_TILE"],
                        "TGC_OFL", className="CondAttrListCollection"))

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
                                                            MaskFileName12      = "TrigT1TGCMaskedChannel._12.db")
    acc.addEventAlgo(tgcL1Alg)
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    acc.merge(L1ConfigSvcCfg(flags))

    # based on: Trigger/TrigT1/TrigT1Muctpi/python/TrigT1MuctpiConfig.py
    muctpiAlg = CompFactory.LVL1MUCTPI.L1Muctpi("L1Muctpi", 
                                                OverlapStrategyName = "LUT",
                                                DumpLUT = False,
                                                FlaggingMode = False,
                                                MultiplicityStrategyName = "INCLUSIVE",
                                                # TODO this value depends on the flags on original however seem always to default to this two settings below
                                                # is definitely different for Run3 
                                                LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB.xml",
                                                RunPeriod = "RUN2",
                                                GeometryXMLFile = "TrigConfMuctpi/TestMioctGeometry_2016_05_30_CS_2600573263.xml",
                                                DoNIMOutput = True,
                                                NIMBarrelBit = 29,
                                                NIMEndcapBit = 30,
                                                LVL1ConfigSvc = acc.getService("LVL1ConfigSvc"))

    acc.addEventAlgo(muctpiAlg)

    return acc

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.MainServicesConfig import  MainServicesCfg
    
    flags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1']
    flags.Common.isOnline=False
    flags.Exec.MaxEvents=25
    flags.Concurrency.NumThreads = 1
    flags.Concurrency.NumConcurrentEvents=1
    flags.Scheduler.ShowDataDeps=True
    flags.Scheduler.CheckDependencies=True
    flags.Scheduler.ShowDataFlow=True

    acc = MainServicesCfg(flags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))
    from AthenaCommon.CFElements import seqAND

    acc.addSequence(seqAND("L1MuonSim"))
    acc.merge(Lvl1MCMuonSimulationCfg(flags), sequenceName="L1MuonSim")
    from AthenaCommon.Constants import DEBUG
    acc.foreach_component("*/L1MuonSim/*").OutputLevel = DEBUG
    acc.printConfig(withDetails=True, summariseProps=True)

    acc.run()
