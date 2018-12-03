#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# CaloCondTools.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-23
# Carlos Solans <carlos.solans@cern.ch>, 2012-11-07
"""
Python helper module for managing COOL DB connections and CaloCondBlobs. 
"""

import cppyy
from PyCool import cool
import time, types, re
g = cppyy.gbl


#=== get a logger
from CaloCondBlobAlgs.CaloCondLogger import CaloCondLogger, getLogger
log = getLogger("CaloCondTools")

#=== useful constants
MINRUN = 0
MINLBK = 0
MAXRUN = cool.Int32Max
MAXLBK = cool.UInt32Max 

#
#______________________________________________________________________
def getCaloPrefix():
    """
    Returns the common Calo prefix used for all COOL folders
    """
    return "/CALO/"

#
#______________________________________________________________________
def getAthenaFolderDescr():
    """
    Returns the run-lumi type folder description needed to read back the folder content
    as a CondAttrListCollection in Athena.
    """
    desc ='<timeStamp>run-lumi</timeStamp>'
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
def openDb(db, schema, mode="READONLY", schema2="COOLONL_CALO", sqlfn="caloSqlite.db"):
    """
    Opens a COOL db connection.
    - connStr: The DB connection string. The following
               abbreviations are recognized:
               * SQLITE: Opens CaloSqlite.db file in current directory
               * ORACLE: Opens ORACLE DB, forces READONLY
    - mode: Can be READONLY (default), RECREATE or UPDATE
    """
    #=== check for valid db names
    validDb = ["SQLITE","ORACLE"]
    if db not in validDb:
        log.error( "DB not valid: %s" % db )
        log.error( "Valid DB are: %s" % validDb )
        raise("Invalid DB")
    elif db == "ORACLE":
        mode = "READONLY" # If using ORACLE, only read
        
    #=== check for valid instance names
    # USE OF KEYWORD "schema" is retained for backward compatability
    #=== see https://twiki.cern.ch/twiki/bin/view/Atlas/CoolProdAcc
    validInstance = ["COMP200", "CMCP200", "OFLP200","CONDBR2"]
    if schema not in validInstance:
        log.error( "DB schema not valid: %s" % schema )
        log.error( "Valid schema are: %s" % validInstance )
        raise("Invalid schema")
        
    #=== check for valid schema names
    #=== see https://twiki.cern.ch/twiki/bin/view/Atlas/CoolProdAcc
    validSchema = ["COOLONL_CALO","COOLOFL_CALO"]
    if schema2 not in validSchema:
        log.error( "DB schema2 not valid: %s" % schema2 )
        log.error( "Valid schema2 are: %s" % validSchema )
        raise("Invalid schema2")
        
    #=== construct connection string
    connStr = ""
    if db=='SQLITE':
        if mode=="READONLY" and not os.path.exists(sqlfn):
            raise Exception( "Sqlite file %s does not exist" % (sqlfn) )
        if (mode=="RECREATE" or mode=="UPDATE") and not os.path.exists(os.path.dirname(sqlfn)):
            dirn=os.path.dirname(sqlfn)
            if dirn: os.makedirs(dirn)
        connStr="sqlite://X;schema=%s;dbname=%s" % (sqlfn,schema)
#        connStr="sqlite://;schema=caloSqlite.db;dbname=%s" % schema
    elif db=='ORACLE':
        connStr='oracle://%s;schema=ATLAS_%s;dbname=%s' % ('ATLAS_COOLPROD',schema2,schema)
#        connStr=schema2+'/'+schema

    return openDbConn(connStr, mode)

#
#______________________________________________________________________
def openDbConn(connStr, mode="READONLY"):
    """
    Opens a COOL db connection.
    - connStr: The DB connection string. The following
               abbreviations are recognized:
               * SQLITE: Opens caloSqlite.db file in current directory (OFLP200)
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
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    log.info( "---------------------------------------------------------------------------------" )
    log.info( "-------------------------- CaloCondTools.openDbConn -----------------------------" )
    log.info( "- using COOL version %s" % dbSvc.serviceVersion()                                  )
    log.info( "- opening CaloDb: %s" %connStr_new                                                 )
    log.info( "- mode: %s" %mode                                                                  )
    log.info( "---------------------------------------------------------------------------------" )

    #=== action depends on mode
    if mode=="READONLY":
        #=== open read only
        try:
            db=dbSvc.openDatabase(connStr_new,True)
        except Exception, e:
            log.debug( e )
            log.critical("Could not connect to %s" % connStr_new )
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
            log.warning( "Could not connect to \'%s\', trying to create it...." % connStr_new )
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
def iovFromRunLumi(runNum, lbkNum):
    """
    Returns COOL timeStamp build from run and lumi block numbers
    """
    return (int(runNum)<<32) + int(lbkNum)

#
#______________________________________________________________________
def decodeTimeString(timeString):
    """
    Returns UNIX time stamp given an input time string
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
        validityKey = iovFromRunLumi(pointInTime[0],pointInTime[1])
    #=== catch other types
    else:
        raise Exception("Unrecognized pointInTime type=%s" % type(pointInTime))
    return cool.ValidityKey(validityKey)

