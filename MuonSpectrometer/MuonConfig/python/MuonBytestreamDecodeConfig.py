#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Constants import DEBUG, INFO

## This configuration function sets up everything for decoding RPC bytestream data into RDOs
#
# The forTrigger paramater is used to put the algorithm in RoI mode
# The function returns a ComponentAccumulator and the data-decoding algorithm, which should be added to the right sequence by the user
def RpcBytestreamDecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the RPC cabling to be setup
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge( RPCCablingConfigCfg(flags)[0] )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags)) 

    # Setup the RPC ROD decoder
    from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
    RPCRodDecoder = Muon__RpcROD_Decoder(name	     = "RpcROD_Decoder" )

    # RAW data provider tool needs ROB data provider service (should be another Config function?)
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
    robDPSvc = ROBDataProviderSvc()
    acc.addService( robDPSvc )

    # Setup the RAW data provider tool
    from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderTool
    MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderTool(name    = "RPC_RawDataProviderTool",
                                                               Decoder = RPCRodDecoder )
    
    # Setup the RAW data provider algorithm
    from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
    RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider",
                                                  ProviderTool = MuonRpcRawDataProviderTool )

    if forTrigger:
        # Configure the RAW data provider for ROI access
        RpcRawDataProvider.DoSeededDecoding = True
        RpcRawDataProvider.RoIs = "MURoIs" # Maybe we don't want to hard code this?

    return acc, RpcRawDataProvider

if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MuonSpectrometer/MuonConfig/python/MuonBytestreamDecodeCfg.py

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

    # Schedule Rpc data decoding - once mergeAll is working can simplify these lines
    rpcdecodingAcc, rpcdecodingAlg = RpcBytestreamDecodeCfg( ConfigFlags ) 
    cfg.merge( rpcdecodingAcc )
    cfg.addEventAlgo( rpcdecodingAlg )

    # Need to add POOL converter  - may be a better way of doing this?
    from AthenaCommon import CfgMgr
    cfg.addService( CfgMgr.AthenaPoolCnvSvc() )
    cfg.getService("EventPersistencySvc").CnvServices += [ "AthenaPoolCnvSvc" ]

    log.info('Print Config')
    cfg.printConfig(withDetails=True)

    # Store config as pickle
    log.info('Save Config')
    with open('MuonBytestreamDecode.pkl','w') as f:
        cfg.store(f)
        f.close()

