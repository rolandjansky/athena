# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# CondDB.py
# Configuration for Athena conditions DB access
# usage:
#   from IOVDbSvc.CondDB import conddb
# then add folders with
#   conddb.addFolder('INDET','/Indet/Align')
#   conddb.addFolder('INDET,'/Indet/Align <tag>my_explicit_tag</tag>') or
#   conddb.addFolderWithTag('INDET','/Indet/Align','my_explicit_tag')
#   conddb.addFolderSplitOnline('INDET','/Indet/OnlineF','/Indet/OfflineF')
#   conddb.addFolderSplitMC('INDET','/Indet/DataFolder','/Indet/MCFolder')
# override a tag with
#   conddb.addOverride('/Indet/Align','override_tag_value')
# block a folder from being added later
#   conddb.blockFolder('/Indet/Align')
# add a folder even though blocked
#   conddb.addFolder('INDET','/Indet/Align',True)
#   conddb.addFolderWithTag('INDET','/Indet/Align','my_explicit_tag',True)
# add markup (additional XML) to existing folder specifications
#   conddb.addMarkup('/Indet/Align','<channelSelection>1:3</channelSelection>')
#
# If the className argument is provided to addFolder, then also register
# with CondInputLoader.

import os

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from IOVSvc.IOVSvcConf import CondSvc
from IOVSvc.IOVSvcConf import CondInputLoader
from AthenaCommon.AlgSequence import AthSequencer
import StoreGate.StoreGateConf as StoreGateConf

condInputLoader = CondInputLoader( "CondInputLoader")
condSeq = AthSequencer("AthCondSeq")

svcMgr += CondSvc()
svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")
condSeq += condInputLoader