#
#______________________________________________________________________
def getCellHash(detectorId,part,module,sample,tower):
    """
    Returns cell subHash given partition,module,sample, and tower (TILE only)
    """
    if detectorId != 48:
        raise Exception('getCellHash only available for TileCells.')
    section = 0 # 1=LB* 2=EB* 3=ITC
    side = 0    # 1=A-side -1=C-side
    if part == 1 or part == 2:
        section = 1
    elif part == 3 or part == 4:
        if sample == 3 or (sample==1 and tower==9) or (sample==2 and tower==8): # Gap and ITC
            section = 3
        else:
            section = 2
    if part%2 ==0:
        side = -1
    else:
        side = 1
    if section==0 or side==0 or module>63 or sample>3 or tower>15:
        raise Exception('Non-physical cell specification')
    
    # hash array for mod0 of entire TileCal.  Use to determine cell hash for any cell
    hash = [0, 1, -1, -1, 2, 3, -1, -1, 4, 5, 6, -1, 7, 8, -1, -1, 9, 10, 11, -1, 12, 13, -1, -1, 14, 15, 16, -1, 17, 18, -1, -1, 19, 20, 4416, -1, 21, 4417, -1, -1, -1, 2880, 2881, 4418, 2882, 2883, -1, 4419, 2884, 2885, 2886, -1, 2887, 2888, -1, 4420, 2889, 2890, -1, -1, 2891, -1, -1, 4421, 1408, 1409, 1410, -1, 1411, 1412, -1, -1, 1413, 1414, 1415, -1, 1416, 1417, -1, -1, 1418, 1419, 1420, -1, 1421, 1422, -1, -1, 1423, 1424, 1425, -1, 1426, 1427, -1, -1, 1428, 1429, 4800, -1, 1430, 4801, -1, -1, -1, 3648, 3649, 4802, 3650, 3651, -1, 4803, 3652, 3653, 3654, -1, 3655, 3656, -1, 4804, 3657, 3658, -1, -1, 3659, -1, -1, 4805]

    # Section specific offset to be added to cell hash for module different from 0
    # [LBC,LBA,EBC,EBA,ITC-C,ITC-A]
    modOffset = [22,23,12,12,6,6]
    if side==1: sideOffset=1
    else:       sideOffset=0

    return hash[sideOffset*16*4+tower*4+sample]+modOffset[sideOffset+2*(section-1)]*module

#======================================================================
#===
#=== CaloBlobReader 
#===
#======================================================================

