#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import DEBUG, INFO

## Small class to hold the names for cache containers, should help to avoid copy / paste errors
class MuonPrdCacheNames(object):
    MdtCache       = "MdtPrdCache"
    CscCache       = "CscPrdCache"
    CscStripCache  = "CscStripPrdCache"
    RpcCache       = "RpcPrdCache"
    TgcCache       = "TgcPrdCache"
    sTgcCache      = "sTgcPrdCache"
    MmCache        = "MmPrdCache"
    RpcCoinCache   = "RpcCoinCache"
    TgcCoinCache   = "TgcCoinCache"


## This configuration function creates the IdentifiableCaches for PRD
#
# The function returns a ComponentAccumulator which should be loaded first
# If a configuration wants to use the cache, they need to use the same names as defined here
def MuonPrdCacheCfg():
    # Use MuonGeometryFlags to identify which configuration is being used
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

    acc = ComponentAccumulator()

    MuonPRDCacheCreator=CompFactory.MuonPRDCacheCreator
    cacheCreator = MuonPRDCacheCreator(CscStripCacheKey  = (MuonPrdCacheNames.CscStripCache if MuonGeometryFlags.hasCSC() else ""),
                                       MdtCacheKey       = MuonPrdCacheNames.MdtCache,
                                       CscCacheKey       = (MuonPrdCacheNames.CscCache if MuonGeometryFlags.hasCSC() else ""),
                                       RpcCacheKey       = MuonPrdCacheNames.RpcCache,
                                       TgcCacheKey       = MuonPrdCacheNames.TgcCache,
                                       sTgcCacheKey      = (MuonPrdCacheNames.sTgcCache if MuonGeometryFlags.hasSTGC() else ""),
                                       MmCacheKey        = (MuonPrdCacheNames.MmCache if MuonGeometryFlags.hasMM() else ""),
                                       TgcCoinCacheKey   = MuonPrdCacheNames.TgcCoinCache,
                                       RpcCoinCacheKey   = MuonPrdCacheNames.RpcCoinCache,
                                       )

    acc.addEventAlgo( cacheCreator, primary=True )
    return acc


## This configuration function sets up everything for decoding RPC RDO to PRD conversion
#
# The function returns a ComponentAccumulator and the data-converting algorithm, which should be added to the right sequence by the user
def RpcRDODecodeCfg(flags, name="RpcRdoToRpcPrepData", **kwargs):
    acc = ComponentAccumulator()

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # We need the RPC cabling to be setup
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge( RPCCablingConfigCfg(flags))

    # Conditions not needed for online
    if not flags.Common.isOnline:
        from MuonConfig.MuonCondAlgConfig import RpcCondDbAlgCfg
        acc.merge(RpcCondDbAlgCfg(flags))

    # Get the RDO -> PRD tool
    kwargs.setdefault("DecodingTool", CompFactory.Muon.RpcRdoToPrepDataToolMT(name="RpcRdoToRpcPrepDataTool",
                                                                              ReadKey="RpcCondDbData" if not flags.Common.isOnline else "",
                                                                              RpcPrdContainerCacheKey=MuonPrdCacheNames.RpcCache if flags.Muon.MuonTrigger else "",
                                                                              RpcCoinDataContainerCacheKey=MuonPrdCacheNames.RpcCoinCache if flags.Muon.MuonTrigger else ""))

    # add RegSelTool
    from RegionSelector.RegSelToolConfig import regSelTool_RPC_Cfg
    kwargs.setdefault("RegSel_RPC", acc.popToolsAndMerge(regSelTool_RPC_Cfg(flags)))

    if flags.Muon.MuonTrigger:
        # Set the algorithm to RoI mode
        kwargs.setdefault("DoSeededDecoding", True)
        from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
        kwargs.setdefault("RoIs", mapThresholdToL1RoICollection("MU"))

    # Add the RDO -> PRD alorithm
    acc.addEventAlgo(CompFactory.RpcRdoToRpcPrepData(name, **kwargs))
    return acc


def TgcRDODecodeCfg(flags, name="TgcRdoToTgcPrepData", **kwargs):
    acc = ComponentAccumulator()

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # We need the TGC cabling to be setup
    from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
    acc.merge(TGCCablingConfigCfg(flags))

    # Get the RDO -> PRD tool
    kwargs.setdefault("DecodingTool", CompFactory.Muon.TgcRdoToPrepDataToolMT(name="TgcRdoToTgcPrepDataTool"))

    # add RegSelTool
    from RegionSelector.RegSelToolConfig import regSelTool_TGC_Cfg
    kwargs.setdefault("RegSel_TGC", acc.popToolsAndMerge(regSelTool_TGC_Cfg(flags)))

    if flags.Muon.MuonTrigger:
        # Set the algorithm to RoI mode
        kwargs.setdefault("DoSeededDecoding", True)
        from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
        kwargs.setdefault("RoIs", mapThresholdToL1RoICollection("MU"))

    # Add the RDO -> PRD alorithm
    acc.addEventAlgo(CompFactory.TgcRdoToTgcPrepData(name, **kwargs))
    return acc


