

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
    rpcCablingSvc.ConfFileName = 'LVL1confAtlas.data' # this should come from config flag maybe?
    rpcCablingSvc.CorrFileName = 'LVL1confAtlas.corr' 
    rpcCablingSvc.ConfFilePath = 'MuonRPC_Cabling/'
    rpcCablingSvc.RPCTriggerRoadsfromCool = True
    #rpcCablingSvc.CosmicConfiguration     = True  # was set like this in egamma test, looks incorrect

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    #conddb.addFolderSplitMC('RPC','/RPC/TRIGGER/CM_THR_ETA', '/RPC/TRIGGER/CM_THR_ETA')
    #conddb.addFolderSplitMC('RPC','/RPC/TRIGGER/CM_THR_PHI', '/RPC/TRIGGER/CM_THR_PHI')


    acc.addConfig( addFolders, flags, 
                   [ '/RPC/TRIGGER/CM_THR_ETA', '/RPC/TRIGGER/CM_THR_ETA', '/RPC/TRIGGER/CM_THR_PHI', 
                     '/RPC/TRIGGER/CM_THR_PHI', '/RPC/CABLING/MAP_SCHEMA', '/RPC/CABLING/MAP_SCHEMA_CORR' ], 
                   'RPC' )

    # that should not be here for sure
    acc.getService('IOVDbSvc').FoldersToMetaData     += ['/GLOBAL/BField/Maps']


    from RPC_CondCabling.RPC_CondCablingConf import RPCCablingDbTool
    RPCCablingDbTool = RPCCablingDbTool()
    RPCCablingDbTool.MapConfigurationFolder = '/RPC/CABLING/MAP_SCHEMA'
    RPCCablingDbTool.MapCorrectionFolder    = '/RPC/CABLING/MAP_SCHEMA_CORR'

    acc.addPublicTool( RPCCablingDbTool )
    rpcCablingSvc.TheRpcCablingDbTool = RPCCablingDbTool

    from MDT_CondCabling.MDT_CondCablingConf import MDTCablingDbTool
    MDTCablingDbTool = MDTCablingDbTool()

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

    # ignored for now
    # if muonCnvFlags.TgcCablingMode =='auto':
    #     ServiceMgr.TGCcablingServerSvc.Atlas=True
    #     ServiceMgr.TGCcablingServerSvc.forcedUse=False
    # elif muonCnvFlags.TgcCablingMode =='12-fold':
    #     ServiceMgr.TGCcablingServerSvc.Atlas=True
    #     ServiceMgr.TGCcablingServerSvc.useMuonTGC_CablingSvc=True
    #     ServiceMgr.TGCcablingServerSvc.forcedUse=True
    # elif muonCnvFlags.TgcCablingMode =='old 12-fold':
    #     ServiceMgr.TGCcablingServerSvc.Atlas=True
    #     ServiceMgr.TGCcablingServerSvc.useMuonTGC_CablingSvc=False
    #     ServiceMgr.TGCcablingServerSvc.forcedUse=True
    # elif muonCnvFlags.TgcCablingMode =='8-fold':
    #     ServiceMgr.TGCcablingServerSvc.Atlas=False
    #     ServiceMgr.TGCcablingServerSvc.forcedUse=True
    # else:
    #     raise RuntimeError("TgcCablingMode=%r not supported" % muonCnvFlags.TgcCablingMode())

    # COOL setting for MuonTGC_Cabling
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.addConfig( addFolders, flags, ['/TGC/CABLING/MAP_SCHEMA','/TGC/CABLING/MAP_SCHEMA'], 'TGC')
    return acc

# def MDTCablingConfig(flags):
#     from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
#     acc = ComponentAccumulator()

# # |=/***** Service MuonMDT_CablingSvc/MuonMDT_CablingSvc ***********************************************
# # | |-AuditFinalize     = False
# # | |-AuditInitialize   = False
# # | |-AuditReinitialize = False
# # | |-AuditRestart      = False
# # | |-AuditServices     = False
# # | |-AuditStart        = False
# # | |-AuditStop         = False
# # | |-DBTool            = PublicToolHandle('MDTCablingDbTool/MDTCablingDbTool')
# # | |-ForcedUse         = True  (default: False)
# # | |-OutputLevel       = 0
# # | |-TagInfoManager    = ServiceHandle('TagInfoMgr')
# # | |-UseOldCabling     = False  (default: False)
# # | |-doCalStreamInit   = False
# # | \----- (End of Service MuonMDT_CablingSvc/MuonMDT_CablingSvc) --------------------------------------
        
#     if flags.get('global.isMC'):
#         acc.addConfig( addFolders, flags, ['/MDT/Ofl/CABLING/MAP_SCHEMA', '/MDT/Ofl/CABLING/MEZZANINE_SCHEMA'], 'MDT_OFL' )
#         MDTCablingDbTool.MapFolders = '/MDT/Ofl/CABLING/MAP_SCHEMA'
#         MDTCablingDbTool.MezzanineFolders    = '/MDT/Ofl/CABLING/MEZZANINE_SCHEMA'
#     else:
#         acc.addConfig( addFolders, flags, ['/MDT/CABLING/MAP_SCHEMA', '/MDT/CABLING/MEZZANINE_SCHEMA'], 'MDT')        
#         MDTCablingDbTool.MapFolders = '/MDT/CABLING/MAP_SCHEMA'
#         MDTCablingDbTool.MezzanineFolders    = '/MDT/CABLING/MEZZANINE_SCHEMA'

#     acc.addPublicTool( MDTCablingDbTool )






#     return acc

if __name__ == '__main__':
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.set('Trigger.L1Decoder.doCalo',True)
    ConfigFlags.set('Trigger.L1Decoder.doMuon',True)
    ConfigFlags.lock()

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()


    acc.addConfig( RPCCablingConfig, ConfigFlags )
    acc.addConfig( TGCCablingConfig, ConfigFlags )

    f=open('MuonConfig.pkl','w')
    acc.store(f)
    f.close()
    
