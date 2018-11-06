# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Heavily based on Trigger/TrigSteer/L1Decoder/python/L1MuonConfig.py
# TODO add MDTs, CSCs

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg

def RPCCablingConfigCfg(flags):
    acc = ComponentAccumulator()
    
    # TODO check if we actually need this here?
    acc.merge(MuonGeoModelCfg(flags)) 

    from MuonCablingServers.MuonCablingServersConf import RPCcablingServerSvc
    RPCCablingSvc =  RPCcablingServerSvc()
    RPCCablingSvc.Atlas = True
    RPCCablingSvc.forcedUse = True
    RPCCablingSvc.useMuonRPC_CablingSvc = True #Needed to switch to new cabling
    acc.addService( RPCCablingSvc )

    
    from MuonRPC_Cabling.MuonRPC_CablingConf import MuonRPC_CablingSvc
    rpcCablingSvc = MuonRPC_CablingSvc()
    rpcCablingSvc.ConfFileName = 'LVL1confAtlas.data' # this should come from config flag maybe ???
    rpcCablingSvc.CorrFileName = 'LVL1confAtlas.corr' 
    rpcCablingSvc.ConfFilePath = 'MuonRPC_Cabling/'
    rpcCablingSvc.RPCTriggerRoadsfromCool = True
    #rpcCablingSvc.CosmicConfiguration     = True  # was set like this in egamma test, looks incorrect ???

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.merge(addFolders(flags, 
                   [ '/RPC/TRIGGER/CM_THR_ETA', '/RPC/TRIGGER/CM_THR_ETA', '/RPC/TRIGGER/CM_THR_PHI', 
                     '/RPC/TRIGGER/CM_THR_PHI', '/RPC/CABLING/MAP_SCHEMA', '/RPC/CABLING/MAP_SCHEMA_CORR' ], 
                   'RPC' ))

    # that should not be here???
    acc.getService('IOVDbSvc').FoldersToMetaData     += ['/GLOBAL/BField/Maps']


    from RPC_CondCabling.RPC_CondCablingConf import RPCCablingDbTool
    RPCCablingDbTool = RPCCablingDbTool()
    RPCCablingDbTool.MapConfigurationFolder = '/RPC/CABLING/MAP_SCHEMA'
    RPCCablingDbTool.MapCorrectionFolder    = '/RPC/CABLING/MAP_SCHEMA_CORR'

    acc.addPublicTool( RPCCablingDbTool )
    rpcCablingSvc.TheRpcCablingDbTool = RPCCablingDbTool

    acc.addService( rpcCablingSvc )

    return acc, rpcCablingSvc

def TGCCablingConfigCfg(flags):
    acc = ComponentAccumulator()
    
    # TODO check if we actually need this here?
    acc.merge(MuonGeoModelCfg(flags)) 
    
    from TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiSvcConf import LVL1TGC__TGCRecRoiSvc
    acc.addService( LVL1TGC__TGCRecRoiSvc() ) 
    
    from MuonCablingServers.MuonCablingServersConf import TGCcablingServerSvc
    TGCCablingSvc = TGCcablingServerSvc() 
    TGCCablingSvc.Atlas=True
    TGCCablingSvc.useMuonTGC_CablingSvc=True
    TGCCablingSvc.forcedUse=True    
    acc.addService( TGCCablingSvc )

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.merge(addFolders(flags, ['/TGC/CABLING/MAP_SCHEMA','/TGC/CABLING/MAP_SCHEMA'], 'TGC'))
    return acc, TGCCablingSvc

if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files =["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    ConfigFlags.lock()

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    result,svc = RPCCablingConfigCfg(ConfigFlags)
    acc.merge( result )
    result,svc = TGCCablingConfigCfg(ConfigFlags)
    acc.merge( result )

    f=open('MuonCabling.pkl','w')
    acc.store(f)
    f.close()

    