def StgcRDODecodeCfg(flags, name="StgcRdoToStgcPrepData", **kwargs):
    acc = ComponentAccumulator()

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    kwargs.setdefault("DecodingTool", CompFactory.Muon.sTgcRdoToPrepDataToolMT(name="sTgcRdoToTgcPrepDataTool"))

    # add RegSelTool
    # from RegionSelector.RegSelToolConfig import regSelTool_STGC_Cfg
    # kwargs.setdefault("RegSel_STGC", acc.popToolsAndMerge(regSelTool_STGC_Cfg(flags)))

    # Add the RDO -> PRD alorithm
    acc.addEventAlgo(CompFactory.StgcRdoToStgcPrepData(name, **kwargs))
    return acc


def MMRDODecodeCfg(flags, name="MM_RdoToMM_PrepData", **kwargs):
    acc = ComponentAccumulator()

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    kwargs.setdefault("DecodingTool", CompFactory.Muon.MmRdoToPrepDataToolMT(name="MmRdoToPrepDataTool"))

    # add RegSelTool
    # from RegionSelector.RegSelToolConfig import regSelTool_MM_Cfg
    # kwargs.setdefault("RegSel_MM", acc.popToolsAndMerge(regSelTool_MM_Cfg(flags)))

    # Add the RDO -> PRD alorithm
    acc.addEventAlgo(CompFactory.MM_RdoToMM_PrepData(name, **kwargs))
    return acc


def MdtRDODecodeCfg(flags, name="MdtRdoToMdtPrepData", **kwargs):
    acc = ComponentAccumulator()
    from MuonConfig.MuonCalibrationConfig import MdtCalibrationToolCfg

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # We need the MDT cabling to be setup
    from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg
    acc.merge(MDTCablingConfigCfg(flags))

    # Get the RDO -> PRD tool
    kwargs.setdefault("DecodingTool", CompFactory.Muon.MdtRdoToPrepDataToolMT(name="MdtRdoToMdtPrepDataTool",
                                                                              CalibrationTool=acc.popToolsAndMerge(MdtCalibrationToolCfg(flags))))

    # add RegSelTool
    from RegionSelector.RegSelToolConfig import regSelTool_MDT_Cfg
    kwargs.setdefault("RegSel_MDT", acc.popToolsAndMerge(regSelTool_MDT_Cfg(flags)))

    if flags.Muon.MuonTrigger:
        # Set the algorithm to RoI mode
        kwargs.setdefault("DoSeededDecoding", True)
        from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
        kwargs.setdefault("RoIs", mapThresholdToL1RoICollection("MU"))

    # Add the RDO -> PRD alorithm
    acc.addEventAlgo(CompFactory.MdtRdoToMdtPrepData(name, **kwargs))
    return acc


def CscRDODecodeCfg(flags, name="CscRdoToCscPrepData", **kwargs):
    acc = ComponentAccumulator()

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # We need the CSC cabling to be setup
    from MuonConfig.MuonCablingConfig import CSCCablingConfigCfg # Not yet been prepared
    acc.merge(CSCCablingConfigCfg(flags))

    from MuonConfig.MuonCondAlgConfig import CscCondDbAlgCfg
    acc.merge(CscCondDbAlgCfg(flags))

    # Get the RDO -> PRD tool
    # TODO: setup dependencies properly
    kwargs.setdefault("CscRdoToCscPrepDataTool", CompFactory.Muon.CscRdoToCscPrepDataToolMT(name="CscRdoToCscPrepDataTool"))

    # add RegSelTool
    from RegionSelector.RegSelToolConfig import regSelTool_CSC_Cfg
    kwargs.setdefault("RegSel_CSC", acc.popToolsAndMerge(regSelTool_CSC_Cfg(flags)))

    if flags.Muon.MuonTrigger:
        # Set the algorithm to RoI mode
        kwargs.setdefault("DoSeededDecoding", True)
        from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
        kwargs.setdefault("RoIs", mapThresholdToL1RoICollection("MU"))

    # Add the RDO -> PRD alorithm
    acc.addEventAlgo(CompFactory.CscRdoToCscPrepData(name, **kwargs))
    return acc


