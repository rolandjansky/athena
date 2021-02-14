#!/usr/bin/env python
"""Run tests for FastCaloSimServices configuration

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""


def FCSServicesMainCfg(flags):
    """Configure event loop for FCSServices"""
    acc = MainServicesCfg(flags)
    if flags.Concurrency.NumThreads > 0:
        AthenaHiveEventLoopMgr = CompFactory.AthenaHiveEventLoopMgr
        elmgr = AthenaHiveEventLoopMgr()
    else:
        AthenaEventLoopMgr = CompFactory.AthenaEventLoopMgr
        elmgr = AthenaEventLoopMgr()
    elmgr.UseSecondaryEventNumber = True
    acc.addService(elmgr)
    return acc


def FastCaloSimServicesMainCfg(ConfigFlags):
    """Main FCSServices configuration"""

    # Construct our accumulator to run
    acc = FCSServicesMainCfg(ConfigFlags)
    acc.merge(PoolReadCfg(ConfigFlags))
    acc.merge(PoolWriteCfg(ConfigFlags))

    # Add TileInfoLoader
    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge(TileInfoLoaderCfg(ConfigFlags))

    # Add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    acc.merge(BeamEffectsAlgCfg(ConfigFlags))

    # Add Kernel_G4FastCaloMT from ISF_MainConfig
    from ISF_Config.ISF_MainConfigNew import Kernel_G4FastCaloMT
    acc.merge(Kernel_G4FastCaloMT(ConfigFlags))

    ItemList = ["EventInfo#*",
                "McEventCollection#TruthEvent",
                "JetCollection#*"]

    if ConfigFlags.Sim.IncludeParentsInG4Event:
        ItemList += ["McEventCollection#GEN_EVENT"]

    ItemList += ["xAOD::JetContainer#*",
                 "xAOD::JetAuxContainer#*"]

    if ConfigFlags.Detector.SimulateID:
        ItemList += ["SiHitCollection#*",
                     "TRTUncompressedHitCollection#*",
                     "TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.SimulateITk:
        ItemList += ["SiHitCollection#*",
                     "TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.SimulateCalo:
        ItemList += ["CaloCalibrationHitContainer#*",
                     "LArHitContainer#*",
                     "TileHitVector#*",
                     "TrackRecordCollection#MuonEntryLayer"]

    if ConfigFlags.Detector.SimulateMuon:
        ItemList += ["RPCSimHitCollection#*",
                     "TGCSimHitCollection#*",
                     "MDTSimHitCollection#*",
                     "TrackRecordCollection#MuonExitLayer"]
        if ConfigFlags.Detector.GeometryCSC:
            ItemList += ["CSCSimHitCollection#*"]
        if ConfigFlags.Detector.GeometrysTGC:
            ItemList += ["sTGCSimHitCollection#*"]
        if ConfigFlags.Detector.GeometryMM:
            ItemList += ["MMSimHitCollection#*"]

    if ConfigFlags.Detector.SimulateLucid:
        ItemList += ["LUCID_SimHitCollection#*"]

    if ConfigFlags.Detector.SimulateFwdRegion:
        ItemList += ["SimulationHitCollection#*"]

    if ConfigFlags.Detector.SimulateZDC:
        ItemList += ["ZDC_SimPixelHit_Collection#*",
                     "ZDC_SimStripHit_Collection#*"]

    if ConfigFlags.Detector.SimulateALFA:
        ItemList += ["ALFA_HitCollection#*",
                     "ALFA_ODHitCollection#*"]

    if ConfigFlags.Detector.SimulateAFP:
        ItemList += ["AFP_TDSimHitCollection#*",
                     "AFP_SIDSimHitCollection#*"]

    # TimingAlg
    ItemList += ["RecoTimingObj#EVNTtoHITS_timings"]

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge( OutputStreamCfg(ConfigFlags,"HITS", ItemList=ItemList, disableEventTag=True) )


    # FIXME hack to match to buggy behaviour in old style configuration
    OutputStreamHITS = acc.getEventAlgo("OutputStreamHITS")
    OutputStreamHITS.ItemList.remove("xAOD::EventInfo#EventInfo")
    OutputStreamHITS.ItemList.remove("xAOD::EventAuxInfo#EventInfoAux.")

    # FIXME hack because deduplication is broken
    PoolAttributes = ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
    PoolAttributes += ["DatabaseName = '" + ConfigFlags.Output.HITSFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'"]
    acc.getService("AthenaPoolCnvSvc").PoolAttributes += PoolAttributes

    # Dump config
    acc.getService("StoreGateSvc").Dump = True
    acc.getService("ConditionStore").Dump = True
    acc.printConfig(withDetails=True, summariseProps=True)

    return acc

if __name__ == '__main__':

    import sys
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    from ISF_FastCaloSimServices.ISF_FastCaloSimServicesTestHelpers import (
        CommonTestArgumentParser, JobOptsDumperCfg, TestMessageSvcCfg,
        defaultTestFlags, postprocessAndLockFlags, printAndRun
    )


    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentFactory import CompFactory

    # Set up new style config
    Configurable.configurableRun3Behavior = True
    # Configure
    args = CommonTestArgumentParser().parse_args()
    defaultTestFlags(ConfigFlags, args)
    postprocessAndLockFlags(ConfigFlags, args)

    # Construct our accumulator to run
    acc = FastCaloSimServicesMainCfg(ConfigFlags)
    acc.merge(JobOptsDumperCfg(ConfigFlags))
    acc.merge(TestMessageSvcCfg(ConfigFlags))

    # Dump pickle
    with open("FCSServices_Config.pkl", "wb") as f:
        acc.store(f)

    # Print and run
    sys.exit(printAndRun(acc, ConfigFlags, args))
