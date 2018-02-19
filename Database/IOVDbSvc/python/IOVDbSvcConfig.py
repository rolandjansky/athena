# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, ConfigurationError
import os

def IOVDbSvcCfg(inputFlags):

    result=ComponentAccumulator()

    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    from IOVSvc.IOVSvcConf import CondSvc
    from IOVSvc.IOVSvcConf import CondInputLoader
    from SGComps.SGCompsConf import ProxyProviderSvc

    #Properties of IOVDbSvc to be set here:
    #online-mode, DBInstance (slite)
    
    #Services it (sometimes) depends upon:
    # m_h_IOVSvc     ("IOVSvc", name),
    #m_h_sgSvc      ("StoreGateSvc", name),
    #m_h_detStore   ("DetectorStore", name),
    #m_h_metaDataStore ("StoreGateSvc/MetaDataStore", name), 
    #m_h_persSvc    ("EventPersistencySvc", name),
    #m_h_clidSvc    ("ClassIDSvc", name),
    #m_h_poolSvc    ("PoolSvc", name),
    #m_h_metaDataTool("IOVDbMetaDataTool"),
    #m_h_tagInfoMgr("TagInfoMgr", name),

    isMC=inputFlags.get("AthenaConfiguration.GlobalFlags.isMC")

    # Set up IOVDbSvc
    iovDbSvc=IOVDbSvc()
    dbname=inputFlags.get("IOVDbSvc.IOVDbConfigFlags.DatabaseInstance")

    localfile="sqlite://;schema=mycool.db;dbname="
    iovDbSvc.dbConnection=localfile+dbname
    # setup knowledge of dbinstance in IOVDbSvc, for global tag x-check
    iovDbSvc.DBInstance=dbname

    if 'FRONTIER_SERVER' in os.environ.keys() and os.environ['FRONTIER_SERVER']!="":
        iovDbSvc.CacheAlign=3


    iovDbSvc.GlobalTag=inputFlags.get("IOVDbSvc.IOVDbConfigFlags.GlobalTag")

    result.addService(iovDbSvc)


    # Set up POOLSvc with appropriate catalogs
    
    from PoolSvc.PoolSvcConf import PoolSvc
    poolSvc=PoolSvc()
    poolSvc.ReadCatalog=["apcfile:poolcond/PoolFileCatalog.xml",
                         "prfile:poolcond/PoolCat_oflcond.xml",
                         "apcfile:poolcond/PoolCat_oflcond.xml",
                         ]

    if not isMC:
         poolSvc.ReadCatalog+=["prfile:poolcond/PoolCat_comcond.xml",
                               "apcfile:poolcond/PoolCat_comcond.xml",
                               ]
    result.addService(poolSvc)
    result.addService(CondSvc())
    result.addService(ProxyProviderSvc(ProviderNames=["IOVDbSvc",]))


    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    if not isMC:
        result.addService(DBReplicaSvc(COOLSQLiteVetoPattern="/DBRelease/"))

    
    return result


#Convenience method to add folders:

def addFolders(inputFlags,folderstrings,detDb=None):

    #result=IOVDbSvcCfg(inputFlags)
    result=ComponentAccumulator()
    result.executeModule(IOVDbSvcCfg,inputFlags)

    
    
    iovDbSvc=result.getService("IOVDbSvc")
    
    if detDb is not None:
        dbname=inputFlags.get("IOVDbSvc.IOVDbConfigFlags.DatabaseInstance")
        if not detDb in _dblist.keys():
            raise ConfigurationError("Error, db shorthand %s not known")
        dbstr="<db>"+_dblist[detDb]+"/"+dbname+"</db>"
    else:
        dbstr=""
    
    #Convenince hack: Allow a single string as parameter:
    if isinstance(folderstrings,str):
        folderstrings=[folderstrings,]

    
    for fs in folderstrings:
        if fs.find("<db>")==-1:
            iovDbSvc.Folders.append(fs+dbstr)
        else:
            iovDbSvc.Folders.append(fs)


    return result


_dblist={
    'INDET':'COOLONL_INDET',
    'INDET_ONL':'COOLONL_INDET',
    'PIXEL':'COOLONL_PIXEL',
    'PIXEL_ONL':'COOLONL_PIXEL',
    'SCT':'COOLONL_SCT',
    'SCT_ONL':'COOLONL_SCT',
    'TRT':'COOLONL_TRT',
    'TRT_ONL':'COOLONL_TRT',
    'LAR':'COOLONL_LAR',
    'LAR_ONL':'COOLONL_LAR',
    'TILE':'COOLONL_TILE',
    'TILE_ONL':'COOLONL_TILE',
    'MUON':'COOLONL_MUON',
    'MUON_ONL':'COOLONL_MUON',
    'MUONALIGN':'COOLONL_MUONALIGN',
    'MUONALIGN_ONL':'COOLONL_MUONALIGN',
    'MDT':'COOLONL_MDT',
    'MDT_ONL':'COOLONL_MDT',
    'RPC':'COOLONL_RPC',
    'RPC_ONL':'COOLONL_RPC',
    'TGC':'COOLONL_TGC',
    'TGC_ONL':'COOLONL_TGC',
    'CSC':'COOLONL_CSC',
    'CSC_ONL':'COOLONL_CSC',
    'TDAQ':'COOLONL_TDAQ',
    'TDAQ_ONL':'COOLONL_TDAQ',
    'GLOBAL':'COOLONL_GLOBAL',
    'GLOBAL_ONL':'COOLONL_GLOBAL',
    'TRIGGER':'COOLONL_TRIGGER',
    'TRIGGER_ONL':'COOLONL_TRIGGER',
    'CALO':'COOLONL_CALO',
    'CALO_ONL':'COOLONL_CALO',
    'FWD':'COOLONL_FWD',
    'FWD_ONL':'COOLONL_FWD',            
    'INDET_OFL':'COOLOFL_INDET',
    'PIXEL_OFL':'COOLOFL_PIXEL',
    'SCT_OFL':'COOLOFL_SCT',
    'TRT_OFL':'COOLOFL_TRT',
    'LAR_OFL':'COOLOFL_LAR',
    'TILE_OFL':'COOLOFL_TILE',
    'MUON_OFL':'COOLOFL_MUON',
    'MUONALIGN_OFL':'COOLOFL_MUONALIGN',
    'MDT_OFL':'COOLOFL_MDT',
    'RPC_OFL':'COOLOFL_RPC',
    'TGC_OFL':'COOLOFL_TGC',
    'CSC_OFL':'COOLOFL_CSC',
    'TDAQ_OFL':'COOLOFL_TDAQ',
    'DCS_OFL':'COOLOFL_DCS',
    'GLOBAL_OFL':'COOLOFL_GLOBAL',
    'TRIGGER_OFL':'COOLOFL_TRIGGER',
    'CALO_OFL':'COOLOFL_CALO',
    'FWD_OFL':'COOLOFL_FWD'
    }
