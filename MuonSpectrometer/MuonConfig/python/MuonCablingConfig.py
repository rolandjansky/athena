# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Heavily based on Trigger/TrigSteer/L1Decoder/python/L1MuonConfig.py
# TODO add MDTs, CSCs

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg

def RPCCablingConfigCfg(flags):
    acc = ComponentAccumulator()
    
    # TODO check if we actually need this here?
    acc.merge(MuonGeoModelCfg(flags)) 

    RpcCablingCondAlg=CompFactory.RpcCablingCondAlg
    RpcCablingAlg = RpcCablingCondAlg("RpcCablingCondAlg")
    acc.addCondAlgo( RpcCablingAlg )

    RPCcablingServerSvc=CompFactory.RPCcablingServerSvc
    RPCCablingSvc =  RPCcablingServerSvc()
    RPCCablingSvc.Atlas = True
    RPCCablingSvc.forcedUse = True
    RPCCablingSvc.useMuonRPC_CablingSvc = True #Needed to switch to new cabling
    acc.addService( RPCCablingSvc )

    
    MuonRPC_CablingSvc=CompFactory.MuonRPC_CablingSvc
    rpcCablingSvc = MuonRPC_CablingSvc()
    rpcCablingSvc.ConfFileName = 'LVL1confAtlas.data' # this should come from config flag maybe ???
    rpcCablingSvc.CorrFileName = 'LVL1confAtlas.corr' 
    rpcCablingSvc.ConfFilePath = 'MuonRPC_Cabling/'
    rpcCablingSvc.RPCTriggerRoadsfromCool = True
    rpcCablingSvc.CosmicConfiguration     = 'HLT' in flags.IOVDb.GlobalTag  # this was set to true by the modifier openThresholdRPCCabling in runHLT_standalone.py

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    dbName = 'RPC_OFL' if flags.Input.isMC else 'RPC'
    acc.merge(addFolders(flags, 
                         [ '/RPC/TRIGGER/CM_THR_ETA', '/RPC/TRIGGER/CM_THR_PHI',
                           '/RPC/CABLING/MAP_SCHEMA', '/RPC/CABLING/MAP_SCHEMA_CORR' ],
                         dbName, className='CondAttrListCollection' ))

    RPCCablingDbTool=CompFactory.RPCCablingDbTool
    RPCCablingDbTool = RPCCablingDbTool()
    RPCCablingDbTool.MapConfigurationFolder = '/RPC/CABLING/MAP_SCHEMA'
    RPCCablingDbTool.MapCorrectionFolder    = '/RPC/CABLING/MAP_SCHEMA_CORR'

    acc.addPublicTool( RPCCablingDbTool )
    rpcCablingSvc.TheRpcCablingDbTool = RPCCablingDbTool

    acc.addService( rpcCablingSvc,primary=True )

    return acc

def TGCCablingConfigCfg(flags):
    acc = ComponentAccumulator()
    
    # TODO check if we actually need this here?
    acc.merge(MuonGeoModelCfg(flags)) 
    
    LVL1TGC__TGCRecRoiSvc=CompFactory.LVL1TGC.TGCRecRoiSvc
    acc.addService( LVL1TGC__TGCRecRoiSvc() ) 
    
    TGCcablingServerSvc=CompFactory.TGCcablingServerSvc
    TGCCablingSvc = TGCcablingServerSvc() 
    TGCCablingSvc.Atlas=True
    TGCCablingSvc.useMuonTGC_CablingSvc=True
    TGCCablingSvc.forcedUse=True    
    acc.addService( TGCCablingSvc, primary=True )

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    dbName = 'TGC_OFL' if flags.Input.isMC else 'TGC'
    acc.merge(addFolders(flags, '/TGC/CABLING/MAP_SCHEMA', dbName))

    return acc

# This should be checked by experts since I just wrote it based on 
# athena/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCablingConfig.py
def MDTCablingConfigCfg(flags):
    acc = ComponentAccumulator()
    
    acc.merge(MuonGeoModelCfg(flags)) 

    MuonMDT_CablingAlg=CompFactory.MuonMDT_CablingAlg
    MDTCablingAlg = MuonMDT_CablingAlg("MuonMDT_CablingAlg")
   
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    if flags.Input.isMC is True:
        MDTCablingAlg.MapFolders = "/MDT/Ofl/CABLING/MAP_SCHEMA" 
        MDTCablingAlg.MezzanineFolders    = "/MDT/Ofl/CABLING/MEZZANINE_SCHEMA" 
        acc.merge( addFolders( flags, ["/MDT/Ofl/CABLING/MAP_SCHEMA",
                                       "/MDT/Ofl/CABLING/MEZZANINE_SCHEMA"], 'MDT_OFL', className="CondAttrListCollection") )
    else:
        MDTCablingAlg.MapFolders = "/MDT/CABLING/MAP_SCHEMA" 
        MDTCablingAlg.MezzanineFolders    = "/MDT/CABLING/MEZZANINE_SCHEMA" 
        acc.merge( addFolders( flags, ["/MDT/CABLING/MAP_SCHEMA",
                                       "/MDT/CABLING/MEZZANINE_SCHEMA"], 'MDT', className="CondAttrListCollection") )

    acc.addCondAlgo( MDTCablingAlg )
   
    return acc


# This should be checked by experts 
def CSCCablingConfigCfg(flags):
    acc = ComponentAccumulator()
    
    acc.merge(MuonGeoModelCfg(flags)) 

    CSCcablingSvc=CompFactory.CSCcablingSvc
    cscCablingSvc = CSCcablingSvc()

    acc.addService( cscCablingSvc, primary=True )

    return acc

if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    result = RPCCablingConfigCfg(ConfigFlags)
    acc.merge( result )
    result = TGCCablingConfigCfg(ConfigFlags)
    acc.merge( result )

    result = MDTCablingConfigCfg(ConfigFlags)
    acc.merge( result )

    result = CSCCablingConfigCfg(ConfigFlags)
    acc.merge( result )

    f=open('MuonCabling.pkl','wb')
    acc.store(f)
    f.close()

    

