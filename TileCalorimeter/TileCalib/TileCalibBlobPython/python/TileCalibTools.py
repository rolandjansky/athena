#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# TileCalibTools.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-23
# Carlos Solans <carlos.solans@cern.ch>, 2012-10-19
# Andrey Kamenshchikov <akamensh@cern.ch>, 23-10-2013 
# Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24
################################################################
"""
Python helper module for managing COOL DB connections and TileCalibBlobs. 
"""

import ROOT
ROOT.gInterpreter.EnableAutoLoading()

import cx_Oracle
from PyCool import cool
import time, types, re, sys, os
import urllib2
import cppyy

cppyy.makeClass('std::vector<float>')
cppyy.makeClass('std::vector<std::vector<float> >')
cppyy.makeClass('std::vector<int>')
cppyy.makeClass('std::vector<std::vector<int> >')
cppyy.makeClass('std::vector<unsigned int>')
cppyy.makeClass('std::vector<std::vector<unsigned int> >')


from TileCalibBlobObjs.Classes import *

sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias

#=== get a logger
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("TileCalibTools")


#======================================================================
#===
#=== Global helper functions
#===
#======================================================================

#
#______________________________________________________________________
#=== useful constants
MINRUN      = 0
MINLBK      = 0
MAXRUN      = cool.Int32Max
MAXLBK      = cool.UInt32Max 
UNIX2COOL   = 1000000000 
UNIXTMAX    = cool.Int32Max
# empty Tile channel for storing laser partition variation. DO NOT CHANGE.
LASPARTCHAN = 43 

#
#______________________________________________________________________
def getLastRunNumber(partition=""):
    """
    Return the run number of last run taken in the pit
    """
    try:
        response = urllib2.urlopen("http://atlas-service-db-runlist.web.cern.ch/atlas-service-db-runlist/cgi-bin/latestRun.py")
        data = response.read().split()
    except:
        data=[]
    return int(data[0])+1 if len(data) else 222222

#
#______________________________________________________________________
def getTilePrefix(ofl=True,splitOnlInOflSchema=True):
    """
    Returns the common Tile prefix used for all COOL folders.
    ofl = False ... single version folders
    ofl = True  ... multiversion folders
    splitOnlInOflSchema = False ... offline only folders or
                    splitOnline folders in Online schema
    splitOnlInOflSchema = True ... splitOnlineFolders in
                    offline schema
    """
    if ofl:
        if splitOnlInOflSchema:
            return "/TILE/OFL02/"
        else:
            return "/TILE/OFL01/"
    else:
        return "/TILE/ONL01/"

#
#______________________________________________________________________
def getTilePrefixes():
    """
    Returns a list of all TILE folder prefixes
    """
    return ["/TILE/ONL01/","/TILE/OFL01/","/TILE/OFL02/"]

#
#______________________________________________________________________
def getAthenaFolderDescr(type="run-lumi"):
    """
    Returns the run-lumi type folder description needed to read back the folder content
    as a CondAttrListCollection in Athena.
    """
    desc ='<timeStamp>'+type+'</timeStamp>'
    desc+='<addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader>'
    desc+='<typeName>CondAttrListCollection</typeName>'
    return desc

#
#______________________________________________________________________
def getAthenaFolderType(folderDescr):
    type = re.compile(".*<timeStamp>(.*)</timeStamp>.*").search(folderDescr)
    if not type:
        raise Exception("No folder type info found in \'%s\'" % folderDescr)
    return type.groups()[0]


#
#______________________________________________________________________
def openDb(db, instance, mode="READONLY",schema="COOLONL_TILE",sqlfn="tileSqlite.db"):
    """
    Opens a COOL db connection.
    - connStr: The DB connection string. The following
               abbreviations are recognized:
               * SQLITE: Opens tileSqlite.db file in current directory (OFLP200)
    - mode: Can be READONLY (default), RECREATE or UPDATE
    """
    #=== check for valid db names
    validDb = ["SQLITE", "ORACLE"]
    if db not in validDb:
        raise Exception( "DB not valid: %s, valid DBs are: %s" % (db,validDb) )

    #=== check for valid instance names
    #=== see https://twiki.cern.ch/twiki/bin/view/Atlas/CoolProdAcc
    validInstance = ["COMP200", "CONDBR2", "CMCP200", "OFLP200"]
    if instance not in validInstance:
        raise Exception( "Instance not valid: %s, valid instance are: %s" % (instance,validInstance) )

    #=== check for valid schema names
    VALIDSCHEMAS = ["COOLONL_TILE","COOLOFL_TILE"]
    if schema not in VALIDSCHEMAS:
        raise Exception( "Schema not valid: %s, valid schemas are: %s" % (schema,VALIDSCHEMAS) )
    
    #=== construct connection string
    connStr = ""
    if db=='SQLITE':
        if mode=="READONLY" and not os.path.exists(sqlfn):
            raise Exception( "Sqlite file %s does not exist" % (sqlfn) )
        if (mode=="RECREATE" or mode=="UPDATE") and not os.path.exists(os.path.dirname(sqlfn)):
            dirn=os.path.dirname(sqlfn)
            if dirn: os.makedirs(dirn)
        connStr="sqlite://X;schema=%s;dbname=%s" % (sqlfn,instance)
    elif db=='ORACLE':
        connStr='oracle://%s;schema=ATLAS_%s;dbname=%s' % ('ATLAS_COOLPROD',schema,instance)
#        connStr=schema+'/'+instance

    return openDbConn(connStr, mode)

#
#______________________________________________________________________
def openDbConn(connStr, mode="READONLY"):
    """
    Opens a COOL db connection or sqlite file.
    - connStr: The DB connection string.
    - mode: Can be READONLY (default), RECREATE or UPDATE
    """
