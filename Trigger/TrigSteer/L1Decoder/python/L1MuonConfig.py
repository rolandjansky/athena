#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

# This file (modified) will be moved in future to Muon realm.
# Since it uses an experimental way of configuring the job
# it is better if it resided here for now.
# Experts, when moving look for ??? for hardwired settings

def RPCCablingConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiSvcConf import LVL1RPC__RPCRecRoiSvc
    acc.addService( LVL1RPC__RPCRecRoiSvc() )



    from MuonCablingServers.MuonCablingServersConf import RPCcablingServerSvc
    RPCCablingSvc =  RPCcablingServerSvc()
    RPCCablingSvc.Atlas = True
    RPCCablingSvc.forcedUse = True
    RPCCablingSvc.useMuonRPC_CablingSvc = True #Needed to switch to new cabling
    acc.addService( RPCCablingSvc )

    # get the GeoModelSvc and add MuonDetectorTool
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg    
    acc.addConfig( GeoModelCfg, flags )

    from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
    detTool = MuonDetectorTool()
    detTool.UseConditionDb = 1
    detTool.UseIlinesFromGM = 1    
    acc.getService('GeoModelSvc').DetectorTools += [ detTool ]

    
    from MuonRPC_Cabling.MuonRPC_CablingConf import MuonRPC_CablingSvc
    rpcCablingSvc = MuonRPC_CablingSvc()
    rpcCablingSvc.ConfFileName = 'LVL1confAtlas.data' # this should come from config flag maybe ???
    rpcCablingSvc.CorrFileName = 'LVL1confAtlas.corr' 
    rpcCablingSvc.ConfFilePath = 'MuonRPC_Cabling/'
    rpcCablingSvc.RPCTriggerRoadsfromCool = True
    #rpcCablingSvc.CosmicConfiguration     = True  # was set like this in egamma test, looks incorrect ???

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    #conddb.addFolderSplitMC('RPC','/RPC/TRIGGER/CM_THR_ETA', '/RPC/TRIGGER/CM_THR_ETA')
    #conddb.addFolderSplitMC('RPC','/RPC/TRIGGER/CM_THR_PHI', '/RPC/TRIGGER/CM_THR_PHI')


    acc.addConfig( addFolders, flags, 
                   [ '/RPC/TRIGGER/CM_THR_ETA', '/RPC/TRIGGER/CM_THR_ETA', '/RPC/TRIGGER/CM_THR_PHI', 
                     '/RPC/TRIGGER/CM_THR_PHI', '/RPC/CABLING/MAP_SCHEMA', '/RPC/CABLING/MAP_SCHEMA_CORR' ], 
                   'RPC' )

    # that should not be here???
    acc.getService('IOVDbSvc').FoldersToMetaData     += ['/GLOBAL/BField/Maps']


    from RPC_CondCabling.RPC_CondCablingConf import RPCCablingDbTool
    RPCCablingDbTool = RPCCablingDbTool()
    RPCCablingDbTool.MapConfigurationFolder = '/RPC/CABLING/MAP_SCHEMA'
    RPCCablingDbTool.MapCorrectionFolder    = '/RPC/CABLING/MAP_SCHEMA_CORR'

    acc.addPublicTool( RPCCablingDbTool )
    rpcCablingSvc.TheRpcCablingDbTool = RPCCablingDbTool

    acc.addService( rpcCablingSvc )

    return acc

def TGCCablingConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiSvcConf import LVL1TGC__TGCRecRoiSvc
    acc.addService( LVL1TGC__TGCRecRoiSvc() ) 
    
    from MuonCablingServers.MuonCablingServersConf import TGCcablingServerSvc
    TGCCablingSvc = TGCcablingServerSvc() 
    TGCCablingSvc.Atlas=True
    TGCCablingSvc.useMuonTGC_CablingSvc=True
    TGCCablingSvc.forcedUse=True    
    acc.addService( TGCCablingSvc )

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.addConfig( addFolders, flags, ['/TGC/CABLING/MAP_SCHEMA','/TGC/CABLING/MAP_SCHEMA'], 'TGC')
    return acc

# for reco running we need MDT and CSC, yet to come

if __name__ == '__main__':
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.set('Trigger.L1Decoder.doCalo',True)
    ConfigFlags.set('Trigger.L1Decoder.doMuon',True)
    ConfigFlags.set('global.InputFiles',["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1",])
    ConfigFlags.lock()

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()


    acc.addConfig( RPCCablingConfig, ConfigFlags )
    acc.addConfig( TGCCablingConfig, ConfigFlags )

    f=open('MuonConfig.pkl','w')
    acc.store(f)
    f.close()
    
