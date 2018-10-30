#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Constants import DEBUG, INFO

## This configuration function sets up everything for decoding RPC RDO to PRD conversion
#
# The forTrigger paramater is used to put the algorithm in RoI mode
# The function returns a ComponentAccumulator and the data-converting algorithm, which should be added to the right sequence by the user
def RpcRDODecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the RPC cabling to be setup
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge( RPCCablingConfigCfg(flags)[0] )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))

    # Get the RDO -> PRD tool
    from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
    RpcRdoToRpcPrepDataTool = Muon__RpcRdoToPrepDataTool()
    
    # Get the RDO -> PRD alorithm
    from MuonRdoToPrepData.MuonRdoToPrepDataConf import RpcRdoToRpcPrepData
    RpcRdoToRpcPrepData = RpcRdoToRpcPrepData(name          = "RpcRdoToRpcPrepData",
                                              DecodingTool  = RpcRdoToRpcPrepDataTool,
                                              PrintPrepData = False )

    if forTrigger:
        # Set the algorithm to RoI mode
        RpcRdoToRpcPrepData.DoSeededDecoding = True
        RpcRdoToRpcPrepData.RoIs = "MURoIs"

    return acc, RpcRdoToRpcPrepData

# This function runs the decoding on a data file
def muonRdoDecodeTestData():
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
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

    # Schedule Rpc bytestream data decoding - once mergeAll is working can simplify these lines
    from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg
    rpcdecodingAcc, rpcdecodingAlg = RpcBytestreamDecodeCfg( ConfigFlags ) 
    cfg.merge( rpcdecodingAcc )
    cfg.addEventAlgo( rpcdecodingAlg )

    # Schedule RDO conversion - can replace this with cfg.mergeAll once that is working
    rpcdecodingAcc, rpcdecodingAlg = RpcRDODecodeCfg( ConfigFlags )
    cfg.merge(rpcdecodingAcc)
    cfg.addEventAlgo(rpcdecodingAlg)

    # Need to add POOL converter  - may be a better way of doing this?
    from AthenaCommon import CfgMgr
    cfg.addService( CfgMgr.AthenaPoolCnvSvc() )
    cfg.getService("EventPersistencySvc").CnvServices += [ "AthenaPoolCnvSvc" ]

    log.info('Print Config')
    cfg.printConfig(withDetails=True)

    # Store config as pickle
    log.info('Save Config')
    with open('MuonRdoDecode.pkl','w') as f:
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
    rpcdecodingAcc, rpcdecodingAlg = RpcRDODecodeCfg( ConfigFlags )
    cfg.merge(rpcdecodingAcc)
    cfg.addEventAlgo(rpcdecodingAlg)

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