# split the name into schema and dbinstance

    splitname=connStr.split('/')
    if (len(splitname)!=2):  # string connection ready to be used as it is
        connStr_new=connStr
    else:                    # construct connection string
        schema=splitname[0]
        instance=splitname[1]
        connStr_new='oracle://%s;schema=ATLAS_%s;dbname=%s' % ('ATLAS_COOLPROD',schema,instance)

    #=== get dbSvc and print header info
    #.. get a CoraCoolDatabaseSvc object
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    log.info( "---------------------------------------------------------------------------------" )
    log.info( "-------------------------- TileCalibTools.openDbConn ----------------------------" )
    log.info( "- using COOL version %s" % dbSvc.serviceVersion()                                  )
    log.info( "- opening TileDb: %s" %connStr_new                                                    )
    log.info( "- mode: %s" %mode                                                                  )
    log.info( "---------------------------------------------------------------------------------" )

    #=== action depends on mode
    if mode=="READONLY":
        #=== open read only
        try:
            db=dbSvc.openDatabase(connStr_new,True)
        except Exception, e:
            log.debug( e )
            log.critical("Could not connect to %s" % connStr )
            return None
        return db
    elif mode=="RECREATE":
        #=== recreating database
        dbSvc.dropDatabase(connStr_new)
        try:
            db = dbSvc.createDatabase(connStr_new)
        except Exception, e:
            log.debug( e )
            log.critical( "Could not create database, giving up..." )
            return None
        return db
    elif mode=="UPDATE":
        #=== update database
        try:
            db=dbSvc.openDatabase(connStr_new,False)
        except Exception, e:
            log.debug( e )
            log.warning( "Could not connect to \'%s\', trying to create it...." % connStr )
            try:
                db=dbSvc.createDatabase(connStr_new)
            except Exception, e:
                log.debug( e )
                log.critical( "Could not create database, giving up..." )
                return None
        return db
    else:
        log.error("Mode \"%s\" not recognized" % mode )
        return None


    
#
#______________________________________________________________________
def coolTimeFromRunLumi(runNum, lbkNum):
    """
    Returns COOL timeStamp build from run and lumi block numbers
    """
    return (int(runNum)<<32) + int(lbkNum)

#
#______________________________________________________________________
def decodeTimeString(timeString):
    """
    Retruns UNIX time stamp given an input time string
    """
    return int(time.mktime(time.strptime(timeString,"%Y-%m-%d %H:%M:%S")))

#
#______________________________________________________________________
def getCoolValidityKey(pointInTime, isSince=True):
    """
    The interpretation of pointInTime depends on their type:
    - tuple(int,int) : run and lbk number 
    - integer        : Values are interpreted as unix time stamps
    - string         : time stamp of format 'yyyy-mm-dd hh:mm:ss'
    """

    validityKey = None
    
    #=== string: convert to unix time and treat as latter
    if type(pointInTime) == types.StringType:
        pointInTime = decodeTimeString(pointInTime)

    #=== integer: unix time stamp
    if type(pointInTime) == types.IntType:
        if pointInTime >=0:
            validityKey = pointInTime * UNIX2COOL
        else:
            if isSince: validityKey = int(time.time()) * UNIX2COOL
            else      : validityKey = cool.ValidityKeyMax
    #=== run-lumi tuple
    elif type(pointInTime) == types.TupleType:
        validityKey = coolTimeFromRunLumi(pointInTime[0],pointInTime[1])
    #=== catch other types
    else:
        raise Exception("Unrecognized pointInTime type=%s" % type(pointInTime))
    return cool.ValidityKey(validityKey)


#
#____________________________________________________________________
def getFolderTag(db, folderPath, globalTag):

    tag=""
    if globalTag.startswith("/") or globalTag.startswith("TileO"):
        tag = globalTag
        log.warning("Using tag as-is for folder %s" % folderPath)
    elif '/TILE/ONL01' in folderPath:
        log.info("Using empty tag for single-version folder %s" % folderPath)
    elif globalTag.startswith(" "):
        log.warning("Using empty tag for folder %s" % folderPath)
    elif globalTag=="":
        tag = TileCalibUtils.getFullTag(folderPath, globalTag)
        log.warning("Using tag with empty suffix for folder %s" % folderPath)
    else:
        schema='COOLOFL_TILE/CONDBR2'
        if isinstance(db, (str, unicode)):
            if 'OFLP200' in db or 'MC' in db:
                schema='COOLOFL_TILE/OFLP200'
                if not globalTag.startswith("OFLCOND"):
                   if globalTag.startswith("RUN"):
                      globalTag='OFLCOND-'+globalTag
                      log.info("Using Simulation global tag \'%s\'" % globalTag)
            elif 'COMP200' in db or 'RUN1' in db:
                schema='COOLOFL_TILE/COMP200'
                if globalTag!='UPD1' and globalTag!='UPD4' and ('UPD1' in globalTag or 'UPD4' in globalTag or 'COND' not in globalTag):
                   log.info("Using suffix \'%s\' as it is" % globalTag)
                else:
                   globalTag='COMCOND-BLKPA-RUN1-06'
                   log.info("Using RUN1 global tag \'%s\'" % globalTag)
        if schema == 'COOLOFL_TILE/CONDBR2':
            if globalTag=='CURRENT' or globalTag=='UPD4' or globalTag=='':
                globalTag=resolveAlias.getCurrent()
                log.info("Resolved CURRENT globalTag to \'%s\'" % globalTag)
            elif globalTag=='CURRENTES' or globalTag=='UPD1':
                globalTag=resolveAlias.getCurrentES()
                log.info("Resolved CURRENT ES globalTag to \'%s\'" % globalTag)
            elif globalTag=='NEXT':
                globalTag=resolveAlias.getNext()
                log.info("Resolved NEXT globalTag to \'%s\'" % globalTag)
            elif globalTag=='NEXTES':
                globalTag=resolveAlias.getNextES()
                log.info("Resolved NEXT ES globalTag to \'%s\'" % globalTag)
        globalTag=globalTag.replace('*','')
        if 'UPD1' in globalTag or 'UPD4' in globalTag or 'COND' not in globalTag:
            tag = TileCalibUtils.getFullTag(folderPath, globalTag)
            log.info("Resolved localTag \'%s\' to folderTag \'%s\'" % (globalTag,tag))
        else:
            if not isinstance(db, (str, unicode)):
                try:
                    folder = db.getFolder(folderPath)
                    tag = folder.resolveTag(globalTag)
                    log.info("Resolved globalTag \'%s\' to folderTag \'%s\'" % (globalTag,tag))
                    schema=""
                except Exception, e:
                    log.warning(e)
                    log.warning("Using %s to resolve globalTag",schema)
            if len(schema):
                dbr = openDbConn(schema,'READONLY')
                folder = dbr.getFolder(folderPath)
                tag = folder.resolveTag(globalTag)
                dbr.closeDatabase()
                log.info("Resolved globalTag \'%s\' to folderTag \'%s\'" % (globalTag,tag))

    return tag

