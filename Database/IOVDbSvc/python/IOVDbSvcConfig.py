# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, ConfigurationError
import os
from AthenaConfiguration.ComponentFactory import CompFactory

def IOVDbSvcCfg(configFlags):

    result=ComponentAccumulator()

    #Add the conditions loader, must be the first in the sequence
    CondInputLoader=CompFactory.CondInputLoader
    result.addCondAlgo(CondInputLoader())

    IOVDbSvc=CompFactory.IOVDbSvc
    CondSvc=CompFactory.CondSvc
    ProxyProviderSvc=CompFactory.ProxyProviderSvc

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

    isMC=configFlags.Input.isMC

    # Set up IOVDbSvc
    iovDbSvc=IOVDbSvc()
    dbname=configFlags.IOVDb.DatabaseInstance

    localfile="sqlite://;schema=mycool.db;dbname="
    iovDbSvc.dbConnection=localfile+dbname
    # setup knowledge of dbinstance in IOVDbSvc, for global tag x-check
    iovDbSvc.DBInstance=dbname

    if 'FRONTIER_SERVER' in os.environ.keys() and os.environ['FRONTIER_SERVER']!="":
        iovDbSvc.CacheAlign=3


    iovDbSvc.GlobalTag=configFlags.IOVDb.GlobalTag

    result.addService(iovDbSvc,primary=True)


    # Set up POOLSvc with appropriate catalogs
    
    PoolSvc=CompFactory.PoolSvc
    poolSvc=PoolSvc()
    poolSvc.MaxFilesOpen=0
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

    DBReplicaSvc=CompFactory.DBReplicaSvc
    if not isMC:
        result.addService(DBReplicaSvc(COOLSQLiteVetoPattern="/DBRelease/"))

    # Get TagInfoMgr
    from EventInfoMgt.TagInfoMgrConfig import TagInfoMgrCfg 
    result.merge(TagInfoMgrCfg(configFlags)[0])
    
    # Set up MetaDataSvc                                                                                             
    from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg
    result.merge(MetaDataSvcCfg(configFlags, ["IOVDbMetaDataTool"]))

    return result


#Convenience method to add folders:
def addFolders(configFlags,folderstrings,detDb=None,className=None,extensible=False,tag=None,db=None):
    tagstr = ''
    if tag is not None:
        tagstr = '<tag>%s</tag>' % tag

    #Convenience hack: Allow a single string as parameter:
    if isinstance(folderstrings,str):
        return addFolderList(configFlags,((folderstrings+tagstr,detDb,className),),extensible,db)

    else: #Got a list of folders
        folderdefs=[]

        for fs in folderstrings:
            folderdefs.append((fs+tagstr,detDb,className))
        
    return addFolderList(configFlags,folderdefs,extensible,db)
    




def addFolderList(configFlags,listOfFolderInfoTuple,extensible=False,db=None):
    """Add access to the given set of folders, in the identified subdetector schema.
    FolerInfoTuple consists of (foldername,detDB,classname)

    If EXTENSIBLE is set, then if we access an open-ended IOV at the end of the list,
    the end time for this range will be set to just past the current event.
    Subsequent accesses will update this end time for subsequent events.
    This allows the possibility of later adding a new IOV using IOVSvc::setRange."""


    result=IOVDbSvcCfg(configFlags)
    iovDbSvc=result.getPrimary()

    loadFolders=[]

    for (fs,detDb,className) in listOfFolderInfoTuple:
       
        #Add class-name to CondInputLoader (if reqired)
        if className is not None:
            loadFolders.append((className, _extractFolder(fs)))

    
        if detDb is not None and fs.find("<db>")==-1:
            if db:  # override database name if provided
                dbname=db
            else:
                dbname=configFlags.IOVDb.DatabaseInstance
            if detDb not in _dblist.keys():
                raise ConfigurationError("Error, db shorthand %s not known" % detDb)
            #Append database string to folder-name
            fs+="<db>"+_dblist[detDb]+"/"+dbname+"</db>"
    
        if extensible:
            fs = fs + '<extensible/>'
    
        #Append (modified) folder-name string to IOVDbSvc Folders property
        iovDbSvc.Folders.append(fs)


    if len(loadFolders)>0:
        result.getCondAlgo("CondInputLoader").Load+=loadFolders
        AthenaPoolCnvSvc=CompFactory.AthenaPoolCnvSvc
        apcs=AthenaPoolCnvSvc()
        result.addService(apcs)
        EvtPersistencySvc=CompFactory.EvtPersistencySvc
        result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[apcs.getFullJobOptName(),]))

    return result
    
def addFoldersSplitOnline(configFlags, detDb, online_folders, offline_folders, className=None, addMCString="_OFL", splitMC=False, tag=None, forceDb=None):
    "Add access to given folder, using either online_folder  or offline_folder. For MC, add addMCString as a postfix (default is _OFL)"
    
    if configFlags.Common.isOnline and not configFlags.Input.isMC:
        folders = online_folders
    elif splitMC and not configFlags.Input.isMC:
        folders = online_folders
    else:
        # MC, so add addMCString
        detDb = detDb+addMCString
        folders = offline_folders
    result = addFolders(configFlags, folders, className=className, detDb=detDb, tag=tag, db=forceDb) 
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

def addOverride(configFlags,folder,tag):
  "Add a tag override for the specified folder"
  result=IOVDbSvcCfg(configFlags)
  iovDbSvc=result.getPrimary()
  iovDbSvc.overrideTags+=['<prefix>%s</prefix> <tag>%s</tag>' % (folder,tag)]


def _extractFolder(folderstr):
    "Extract the folder name (non-XML text) from a IOVDbSvc.Folders entry"
    fname=""
    xmltag=""
    ix=0
    while ix<len(folderstr):
        if (folderstr[ix]=='<' and xmltag==""):
            ix2=folderstr.find('>',ix)
            if (ix2!=-1):
                xmltag=(folderstr[ix+1:ix2]).strip()
                ix=ix2+1
        elif (folderstr[ix:ix+2]=='</' and xmltag!=""):
            ix2=folderstr.find('>',ix)
            if (ix2!=-1):
                xmltag=""
                ix=ix2+1
        else:
            ix2=folderstr.find('<',ix)
            if ix2==-1: ix2=len(folderstr)
            if (xmltag==""): fname=fname+folderstr[ix:ix2]
            ix=ix2
    return fname.strip()

    
        
if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc  = IOVDbSvcCfg(ConfigFlags)

    f=open('test.pkl','wb')
    acc.store(f)
    f.close()
