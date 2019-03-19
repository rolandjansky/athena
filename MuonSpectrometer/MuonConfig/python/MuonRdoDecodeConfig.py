#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Constants import VERBOSE, DEBUG, INFO

## This configuration function sets up everything for decoding RPC RDO to PRD conversion
#
# The forTrigger paramater is used to put the algorithm in RoI mode
# The function returns a ComponentAccumulator and the data-converting algorithm, which should be added to the right sequence by the user
def RpcRDODecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the RPC cabling to be setup
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge( RPCCablingConfigCfg(flags) )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
    RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool(name = "RpcRdoToRpcPrepDataTool")
    acc.addPublicTool( RpcRdoToRpcPrepDataTool ) # This should be removed, but now defined as PublicTool at MuFastSteering 
    
    # Get the RDO -> PRD alorithm
    from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
    RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name          = "RpcRdoToRpcPrepData",
                                              DecodingTool  = RpcRdoToRpcPrepDataTool,
                                              PrintPrepData = False )

    if forTrigger:
        # Set the algorithm to RoI mode
        RpcRdoToRpcPrepData.DoSeededDecoding = True
        RpcRdoToRpcPrepData.RoIs = "MURoIs"

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
    from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
    TgcRdoToTgcPrepDataTool = Muon__TgcRdoToPrepDataTool(name           = "TgcRdoToTgcPrepDataTool")
    acc.addPublicTool( TgcRdoToTgcPrepDataTool ) # This should be removed, but now defined as PublicTool at MuFastSteering 
    
    # Get the RDO -> PRD alorithm
    from MuonRdoToPrepData.MuonRdoToPrepDataConf import TgcRdoToTgcPrepData
    TgcRdoToTgcPrepData = TgcRdoToTgcPrepData(name          = "TgcRdoToTgcPrepData",
                                              DecodingTool  = TgcRdoToTgcPrepDataTool,
                                              PrintPrepData = False )

    if forTrigger:
        # Set the algorithm to RoI mode
        TgcRdoToTgcPrepData.DoSeededDecoding = True
        TgcRdoToTgcPrepData.RoIs = "MURoIs"

    acc.addEventAlgo(TgcRdoToTgcPrepData)
    return acc

def MdtRDODecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the MDT cabling to be setup
    from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg
    acc.merge( MDTCablingConfigCfg(flags) )

    from MuonConfig.MuonCalibConfig import MdtCalibrationSvcCfg
    acc.merge( MdtCalibrationSvcCfg(flags)  )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
    MdtRdoToMdtPrepDataTool = Muon__MdtRdoToPrepDataTool(name = "MdtRdoToMdtPrepDataTool")
    acc.addPublicTool( MdtRdoToMdtPrepDataTool ) # This should be removed, but now defined as PublicTool at MuFastSteering 
    
    # Get the RDO -> PRD alorithm
    from MuonRdoToPrepData.MuonRdoToPrepDataConf import MdtRdoToMdtPrepData
    MdtRdoToMdtPrepData = MdtRdoToMdtPrepData(name          = "MdtRdoToMdtPrepData",
                                              DecodingTool  = MdtRdoToMdtPrepDataTool,
                                              PrintPrepData = False )

    if forTrigger:
        # Set the algorithm to RoI mode
        MdtRdoToMdtPrepData.DoSeededDecoding = True
        MdtRdoToMdtPrepData.RoIs = "MURoIs"

    acc.addEventAlgo(MdtRdoToMdtPrepData)
    return acc

def CscRDODecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the CSC cabling to be setup
    from MuonConfig.MuonCablingConfig import CSCCablingConfigCfg # Not yet been prepared
    acc.merge( CSCCablingConfigCfg(flags) )

    from MuonConfig.MuonCalibConfig import CscCoolStrSvcCfg
    acc.merge( CscCoolStrSvcCfg(flags)  )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
    CscRdoToCscPrepDataTool = Muon__CscRdoToCscPrepDataTool(name           = "CscRdoToCscPrepDataTool")
    acc.addPublicTool( CscRdoToCscPrepDataTool ) # This should be removed, but now defined as PublicTool at MuFastSteering 
    
    # Get the RDO -> PRD alorithm
    from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
    CscRdoToCscPrepData = CscRdoToCscPrepData(name                    = "CscRdoToCscPrepData",
                                              CscRdoToCscPrepDataTool = CscRdoToCscPrepDataTool,
                                              PrintPrepData           = False )

    if forTrigger:
        # Set the algorithm to RoI mode
        CscRdoToCscPrepData.DoSeededDecoding = True
        CscRdoToCscPrepData.RoIs = "MURoIs"

    acc.addEventAlgo(CscRdoToCscPrepData)
    return acc

def CscClusterBuildCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # Get cluster creator tool
    from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilderTool
    CscClusterBuilderTool = CscThresholdClusterBuilderTool(name = "CscThesholdClusterBuilderTool" )
    acc.addPublicTool( CscClusterBuilderTool ) # This should be removed, but now defined as PublicTool at MuFastSteering 
  
    #CSC cluster building
    from CscClusterization.CscClusterizationConf import CscThresholdClusterBuilder
    CscClusterBuilder = CscThresholdClusterBuilder(name            = "CscThesholdClusterBuilder",
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

    log.setLevel(DEBUG)
    log.info('About to setup Rpc Raw data decoding')

    cfg=ComponentAccumulator()

        
    # Seem to need this to read BS properly
    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    cfg.merge(TrigBSReadCfg(ConfigFlags ))

    # Schedule Rpc bytestream data decoding 
    if forTrigger:
        # cache creators loaded independently
        from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheCfg
        cfg.merge( MuonCacheCfg() )

    # Schedule Rpc bytestream data decoding - once mergeAll is working can simplify these lines
    from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg
    rpcdecodingAcc  = RpcBytestreamDecodeCfg( ConfigFlags ) 
    cfg.merge( rpcdecodingAcc )

    # Schedule Mdt data decoding 
    from MuonConfig.MuonBytestreamDecodeConfig import TgcBytestreamDecodeCfg
    tgcdecodingAcc  = TgcBytestreamDecodeCfg( ConfigFlags ) 
    cfg.merge( tgcdecodingAcc )

    from MuonConfig.MuonBytestreamDecodeConfig import MdtBytestreamDecodeCfg
    mdtdecodingAcc = MdtBytestreamDecodeCfg( ConfigFlags, forTrigger )
    # Put into a verbose logging mode to check the caching
    if forTrigger:
        mdtdecodingAcc().ProviderTool.OutputLevel = VERBOSE    
    cfg.merge( mdtdecodingAcc )

    from MuonConfig.MuonBytestreamDecodeConfig import CscBytestreamDecodeCfg
    cscdecodingAcc  = CscBytestreamDecodeCfg( ConfigFlags, forTrigger ) 
    # Put into a verbose logging mode to check the caching
    if forTrigger:
        cscdecodingAcc().ProviderTool.OutputLevel = VERBOSE 
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
    from AthenaCommon import CfgMgr
    cfg.addService( CfgMgr.AthenaPoolCnvSvc() )
    cfg.getService("EventPersistencySvc").CnvServices += [ "AthenaPoolCnvSvc" ]

    log.info('Print Config')
    cfg.printConfig(withDetails=True)

    if forTrigger:
        pklName = 'MuonRdoDecode_Cache.pkl'
    else:
        pklName = 'MuonRdoDecode.pkl'

    # Store config as pickle
    log.info('Save Config')
    with open(pklName,'w') as f:
        cfg.store(f)
        f.close()

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

    # Schedule RDO conversion - can replace this with cfg.mergeAll once that is working
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
    with open('MuonRdoDecode.pkl','w') as f:
        cfg.store(f)
        f.close()
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MuonSpectrometer/MuonConfig/python/MuonRdoDecodeConfig.py
    muonRdoDecodeTestData()
    #muonRdoDecodeTestMC()