#
#____________________________________________________________________
def runLumiFromCoolTime(iov):
    return (int(iov >> 32), int(iov & 0xFFFFFFFF))

#
#____________________________________________________________________
def copyFolder(dbr, dbw, folder, tagr, tagw, chanNum, pointInTime1, pointInTime2):

    log.info("Copy channel %i" % chanNum)

    folderR = dbr.getFolder(folder)
    folderW = dbw.getFolder(folder)
    
    chansel = cool.ChannelSelection(chanNum)

    iov1 = getCoolValidityKey(pointInTime1,False)
    iov2 = getCoolValidityKey(pointInTime2,False)
    
    if tagr=='':
        multiVersion = False
        objs = folderR.browseObjects(iov1,iov2,chansel)
    else:
        multiVersion = True
        objs = folderR.browseObjects(iov1,iov2,chansel,tagr)
    while objs.goToNext():
        obj=objs.currentRef()
        sinceCool=obj.since()
        if sinceCool < iov1:
            sinceCool = iov1
        untilCool=obj.until()
        data=obj.payload()
        sinceTup = runLumiFromCoolTime(sinceCool)
        untilTup = runLumiFromCoolTime(untilCool)
        log.debug("Copy entry: [%i,%i] - [%i,%i]: " % (sinceTup[0],sinceTup[1],untilTup[0],untilTup[1]), data)
        folderW.storeObject(sinceCool, untilCool, data, chanNum, tagw, multiVersion)



#======================================================================
#===
#=== TileBlobWriter 
#===
#======================================================================

