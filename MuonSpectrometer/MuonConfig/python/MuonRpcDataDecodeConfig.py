from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Constants import DEBUG

def RpcRawDataDecodeConfig(flags):
    acc = ComponentAccumulator()

    # We need the RPC cabling to be setup
    from MuonConfig.MuonCablingConfig import RPCCablingConfig
    acc.merge( RPCCablingConfig(flags)[0] )

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
                                                               Decoder = RPCRodDecoder, OutputLevel=DEBUG )
    
    # Setup the RAW data provider algorithm
    from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
    RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider",
                                                  ProviderTool = MuonRpcRawDataProviderTool, OutputLevel=DEBUG )

    
    acc.addEventAlgo( RpcRawDataProvider )

    return acc

if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MuonSpectrometer/MuonConfig/python/MuonRpcDataDecodeConfig.py

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
    from AthenaCommon.Constants import DEBUG

    log.setLevel(DEBUG)
    log.debug('About to setup Rpc Raw data decoding')

    cfg=ComponentAccumulator()
    
    nThreads=1
    
    from StoreGate.StoreGateConf import SG__HiveMgrSvc
    eventDataSvc = SG__HiveMgrSvc("EventDataSvc")
    eventDataSvc.NSlots = nThreads
    eventDataSvc.OutputLevel = DEBUG
    cfg.addService( eventDataSvc )
    
    from SGComps.SGCompsConf import SGInputLoader
    inputLoader = SGInputLoader(DetStore = 'StoreGateSvc/DetectorStore',
                                EvtStore = 'StoreGateSvc',
                                ExtraInputs = [],
                                ExtraOutputs = [],
                                FailIfNoProxy = False,
                                Load = [],
                            NeededResources = [])
    
    cfg.addEventAlgo( inputLoader)
    
    # Seem to need this to read BS properly
    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    cfg.merge(TrigBSReadCfg(ConfigFlags ))

    # Schedule Rpc data decoding
    cfg.merge( RpcRawDataDecodeConfig( ConfigFlags ) )

    # Need to add POOL converter  - may be a better way of doing this?
    from AthenaCommon import CfgMgr
    cfg.addService( CfgMgr.AthenaPoolCnvSvc() )
    cfg.getService("EventPersistencySvc").CnvServices += [ "AthenaPoolCnvSvc" ]

    log.debug('Print Config')
    cfg.printConfig(withDetails=True)

    log.debug('Save Config')
    with open('MuonRpcDataDecode.pkl','w') as f:
        cfg.store(f)
        f.close()
