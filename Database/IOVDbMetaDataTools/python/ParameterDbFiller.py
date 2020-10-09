# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @author: RD Schaffer <R.D.Schaffer@cern.ch>
# @date:   May 2008
#
#

from __future__ import print_function

__version__ = "$Id: ParameterDbFiller.py,v 1.3 2008-11-13 12:25:23 schaffer Exp $"
__author__  = "RD Schaffer <R.D.Schaffer@cern.ch>"

import sys
import os
import collections
import six
from PyCool import cool

class ParameterDbFillerError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)


class ParameterDbFiller(object):
    """
    A simple class used to fill SQLite db file with parameters for simulation
    and/or digitization.
    """
    
    def __init__(self):
        object.__init__(self)

        self.beginRun      = None
        self.endRun        = None
        self.simParams     = collections.OrderedDict()
        self.digitParams   = collections.OrderedDict()
        self.digitParams64 = collections.OrderedDict()
        return

    # 
    def setBeginRun(self, beginRun):
        self.beginRun    = beginRun << 32
        return

    def setEndRun(self, endRun):
        self.endRun      = endRun << 32
        return

    def addSimParam(self, name, value):
        self.simParams[name] = value
        return

    def addDigitParam(self, name, value):
        self.digitParams[name] = value
        return

    def addDigitParam64(self, name, value):
        self.digitParams64[name] = value
        return

    def genSimDb(self, dbFileName = None):
        # Allow to define specific file name, otherwise use 
        if dbFileName is None:
            # Set to default value
            dbFileName = "SimParams.db"

        # Generate db: args -  file name, dbname, params, folder path
        self.genDb(dbFileName, 'SIMPARAM', self.simParams, '/Simulation/Parameters')


    def genDigitDb(self, dbFileName = None):
        # Allow to define specific file name, otherwise use 
        if dbFileName is None:
            # Set to default value
            dbFileName = "DigitParams.db"

        # Generate db: args -  file name, dbname, params, folder path
        self.genDb(dbFileName, 'DIGPARAM', self.digitParams, '/Digitization/Parameters', self.digitParams64)


    def genDb(self, dbFileName, dbName, params, folderPath, params64 = {}):

        # Do checks
        if self.beginRun is None:
            raise ParameterDbFillerError ('Must set begin run number before generating db')
        if self.endRun is None:
            raise ParameterDbFillerError ('Must set end run number before generating db')
        if len(params) == 0:
            raise ParameterDbFillerError ('No parameters for db ' + dbName)

        # remove existing db, if any
        try:
            os.remove(dbFileName)
            print ("ParameterDbFiller.genDb:  Removed db", dbFileName)
        except Exception:
            pass

        # get database service and open database
        dbSvc=cool.DatabaseSvcFactory.databaseService()

        # build dbstring - database accessed via physical name
        dbstring="sqlite://;schema=" + dbFileName + ";dbname=" + dbName
        try:
            db=dbSvc.createDatabase(dbstring)
        except Exception as e:
            print ('ParameterDbFiller.genDb:  Problem creating database',e)
            sys.exit(-1)
            print ("ParameterDbFiller.genDb:  Created database",dbstring)
        
        # setup a folder payload specification
        spec=cool.RecordSpecification()
        for key in params:
            spec.extend(key, cool.StorageType.String4k)
            pass
        # add in spec for long strings
        for key in params64:
            spec.extend(key, cool.StorageType.String64k)

        # folder meta-data - note for Athena this has a special meaning
        desc = '<timeStamp>run-event</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'
        # create the folder - single version
        # last argument is createParents - if true, automatically creates parent
        # folders if needed
        # note this will not work if the database already exists - delete mycool.db first
        # myfolder=db.createFolder(folderPath, spec, desc, cool.FolderVersioning.SINGLE_VERSION,True)
        folderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, spec)
        myfolder   = db.createFolder(folderPath, folderSpec, desc, True)

        # now fill in parameters
        data = cool.Record(spec)
        for k, v in six.iteritems(params):
            data[k] = v
        for k, v in six.iteritems(params64):
            data[k] = v

        print ("ParameterDbFiller.genDb:  Recording parameters", data)

        # store object with IOV valid from 0-10, channel 3
        myfolder.storeObject(self.beginRun, self.endRun, data, 0)
        print ("ParameterDbFiller.genDb:  Stored object")

        # finalize
        db.closeDatabase()

        # Try to dump out db as test
        self.dumpDb(dbstring)

    def dumpDb(self, dbstring):

        print ("ParameterDbFiller.dumpDb:  Dumping database:", dbstring)
        # get database service and open database
        dbSvc = cool.DatabaseSvcFactory.databaseService()
        # database accessed via physical name
        try:
            db = dbSvc.openDatabase(dbstring,False)
        except Exception as e:
            print ('ParameterDbFiller.dumpDb:  Problem opening database',e)
            sys.exit(-1)

        # Loop over folders
        folders = db.listAllNodes()
        for ff in folders:
            # Get Folder
            if (not db.existsFolder(ff)):
                continue
            try:
                f = db.getFolder(ff)
                print ("ParameterDbFiller.dumpDb:  Dumping folder " + str(ff))
            except Exception:
                #print ("Skipping " + str(ff))
                continue

        # get tags
        # tags  = f.listTags()
        # print ("for tags ", end='')
        # for tag in tags: print (tag)
    
        # for tag in tags:

        nobjs = f.countObjects( cool.ValidityKeyMin,
                                cool.ValidityKeyMax,
                                cool.ChannelSelection.all())

        print ("ParameterDbFiller.dumpDb:  number of IOV payloads", nobjs)

        objs = f.browseObjects( cool.ValidityKeyMin,
                                cool.ValidityKeyMax,
                                cool.ChannelSelection.all())
        i = 0
        while objs.goToNext():
            obj = objs.currentRef()
            print ("ParameterDbFiller.dumpDb:  Payload", i, end='')
            print ("since [r,l]: [", obj.since() >> 32,',',obj.since()%0x100000000,']', end='')
            print ("until [r,l]: [", obj.until() >> 32,',',obj.until()%0x100000000,']', end='')
            print ("payload", obj.payload(), end='')
            print ("chan",obj.channelId() )
            i += 1

        objs.close()

        # finish
        db.closeDatabase()