#
#______________________________________________________________________
class TileBlobWriter(TileCalibLogger):
    """
    TileCalibBlobWriterBase is a helper class, managing the details of
    COOL interactions for the user of TileCalibBlobs.
    """

    #____________________________________________________________________
    def __init__(self, db, folderPath, calibDrawerType,
                 isMultiVersionFolder=True, isRunLumiTimeStamp=True):
        """
        Input:
        - db        : db should be an open database connection
        - folderPath: full folder path to create or update
        """

        #=== initialize base class
        TileCalibLogger.__init__(self,"TileBlobWriter")

        #=== store db
        self.__db  = db

        #=== determine folder mode
        folderMode = cool.FolderVersioning.MULTI_VERSION
        if not isMultiVersionFolder:
            folderMode = cool.FolderVersioning.SINGLE_VERSION

        #=== determine folder description
        folderDescr = getAthenaFolderDescr("run-lumi")
        if not isRunLumiTimeStamp:
            folderDescr = getAthenaFolderDescr("time")

        #=== access folder in db
        try:
            #=== create folder if it does not exist
            if self.__db.existsFolder(folderPath):
                self.__folder = self.__db.getFolder(folderPath)
                #=== check folder mode
                modeInCool = self.__folder.versioningMode()
                if modeInCool!=folderMode:
                    str = "Incompatible folder mode detected, COOL folder has type "
                    if modeInCool==cool.FolderVersioning.MULTI_VERSION: str += "MULTI"
                    else:                                               str += "SINGLE"
                    raise Exception(str)
            else:
                #=== create folder if it does not exist
                payloadSpec = cool.RecordSpecification()
                payloadSpec.extend( 'TileCalibBlob', cool.StorageType.Blob64k )
                folderSpec = cool.FolderSpecification(folderMode, payloadSpec)
                self.__folder = db.createFolder(folderPath, folderSpec, folderDescr, True)
        except Exception, e:
            self.log().critical( e )
            raise

        #=== initialize channel dictionaries
        self.__chanDictRecord = {} # <int, cool.Record    >
        self.__chanDictDrawer = {} # <int, TileCalibDrawer>
        
        #=== create default vectors based on calibDrawerType
        self.__calibDrawerType = calibDrawerType
        if   calibDrawerType=='Flt':
            cppyy.makeClass('std::vector<float>')
            self.__defVec = cppyy.gbl.std.vector('std::vector<float>')()
        elif calibDrawerType=='Bch' or calibDrawerType=='Int':                    
            cppyy.makeClass('std::vector<unsigned int>')
            self.__defVec = cppyy.gbl.std.vector('std::vector<unsigned int>')()
        else:
            raise Exception("Unknown calibDrawerType: %s" % calibDrawerType)
        

    #____________________________________________________________________
    def register(self, since=(MINRUN,MINLBK), until=(MAXRUN,MAXLBK), tag=""):
        """
        Registers the folder in the database.
        - since: lower limit of IOV
        - until: upper limit of IOV
        - tag  : The cool folder tag to write to

        The interpretation of the 'since' and 'until' inputs depends on their type:
        - tuple(int,int) : run and lbk number 
        - integer        : Values are interpreted as unix time stamps
                           If since<0, current time is assumed
                           If until<0, infinity is assumed
        - string         : time stamp of format 'yyyy-mm-dd hh:mm:ss'
        """

        #=== check for inconsistent input
        if type(since)!=type(until):
            raise Exception("Inconsistent types: since=%s, until=%s" % (type(since),type(until)))

        #=== write to user tag only if multiversion mode
        userTagOnly = True
        if self.__folder.versioningMode()==cool.FolderVersioning.SINGLE_VERSION:
            userTagOnly = False
            #=== no folder Tag allowed for singleversion
            if tag!="":
                self.log().warning( "Trying to store with tag \"%s\" to SINGLE_VERSION folder" % tag )
                self.log().warning( "... resetting tag to \"\"!"                                     )
                tag=""

        #=== get IOV limits
        sinceCool = getCoolValidityKey(since, True )
        untilCool = getCoolValidityKey(until, False)
        if untilCool <= sinceCool:
            raise Exception("Until(%i) <= Since(%i)" % (untilCool,sinceCool))

        #=== build IOV string
        iovString = ""       
        if type(since) == types.TupleType:
            iovString = "[%i,%i] - [%i,%i]" % (since[0],since[1],until[0],until[1])
        else:
            sinceInfo = time.localtime( sinceCool / UNIX2COOL )
            untilInfo = time.localtime(min(UNIXTMAX, (untilCool/UNIX2COOL)))
            untilStr = "<infinity>"
            if untilCool<cool.ValidityKeyMax:
                untilStr = time.asctime(untilInfo)
                if (untilCool/UNIX2COOL)>UNIXTMAX:
                    untilStr = " > "+untilStr
            iovString = "[%s] - [%s]" % (time.asctime(sinceInfo), untilStr)

        #=== build tag
        folderTag=tag

        #=== print info
        comment=self.getComment()
        noComment = (comment is None) or (comment == "None") or (comment.startswith("None") and comment.endswith("None"))
        self.log().info( "Registering folder %s with tag \"%s\"" % (self.__folder.fullPath(),folderTag))
        self.log().info( "... with IOV          : %s"            % iovString                          )
        if noComment:
            self.log().info( "... WITHOUT comment field" )
        else:
            self.log().info( "... with comment field: \"%s\""        % self.getComment()                  )

        #=== register all channels by increasing channel number
        chanList = sorted(self.__chanDictRecord.keys())
        cnt=0
        for chanNum in chanList:
            if chanNum==1000 and noComment: continue
            data = self.__chanDictRecord[chanNum]
            strout = "cool channel=%4i" % chanNum
            self.log().debug("Registering %s %s" % (strout, data))
            channelId = cool.ChannelId(chanNum)
            self.__folder.storeObject(sinceCool, untilCool, data, channelId, folderTag, userTagOnly)
            cnt+=1
        if noComment:
            self.log().info( "... %d cool channels have been written in total" % cnt )
        else:
            self.log().info( "... %d cool channels have been written in total (including comment field)" % cnt )

    #____________________________________________________________________
    def setComment(self, author, comment):
        """
        Sets a general comment in the comment channel.
        """
        try:
            chanNum = TileCalibUtils.getCommentChannel()
            data = self.__chanDictRecord.get(chanNum)
            if not data:
                spec = self.__folder.payloadSpecification()
                data = cool.Record( spec )
                self.__chanDictRecord[chanNum] = data
            blob = data['TileCalibBlob']
            cmt = TileCalibDrawerCmt.getInstance(blob,author,comment)
        except Exception, e:
            self.log().critical( e )
        
    #____________________________________________________________________
    def getComment(self):
        """
        Returns the general comment (default if none is set)
        """
        try:
            chanNum = TileCalibUtils.getCommentChannel()
            data = self.__chanDictRecord.get(chanNum)
            if not data:
                return "<No general comment!>"
            blob = data['TileCalibBlob']
            cmt = TileCalibDrawerCmt.getInstance(blob)
            return cmt.getFullComment()
        except Exception, e:
            self.log().critical( e )

    #____________________________________________________________________
    def getDrawer(self, ros, drawer, calibDrawerTemplate=None):
        """
        Returns a TileCalibDrawer object of requested type
        for the given ROS and drawer.
        """
        try:

            #=== check for already initialized calibDrawers
            chanNum = TileCalibUtils.getDrawerIdx(ros,drawer)
            calibDrawer = self.__chanDictDrawer.get(chanNum,None)

            #=== initialize new calibDrawer if needed
            if not calibDrawer:

                #=== create new blob
                spec = self.__folder.payloadSpecification()
                data = cool.Record( spec )
                self.__chanDictRecord[chanNum] = data
                blob = data['TileCalibBlob']

                #=== Create calibDrawer based on requested calibDrawerType
                if   self.__calibDrawerType=='Flt':
                    calibDrawer = TileCalibDrawerFlt.getInstance(blob,self.__defVec,0,0)
                elif self.__calibDrawerType=='Int':
                    calibDrawer = TileCalibDrawerInt.getInstance(blob,self.__defVec,0,0)
                elif self.__calibDrawerType=='Bch':
                    calibDrawer = TileCalibDrawerBch.getInstance(blob,self.__defVec,0,0)
                else:
                    raise Exception( "Invalid blob type requested: %s" % type )

            #=== clone if requested
            if calibDrawerTemplate:
                calibDrawer.clone(calibDrawerTemplate)
            
            #=== put updated calibDrawer in dictionary and return
            self.__chanDictDrawer[chanNum] = calibDrawer
            return calibDrawer

        except Exception, e:
            self.log().critical( e )
            return None

    #____________________________________________________________________
    def zeroBlob(self, ros, drawer):
        """
        Resets blob size to zero
        """
        try:
            calibDrawer = None
            chanNum = TileCalibUtils.getDrawerIdx(ros,drawer)
            data = self.__chanDictRecord.get(chanNum)
            if not data:
                spec = self.__folder.payloadSpecification()
                data = cool.Record( spec )
                self.__chanDictRecord[chanNum] = data
            blob = data['TileCalibBlob']
            blob.resize(0)
        except Exception, e:
            self.log().critical( e )
            return None



#======================================================================
#===
#=== TileBlobReader 
#===
#======================================================================

