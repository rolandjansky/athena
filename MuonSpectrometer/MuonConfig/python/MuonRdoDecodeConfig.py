#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
# The forTrigger paramater is used to put the algorithm in RoI mode
# The function returns a ComponentAccumulator and the data-converting algorithm, which should be added to the right sequence by the user
def RpcRDODecodeCfg(flags, forTrigger=False):
    from MuonConfig.MuonCondAlgConfig import RpcCondDbAlgCfg # MT-safe conditions access
    acc = RpcCondDbAlgCfg(flags)

    # We need the RPC cabling to be setup
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge( RPCCablingConfigCfg(flags) )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    Muon__RpcRdoToPrepDataToolMT=CompFactory.Muon.RpcRdoToPrepDataToolMT
    RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataToolMT(name = "RpcRdoToRpcPrepDataTool")
    if flags.Common.isOnline: 
        RpcRdoToRpcPrepDataTool.ReadKey = "" ## cond data not needed online

    if forTrigger:
        RpcRdoToRpcPrepDataTool.RpcPrdContainerCacheKey      = MuonPrdCacheNames.RpcCache
        RpcRdoToRpcPrepDataTool.RpcCoinDataContainerCacheKey = MuonPrdCacheNames.RpcCoinCache

    # Get the RDO -> PRD alorithm
    RpcRdoToRpcPrepData=CompFactory.RpcRdoToRpcPrepData
    RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name          = "RpcRdoToRpcPrepData",
                                              DecodingTool  = RpcRdoToRpcPrepDataTool,
                                              PrintPrepData = False )
    # add RegSelTool
    from RegionSelector.RegSelToolConfig import regSelTool_RPC_Cfg
    RpcRdoToRpcPrepData.RegSel_RPC = acc.popToolsAndMerge( regSelTool_RPC_Cfg( flags ) )

    if forTrigger:
        # Set the algorithm to RoI mode
        RpcRdoToRpcPrepData.DoSeededDecoding = True
        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
        RpcRdoToRpcPrepData.RoIs = mapThresholdToL1RoICollection("MU")

    acc.addEventAlgo(RpcRdoToRpcPrepData)
    return acc

def TgcRDODecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the TGC cabling to be setup
    from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
    acc.merge( TGCCablingConfigCfg(flags) )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    Muon__TgcRdoToPrepDataToolMT=CompFactory.Muon.TgcRdoToPrepDataToolMT
    TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataToolMT(name           = "TgcRdoToTgcPrepDataTool")
    
    # Get the RDO -> PRD alorithm
    TgcRdoToTgcPrepData=CompFactory.TgcRdoToTgcPrepData
    TgcRdoToTgcPrepData = TgcRdoToTgcPrepData(name          = "TgcRdoToTgcPrepData",
                                              DecodingTool  = TgcRdoToTgcPrepDataTool,
                                              PrintPrepData = False )
    # add RegSelTool
    from RegionSelector.RegSelToolConfig import regSelTool_TGC_Cfg
    TgcRdoToTgcPrepData.RegSel_TGC = acc.popToolsAndMerge( regSelTool_TGC_Cfg( flags ) )

    if forTrigger:
        # Set the algorithm to RoI mode
        TgcRdoToTgcPrepData.DoSeededDecoding = True
        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
        TgcRdoToTgcPrepData.RoIs = mapThresholdToL1RoICollection("MU")

    acc.addEventAlgo(TgcRdoToTgcPrepData)
    return acc

def MdtRDODecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the MDT cabling to be setup
    from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg
    acc.merge( MDTCablingConfigCfg(flags) )

    from MuonConfig.MuonCalibrationConfig import MdtCalibDbAlgCfg
    acc.merge (MdtCalibDbAlgCfg(flags))

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    Muon__MdtRdoToPrepDataToolMT=CompFactory.Muon.MdtRdoToPrepDataToolMT
    MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataToolMT(name = "MdtRdoToMdtPrepDataTool")
    
    # Get the RDO -> PRD alorithm
    MdtRdoToMdtPrepData=CompFactory.MdtRdoToMdtPrepData
    MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name          = "MdtRdoToMdtPrepData",
                                              DecodingTool  = MdtRdoToMdtPrepDataTool,
                                              PrintPrepData = False )
    # add RegSelTool
    from RegionSelector.RegSelToolConfig import regSelTool_MDT_Cfg
    MdtRdoToMdtPrepData.RegSel_MDT = acc.popToolsAndMerge( regSelTool_MDT_Cfg( flags ) )

    if forTrigger:
        # Set the algorithm to RoI mode
        MdtRdoToMdtPrepData.DoSeededDecoding = True
        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
        MdtRdoToMdtPrepData.RoIs = mapThresholdToL1RoICollection("MU")

    acc.addEventAlgo(MdtRdoToMdtPrepData) 
    return acc


def CscRDODecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the CSC cabling to be setup
    from MuonConfig.MuonCablingConfig import CSCCablingConfigCfg # Not yet been prepared
    acc.merge( CSCCablingConfigCfg(flags) )

    from MuonConfig.MuonCondAlgConfig import CscCondDbAlgCfg
    acc.merge( CscCondDbAlgCfg(flags)  )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    Muon__CscRdoToCscPrepDataToolMT=CompFactory.Muon.CscRdoToCscPrepDataToolMT
    CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataToolMT(name           = "CscRdoToCscPrepDataTool")
    
    # Get the RDO -> PRD alorithm
    CscRdoToCscPrepData=CompFactory.CscRdoToCscPrepData
    CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepData",
                                              CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                              PrintPrepData           = False )
    # add RegSelTool
    from RegionSelector.RegSelToolConfig import regSelTool_CSC_Cfg
    CscRdoToCscPrepData.RegSel_CSC = acc.popToolsAndMerge( regSelTool_CSC_Cfg( flags ) )

    if forTrigger:
        # Set the algorithm to RoI mode
        CscRdoToCscPrepData.DoSeededDecoding = True
        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
        CscRdoToCscPrepData.RoIs = mapThresholdToL1RoICollection("MU")

    acc.addEventAlgo(CscRdoToCscPrepData)
    return acc

def CscClusterBuildCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # Get cluster creator tool
    CscThresholdClusterBuilderTool=CompFactory.CscThresholdClusterBuilderTool
    CscClusterBuilderTool = CscThresholdClusterBuilderTool(name = "CscThresholdClusterBuilderTool" )
  
    #CSC cluster building
    CscThresholdClusterBuilder=CompFactory.CscThresholdClusterBuilder
    CscClusterBuilder = CscThresholdClusterBuilder(name            = "CscThresholdClusterBuilder",
                                                   cluster_builder = CscClusterBuilderTool )
    acc.addEventAlgo(CscClusterBuilder)

    return acc


# This function runs the decoding on a data file
def muonRdoDecodeTestData( forTrigger = False ):
    # Add a flag, forTrigger, which will initially put the ByteStreamDecodeCfg code into "Cached Container" mode
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

    # Seem to need this to read BS properly
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    cfg.merge(ByteStreamReadCfg(ConfigFlags ))

    # Add the MuonCache to ComponentAccumulator for trigger/RoI testing mode
    if forTrigger:
        # cache creators loaded independently
        from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheCfg
        cfg.merge( MuonCacheCfg() )

    # Schedule Rpc bytestream data decoding 
    from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg
    rpcdecodingAcc  = RpcBytestreamDecodeCfg( ConfigFlags, forTrigger ) 
    cfg.merge( rpcdecodingAcc )

    # Schedule Mdt bytestream data decoding 
    from MuonConfig.MuonBytestreamDecodeConfig import TgcBytestreamDecodeCfg
    tgcdecodingAcc = TgcBytestreamDecodeCfg( ConfigFlags, forTrigger ) 
    cfg.merge( tgcdecodingAcc )

    from MuonConfig.MuonBytestreamDecodeConfig import MdtBytestreamDecodeCfg
    mdtdecodingAcc = MdtBytestreamDecodeCfg( ConfigFlags, forTrigger )
    cfg.merge( mdtdecodingAcc )

    from MuonConfig.MuonBytestreamDecodeConfig import CscBytestreamDecodeCfg
    cscdecodingAcc  = CscBytestreamDecodeCfg( ConfigFlags, forTrigger) 
    cfg.merge( cscdecodingAcc )

    # Schedule RDO conversion 
    rpcdecodingAcc  = RpcRDODecodeCfg( ConfigFlags )
    cfg.merge(rpcdecodingAcc)

    tgcdecodingAcc = TgcRDODecodeCfg( ConfigFlags )
    cfg.merge(tgcdecodingAcc)

    mdtdecodingAcc = MdtRDODecodeCfg( ConfigFlags )
    cfg.merge(mdtdecodingAcc)

    cscdecodingAcc = CscRDODecodeCfg( ConfigFlags )
    cfg.merge(cscdecodingAcc)

    cscbuildingAcc = CscClusterBuildCfg( ConfigFlags )
    cfg.merge(cscbuildingAcc)

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
    # RPC decoding
    rpcdecodingAcc = RpcRDODecodeCfg( ConfigFlags )
    cfg.merge(rpcdecodingAcc)

    # TGC decoding
    tgcdecodingAcc  = TgcRDODecodeCfg( ConfigFlags )
    cfg.merge(tgcdecodingAcc)

    # MDT decoding
    mdtdecodingAcc = MdtRDODecodeCfg( ConfigFlags )
    cfg.merge(mdtdecodingAcc)

    # CSC decoding
    cscdecodingAcc = CscRDODecodeCfg( ConfigFlags )
    cfg.merge(cscdecodingAcc)

    cscbuildingAcc = CscClusterBuildCfg( ConfigFlags )
    cfg.merge(cscbuildingAcc)

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