#
#______________________________________________________________________
class CaloBlobReader(CaloCondLogger):
    """
    CaloBlobReader is a helper class, managing the details of COOL interactions
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
        CaloCondLogger.__init__(self,"CaloBlobReader")

        #=== try to open db
        try:
            self.__db  = db
            self.__folder = self.__db.getFolder(folder)
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

        #=== initialize dictionary to keep reference to DB object
        #=== and timestamp, so they do not go out of scope
        self.__objDict = {}

    #____________________________________________________________________
    def getCells(self, systemId, pointInTime):
        """
        Returns a CaloCondBlob object for the given system.
        """

        validityKey = getCoolValidityKey(pointInTime)
        self.log().debug("Validity key is %s" % validityKey)
        try:
            calibDrawer = None
            #=== Have we retrieved data previously?
            key = (systemId,validityKey)
            obj = self.__objDict.get(key)
            #=== ... if not, get it from DB
            if not obj:
                channelId = cool.ChannelId(systemId)
                obj = self.__folder.findObject(validityKey, channelId, self.__tag)
                self.log().debug("Fetching from DB: %s" % obj)
                blob = obj.payload()[0]
                self.log().debug("blob size: %d" % blob.size())
                #=== store object in dictionary
                self.__objDict[key] = obj
            #=== get blob
            blob = obj.payload()[0]
            self.log().debug("blob size: %d" % blob.size())

            #=== create CaloCondBlob object
            flt = g.CaloCondBlobFlt.getInstance(blob)
            return flt
        except Exception, e:
            self.log().error("Fetching of systemId=%i failed with exception %s"%(systemId,e))
            return None


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
#=== CaloBlobWriter 
#===
#======================================================================

#
#______________________________________________________________________
class CaloBlobWriter(CaloCondLogger):
    """
    Helper class that enables writing to Calo DB
    """

    #____________________________________________________________________
    def __init__(self, db, folderPath, caloBlobType=None,
            isMultiVersionFolder=True, isRunLumiTimeStamp=True):
        """
        Input:
        - db        : db should be an open database connection 
        - folderPath: full folder path to create or update
        """

        #=== initialize base class
        CaloCondLogger.__init__(self,"CaloBlobWriter")

        #=== store db
        self.__db  = db

        #=== determine folder mode
        folderMode = cool.FolderVersioning.MULTI_VERSION
        if not isMultiVersionFolder:
            folderMode = cool.FolderVersioning.SINGLE_VERSION

        #=== determine folder description
        folderDescr = getAthenaFolderDescr()

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
                payloadSpec.extend( 'CaloCondBlob16M', cool.StorageType.Blob16M )
                folderSpec = cool.FolderSpecification(folderMode, payloadSpec)
                self.__folder = db.createFolder(folderPath, folderSpec, folderDescr, True)
        except Exception, e:
            self.log().critical( e )
            raise

        #=== initialize channel dictionaries
        self.__chanDictRecord  = {} # <int, cool.Record    >
        self.__chanDictCells   = {} # <int, CaloCondBlobFlt>

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
        self.log().info( "Registering folder %s with tag \"%s\"" % (self.__folder.fullPath(),folderTag))
        self.log().info( "... with IOV          : %s"            % iovString                          )
        #self.log().info( "... with comment field: \"%s\""        % self.__chanDictDrawer[48].getComment()                  )

        #=== register all channels by increasing channel number
        chanList = sorted(self.__chanDictRecord.keys())
        for chanNum in chanList:
            data = self.__chanDictRecord[chanNum]
            strout = "cool channel=%4i" % chanNum
            self.log().debug("Registering %s %s" % (strout, data))
            channelId = cool.ChannelId(chanNum)
            self.__folder.storeObject(sinceCool, untilCool, data, channelId, folderTag, userTagOnly)

    #____________________________________________________________________
    def getCells(self, systemId):
        """
        Returns a CaloCondBlob object of given system Id.
        """

        #try:
        chanNum = cool.ChannelId(systemId)
        flt = self.__chanDictCells.get(chanNum,None)
        #=== ... if not, get it from DB
        if not flt:
            #=== create new blob
            spec = self.__folder.payloadSpecification()
            data = cool.Record( spec )
            self.__chanDictRecord[chanNum] = data
            for key in data:
                blob = data[key]
            flt = g.CaloCondBlobFlt.getInstance(blob)

        self.__chanDictCells[chanNum] = flt    
        return flt
        
        #except Exception, e:
        #    self.log().critical( e )
        #    return None

    #____________________________________________________________________
    #def setComment(self, author, comment):
    #    """
    #    Sets a general comment in the comment channel.
    #    """
    #    try:
    #        chanNum = TileCalibUtils.getCommentChannel()
    #        data = self.__chanDictRecord.get(chanNum)
    #        if not data:
    #            spec = self.__folder.payloadSpecification()
    #            data = cool.Record( spec )
    #        self.__chanDictRecord[chanNum] = data
    #        blob = data['TileCalibBlob']
    #        cmt = TileCalibDrawerCmt.getInstance(blob,author,comment)
    #    except Exception, e:
    #        self.log().critical( e )

            

    #____________________________________________________________________
    def zeroBlob(self, systemId):
        """
        Resets blob size to zero
        """
        try:
            calibDrawer = None
            data = self.__chanDictRecord.get(systemId)
            if not data:
                spec = self.__folder.payloadSpecification()
                data = cool.Record( spec )
                self.__chanDictRecord[chanNum] = data
            blob = data['CaloCondBlob16M']
            blob.resize(0)
        except Exception, e:
            self.log().critical( e )
            return None