#
#______________________________________________________________________
class TileBlobReader(TileCalibLogger):
    """
    TileCalibBlobReader is a helper class, managing the details of COOL interactions for
    the user of TileCalibBlobs.
    """

    #____________________________________________________________________
    def __init__(self, db, folder, tag=""):
        """
        Input:
        - db    : db should be an open database connection 
        - folder: full folder path
        - tag   : The folder tag, e.g. \"000-00\"
        """
        #=== initialize base class
        TileCalibLogger.__init__(self,"TileBlobReader")

        #=== try to open db
        try:
            self.__db  = db                             # type: CoraCoolDatabase
            self.__folder = self.__db.getFolder(folder) # type: CoraCoolFolder
        except Exception, e:
            self.log().critical( e )
            raise

        #=== determine if "run-lumi" or "time" folder
        validFolderTypes = ("run-lumi","time")
        folderDescr = self.__folder.description()
        self.__folderType = getAthenaFolderType(folderDescr)
        if self.__folderType not in validFolderTypes:
            raise Exception("Invalid folder type found: \'%s\'" % self.__folderType)

        #=== use camelized full folder path only if tag is given
        self.__tag = tag

        #=== initialize dictionary to keep reference to DB object of given ros/drawer
        #=== and timestamp, so they do not go out of scope
        self.__objDict = {}

    #____________________________________________________________________
    def getComment(self, pointInTime):
        """
        Returns the general comment (default if none is set)
        """
        validityKey = getCoolValidityKey(pointInTime)
        try:
            chanNum = TileCalibUtils.getCommentChannel()
            obj = self.__folder.findObject(validityKey, chanNum, self.__tag)
            self.log().debug("getComment:Fetching from DB: %s" % obj)
            blob = obj.payload()[0]
            cmt = TileCalibDrawerCmt.getInstance(blob)
            return cmt.getFullComment()
        except Exception, e:
            return "<no comment found>"
        
    #____________________________________________________________________
    def getDefault(self, ros, drawer):
        """
        Returns a default drawer number (among first 20 COOL channels) for any drawer in any partition
        """
        if ros==0:
           if drawer<=4 or drawer==12 or drawer>=20:
              drawer1=0
           elif drawer<12:
              drawer1=4
           else:
              drawer1=12
        elif ros==1 or ros==2:
           drawer1=4
        elif ros==3:
           OffsetEBA = [ 0, 0, 0, 0, 0, 0, 3, 2, #// Merged E+1: EBA07; Outer MBTS: EBA08
                         0, 0, 0, 0, 7, 6, 5, 7, #// D+4: EBA13, EBA16; Special D+4: EBA14; Special D+40: EBA15
                         7, 6, 6, 7, 0, 0, 0, 2, #// D+4: EBA17, EBA20; Special D+4: EBA18, EBA19; Outer MBTS: EBA24
                         3, 0, 0, 0, 0, 0, 0, 0, #// Merged E+1:  EBA25
                         0, 0, 0, 0, 0, 0, 1, 1, #// Inner MBTS + special C+10: EBA39, EBA40
                         1, 1, 2, 3, 0, 0, 0, 0, #// Inner MBTS + special C+10: EBA41, EBA42; Outer MBTS: EBA43; Merged E+1: EBA44
                         0, 0, 0, 0, 3, 2, 1, 1, #// Merged E+1: EBA53; Outer MBTS: EBA54; Inner MBTS + special C+10: EBA55, EBA56
                         1, 1, 0, 0, 0, 0, 0, 0] #// Inner MBTS + special C+10: EBA57, EBA58
           drawer1 = 12 + OffsetEBA[drawer]
        elif ros==4:
           OffsetEBC = [ 0, 0, 0, 0, 0, 0, 3, 2, #// Merged E-1: EBC07; Outer MBTS: EBC08
                         0, 0, 0, 0, 7, 6, 6, 7, # // D-4: EBC13, EBC16; Special D-4: EBC14, EBC15; 
                         7, 5, 6, 7, 0, 0, 0, 2, #// D-4: EBC17, EBC20; Special D-40 EBC18; Special D-4: EBC19; Outer MBTS: EBC24
                         3, 0, 0, 3, 4, 0, 3, 4, #// Merged E-1:  EBC25, EBC28, EBC31; E-4': EBC29, EBC32
                         0, 4, 3, 0, 4, 3, 1, 1, #// E-4': EBC34, EBC37; Merged E-1: EBC35, EBC38; Inner MBTS + special C-10: EBC39, EBC40
                         1, 1, 2, 3, 0, 0, 0, 0, #// Inner MBTS + special C-10: EBC41, EBC42; Outer MBTS: EBC43; Merged E-1: EBC44
                         0, 0, 0, 0, 3, 2, 1, 1, #// Merged E-1: EBC53; Outer MBTS: EBC54; Inner MBTS + special C-10: EBC55, EBC56
                         1, 1, 0, 0, 0, 0, 0, 0] #// Inner MBTS + special C-10: EBC57, EBC58
           drawer1 = 12 + OffsetEBC[drawer]
        else:
           drawer1=0

        return (0,drawer1)

    #____________________________________________________________________
    def getDrawer(self, ros, drawer, pointInTime, printError=True, useDefault=True):
        """
        Returns a TileCalibDrawer object for the given ROS and drawer.
        """

        validityKey = getCoolValidityKey(pointInTime)
        self.log().debug("Validity key is %s" % validityKey)
        try:
            calibDrawer = None
            #=== Have we retrieved data previously?
            key = (ros,drawer,validityKey)
            obj = self.__objDict.get(key)
            #=== ... if not, get it from DB
            if not obj:
                chanNum = TileCalibUtils.getDrawerIdx(ros,drawer)
                obj = self.__folder.findObject(validityKey, chanNum, self.__tag)
                self.log().debug("Fetching from DB: %s" % obj)
                blob = obj.payload()[0]
                self.log().debug("blob size: %d" % blob.size())
                #=== default policy 
                if not useDefault and blob.size()==0:
                    return 0
                while blob.size()==0:
                    #=== no default at all?
                    if ros==0 and drawer==0:
                        raise Exception('No default available')
                    #=== follow default policy
                    ros,drawer = self.getDefault(ros,drawer)
                    chanNum = TileCalibUtils.getDrawerIdx(ros,drawer)
                    obj = self.__folder.findObject(validityKey, chanNum, self.__tag)
                    blob = obj.payload()[0]
                #=== store object in dictionary
                self.__objDict[key] = obj
            #=== get blob
            blob = obj.payload()[0]
            self.log().debug("blob size: %d" % blob.size())
                
            #=== create calibDrawer depending on type
            calibDrawer = TileCalibDrawerCmt.getInstance(blob)
            typeName =    TileCalibType.getClassName(calibDrawer.getObjType())
            del calibDrawer
            if   typeName=='TileCalibDrawerFlt':
                calibDrawer = TileCalibDrawerFlt.getInstance(blob)
                self.log().debug( "typeName = Flt " )
            elif typeName=='TileCalibDrawerInt':
                calibDrawer = TileCalibDrawerInt.getInstance(blob)
                self.log().debug( "typeName = Int " )
            elif typeName=='TileCalibDrawerBch':
                calibDrawer = TileCalibDrawerBch.getInstance(blob)
                self.log().debug( "typeName = Bch " )
            elif typeName=='TileCalibDrawerOfc':
                calibDrawer = TileCalibDrawerOfc.getInstance(blob)
                self.log().debug( "typeName = Ofc " )
            else:
                raise Exception( "Invalid blob type requested: %s" % typeName )
            return calibDrawer
        except Exception, e:
            if printError: self.log().error("TileCalibTools.getDrawer(): Fetching of ros=%i, drawer=%i failed with exception %s"%(ros,drawer,e))
            return None

    #____________________________________________________________________
    def getDefaultDrawer(self, ros, drawer, pointInTime, printError=True):
        """
        Returns a TileCalibDrawer object for the given ROS and drawer.
        """

        validityKey = getCoolValidityKey(pointInTime)
        self.log().debug("Validity key is %s" % validityKey)
        try:
            calibDrawer = None
            #=== Have we retrieved data previously?
            key = (ros,drawer,validityKey)
            obj = self.__objDict.get(key)
            #=== ... if not, get it from DB
            if not obj:
                chanNum = TileCalibUtils.getDrawerIdx(ros,drawer)
                obj = self.__folder.findObject(validityKey, chanNum, self.__tag)
                self.log().debug("Fetching from DB: %s" % obj)
                blob = obj.payload()[0]
                self.log().debug("blob size: %d" % blob.size())
                #=== default policy 
                while blob.size()==0:
                    #=== no default at all?
                    if ros==0 and drawer==0:
                        raise Exception('No default available')
                    #=== follow default policy
                    ros,drawer = self.getDefault(ros,drawer)
                    chanNum = TileCalibUtils.getDrawerIdx(ros,drawer)
                    obj = self.__folder.findObject(validityKey, chanNum, self.__tag)
                    blob = obj.payload()[0]
                #=== store object in dictionary
                self.__objDict[key] = obj
            #=== get blob
            blob = obj.payload()[0]
            self.log().debug("blob size: %d" % blob.size())
                
            #=== create calibDrawer depending on type
            calibDrawer = TileCalibDrawerCmt.getInstance(blob)
            typeName =    TileCalibType.getClassName(calibDrawer.getObjType())
            del calibDrawer
            if   typeName=='TileCalibDrawerFlt':
                calibDrawer = TileCalibDrawerFlt.getInstance(blob)
                self.log().debug( "typeName = Flt " )
            elif typeName=='TileCalibDrawerInt':
                calibDrawer = TileCalibDrawerInt.getInstance(blob)
                self.log().debug( "typeName = Int " )
            elif typeName=='TileCalibDrawerBch':
                calibDrawer = TileCalibDrawerBch.getInstance(blob)
                self.log().debug( "typeName = Bch " )
            elif typeName=='TileCalibDrawerOfc':
                calibDrawer = TileCalibDrawerOfc.getInstance(blob)
                self.log().debug( "typeName = Ofc " )
            else:
                raise Exception( "Invalid blob type requested: %s" % typeName )
            return calibDrawer
        except Exception, e:
            if printError: self.log().error("TileCalibTools.getDefaultDrawer(): Fetching of ros=%i, drawer=%i failed with exception %s"%(ros,drawer,e))
            return None

    #____________________________________________________________________
    def getDBobjsWithinRange(self, ros, drawer, point1inTime=(0,0), point2inTime=(2147483647,4294967295), printError=True):
        """
        Returns all DB objects for the given ROS and drawer, within given validity range -- default: [0-Infty)
        Check getBlobsWithinRange for an example on how to loop over objects and check validity ranges.
        """

        validityKey1 = getCoolValidityKey(point1inTime,True)
        validityKey2 = getCoolValidityKey(point2inTime,False)

        #print "Validity keys range is %s - %s" % (validityKey1, validityKey2)
        self.log().debug("Validity key range is %s - %s" % (validityKey1,validityKey2))

        objs = None
        try:
            dbChanNum = drawer if ros<0 else TileCalibUtils.getDrawerIdx(ros,drawer)
            dbChanSel = cool.ChannelSelection(dbChanNum)
            #self.log().debug("Fetching blobs from DB: %s" % obj)
            objs = self.__folder.browseObjects(validityKey1,validityKey2,dbChanSel,self.__tag)
        except Exception, e:
            if printError: self.log().error("TileCalibTools.getDBobjsWithinRange(): Fetching of ros=%i, drawer=%i failed with exception %s"%(ros,drawer,e))

        return objs

    #____________________________________________________________________
    def getBlobsWithinRange(self, ros, drawer, point1inTime=(0,0), point2inTime=(2147483647,4294967295)):
        """
        Returns all blob objects for the given ROS and drawer, within given validity range -- default: [0-Infty)
        Note: the blobs don't contain validity range info.  Check method getDBobjsWithinRange()
        """

        validityKey1 = getCoolValidityKey(point1inTime,True)
        validityKey2 = getCoolValidityKey(point2inTime,False)

        print "Validity keys range is %s - %s" % (validityKey1, validityKey2)
        self.log().debug("Validity key range is %s - %s" % (validityKey1,validityKey2))

        objs = getDBobjsWithinRange(self, ros, drawer, point1inTime, point2inTime)

        #-- Loop over objs to extract blobs
        blobs = []
        calibDrawer = None
        while objs.goToNext():
            obj=objs.currentRef()
            sinceCool=obj.since()
            if sinceCool < validityKey1:
                sinceCool = validityKey1
            untilCool=obj.until()
            blob = obj.payload()[0]
            print "[%d,%d)-[%d,%d) - %s" % ((sinceCool>>32),(sinceCool&0xFFFFFFFF),(untilCool>>32),(untilCool&0xFFFFFFFF),blob)
            self.log().debug("blob size: %d" % blob.size())

            #=== default policy 
            while blob.size()==0:
                #=== no default at all?
                if ros==0 and drawer==0:
                    raise Exception('No default available')
                #=== follow default policy
                ros,drawer = self.getDefault(ros,drawer)
                chanNum = TileCalibUtils.getDrawerIdx(ros,drawer)
                obj = self.__folder.findObject(sinceCool, chanNum, self.__tag)
                blob = obj.payload()[0]
                self.log().debug("blob size: 0 --> default: %d" % blob.size())

            #=== store object in dictionary
            self.__objDict[sinceCool] = obj

            #=== create calibDrawer depending on type
            calibDrawer = TileCalibDrawerCmt.getInstance(blob)
            typeName =    TileCalibType.getClassName(calibDrawer.getObjType())
            del calibDrawer
            if   typeName=='TileCalibDrawerFlt':
                calibDrawer = TileCalibDrawerFlt.getInstance(blob)
                self.log().debug( "typeName = Flt " )
            elif typeName=='TileCalibDrawerInt':
                calibDrawer = TileCalibDrawerInt.getInstance(blob)
                self.log().debug( "typeName = Int " )
            elif typeName=='TileCalibDrawerBch':
                calibDrawer = TileCalibDrawerBch.getInstance(blob)
                self.log().debug( "typeName = Bch " )
            elif typeName=='TileCalibDrawerOfc':
                calibDrawer = TileCalibDrawerOfc.getInstance(blob)
                self.log().debug( "typeName = Ofc " )
            else:
                raise Exception( "Invalid blob type requested: %s" % typeName )

            blobs.append( calibDrawer )

        return blobs

    #____________________________________________________________________
    def folderIsMultiVersion(self):
        """
        Returns true if MultiVersion folder is connected
        """
        if self.__folder.versioningMode()==cool.FolderVersioning.MULTI_VERSION:
            return True
        else:
            return False