class CondDB:
    "Class to hold configuration information for Athena conditions DB access"
    def __init__(self,doOldStyleConfig=False):
        "Setup conditions DB - IOVDbSvc and connections according to GlobalFlags"
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.Logging import logging
        self.msg = logging.getLogger( 'IOVDbSvc.CondDB' )
        self.msg.debug("Loading basic services for CondDBSetup...")
    
        # AthenaPool and IOVDbSvc configuration
        from AthenaPoolCnvSvc import AthenaPool
        from IOVDbSvc import IOVDb
        # local access to IOVDbSvc parameters
        from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
        self.iovdbsvc=svcMgr.IOVDbSvc

        # initialise list of allowed DBs
        self.dblist={}
        # and list of blocked folders
        self.blocklist=[]
        
        # decide which instance to setup
        # default database
        self.dbname=''
        # names for MC and data database in case forceMC/forceData are used
        self.dbmc=''
        self.dbdata=''
        self.poolcats=[]
        from AthenaCommon.GlobalFlags import globalflags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        self.isOnline=athenaCommonFlags.isOnline()
        self.isMC=not globalflags.DataSource()=='data'
        if (globalflags.DetGeo() in ['atlas','commis']):
            # ATLAS full or commissioning geometry
            self.dbmc='OFLP200'
            self.dbdata=globalflags.DatabaseInstance() #could be 'auto'
            if self.isMC:
                # Monte Carlo
                self.dbname=self.dbmc
                self.poolcats=['oflcond']
            else:
                # real data
                if (self.dbdata=='auto'):
                    from RecExConfig.RecFlags import rec
                    self.dbdata=self._InstanceFromProjectName(rec.projectName())
                    self.msg.info("Configuring database instance %s based on project tag %s" % (self.dbdata,rec.projectName()))
                self.dbname=self.dbdata
                self.poolcats=['comcond','oflcond']
        elif (globalflags.DetGeo() in ['ctbh8','ctbh6']):
            self.dbmc='TMCP200'
            self.dbdata='TBDP200'
            if self.isMC:
                # 2004 combined testbeam, Monte Carlo
                self.dbname=self.dbmc
                self.poolcats=['tbcond','oflcond']
            else:
                # 2004 combined testbeam, real data
                self.dbname=self.dbdata
                self.poolcats=['tbcond','oflcond']
        else:
            raise RuntimeError("Unknown globalflags.DetGeo: %s" % globalflags.DetGeo())
        if (self.dbname!=''):
            self.msg.info('Setting up conditions DB access to instance %s' % self.dbname)
            # set up all access options - online schemas
            self._SetAcc('INDET','COOLONL_INDET')
            self._SetAcc('INDET_ONL','COOLONL_INDET')
            self._SetAcc('PIXEL','COOLONL_PIXEL')
            self._SetAcc('PIXEL_ONL','COOLONL_PIXEL')
            self._SetAcc('SCT','COOLONL_SCT')
            self._SetAcc('SCT_ONL','COOLONL_SCT')
            self._SetAcc('TRT','COOLONL_TRT')
            self._SetAcc('TRT_ONL','COOLONL_TRT')
            self._SetAcc('LAR','COOLONL_LAR')
            self._SetAcc('LAR_ONL','COOLONL_LAR')
            self._SetAcc('TILE','COOLONL_TILE')
            self._SetAcc('TILE_ONL','COOLONL_TILE')
            self._SetAcc('MUON','COOLONL_MUON')
            self._SetAcc('MUON_ONL','COOLONL_MUON')
            self._SetAcc('MUONALIGN','COOLONL_MUONALIGN')
            self._SetAcc('MUONALIGN_ONL','COOLONL_MUONALIGN')
            self._SetAcc('MDT','COOLONL_MDT')
            self._SetAcc('MDT_ONL','COOLONL_MDT')
            self._SetAcc('RPC','COOLONL_RPC')
            self._SetAcc('RPC_ONL','COOLONL_RPC')
            self._SetAcc('TGC','COOLONL_TGC')
            self._SetAcc('TGC_ONL','COOLONL_TGC')
            self._SetAcc('CSC','COOLONL_CSC')
            self._SetAcc('CSC_ONL','COOLONL_CSC')
            self._SetAcc('TDAQ','COOLONL_TDAQ')
            self._SetAcc('TDAQ_ONL','COOLONL_TDAQ')
            self._SetAcc('GLOBAL','COOLONL_GLOBAL')
            self._SetAcc('GLOBAL_ONL','COOLONL_GLOBAL')
            self._SetAcc('TRIGGER','COOLONL_TRIGGER')
            self._SetAcc('TRIGGER_ONL','COOLONL_TRIGGER')
            self._SetAcc('CALO','COOLONL_CALO')
            self._SetAcc('CALO_ONL','COOLONL_CALO')
            self._SetAcc('FWD','COOLONL_FWD')
            self._SetAcc('FWD_ONL','COOLONL_FWD')            
            # set up all access options - offline schemas
            # only do this if isOnline flag is NOT set
            # or MC flag is set, to allow HLT testing using MC database
            if (self.isMC or not self.isOnline):
                self._SetAcc('INDET_OFL','COOLOFL_INDET')
                self._SetAcc('PIXEL_OFL','COOLOFL_PIXEL')
                self._SetAcc('SCT_OFL','COOLOFL_SCT')
                self._SetAcc('TRT_OFL','COOLOFL_TRT')
                self._SetAcc('LAR_OFL','COOLOFL_LAR')
                self._SetAcc('TILE_OFL','COOLOFL_TILE')
                self._SetAcc('MUON_OFL','COOLOFL_MUON')
                self._SetAcc('MUONALIGN_OFL','COOLOFL_MUONALIGN')
                self._SetAcc('MDT_OFL','COOLOFL_MDT')
                self._SetAcc('RPC_OFL','COOLOFL_RPC')
                self._SetAcc('TGC_OFL','COOLOFL_TGC')
                self._SetAcc('CSC_OFL','COOLOFL_CSC')
                self._SetAcc('TDAQ_OFL','COOLOFL_TDAQ')
                self._SetAcc('DCS_OFL','COOLOFL_DCS')
                self._SetAcc('GLOBAL_OFL','COOLOFL_GLOBAL')
                self._SetAcc('TRIGGER_OFL','COOLOFL_TRIGGER')
                self._SetAcc('CALO_OFL','COOLOFL_CALO')
                self._SetAcc('FWD_OFL','COOLOFL_FWD')
            else:
                self.msg.info('Running in online mode - no access to COOLOFL schemas')
                self.iovdbsvc.OnlineMode=True
                
            # setup default connection to localfile, and put in LOCAL symbol
            localfile="sqlite://;schema=mycool.db;dbname="
            self.dblist['LOCAL']=localfile
            self.iovdbsvc.dbConnection=localfile+self.dbname

            # setup knowledge of dbinstance in IOVDbSvc, for global tag x-check
            self.iovdbsvc.DBInstance=self.dbname


            # setup DBReplicaSvc to veto DBRelease SQLite files if real data
            if not self.isMC:
                from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
                svcMgr+=DBReplicaSvc(COOLSQLiteVetoPattern="/DBRelease/")

            # enable Frontier cache alignment if it looks like Frontier will
            # be used (via FRONTIER_SERVER variable)
            if 'FRONTIER_SERVER' in os.environ.keys() and os.environ['FRONTIER_SERVER']!="":
                self.iovdbsvc.CacheAlign=3
            # setup PoolSvc catalogues
            from PoolSvc.PoolSvcConf import PoolSvc
            svcMgr+=PoolSvc()
            # add the standard catalogues
            for i in self.poolcats:
                svcMgr.PoolSvc.ReadCatalog+=["prfile:poolcond/PoolCat_%s.xml" % i]
            # also add entries which will be resolved using ATLAS_POOLCOND_PATH
            # (if set) - the actual resolution is done inside PoolSvc C++
            for i in self.poolcats:
                svcMgr.PoolSvc.ReadCatalog+=["apcfile:poolcond/PoolCat_%s.xml" % i]
            svcMgr.PoolSvc.ReadCatalog+=["apcfile:poolcond/PoolFileCatalog.xml"]
        else:
            raise RuntimeError('Not enough configuration information to setup ConditionsDB access (are GlobalFlags being used?)')
        self.msg.debug("Loading basic services for CondDBSetup... [DONE]")

    def addFolder(self,ident,folder,force=False,forceMC=False,forceData=False,
                  className=None):
        "Add access to the given folder, in the identified subdetector schema"
        # first check if access to this folder was blocked, unless forcing
        for block in self.blocklist:
            if (folder.find(block)>=0 and force==False): return
        folderadd=''
        # now check if ident is defined, and add folder
        if (ident in self.dblist.keys()):
            folderadd="<db>%s%s</db> %s" % (self.dblist[ident],self._GetName(forceMC,forceData),folder)
        elif (ident==''):
            folderadd=folder
        elif (ident.find('.')>0):
            # interpret the identifier as a filename for SQLite file
            folderadd='<db>sqlite://;schema=%s;dbname=%s</db> %s' % (ident,self._GetName(forceMC,forceData),folder)
        else:
            raise RuntimeError("Conditions database identifier %s is not defined" % ident)
        self.iovdbsvc.Folders+=[folderadd]

        if className:
            condInputLoader.Load += [ (className, self.extractFolder(folder)) ]

    def addFolderWithTag(self,ident,folder,tag,force=False,forceMC=False,forceData=False,className=None):
        "Add access to the given folder/schema, using a specified tag"
        self.addFolder(ident,folder+" <tag>%s</tag>" % tag,force,forceMC,forceData,className=className)

    def addFolderSplitOnline(self,ident,folder1,folder2,force=False,forceMC=False,forceData=False,className=None):
        "Add access to given folder, using folder1 online, folder2 offline"
        if self.isOnline and not self.isMC:
            self.addFolder(ident,folder1,force=force,forceMC=forceMC,forceData=forceData,className=className)
        else:
            self.addFolder(ident+'_OFL',folder2,force=force,forceMC=forceMC,forceData=forceData,className=className)

    def addFolderSplitMC(self,ident,folder1,folder2,force=False,forceMC=False,forceData=False,className=None):
        "Add access to given folder, using folder1 (online) for real data, folde2 (offline) for MC"
        if self.isMC:
            self.addFolder(ident+'_OFL',folder2,force=force,forceMC=forceMC,forceData=forceData,className=className)
        else:
            self.addFolder(ident,folder1,force=force,forceMC=forceMC,forceData=forceData,className=className)

    def addOverride(self,folder,tag):
        "Add a tag override for the specified folder"
        self.iovdbsvc.overrideTags+=['<prefix>%s</prefix> <tag>%s</tag>' % (folder,tag)]

    def toMetaData(self,folder):
        "Add a folder to the list to be written out for meta-data"
        self.iovdbsvc.FoldersToMetaData+=[folder]

    def blockFolder(self,folder):
        "Block use of specified conditions DB folder so data can be read from elsewhere"
        self.blocklist+=[folder]
        # check existing list of folders and remove it if found
        for i in range(0,len(self.iovdbsvc.Folders)):
            if (self.iovdbsvc.Folders[i].find(folder)>=0):
                del self.iovdbsvc.Folders[i]
                break
            
    def folderRequested(self,folder):
        "Return true if the given folder has already been requested"
        for i in self.iovdbsvc.Folders:
            if (i.find(folder)>=0): return True
        return False

    def addMarkup(self,folder,markup):
        "Add given XML markup to folder string for given folder"
        nmod=0
        for i in range(0,len(self.iovdbsvc.Folders)):
            if (self.iovdbsvc.Folders[i].find(folder)>=0):
                self.iovdbsvc.Folders[i]+=markup
                nmod+=1
        if (nmod==0):
            raise RuntimeError("conddb.addMarkup: Folder %s is not known to IOVDbSvc" % folder)
        
                
    def setGlobalTag(self,tag):
        "Set the GlobalTag value used as the key for hierarhical conditions"
        self.iovdbsvc.GlobalTag=tag
        

    def setRequireLock(self,lock=True):
        "Set the flag indicating global tags will be required to be locked"
        self.iovdbsvc.CheckLock=lock
        

    def extractFolder(self,folderstr):
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
        

    def dumpFolderTags(self,outfile,folderNames=False,folderOrigin=False):
        "Write out folder-specific tag names to file, for use with AtlCoolCopy tools"
        ofile=open(outfile,'w')
        for folderstr in self.iovdbsvc.Folders:
            # extract tag specifications and write in file
            if (folderNames):
                # also extract the name of the folder - all non-XML text
                fname=self.extractFolder(folderstr)
                if (fname!=""):
                    dbtype=""
                    # find the database schema and insert :ONL or :OFL
                    if (folderOrigin):
                        if '<db>' in folderstr:
                            idx1=folderstr.find('<db>')+4
                            idx2=folderstr.find('</db>')
                            if (idx2>0):
                                dbname=folderstr[idx1:idx2].strip()
                                if 'ONL' in dbname:
                                    dbtype=':ONL'
                                elif 'OFL' in dbname:
                                    dbtype=':OFL'
                    ofile.write('--folder %s%s\n' % (fname,dbtype))
            if '<tag>' in folderstr:
                idx1=folderstr.find('<tag>')+5
                idx2=folderstr.find('</tag>')
                if (idx2>0):
                    tag=folderstr[idx1:idx2].strip()
                    # do not write out TagInfo magic tags, except when giving
                    # folderOrigin
                    if ((tag.find('TagInfo')!=0 and tag!='HEAD') or folderOrigin):
                        ofile.write('--tag %s\n' % tag)
        # also extract any special tag names from overrideTags list
        for folderstr in self.iovdbsvc.overrideTags:
            # extract tag specifications and write in file
            if '<tag>' in folderstr:
                idx1=folderstr.find('<tag>')+5
                idx2=folderstr.find('</tag>')
                if (idx2>0):
                    tag=folderstr[idx1:idx2].strip()
                    # do not write out TagInfo magic tags
                    if (tag.find('TagInfo')!=0 and tag!='HEAD'):
                        ofile.write('--tag %s\n' % tag)
        ofile.close()

    def dumpFolderReadReal(self,outfile):
        "Dump configuration information for use in ReadReal in AthenaDBTestRec"
        ofile=open(outfile,'w')
        ofile.write('FolderList=%s\n' % self.iovdbsvc.Folders.__repr__())
        ofile.close()

    def GetInstance(self,forceMC=False,forceData=False):
        "Get the name of the DB instance in use (e.g. OFLP200)"
        return self._GetName(forceMC,forceData)


    def _SetAcc(self,ident,schema):
        "Internal helper function to setup database access string"
        self.dblist[ident]="%s/" % schema

    def _GetName(self,forceMC,forceData):
        "Internal get db instance name, taking into account forceData/MC flags"
        if forceMC:
            return self.dbmc
        elif forceData:
            return self.dbdata
        else:
            return self.dbname
        
#decide database instance based on project tag dataXX_
    def _InstanceFromProjectName(self,projectName):
        try:
            year=int(projectName[4:6]);
        except:
            self.msg.warning("Failed to extract year from project tag "+ projectName+". Guessing run2")
            return "CONDBR2"
        
        if (year>13):
            return "CONDBR2"
        else:
            return "COMP200"
        

# make instance for use
# false indicates no backward compatibility objects
conddb=CondDB(False)