def CscClusterBuildCfg(flags, name="CscThresholdClusterBuilder"):
    acc = ComponentAccumulator()

    # Get cluster creator tool
    CscThresholdClusterBuilderTool=CompFactory.CscThresholdClusterBuilderTool
    CscClusterBuilderTool = CscThresholdClusterBuilderTool(name = "CscThresholdClusterBuilderTool" )

    #CSC cluster building
    CscThresholdClusterBuilder=CompFactory.CscThresholdClusterBuilder
    CscClusterBuilder = CscThresholdClusterBuilder(name            = name,
                                                   cluster_builder = CscClusterBuilderTool )
    acc.addEventAlgo(CscClusterBuilder)

    return acc


def MuonPRD_MultiTruthMakerCfg(flags, name="MuonPRD_MultiTruthMaker", **kwargs):
    acc = ComponentAccumulator()
    # TODO: properly declare dependencies
    acc.addEventAlgo(CompFactory.MuonPRD_MultiTruthMaker(name, **kwargs))
    return acc


def MuonRDOtoPRDConvertorsCfg(flags):
    # Schedule RDO conversion
    acc = ComponentAccumulator()

    if flags.Detector.GeometryRPC:
        acc.merge(RpcRDODecodeCfg(flags))

    if flags.Detector.GeometryTGC:
        acc.merge(TgcRDODecodeCfg(flags))

    if flags.Detector.GeometrysTGC:
        acc.merge(StgcRDODecodeCfg(flags))

    if flags.Detector.GeometryMM:
        acc.merge(MMRDODecodeCfg(flags))

    if flags.Detector.GeometryMDT:
        acc.merge(MdtRDODecodeCfg(flags))

    if flags.Detector.GeometryCSC:
        acc.merge(CscRDODecodeCfg(flags))
        acc.merge(CscClusterBuildCfg(flags))

    if flags.Input.isMC:
        acc.merge(MuonPRD_MultiTruthMakerCfg(flags))

    return acc


# This function runs the decoding on a data file
def muonRdoDecodeTestData( forTrigger = False ):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW

    # Set global tag by hand for now
    ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2018-13"#"CONDBR2-BLKPA-2015-17"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"#"ATLAS-R2-2015-03-01-00"

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaCommon.Logging import log

    log.setLevel(INFO)
    log.info('About to setup Raw data decoding')

    cfg=ComponentAccumulator()


    # Add the MuonCache to ComponentAccumulator for trigger/RoI testing mode
    if forTrigger:
        # cache creators loaded independently
        from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheCfg
        cfg.merge( MuonCacheCfg() )

    if ConfigFlags.Input.Format == 'BS':
        from MuonConfig.MuonBytestreamDecodeConfig import MuonByteStreamDecodersCfg
        cfg.merge( MuonByteStreamDecodersCfg( ConfigFlags) )

    cfg.merge( MuonRDOtoPRDConvertorsCfg( ConfigFlags) )

    # Need to add POOL converter  - may be a better way of doing this?
    cfg.addService( CompFactory.AthenaPoolCnvSvc() )
    cfg.getService("EventPersistencySvc").CnvServices += [ "AthenaPoolCnvSvc" ]

    log.info('Print Config')
    cfg.printConfig(withDetails=True)

    if forTrigger:
        pklName = 'MuonRdoDecode_Cache.pkl'
    else:
        pklName = 'MuonRdoDecode.pkl'

    # Store config as pickle
    log.info('Save Config')
    with open(pklName,'wb') as f:
        cfg.store(f)
        f.close()
    return cfg

# This function runs the decoding on a MC file
def muonRdoDecodeTestMC():
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7572_tid07644622_00/RDO.07644622._000001.pool.root.1"]

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaCommon.Logging import log

    log.setLevel(DEBUG)
    log.info('About to setup Rpc RDO data decoding')

    cfg=ComponentAccumulator()

    # We are reading a pool file for this test
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Schedule RDO conversion
    cfg.merge( MuonRDOtoPRDConvertorsCfg( ConfigFlags) )

    log.info('Print Config')
    cfg.printConfig(withDetails=True)

    # Store config as pickle
    log.info('Save Config')
    with open('MuonRdoDecode.pkl','wb') as f:
        cfg.store(f)
        f.close()
    return cfg

if __name__=="__main__":
    # To run this, do e.g.
    # python ../athena/MuonSpectrometer/MuonConfig/python/MuonRdoDecodeConfig.py
    cfg = muonRdoDecodeTestData()
    #muonRdoDecodeTestMC()