#======================================================================
#===
#=== TileASCIIParser
#===
#======================================================================

#
#______________________________________________________________________
class TileASCIIParser(TileCalibLogger):
    """
    This is a class capable of parsing TileCal conditions data stored in
    ASCII files. Both the single and multi-line formats are supported. 
    """

    #____________________________________________________________________
    def __init__(self, fileName, calibId, isSingleLineFormat=True):
        """
        Input:
        - fileName          : input file name
        - isSingleLineFormat: if False, multi line format is assumed
        """
        
        TileCalibLogger.__init__(self,"TileASCIIParser")
        self.__dataDict = {}
        try:
            lines = open(fileName,"r").readlines()
        except Exception, e:
            self.log().error( "TileCalibASCIIParser::ERROR: Problem opening input file:" )
            self.log().error( e )
            return 

        for line in lines:
            fields = line.strip().split()
            #=== ignore empty and comment lines
            if not len(fields)          : continue
            if fields[0].startswith("#"): continue 

            #=== read in fields
            type = fields[0]
            frag = fields[1]
            chan = fields[2]
            data = fields[3:] 
            if not isSingleLineFormat:
                raise Exception("Multiline format not implemented yet")

            #=== check for correct calibId
            if type!=calibId:
                raise Exception("%s is not calibId=%s" % (type,calibId))

            #=== decode fragment
            if not (frag.startswith('0x') or frag.startswith('-0x') or frag.startswith('h_')): #akamensh
                raise Exception("Misformated fragment %s" % frag)
            if frag.startswith('0x') or frag.startswith('-0x'):
                frg = int(frag,16)
                ros = frg>>8
                if frg<0: mod = (-frg)&255
                else: mod = frg&255
                chn = int(chan)
            elif frag.startswith('h_'):
                part_dict = {'LBA':1,'LBC':2,'EBA':3,'EBC':4}
                partname = str(frag[2:5])
                ros=part_dict[partname]
                mod = int(frag[5:])-1
                if (chan.startswith('ch')):
                    chn = int(chan[2:])
                else:
                    pmt = int (chan)
                    chn=self.PMT2channel(ros,mod,pmt)

            #=== fill dictionary
            dictKey = (ros,mod,chn)
            self.__dataDict[dictKey] = data

    #____________________________________________________________________
    def getData(self, ros, drawer, channel):
        dictKey = (int(ros), int(drawer), int(channel))
        data = self.__dataDict.get(dictKey,[])
        return data

    #____________________________________________________________________
    def getDict(self):
        import copy
        return copy.deepcopy(self.__dataDict)
    
    #____________________________________________________________________
    def PMT2channel(self,ros,drawer,pmt):
        "Reorder the PMTs (SV: how to get that from region.py???)"
        "This takes ros [1-4], drawer [0-63], pmt [1-48]"     

        PMT2chan_Special={1:0,2:1,3:2,4:3,5:4,6:5,7:6,8:7,9:8,10:9, 
                      11:10,12:11,13:12,14:13,15:14,16:15,17:16,18:17, 19:18, 20:19, 
                      21:20,22:21,23:22,24:23,27:24,26:25,25:26,31:27,32:28,28:29, 
                      33:30,29:31,30:32,36:33,35:34,34:35,44:36,38:37,37:38,43:39,42:40, 
                      41:41,45:42,39:43,40:44,48:45,47:46,46:47}
  

        PMT2chan_LB={1:0,2:1,3:2,4:3,5:4,6:5,7:6,8:7,9:8,10:9,
                     11:10,12:11,13:12,14:13,15:14,16:15,17:16,18:17,19:18,20:19,
                     21:20,22:21,23:22,24:23,27:24,26:25,25:26,30:27,29:28,28:29,
                     33:30,32:31,31:32,36:33,35:34,34:35,39:36,38:37,37:38,42:39,41:40,
                     40:41,45:42,44:43,43:44,48:45,47:46,46:47}
        
        
        PMT2chan_EB={1:0,2:1,3:2,4:3,5:4,6:5,7:6,8:7,9:8,10:9,
                     11:10,12:11,13:12,14:13,15:14,16:15,17:16,18:17,19:18,20:19,
                     21:20,22:21,23:22,24:23,25:24,26:25,27:26,28:27,31:28,32:29,
                     33:30,29:31,30:32,35:33,36:34,34:35,44:36,38:37,37:38,43:39,42:40,
                     41:41,39:42,40:43,45:44,46:45,47:46,48:47}

        if ros <= 2: 
            chan = PMT2chan_LB[pmt]
        elif (ros == 3 and drawer == 14) or (ros == 4 and drawer == 17):
            chan = PMT2chan_Special[pmt]
        else:
            chan = PMT2chan_EB[pmt]
    
        return chan    

