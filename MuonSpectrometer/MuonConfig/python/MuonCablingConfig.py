# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def RPCCablingConfigCfg(flags):
    acc = ComponentAccumulator()

    dbName = 'RPC_OFL' if flags.Input.isMC else 'RPC'
    dbRepo="MuonRPC_Cabling/ATLAS.data"
    rpcCabMap="/RPC/CABLING/MAP_SCHEMA"
    rpcCabMapCorr="/RPC/CABLING/MAP_SCHEMA_CORR"
    rpcTrigEta="/RPC/TRIGGER/CM_THR_ETA"
    rpcTrigPhi="/RPC/TRIGGER/CM_THR_PHI"

    # This block with conditions override is only used in Trigger and Reco, and only needed until mid-May 2022.
    # See ATR-25059 for discussion. To avoid this ConfigFlags based block being executed in simulation/digitization,
    # skip this if ProductionStep is not Reconstruction or Default (i.e. unset)
    from AthenaConfiguration.Enums import ProductionStep
    if flags.Common.ProductionStep in [ProductionStep.Reconstruction, ProductionStep.Default] and \
            flags.Trigger.doLVL1 and flags.Trigger.enableL1MuonPhase1:
        # Run3 trigger roads are not avaialble in the global tag yet (OFLCOND-MC16-SDR-RUN3-01)
        # Relevant folder tags are set for now, until new global tag (RUN3-02) becomes avaialble
        rpcTrigEta="/RPC/TRIGGER/CM_THR_ETA <tag>RPCTriggerCMThrEta_RUN12_MC16_04</tag> <forceRunNumber>330000</forceRunNumber>"
        rpcTrigPhi="/RPC/TRIGGER/CM_THR_PHI <tag>RPCTriggerCMThrPhi_RUN12_MC16_04</tag> <forceRunNumber>330000</forceRunNumber>"
        from AthenaConfiguration.Enums  import LHCPeriod
        if flags.Input.isMC and flags.GeoModel.Run >= LHCPeriod.Run3:        # from Run3 on geometry
           rpcCabMap="/RPC/CABLING/MAP_SCHEMA <tag>RPCCablingMapSchema_2015-2018Run3-4</tag> <forceRunNumber>330000</forceRunNumber>"
           rpcCabMapCorr="/RPC/CABLING/MAP_SCHEMA_CORR <tag>RPCCablingMapSchemaCorr_2015-2018Run3-4</tag> <forceRunNumber>330000</forceRunNumber>"


    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.merge(addFolders(flags, [rpcCabMap,rpcCabMapCorr], dbName, className='CondAttrListCollection' ))
    # Same protection of ProductionStep as above, ATR-25059
    if flags.Common.ProductionStep in [ProductionStep.Reconstruction, ProductionStep.Default] and \
            flags.Trigger.doLVL1 and not flags.Input.isMC:
        # RPC trigger roads in the online database are not up-to-dated
        # Use offline database for now
        # Will switch to online database once online database has been updated (ATR-23465)
        if flags.Trigger.enableL1MuonPhase1:
            acc.merge(addFolders(flags, [rpcTrigEta,rpcTrigPhi], detDb='RPC_OFL', className='CondAttrListCollection', db='OFLP200'))
        else:
            conddbNameOffline = flags.Trigger.L1MuonSim.CondDBOffline if flags.Trigger.L1MuonSim.CondDBOffline != '' else "OFLCOND-MC16-SDR-RUN2-04"
            acc.merge(addFolders(flags, [rpcTrigEta,rpcTrigPhi], detDb='RPC_OFL', className='CondAttrListCollection', tag=conddbNameOffline, db='OFLP200' ))
    else:
        acc.merge(addFolders(flags, [rpcTrigEta,rpcTrigPhi], dbName, className='CondAttrListCollection' ))

    RpcCablingCondAlg=CompFactory.RpcCablingCondAlg
    RpcCablingAlg = RpcCablingCondAlg("RpcCablingCondAlg",DatabaseRepository=dbRepo)
    acc.addCondAlgo( RpcCablingAlg )

    return acc


def TGCCablingDbToolCfg(flags):
    acc = ComponentAccumulator()

    filename = 'ASD2PP_diff_12_OFL.db' if flags.Input.isMC else 'ASD2PP_diff_12_ONL.db'
    acc.setPrivateTools(CompFactory.TGCCablingDbTool(name = "TGCCablingDbTool",
                                                     filename_ASD2PP_DIFF_12 = filename))

    return acc


def MuonTGC_CablingSvcCfg(flags):
    acc = ComponentAccumulator()

    svc = CompFactory.MuonTGC_CablingSvc()
    tool = acc.popToolsAndMerge(TGCCablingDbToolCfg(flags))
    # The same tool is used as a public tool by TGCCableASDToPP and a
    # private tool by MuonTGC_CablingSvc - not great...
    acc.addPublicTool(tool)
    svc.TGCCablingDbTool = tool
    acc.addService(svc, primary = True)

    return acc


def TGCCablingConfigCfg(flags):
    acc = ComponentAccumulator()

    # No ServiceHandle in TGCcablingServerSvc
    acc.merge(MuonTGC_CablingSvcCfg(flags))

    TGCcablingServerSvc=CompFactory.TGCcablingServerSvc
    TGCCablingSvc = TGCcablingServerSvc()
    acc.addService( TGCCablingSvc, primary=True )

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    dbName = 'TGC_OFL' if flags.Input.isMC else 'TGC'
    acc.merge(addFolders(flags, '/TGC/CABLING/MAP_SCHEMA', dbName))

    return acc

# This should be checked by experts since I just wrote it based on 
# athena/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCablingConfig.py
def MDTCablingConfigCfg(flags):
    acc = ComponentAccumulator()

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

    CSCcablingSvc=CompFactory.CSCcablingSvc
    cscCablingSvc = CSCcablingSvc()

    acc.addService( cscCablingSvc, primary=True )

    return acc

#All the cabling configs together (convenience function)
def MuonCablingConfigCfg(flags):
    acc = ComponentAccumulator()

    result = RPCCablingConfigCfg(flags)
    acc.merge( result )

    result = TGCCablingConfigCfg(flags)
    acc.merge( result )

    result = MDTCablingConfigCfg(flags)
    acc.merge( result )

    result = CSCCablingConfigCfg(flags)
    acc.merge( result )

    return acc

if __name__ == '__main__':
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    result = MuonCablingConfigCfg(ConfigFlags)
    acc.merge( result )

    f=open('MuonCabling.pkl','wb')
    acc.store(f)
    f.close()

    

