# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger()
log.info("importing module %r",__name__)

from AthenaCommon.GlobalFlags import globalflags
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
from AthenaCommon.AppMgr import theApp,ToolSvc,ServiceMgr
from AthenaCommon.DetFlags import DetFlags
from MuonCnvExample.MuonCnvFlags import muonCnvFlags

from AthenaCommon.AlgSequence import AthSequencer
from MuonMDT_Cabling.MuonMDT_CablingConf import MuonMDT_CablingAlg
condSequence = AthSequencer("AthCondSeq")
if DetFlags.MDT_on():
    condSequence += MuonMDT_CablingAlg("MuonMDT_CablingAlg")

# defaults have to be re-set now since the jobproperties and trigger flags are now available # SS
muonCnvFlags.setDefaults()

if globalflags.DataSource() == 'data':
    # TODO: move these flags to this package - or remove them altogether
    muonByteStreamFlags.TgcDataType = "atlas"
    muonByteStreamFlags.RpcDataType = "atlas"
    muonByteStreamFlags.MdtDataType = "atlas"
    
log.info("configuring Muon cabling in MuonCablingConfig")
if DetFlags.readRDOBS.RPC_on() or DetFlags.readRDOPool.RPC_on() or DetFlags.readRIOPool.RPC_on() or DetFlags.digitize.RPC_on():
    try:
        log.info("importing the MetaReader for MuoncablingConfig")
        from PyUtils.MetaReaderPeekerFull import metadata
        if 'RPC_CablingType' in metadata['/TagInfo']:
            cablingTag = metadata['/TagInfo']['RPC_CablingType']
            log.info("Have retrieved RPC taginfo of " + str(cablingTag) + ". Setting default mode to new" )
            muonCnvFlags.RpcCablingMode = 'new'
        else:
            log.info("No RPC cabling taginfo found. Using normal configuration.")
    except KeyError:
        log.info("No metadata/Taginfo found. Using normal configuration for RPC")
    log.info("RPC cabling is using mode: %s",muonCnvFlags.RpcCablingMode())

    rpcCabMap="/RPC/CABLING/MAP_SCHEMA"
    rpcCabMapCorr="/RPC/CABLING/MAP_SCHEMA_CORR"

    rpcTrigEta="/RPC/TRIGGER/CM_THR_ETA"
    rpcTrigPhi="/RPC/TRIGGER/CM_THR_PHI"

    rpcDbName = 'RPC'
    from IOVDbSvc.CondDB import conddb
    conddb.addFolderSplitMC(rpcDbName,rpcCabMap,rpcCabMap,className='CondAttrListCollection')
    conddb.addFolderSplitMC(rpcDbName,rpcCabMapCorr,rpcCabMapCorr,className='CondAttrListCollection')
    conddb.addFolderSplitMC(rpcDbName,rpcTrigEta,rpcTrigEta,className='CondAttrListCollection')
    conddb.addFolderSplitMC(rpcDbName,rpcTrigPhi,rpcTrigPhi,className='CondAttrListCollection')

    dbRepo="MuonRPC_Cabling/ATLAS.data"
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    enableL1MuonPhase1 = ConfigFlags.Trigger.enableL1MuonPhase1 if ConfigFlags.Trigger.enableL1MuonPhase1 is not None else False
    if enableL1MuonPhase1:
        dbRepo="MuonRPC_Cabling/RUN3_roads_4_6_8_10_12"

    from RPC_CondCabling.RPC_CondCablingConf import RpcCablingCondAlg
    condSequence += RpcCablingCondAlg("RpcCablingCondAlg",DatabaseRepository=dbRepo)

if DetFlags.readRDOBS.TGC_on() or DetFlags.readRDOPool.TGC_on() or DetFlags.readRIOPool.TGC_on() or DetFlags.digitize.TGC_on():
    log.info("TGC cabling is using mode: %s",muonCnvFlags.TgcCablingMode())
    
    from MuonCablingServers.MuonCablingServersConf import TGCcablingServerSvc
    ServiceMgr += TGCcablingServerSvc()
    theApp.CreateSvc += [ "TGCcablingServerSvc" ]

    if muonCnvFlags.TgcCablingMode =='8-fold':  
        ServiceMgr.TGCcablingServerSvc.Atlas=False
    elif muonCnvFlags.TgcCablingMode not in ['auto', '12-fold']:
        # No longer support 'old 12-fold' since it apparently relies on a service which is gone
        raise RuntimeError("TgcCablingMode=%r not supported" % muonCnvFlags.TgcCablingMode())
    
    # COOL setting for MuonTGC_Cabling 
    if muonCnvFlags.TgcCablingMode=='12-fold' or muonCnvFlags.TgcCablingMode=='auto':
        from IOVDbSvc.CondDB import conddb
        conddb.addFolderSplitMC('TGC','/TGC/CABLING/MAP_SCHEMA','/TGC/CABLING/MAP_SCHEMA')

    from TGC_CondCabling.TGC_CondCablingConf import TGCCablingDbTool
    TGCCablingDbTool = TGCCablingDbTool()
    if globalflags.DataSource() == 'geant4':
        TGCCablingDbTool.filename_ASD2PP_DIFF_12='ASD2PP_diff_12_OFL.db'
    else:
        TGCCablingDbTool.filename_ASD2PP_DIFF_12='ASD2PP_diff_12_ONL.db'
    ToolSvc+=TGCCablingDbTool

if DetFlags.readRDOBS.MDT_on() or DetFlags.readRDOPool.MDT_on()  or DetFlags.readRIOPool.MDT_on() or DetFlags.digitize.MDT_on():
    log.info("MDT cabling is using mode: %s",muonCnvFlags.MdtCablingMode())

    if muonCnvFlags.MdtCablingMode!='old':
      # new cabling service, access to COOL for cabling map
      log.info("Adding MDT/CABLING folders to conddb") 
      from IOVDbSvc.CondDB import conddb 
      IOVDbSvc = ServiceMgr.IOVDbSvc 
      if globalflags.DataSource()=='data': 
          conddb.addFolder("MDT","/MDT/CABLING/MAP_SCHEMA",className='CondAttrListCollection') 
          conddb.addFolder("MDT","/MDT/CABLING/MEZZANINE_SCHEMA",className='CondAttrListCollection') 
          MuonMDT_CablingAlg.MapFolders = "/MDT/CABLING/MAP_SCHEMA" 
          MuonMDT_CablingAlg.MezzanineFolders    = "/MDT/CABLING/MEZZANINE_SCHEMA" 
      else: 
          conddb.addFolder("MDT_OFL","/MDT/Ofl/CABLING/MAP_SCHEMA",className='CondAttrListCollection') 
          conddb.addFolder("MDT_OFL","/MDT/Ofl/CABLING/MEZZANINE_SCHEMA",className='CondAttrListCollection')    
          MuonMDT_CablingAlg.MapFolders = "/MDT/Ofl/CABLING/MAP_SCHEMA" 
          MuonMDT_CablingAlg.MezzanineFolders    = "/MDT/Ofl/CABLING/MEZZANINE_SCHEMA"
       