#======================================================================
#===
#=== TileASCIIParser2
#===
#======================================================================

#
#______________________________________________________________________
class TileASCIIParser2(TileCalibLogger):
    """
    This is a class capable of parsing TileCal conditions data stored in
    ASCII files. Both the single and multi-line formats are supported. 
    """

    #____________________________________________________________________
    def __init__(self, fileName, calibId="", isSingleLineFormat=True):
        """
        Input:
        - fileName          : input file name
        - calibId           : like Ped, ...  but can be empty string as well
        - isSingleLineFormat: if False, multi line format is assumed
        """
        
        TileCalibLogger.__init__(self,"TileASCIIParser2")
        self.__dataDict = {}
        try:
            lines = open(fileName,"r").readlines()
        except Exception, e:
            self.log().error( "TileCalibASCIIParser2::ERROR: Problem opening input file:" )
            self.log().error( e )
            return 

        self.log().info("Parsing file %s",fileName)
        if len(calibId)>0:
            self.log().info("Looking for prefix %s",calibId)

        for line in lines:
            fields = line.strip().split()
            #=== ignore empty and comment lines
            if not len(fields)          : continue
            if fields[0].startswith("#"): continue 

            #=== read in fields
            if len(calibId)>0:
                type = fields[0]
                frag = fields[1]
                chan = fields[2]
                gain = fields[3]
                data = fields[4:] 

                #=== check for correct calibId
                if type!=calibId:
                    raise Exception("%s is not calibId=%s" % (type,calibId))
            else:
                frag = fields[0]
                chan = fields[1]
                gain  = fields[2]
                data = fields[3:] 

            if not isSingleLineFormat:
                raise Exception("Multiline format not implemented yet")

            #=== decode fragment 
            if frag.startswith('0x') or frag.startswith('-0x'):
                frg = int(frag,16)
                ros = frg>>8
                if frg<0: mod = (-frg)&255
                else: mod = frg&255
            elif (frag.startswith("AUX") or 
                  frag.startswith("LBA") or 
                  frag.startswith("LBC") or 
                  frag.startswith("EBA") or 
                  frag.startswith("EBC") or
                  frag.startswith("ALL") or
                  frag.startswith("XXX") ):
                part_dict = {'AUX':0,'LBA':1,'LBC':2,'EBA':3,'EBC':4,'ALL':5,'XXX':-1}
                partname = str(frag[0:3])
                ros=part_dict[partname]
                mod = int(frag[3:])-1
            else:
                raise Exception("Unknown fragment %s" % frag)

            chn = int(chan)
            adc = int(gain)
            
            #=== fill dictionary
            if ros<0:
               rosmin=0
               rosmax=5
            elif ros>=5:
               rosmin=1
               rosmax=5
            else:
               rosmin=ros
               rosmax=ros+1

            if mod<0 or mod>=64:
               modmin=0
               modmax=64
            else:
               modmin=mod
               modmax=mod+1

            allchannels=True
            if chn<-2:
               chnmin=0
               chnmax=-chn
            elif chn<0:
               chnmin=0
               chnmax=48
               allchannels=(chn==-1) # if chn=-2 only connected channels will be updated
            else:
               chnmin=chn
               chnmax=chn+1

            if adc<-1:
               adcmin=0
               adcmax=-adc
            elif adc<0:
               adcmin=0
               adcmax=2
            else:
               adcmin=adc
               adcmax=adc+1

            for ros in xrange(rosmin,rosmax):
               for mod in xrange(modmin,modmax):
                  for chn in xrange(chnmin,chnmax):
                     if allchannels or self.channel2PMT(ros,mod,chn)>0: 
                        for adc in xrange (adcmin,adcmax):
                           dictKey = (ros,mod,chn,adc)
                           self.__dataDict[dictKey] = data

    #____________________________________________________________________
    def getData(self, ros, drawer, channel, adc):
        dictKey = (int(ros), int(drawer), int(channel), int(adc))
        data = self.__dataDict.get(dictKey,[])
        return data

    #____________________________________________________________________
    def getDict(self):
        import copy
        return copy.deepcopy(self.__dataDict)
        
    #____________________________________________________________________
    def channel2PMT(self,ros,drawer,chan):
        "Convert channel numbet to PMT number, negative for disconnected channels"
        "This takes ros [1-4], drawer [0-63], chan [0-47]"     

        chan2PMT_LB=[  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
                      13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
                      27, 26, 25, 30, 29, 28,-33,-32, 31, 36, 35, 34,
                      39, 38, 37, 42, 41, 40, 45,-44, 43, 48, 47, 46 ]
        
        chan2PMT_EB=[  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
                      13, 14, 15, 16, 17, 18,-19,-20, 21, 22, 23, 24,
                     -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34,
                      44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46 ]

        chan2PMT_Sp=[ -1, -2, -3, -4,  5,  6,  7,  8,  9, 10, 11, 12, # first 4 do not exist
                      13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, # PMT 19 and 20 exist
                     -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34,
                      44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46 ]

        if ros <= 2: 
            pmt = chan2PMT_LB[chan]
        elif (ros == 3 and drawer == 14) or (ros == 4 and drawer == 17):
            pmt = chan2PMT_Sp[chan]
        else:
            pmt = chan2PMT_EB[chan]
    
        return pmt

