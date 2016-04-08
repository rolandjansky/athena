# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @author: G.Unal
# @date:   July 2008
#
#

__version__ = "$Id: LArHVMapDbFiller.py"
__author__  = "G.Unal"

import sys, os, string
from PyCool import cool,coral

class LArHVMapDbFillerError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)


class LArHVMapDbFiller(object):
    """
    A simple class used to fill SQLite db file with parameters for simulation
    and/or digitization.
    """
    
    def __init__(self):
        object.__init__(self)

        self.beginTime   = None
        self.endTime     = None
        self.FileName    = None
        self.FolderTag   = None
        self.Params   = {}
        return

    # 
    def setBegin(self,run,lumi):
        print " setBegin  run,lumi ",run
        self.beginTime    = (long(run) << 32) + long(lumi)
        print "    time ",self.beginTime
        return

    def setEnd(self,run,lumi): 
        print "setEnd runmlumi ",run
        self.endTime      = (long(run) << 32) + long(lumi)
        print "   time ",self.endTime
        return

    def setFileName(self, fileName):
        self.FileName = fileName
        return

    def setFolderTag(self,folderTag):
        self.FolderTag = folderTag
        return

    def readFile(self):
        value = ''
        f = file(self.FileName,'r')
        for lines in f:
            value += lines
        self.Params['LArHVMap'] = value
        return

    def genDb(self):

        dbFileName = 'HVmap.db'
        dbName = 'COMP200'
        folderPath = '/LAR/IdentifierOfl/HVLineToElectrodeMap'
        # Do checks
        if self.beginTime == None:
            raise LArHVMapDbFillerError, 'Must set begin run number before generating db'
        if self.endTime == None:
            self.endTime = cool.ValidityKeyMax
        if self.FileName == None:
            raise LArHVMapDbFillerError, 'Must give an input file for LArHVToElectrode.data'
        if self.FolderTag == None:
            raise LArHVMapDbFillerError, 'Must give a folder tag'

        self.readFile()

        if len(self.Params) == 0:
            raise LArHVMapDbFillerError, 'No parameters for db ' + dbName

        # remove existing db, if any
        try:
            os.remove(dbFileName)
            print "LArHVMapDbFiller.genDb:  Removed db", dbFileName
        except:
            pass

        # get database service and open database
        dbSvc=cool.DatabaseSvcFactory.databaseService()

        # build dbstring - database accessed via physical name
        dbstring="sqlite://;schema=" + dbFileName + ";dbname=" + dbName
        try:
            db=dbSvc.createDatabase(dbstring)
        except Exception,e:
            print 'LArHVMapDbFiller.genDb:  Problem creating database',e
            sys.exit(-1)
            print "LArHVMapDbFiller.genDb:  Created database",dbstring
        
        # setup a folder payload specification
        spec=cool.RecordSpecification()
        for key in self.Params:
            spec.extend(key, cool.StorageType.String16M)

        # folder meta-data - note for Athena this has a special meaning
        desc='<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'
        # create the folder - single version
        # last argument is createParents - if true, automatically creates parent
        # folders if needed
        # note this will not work if the database already exists - delete mycool.db first
        myfolder=db.createFolder(folderPath, spec, desc, cool.FolderVersioning.MULTI_VERSION,True)
        # now fill in simlation parameters
        data=cool.Record(spec)
        for k, v in self.Params.iteritems():
            data[k] = v

        print "LArHVMapDbFiller.genDb:  Recording parameters", data

        # store object with IOV valid from 0-10, channel 3
        myfolder.storeObject(self.beginTime, self.endTime, data, 0, self.FolderTag)
        print "LArHVMapDbFiller.genDb:  Stored object"

        # finalize
        db.closeDatabase()

        # Try to dump out db as test
        self.dumpDb(dbstring)

    def dumpDb(self, dbstring):

        print "LArHVMapDbFiller.dumpDb:  Dumping database:", dbstring
        # get database service and open database
        dbSvc = cool.DatabaseSvcFactory.databaseService()
        # database accessed via physical name
        try:
            db = dbSvc.openDatabase(dbstring,False)
        except Exception,e:
            print 'LArHVMapDbFiller.dumpDb:  Problem opening database',e
            sys.exit(-1)

        # Loop over folders
        folders = db.listAllNodes()
        for ff in folders:
            # Get Folder
            try:
                f = db.getFolder(ff)
                print "LArHVMapDbFiller.dumpDb:  Dumping folder " + str(ff)
            except:
                #print "Skipping " + str(ff)
                continue

        # get tags
        # tags  = f.listTags()
        # print "for tags ",
        # for tag in tags: print tag
    
        # for tag in tags:

            nobjs = f.countObjects( cool.ValidityKeyMin,
                                    cool.ValidityKeyMax,
                                    cool.ChannelSelection.all())

            print "LArHVMapDbFiller.dumpDb:  number of IOV payloads", nobjs

            objs = f.browseObjects( cool.ValidityKeyMin,
                                    cool.ValidityKeyMax,
                                    cool.ChannelSelection.all())
            i = 0
            while objs.goToNext():
                obj = objs.currentRef()
                print "LArHVMapDbFiller.dumpDb:  Payload", i,
                print "since [r,l]: [", obj.since() ,']',
                print "until [r,l]: [", obj.until() ,']',
                print "payload", obj.payload(),
                print "chan",obj.channelId() 
                i += 1

            objs.close()

        # finish
        db.closeDatabase()