#======================================================================
#===
#=== TileASCIIParser3
#===
#======================================================================

#______________________________________________________________________
class TileASCIIParser3(TileCalibLogger):
    """
    This is a class capable of parsing TileCal conditions data stored in
    ASCII files.
    """

    #____________________________________________________________________
    def __init__(self, fileName, calibId):
        """
        Input:
        - fileName          : input file name
        - calibId           : like Trip, ...
        """
        
        TileCalibLogger.__init__(self,"TileASCIIParser3")
        self.__dataDict = {}
        try:
            lines = open(fileName,"r").readlines()
        except Exception, e:
            self.log().error( "TileCalibASCIIParser3::ERROR: Problem opening input file:" )
            self.log().error( e )
            return 

        for line in lines:
            fields = line.strip().split()
            #=== ignore empty and comment lines
            if not len(fields)          : continue
            if fields[0].startswith("#"): continue 

            #=== read in fields
            type = fields[0]
            frag = fields[1]
            data = fields[2:] 

            #=== check for correct calibId
            if type != calibId:
                raise Exception("%s is not calibId=%s" % (type, calibId))

            #=== decode fragment 
            if not (frag.startswith('0x') or frag.startswith('-0x')):
                raise Exception("Misformated fragment %s" % frag)

            frg = int(frag,16)
            ros = frg>>8
            if frg<0: mod = (-frg)&255
            else: mod = frg&255
            
            #=== fill dictionary
            dictKey = (ros, mod)
            self.__dataDict[dictKey] = data

    #____________________________________________________________________
    def getData(self, ros, drawer):
        dictKey = (int(ros), int(drawer))
        data = self.__dataDict.get(dictKey,[])
        return data

    #____________________________________________________________________
    def getDict(self):
        import copy
        return copy.deepcopy(self.__dataDict)




        
