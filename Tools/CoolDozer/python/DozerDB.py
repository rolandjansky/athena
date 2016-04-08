#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DozerDB.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date May 19, 2008 
# @brief implementation of DozerDB class
# Sadly, artificial intelligence will probably never be a match for natural stupidity.

import sys
sys.argv.append("-b")

import DozerObject
from DozerUtils import DozerLogger, DozerError, DozerSingleton, DozerVersion
from DozerValid import DozerValid
from DozerConfig import DozerConfig
from DozerSchema import DozerSchema
from DozerSchemaConfig import DozerSchemaConfig
from DozerChannel import DozerChannel
from DozerData import DozerData 
from DozerIOV import DozerIOV
from DozerGraph import DozerGraph, DozerStyle
from DozerRecordSet import DozerRecordSet

from PyCool import cool, coral
import ROOT

from types import *
import xml.dom.minidom
import os, os.path
import unittest

##
# @class DozerRTConfig
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief bag class to hold runtime configuration for CoolDozer class  
class DozerRTConfig( DozerObject.DozerObject ):
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param package valid package name 
    # @param testFramework valid test farmework name
    # @param nightly valid nightly name
    # @param install valid install name
    # @param cmtconfig valid cmtconfig name
    # @param project valid project name
    # @param jobId job id
    # @param log DozerLogger instance
    def __init__(self, 
                 package, 
                 testFramework,
                 nightly, 
                 install,
                 cmtconfig,
                 project,
                 jobId, log=None):
        
        super( DozerRTConfig, self).__init__( log )
        self.__log = log
        
        self.cv = DozerValid( log=self.__log )

        
        self.debug("contructing RuntimeConfig %s/%s/%s/%s/%s/%s" % ( package,
                                                                     testFramework,
                                                                     nightly,
                                                                     install,
                                                                     cmtconfig,
                                                                     project ) )
        self.__package = package

        if ( testFramework in self.cv.frameworks() and "*" not in testFramework):
            self.__testFramework = testFramework
        else:
            raise DozerError("wrong runtime configuration, testFramework cannot be set to %s" % str(testFramework) )

        if ( nightly in self.cv.nightlies() and  "*" not in nightly ):
            self.__nightly = str(nightly) 
        else:
            raise DozerError("wrong runtime configuration, nightly cannot be set to %s" % str(nighlty) )
        
        if ( install in self.cv.installs() and "*" not in install ):
            self.__install = install
        else:
            raise DozerError("wrong runtime configuration, install cannot be set to %s" % str(install) )
        
        if ( cmtconfig in self.cv.cmtconfigs() and "*" not in cmtconfig ):
            self.__cmtconfig = cmtconfig
        else:
            raise DozerError("wrong runtime configuration, cmtconfig cannot be set to %s" % str(cmtconfig) )

        if ( project in self.cv.projects() and "*" not in project ):
            self.__project = project
        else:
            raise DozerError("wrong runtime configuration, project cannot be set to %s" % str(project) )
        

        if ( "*" not in jobId ):
            self.__jobId = jobId
        else:
            raise DozerError("wrong runtime configuration, jobId cannot be set to %s" % str(project) )

    ## package name getter
    # @param self "Me, myself and Irene"
    def package( self ):
        return self.__package

    ## test framework name getter
    # @param self "Me, myself and Irene"
    def testFramework( self ):
        return self.__testFramework

    ## nightly name getter
    # @param self "Me, myself and Irene"
    def nightly( self ):
        return self.__nightly
    
    ## install name getter
    # @param self "Me, myself and Irene"
    def install( self ):
        return self.__install

    ## cmtconfig name getter
    # @param self "Me, myself and Irene"
    def cmtconfig( self ):
        return self.__cmtconfig

    ## project name getter
    # @param self "Me, myself and Irene"
    def project( self ):
        return self.__project

    ## job id getter
    # @param self "Me, myself and Irene"  
    def jobId( self ):
        return self.__jobId

    ## str() operator
    # @param self "Me, myself and Irene"
    def __str__( self ):
        return "DozerRTConfig package='%s' testFramework='%s' "\
            "nigthtly='%s' install='%s' cmtconfig='%s' project='%s' jobId='%s'" % ( self.__package, 
                                                                                    self.__testFramework,
                                                                                    self.__nightly,
                                                                                    self.__install,
                                                                                    self.__cmtconfig,
                                                                                    self.__project,
                                                                                    self.__jobId )
                                                                         

## 
# @class DozerDB
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief interface to the cool database
# 
# <h3>Usage</h3>
#
# DozerDB is the only class that links CoolDozer API with COOL database instance using PyCool API.
#
# <div style="color: #660000; background-color: #ff9999;">
# <div style="position: relative; top: 0pt; left: 0pt; font-size: 20pt; width: 80pt; padding: 2pt 5pt; 
# color: #ff9999; background-color: #660000; float: left;"> <b>Beware!</b> </div> 
# <div style="font-family: Courier; position: relative; top: 0pt; left: 10pt; 
# padding-left: 100pt; padding-right: 10pt; padding-top: 10pt; padding-bottom: 10pt;">
# DozerDB class is a singleton, it cannot be instated twice. 
# If you want to switch your application between
# different test frameworks you have to destroy current DozerDB instance and create a new one. 
# </div></div>
# 
# <ul>
# <li> Constructing
#
# <div style="color: #333333; background-color: #cccccc;">
# <div style="position: relative; top: 0pt; left: 0pt; font-size: 20pt; width: 80pt; padding: 2pt 5pt; 
# color: #cccccc; background-color: #333333; float: left;"> <b>Example</b> </div> 
# <div style="font-family: Courier; position: relative; top: 0pt; left: 10pt; 
# padding-left: 100pt; padding-right: 10pt; padding-top: 10pt; padding-bottom: 10pt;"> 
# <b><pre> 
#[lxplus203] ~/scratch2/CoolDozer/python % python 
#Python 2.5 (r25:51908, Oct 18 2007, 16:04:48) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-8)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> import DozerDB
#>>> dozer = DozerDB.DozerDB("Dummy", "RTT")      # [1] invoking constructor for DozerDB instance
#>>> dozer.package()                              # [2] checking package name 
#'Dummy'
#>>> dozer.framework()                            # [3] checking test framework name
#'RTT'
#</pre></b>
#</div></div>
#
# <li> Connections to the dbs
#
# Invoking DozerDB constructor opens the db instance for test framework. 
#
# <div style="color: #333333; background-color: #cccccc;">
# <div style="position: relative; top: 0pt; left: 0pt; font-size: 20pt; width: 80pt; padding: 2pt 5pt; 
# color: #cccccc; background-color: #333333; float: left;"> <b>Example</b> </div> 
# <div style="font-family: Courier; position: relative; top: 0pt; left: 10pt; 
# padding-left: 100pt; padding-right: 10pt; padding-top: 10pt; padding-bottom: 10pt;"> 
# <b><pre> 
#>>> dozer.isOpened()                              # [1.1] checking if database has been opened
#True
#>>> dozer.dbId()                                  # [1.2] getting database id
#'sqlite://;schema=/afs/cern.ch/atlas/project/RTT/Results/dozer/db/RTT.db;dbname=RTT'
#>>> dozer.closeDB()                               # [2.1] closing database
#True
#>>> dozer.dbId()                                  # [2.2] gatting database id
#''
#>>> dozer.isOpened()                              # [2.3] checking connection
#False
#>>> dozer.openDB()                                # [3.1] database reopenning 
#True 
#>>> dozer.isOpened()                              # [3.2] checking connection                   
#True
#>>> dozer.dbId()                                  # [3.3] getting database id
#'sqlite://;schema=/afs/cern.ch/atlas/project/RTT/Results/dozer/db/RTT.db;dbname=RTT'
#</pre></b>
#</div></div>
#
# <div style="color: #660000; background-color: #ff9999;">
# <div style="position: relative; top: 0pt; left: 0pt; font-size: 20pt; width: 80pt; padding: 2pt 5pt; 
# color: #ff9999; background-color: #660000; float: left;"> <b>Beware!</b> </div> 
# <div style="font-family: Courier; position: relative; top: 0pt; left: 10pt; 
# padding-left: 100pt; padding-right: 10pt; padding-top: 10pt; padding-bottom: 10pt;"> 
# Be careful when using <i>SQLite</i> database backend! <i>SQLite</i> allows to make mutliple connections to the database 
# instance for reading only! Only one connection could write to database at a time! For <i>Oracle</i> and <i>my_sql</i> it is not a problem...
# </div> </div>
# 
#
# <li> Adding DozerSchema to database
#
# <div style="color: #000033; background-color: #ccccff;">
# <div style="position: relative; top: 0pt; left: 0pt; font-size: 20pt; width: 80pt; padding: 2pt 5pt;
# color: #ccccff; background-color: #000033; float: left;"> <b>Notice.</b> </div> 
# <div style="font-family: Courier; position: relative; top: 0pt; left: 10pt; 
# padding-left: 100pt; padding-right: 10pt; padding-top: 10pt; padding-bottom: 10pt;"> 
# To add DozerSchema instance to the database use '+=' operator:
#<b><pre>
# (DozerDB instance) += (DozerSchema instance)
#</pre></b> 
# </div> </div>
#
# <div style="color: #660000; background-color: #ff9999;">
# <div style="position: relative; top: 0pt; left: 0pt; font-size: 20pt; width: 80pt; padding: 2pt 5pt; 
# color: #ff9999; background-color: #660000; float: left;"> <b>Beware!</b> </div> 
# <div style="font-family: Courier; position: relative; top: 0pt; left: 10pt; 
# padding-left: 100pt; padding-right: 10pt; padding-top: 10pt; padding-bottom: 10pt;"> 
# Implicit DozerSchemas - those which contain '*' in nightly, install, cmtconfig or project name - 
# cannot be added to the database. '+=' operator accepts only <i>explicit</i> DozerSchemas. 
# </div> </div>
#
# Once you add DozerSchema to DozerDB instance it creates internal structure of cool.IFolderSets, 
# cool.IFolders and database channels as shown on picture below 
#
# @image html DozerDB_database_structure.png "The CoolDozer database internal structure" 
#
# <div style="color: #660000; background-color: #ff9999;">
# <div style="position: relative; top: 0pt; left: 0pt; font-size: 20pt; width: 80pt; padding: 2pt 5pt; 
# color: #ff9999; background-color: #660000; float: left;"> <b>Beware!</b> </div> 
# <div style="font-family: Courier; position: relative; top: 0pt; left: 10pt; 
# padding-left: 100pt; padding-right: 10pt; padding-top: 10pt; padding-bottom: 10pt;"> 
# Channel of id=0 is a special one - it holds only empty DozerSchema. Never and ever overwrite it contents by some external software! 
# </div> </div>
#
# <li> Schema evolution
#
#  <div style="color: #666600; background-color: #ffff99;">
# <div style="position: relative; top: 0pt; left: 0pt; font-size: 20pt; width: 80pt; padding: 2pt 5pt; 
# color: #ffff99; background-color: #666600; float: left;"> 
# <b>Trick?</b> </div> 
# <div style="font-family: Courier; position: relative; top: 0pt; left: 10pt; 
# padding-left: 100pt; padding-right: 10pt; padding-top: 10pt; padding-bottom: 10pt;"> 
# Schema evolution can be obtained by adding to DozerDB another DozerSchema for the same configuration - same nightly, same install,
# same cmtconfig, same project, but different DozerChannel list. This operation recreates the internal structure of database.
# New channels are added, obsolete channels are removed. 
# </div> </div>
#
# <li> cd, ls, pwd - listing database folders and channels
#
# <div style="color: #333333; background-color: #cccccc;">
# <div style="position: relative; top: 0pt; left: 0pt; font-size: 20pt; width: 80pt; padding: 2pt 5pt; 
# color: #cccccc; background-color: #333333; float: left;"> <b>Example</b> </div> 
# <div style="font-family: Courier; position: relative; top: 0pt; left: 10pt; 
# padding-left: 100pt; padding-right: 10pt; padding-top: 10pt; padding-bottom: 10pt;"> 
# <b><pre> 
# >>> dozer.ls()                                          # [1] list contents of pwd
#('folders', '/Dummy/dev')
#>>> dozer.cd('dev')                                      # [2] go to subdirectory 'dev'
#True
#>>> dozer.ls()                                           # [3] list contents of pwd
#('folders', '/Dummy/dev/build')
#>>> dozer.cd("build")                                    # [4] go to subdirectory 'build'
#True
#>>> dozer.pwd()                                          # [5] where am I?
#'/Dummy/dev/build'
#>>> dozer.ls()                                           # [6] list contents of pwd
#('folders', '/Dummy/dev/build/i686-slc4-gcc34-opt')
#>>> dozer.cd("i686-slc4-gcc34-opt/offline")              # [7] go to 'i686-slc4-gcc34-opt/offline'
#True
#>>> dozer.pwd()                                          # [8] where am I?
#'/Dummy/dev/build/i686-slc4-gcc34-opt/offline'
#>>> dozer.ls()                                           # [9] what's inside pwd?
#('channels', {'cooldozer metadata': ('cooldozer metadata', 0L), 'testChannelRTT': ('testChannelRTT', 1L)})
#</pre></b>
#</div></div>
#
# <li> Adding payload
# 
# For adding records to the database you can use two methods: DozerDB.addSchemaPayload and DozerDB.addChannelPayload. 
# The prefered way is using DozerDB.addSchemaPayload as you don't have to check if such DozerSchema has already been added
# to the database, while in case  DozerDB.addChannelPayload the channel you want has to be created first inside database and 
# you have to change directory inside database to folder in which the DozerChannel is defined.   
#
# <li> Reading payload back
#
# This is available by using DozerDB.get function.
#
# 
#</ul>
#
class DozerDB( DozerObject.DozerObject ):
    
    ## metaclass
    __metaclass__ = DozerSingleton

    ## Cool db handle
    __db = None

    ## Cool IFolder(Set) handle
    __folder = None

    ## flag for dry run
    __dry = False

    ## channel name for storing metadata 
    __metaName = str("cooldozer metadata")

    ## dict to store keys for folders and channels
    __objDict = { }

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param package cvs package name 
    # @param framework test framework name
    # @param dry flag to trigger dry run 
    # @param db name of DATABASE (as read from <connect> tag db attribute )
    # @param log DozerLogger instance 
    def __init__( self, package, framework, dry=False, db="TEST", log=None):

        super( DozerDB, self ).__init__( log )
        self.__log = log
                                                                         
        self.__iov = DozerIOV( log=self.__log ) 

        self.__dv = DozerVersion()
        self.info( self.__dv )

        self.__cv = DozerValid( log=self.__log ) 

        self.info("setting package to '%s'" % package )
        self.__package = package

        self.info("setting test framework to '%s'" % framework )
        self.__framework = framework

        self.info("setting db to " + db )
        

        self.__dry = False
        if ( type(dry) is BooleanType ):
            self.__dry = dry
            if ( dry == True ): 
                db = "TEST"
              
        if ( ( type(db) is StringType ) and 
             ( db in self.__cv.connects().keys() ) ):
            if ( db == "TEST" ):
                self.info("*** DRY RUN ***")
                self.__dry = True
                self.__connect = self.__cv.connects()["TEST"] % self.__framework
                self.debug("will use 'TEST' db [DRY RUN]")
            else:
                self.__connect = self.__cv.connects()[db]
                self.debug("will use " + db + " db")
        else:
            self.error("know nothing about '" + str(db) + "' db, exiting!")
            raise DozerError("know nothing about '" + str(db) + "' db, exiting!") 
                
        self.__dbSvc = self.__getDBSvc()
        self.info("will use PyCool %s" % self.__dbSvc.serviceVersion() )
        self.openDB()
        
    ## package name getter
    # @param self "Me, myself and Irene"
    def package( self ):
        return self.__package

    ## test framework getter
    # @param self "Me, myself and Irene"
    def framework( self ):
        return self.__framework 

    # dbDvc getter
    # @return cool.DatabaseSvcFactory.databaseService()
    def __getDBSvc( self ):
        return cool.DatabaseSvcFactory.databaseService()

    ## opens db 
    # @param self "Me, myself and Irene" 
    def openDB( self, readOnly=False ):
        self.debug("will try to open db for test framework=%s" % self.__framework )
        if ( not self.__dbSvc ):
            self.__dbSvc = self.__getDBSvc() 
        try:
            self.__db =  self.__dbSvc.openDatabase( str(self.__connect), readOnly )
            self.debug("database for framework '%s' has been opened" % self.__framework )
        except Exception , value:
            self.error("cannot open db for framework %s" % self.__framework )
            try:
                if ( self.__dry ):
                    self.info( "DRY RUN, trying to recreate db..." )
                    self.__dbSvc.dropDatabase( str(self.__connect) )
                    self.__db = self.__dbSvc.createDatabase( str( self.__connect ) )
                    self.debug( "database for test framework '%s' has been created" % self.__framework )
            except Exception, value:
                self.error ( "creation of db failed - " + value.__str__() )        
        if ( self.__db ):
            path = "/" + self.__package
            try:
                if ( self.__db.existsFolderSet( str(path) ) ):
                    self.__folder = self.__db.getFolderSet( str(path) )
                else:
                    self.info("will create folder set " + path )
                    self.__folder = self.__db.createFolderSet( str(path) )
            except:
                self.error("cannot cd or create IFolderSet " + path)
                return False
            self.pwd()
            return True
        return False
            

    ## database Id getter
    # @param self "Me, myself and Irene"
    # @return string with db id
    def dbId( self ):
        if ( self.isOpened() ):
            dbId = self.__db.databaseId() 
            self.info( dbId )
            return dbId 
        else:
            self.info("cannot get dbId, db hasn't been opened?")
            return ""

    ## checks if db is opened
    # @param self "Me, myself and Irene"
    # @return True is db is opened, False otherwise  
    def isOpened( self ):
        if ( self.__db ): return True
        return False 

    ## checks if this is DRY RUN
    # @param self "Me, myself and Irene"
    def isDry( self ):
        if ( self.__dry ): return True
        return False

    ## db closing
    # @param self "Me, myself and Irene"
    def closeDB( self ):
        if ( self.isOpened() ):
            self.__db.closeDatabase()
            self.info("database for test framework '%s' has been closed" % self.__framework )
            self.__db = None
            return True
        else:
            self.error("cannot close db for framework '%s', db wasn't opened?" % self.__framework )
            return False

    ## pwd getter
    # @param self "Me, myself and Irene"
    # @return pwd
    def pwd( self ):
        if ( self.isOpened() ):
            if ( self.__folder ):
                fullPath = self.__folder.fullPath()
                self.debug( "pwd %s" % fullPath )
                return fullPath
        else:
            self.error("pwd: cannot get db handle")
            return ""


    ## change cwd 
    # @param self "Me, myself and Irene"
    # @param where path
    def cd( self, where="." ):
        if ( where.endswith("/") ): where = where.rstrip("/")
        if ( where == "" ): where = "/"
        if ( not where.startswith("/") ):
            where = os.path.normpath( os.path.join(self.__folder.fullPath(), where ) )
            
        self.debug("will try to change directory to " + where )
        try:
            self.__folder = self.__db.getFolder( str(where) )
            self.debug( "Folder path=%s" % self.__folder.fullPath() )
            self.pwd()
            return True
        except:
            try:
                self.__folder = self.__db.getFolderSet( str(where) )
                self.debug("Folder path=%s" % self.__folder.fullPath() )
                self.pwd()
                return True
            except:
                pass
        self.error( "Folder path=%s" % self.__folder.fullPath() )
        return False


    ## creates cool.RecordSpecification
    # @param self "Me, myself and Irene"
    # @return cool.RecordSpecification() 
    def __recordSpec( self ):
         self.debug("creating record specification")
         recordSpec = cool.RecordSpecification()
         recordSpec.extend( "xml", cool.StorageType.String64k )
         self.debug( recordSpec )
         return recordSpec
         

    ## += operator for schema 
    # @param self "Me, myself and Irene"
    # @param schema DozerSchema instance
    def __iadd__( self, schema ):

        if ( isinstance( schema, DozerSchema) ):
            
            path = schema.pathChunk()
            self.info("adding schema '%s'" % path )
            if ( not schema.isSpecific()  ):
                self.error("cannot add generic schema '%s' to db, next try to add specific one!" % path )
                return self 
            else:
                path = "/" + self.__package + "/" + path
                description = "CoolDozer Folder for schema %s" % path
                if ( not self.isOpened() ):
                    raise DozerError("cannot add schema, db is not opened!")
                
                if ( self.__db.existsFolder( str(path) ) ):
                    self.info("db folder path='%s' already in database, will only update schema" % path)
                    self.cd( path )
                    self.__createSchema( schema )
                else:
                    
                    self.info("will create db folder name='%s' description='%s'" % ( path, description ) )
                    try:
                        #self.error( self.__db )
                        #self.error( self.__connect )
                        #self.error( self.isOpened() )
                        #self.error( self.__db.databaseId() )
                        if self.isOpened():
                            self.closeDB()
                            
                        ## switch to OWNER for a while
                        self.info("switching to DB OWNER account...")
                        ## save original db URI string
                        self.__connectSAVE = self.__connect
                        ## get db URI for OWNER 
                        self.__connect = self.__cv.connects()["PROD_OWNER"]
                        dbSvc = cool.DatabaseSvcFactory.databaseService()
                        dbOwner = dbSvc.openDatabase( str(self.__connect), False )
                        
                        if ( dbOwner ): self.info("db for OWNER opened")
                        else: 
                            raise DozerError("can't open db for OWNER, no more sessions left on server?")
          
                        self.info( "will create folder " + str(path) )
                        
                        recordSpec = cool.RecordSpecification()
                        recordSpec.extend( "xml", cool.StorageType.String64k )

                        dbOwner.createFolder( path, 
                                              recordSpec, 
                                              description,
                                              1,
                                              True )              
                        self.info( "folder %s has been created" % path )
                        self.cd( path )
                        ## create schema
                        self.__createSchema( schema )
                        ## close db
                        self.closeBD()
                        ## reopen for PROD (writer)
                        self.info("swiching back to original DB...")
                        self.__connect = self.__connectSAVE
                        self.openDB() 
                        self.__folder = self.cd( str(path) )
                    except Exception, value:
                        msg = "creation of folder %s falied - %s" % ( path, value.__str__() )
                        self.epanic( msg )
                        raise DozerError( msg )

        return self 

    ##
    # @param self "Me, myself and Irene"
    # @param since DozerIOV or long int timestamp
    # @param until DozerIOV or long int timestamp
    # @return tuple ( DozerIOV, DozerIOV) or (None, None)
    def __sinceUntil( self, since, until=None ):
        
        sinceIOV = untilIOV = None
        if ( isinstance(since, DozerIOV) ):
            sinceIOV = since 
        elif ( type( since ) in (LongType, IntType) ):
            sinceIOV = DozerIOV( long(since) )

        if ( until == None ):
            untilIOV = DozerIOV( DozerIOV.max() )
        elif ( isinstance(until, DozerIOV) ):
            untilIOV = until
        elif ( type(until) in ( LongType, IntType ) ):
            untilIOV = DozerIOV( long( until ) )
            

        if ( None not in ( sinceIOV, untilIOV) ):
            if ( sinceIOV < untilIOV ):
                self.debug( "since %s" % sinceIOV )
                self.debug( "until %s" % untilIOV )
                return ( sinceIOV, untilIOV )
            else:
                self.error("wrong values for arguments 'since' and 'until', 'since' >= 'until'!")
                self.debug( "since %s" % sinceIOV )
                self.debug( "until %s" % untilIOV )
        else:
            self.error("wrong values for arguments 'since' or 'until', wrong types?")

        return ( None, None )
 

    ## add one DozerChannel as payload (works only if current directory has got this channel in!)
    # @param self "Me, myself and Irene"
    # @param channel DzoerChannel instance
    # @param since DozerIOV or long integer for since timestamp
    # @param @until DozerIOV or long integer for until timestamp (default = cool stamp max)
    # @return True/False if operation completed sucessfully/with errors
    # @throw DozerError when channel isn't a DozerChannel instance, wrong types for since and until, since >= until, db wasn't opened
    def addChannelPayload( self, channel, since, until=None ):

        since, until = self.__sinceUntil( since, until )
    
        if ( None in ( since, until ) ):
            raise DozerError("wrong arguments for 'since' or 'until' passed to DozerDB.addChannelPayload")
        

        if ( isinstance(channel, DozerChannel) == False ):
            raise DozerError("wrong type for argument 'channel' in DozerDB.addChannelPayload, should be DozerChannel instance")
        
        if ( not self.isOpened() ):
            raise DozerError("cannot add payload, db is nor opened!")
    
        channelName = str( channel.name() )
    
        try:
            if ( self.__folder.existsChannel( channelName ) ):
                channelId = self.__folder.channelId( channelName )
                
                data = cool.Record( self.__recordSpec() )
                data["xml"] = str( channel.xml().toxml() )
                self.debug( "storing channel name='%s' since=%s until=%s data=%s" % ( channelName, 
                                                                                      since.AsISO(), 
                                                                                      until.AsISO(),
                                                                                      data["xml"] ) )
                self.__folder.storeObject( since.AsCool(), until.AsCool(), data, channelId  )
                self.info("payload for channel name='%s' has been added" % channelName )
                self.__folder.flushStorageBuffer()
                return True
            else:
                self.error("DozerChannel name='%s' not found in db folder %s" % (channelName, self.__folder.fullPath() ) )
        except:
            self.epanic("unknow error when processing DozerDB.addChannelPayload")
            
        return False
        
    ## add whole DozerSchema as db payload
    # @param self "Me, myself and Irene"
    # @param schema DozerSchema instance
    # @param since DozerIOV or long for since point in time
    # @param until DozerIOV or long for until point in time
    # @throw DozerError 1. if since > until 2. schwma isn't DozerSchema instance
    def addSchemaPayload( self, schema, since, until=None ):

        if ( isinstance( schema, DozerSchema ) == False ) :
            raise DozerError("cannot add payload, schema parameter isn't an instnace of DozerSchema class")
    
        if ( until == None ) : 
            until = DozerIOV.max()
        elif ( isinstance(until, DozerIOV) ):
            until = until.AsCool()
        elif ( type( until) in (LongType, IntType) ):
            until = long( until )
        else:
            raise DozerError("parameter 'until' for DozerDB.addPayload isn't of type None, DozerIOV or (long) integer")

        if ( isinstance(since, DozerIOV) ):
            since = since.AsCool()
        elif ( type(since) in ( LongType, IntType ) ):
            since = long(since)
        else:
            raise DozerError("parameter 'since' for DozerDB.addPayload is not of type DozerIOV or (long) integer")

        if ( since > until ):
            raise DozerError("wrong values for parameters 'since' and 'until' in DozerDB.addPayload, 'since' > 'until'!")

        if ( schema.isSpecific() ):
            path = "/" + self.__package + "/" + schema.pathChunk()
    
            if ( not self.isOpened() ):
                raise DozerError("cannot add payload, db is nor opened!")
            self += schema

            self.cd( path )
            self.pwd()
            self.cd(path)
            self.pwd()
            for chKey in schema:
                channel = schema[chKey]
                chName = str(channel.name())  
                
                self.error( self.__folder.fullPath() )
                

                if ( self.__folder.existsChannel( chName ) ):
                    channelId = self.__folder.channelId(chName)
                   
                    data = cool.Record( self.__recordSpec() )
                    data["xml"] = str( channel.xml().toxml() )
                    self.debug( "storing channel name='%s' since=%s until=%s data=%s" % ( chName, 
                                                                                            DozerIOV(since).AsISO(), 
                                                                                            DozerIOV(until).AsISO(),
                                                                                            data["xml"] ) )
                    self.__folder.storeObject( since, until, data, channelId  )
                else:
                    self.warn("channel name='%s' not found in db, skipping!" % channel.name() )
                    
            self.__folder.flushStorageBuffer()
                
            


    ## create cool FolderSet structure + Folder + channels in it
    # @param self "Me, myself and Irene"
    # @param schema DozerSchema instance
    def __createSchema( self, schema ):
        self.info("*** MAKING COPY OF CLEAN SCHEMA ***")
        schema = DozerSchema.fromXMLDoc( schema.xml() )
        schema.reset()

        self.pwd()
        self.__dbSchema = None
        
        
        if ( self.__folder and self.__folder.existsChannel( str("cooldozer metadata") ) ):
            dbSchemaObj = self.__getChannel( str("cooldozer metadata"), since=self.__iov.today() )
            if ( dbSchemaObj ):
                self.__dbSchema = DozerSchema.fromXMLString( dbSchemaObj.payload()["xml"] )
        else:            
            self.__folder.createChannel(0, "cooldozer metadata", "cooldozer metadata" )
        data = cool.Record( self.__recordSpec() )
        schemaXML  = schema.xml().toxml()
        data["xml"] = str( schema.xml().toxml() )
        self.__folder.storeObject( self.__iov.today().AsCool(), self.__iov.max(), data, 0L )
        self.__folder.flushStorageBuffer()
        

        self.debug("old database schema (as read from cooldozer metadata channel):\n" + str( self.__dbSchema ) )
        self.debug("new database schema (as read from function argument):\n" + str(schema))
  
        if ( self.__dbSchema == None ):
            self.info("new configuration, will create schema from scratch")
            channelId = 1
            for chKey in schema:
                channel = schema[chKey]
                rtName = str(channel.name())
                rtUUID = str(channel.uuid())
                self.__folder.createChannel( channelId, rtName, rtUUID )
                self.info("channel name='%s' uuid='%s' channelId=%d has been created" % ( rtName, 
                                                                                          rtUUID, 
                                                                                          channelId ) )
                channelId += 1
        else:

            self.info("*** METADATA UPDATE ***")
            data = cool.Record( self.__recordSpec() )
            schemaXML  = schema.xml().toxml()
            data["xml"] = str( schema.xml().toxml() )
            self.__folder.storeObject( self.__iov.today().AsCool(), self.__iov.max(), data, 0L )
            self.__folder.flushStorageBuffer()
            
            dbKeys = self.__dbSchema.keys() 
            rtKeys = schema.keys()
            
            toDrop = []
            toCreate = []

            for dbKey in dbKeys:
                if ( dbKey not in rtKeys ):
                    toDrop.append( dbKey )
                    
            for rtKey in rtKeys:
                if ( rtKey not in dbKeys ):
                    toCreate.append(rtKey)

            self.usedId = self.__folder.listChannels()
            self.__usedIds = [ ]
            for i in self.usedId:
                self.__usedIds.append(i)

                
            self.info("*** DROPPING OF CHANNELS: " + str(toDrop) +" ***")    
            for dbKey in dbKeys:
                if ( self.__folder.existsChannel( str( dbKey) ) ):
                    id = self.__folder.channelId( str(dbKey) )
                    if ( dbKey in toDrop ):
                        try:
                            self.info("dropping db channel name='%s' channelId=%d" % ( dbKey, id ) )
                            what = self.__folder.dropChannel(id)
                            if ( what ): self.info( "channel name='%s' has been removed" % dbKey )
                        except:
                            self.panic("cannot drop channel inside db, channel contains IOVs...")
                            pass
                    else:
                       self.__usedIds.append(id)
            

            self.usedId = self.__folder.listChannels()
            self.__usedIds = [ ]
            for i in self.usedId:
                self.__usedIds.append(i)
            self.__usedIds.sort()

            self.info("*** CREATION OF NEW CHANNELS: " + str(toCreate) + " ***" )
            for rtKey in toCreate:                
                rtName = str( schema[rtKey].name() )
                rtUUID = str( schema[rtKey].uuid() )
                if ( rtKey in dbKeys ):
                    if ( self.__folder.existsChannel( str( rtKey ) ) ):
                        self.debug("channel name='%s' already in db, creation skipped" % rtKey)
                        continue
                else:
                    self.info("creating channel name='%s'" % rtKey )
                    channelId = self.__getNextId( ) 
                    self.__folder.createChannel( channelId, rtName, rtUUID )
                    data = cool.Record( self.__recordSpec() )
                    data["xml"] = str( schema[rtName].xml().toxml() )
                    self.__folder.storeObject( 0L, self.__iov.today().AsCool(), data, 0L )
                    self.__folder.flushStorageBuffer()

    ## free channelId getter 
    # @param self "Me, myself and Irene"
    # @param last last used channelId
    def __getNextId( self, last=0L ):
        id = last
        while True:
            if ( id in self.__usedIds ): 
                id = id+1L 
            else:
                self.__usedIds.append( id )
                self.__usedIds.sort()
                return id

    ## channel getter
    # @param self "Me, myself and Irene"
    # @param name channel name
    # @param since DozerIOV, point in time 
    # @param until DozerIOV, point in time
    def __getChannel( self, name, since, until=None ):
        channelId = self.__folder.channelId( str(name) )
        channel = self.__folder.findObject( since.AsCool() , channelId )
        return channel
                 

    ## list current cool folder
    # @param self "Me, myself and Irene"
    # @return tuple ( what, dictionary )
    def ls( self ):
        out = { "channels" : {} ,
                "folders"  : {} }
        if ( not self.isOpened() ):
            raise DozerError("cannot list, db is not opened!")
        
        if ( self.__folder.isLeaf() ):
            try:
                channels = self.__folder.listChannels() 
                for channel in channels:
                    chName = self.__folder.channelName( channel)
                    chUUID = self.__folder.channelName( channel )
                    self.debug("channel id='%d' name='%s'" % (channel, chName ) )
                    out["channels"][chName] = ( chUUID, channel )
            except:
                self.epanic("can't read list fo channels for folder " +  self.__folder.fullPath()  + ", skipping...")
                pass
            return "channels", out["channels"]
        else:
            for folder in self.__folder.listFolderSets():
                out["folders"][folder] = folder
            
            for folder in self.__folder.listFolders():
                out["folders"][folder] = folder
       
        return "folders", out["folders"]            
        
    ## get obj from path
    # @param self "Me, myself and Irene"
    # @param path path inside db
    # @param since DozerIOV or long point in time
    # @param until DozerIOV or long cool iov
    def get( self, path=".", since=None, until=None ):

        if ( until == None ):
            until = self.__iov.today().AsCool()
        elif ( isinstance( until, DozerIOV ) ):
            until = until.AsCool()
        elif ( type(until) in (LongType, IntType ) ):
            until = long(until)
        else:
            raise DozerError("wrong type for argument 'until' in DozerDB.get call, must be None, DozerIOV or integer")

        if ( since == None ):
            since = self.__iov.lastNDays(7)
        elif ( isinstance(since, DozerIOV) ):
            since = since.AsCool()
        elif ( type(since) in (LongType, IntType) ):
            since = long(since)
        else:
            raise DozerError("wrong type for argument 'since' in DozerDB.get call, must be None, DozerIOV or integer")

        if ( since > until ):
            raise DozerError("wrong values for parameters 'since' and 'until' in DozerDB.get, 'since' > 'until'!")

        normpath = os.path.normpath( path )
        
        if ( not normpath.startswith( self.pathChunk() ) ):
            self.debug("path doesn't start with package name, extending... ")
            normpath = os.path.normpath( self.pathChunk() + "/" + normpath )
           
        if normpath.endswith(".") : normpath = normpath.rstrip(".")
     
        self.debug( "path='%s' normpath='%s'" % ( path, normpath ) )
        if ( not self.isOpened() ): 
            raise DozerError("cannot get from db, db is not opened!")

        words = str(normpath).split("/")
      
   
        if ( len(words) <= 7 ):
            self.cd( normpath )
            self.pwd()
        elif ( len(words) == 8 ):
            path =  "/".join(words[:7])
           
            channel = words[7]
            self.cd(path )
            recordSet = None
            if self.__folder.existsChannel( channel ):
                channelId = self.__folder.channelId( str(channel) )
                self.debug( "channel '%s' found in path '%s'" % ( channel, path ) )
                package, nightly, install, cmtconfig, project, jobId = path.split("/")[1:]
                
                self.debug( "channelId=%d since=%s until=%s" % ( channelId,
                                                                 DozerIOV(since).AsISO(), 
                                                                 DozerIOV(until).AsISO() ) ) 
                payloads = self.__folder.browseObjects( since, 
                                                        until, 
                                                        cool.ChannelSelection(channelId) )
                
                
                empty = "empty" if payloads.isEmpty() == 1 else "not empty"
                self.debug( "payload is " + empty + ", records in payload = " + str(payloads.size() ) )
                #if empty == "empty":
                #    return None
                recordSet  = DozerRecordSet( package,
                                             self.__framework,
                                             nightly, 
                                             install, 
                                             cmtconfig,
                                             project, 
                                             jobId,
                                             since, 
                                             until )
                
                
                for obj in payloads:
                    self.debug( "adding payload since=" + str(obj.since()) + " until=" + str(obj.until()))
                    recordSet += ( obj.since(), 
                                   obj.until(),
                                   DozerChannel.fromXMLString( obj.payload()["xml"] ) ) 

            return recordSet
      


    ## schema getter
    # @param self "Me, myself and Irene"
    # @param since cool point in time (if not set today cool timestamp will be used)
    # @param fromPath path inside Dozer cool database
    # @return None or DozerSchema instance
    def getSchema( self, fromPath=None, since=None ):
        if ( not self.isOpened() ):
            raise DozerError("cannot read back schema from db, db is not opened!")

        if ( not since ):
            since = self.__iov.today()
                    
       
        if ( fromPath ):

            if ( not fromPath.startswith( self.pathChunk() ) ):
                fromPath = self.pathChunk() + "/" + fromPath 
 
            self.cd( str( fromPath ) )
                
            pwd = self.pwd()

            try:
                if ( self.__folder.existsChannel( str("cooldozer metadata") ) ):
                    dbSchemaObj = self.__getChannel( str("cooldozer metadata"), since )
                    if ( dbSchemaObj ):
                        schema = DozerSchema.fromXMLString( dbSchemaObj.payload()["xml"] )
                        return schema
            except:
                self.error("cannot find schema in cool Folder path '%s'" % pwd )
      


    ## helper path builer function
    # @param self "Me, myself and Irene"
    # @return string "/" + "package name" 
    def pathChunk( self ):
        return str("/" + self.__package ) 

    def __len__( self ):
        return len( self.__objDict.keys() )

    def __getitem__( self, key ):

        pass
    
    def __items__( self ):
        pass

    def __iteritems__( self ):
        pass

    def __iter__( self ):
        return self.__objDict.__iter__()

    def __contains__( self, key ):
        return key in self.__objDict.keys()

    def has_key( self, key ):
        return key in self.__objDict.keys()

    def keys( self ):
        return self.__objDict.keys()
        

    


##
# @class test_DozerDB
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test case for DozerDB class
class test_DozerDB( unittest.TestCase ):
    
    def setUp( self ):
        self.__log = DozerLogger("test_CoolDozer.log", "test_DozerDB", toConsole=True)
        self.__log.toConsole( True )

        globals()["laoggerdjsfhksdj"] = self.__log

        self.dataRTT = DozerData("testDataRTT", "Int", "N_{obs}")
        self.chanRTT = DozerChannel("testChannelRTT", "testChannelDescriptionRTT" )
        self.scheRTT = DozerSchema("*", "build", "i686-slc4-gcc34-opt", "AtlasProduction" , "job_1")
        self.confRTT = DozerConfig("RTT")

        self.chanRTT += self.dataRTT
        self.scheRTT += self.chanRTT
        self.confRTT += self.scheRTT

        self.db = DozerDB( "DummyTestPackage", "RTT", dry=True )

    ## __init__()
    # @param self "Me, myself and Irene"
    def test_01_constructor( self ):

        try:
            db = DozerDB( "Dummy", "RTT", dry=True )
        except: 
            pass
        self.assertEqual( isinstance( db, DozerDB ), True)

        try:
            db = DozerDB( "Dummy", "RTT", dry="PROD_READ" )
        except: 
            pass
        self.assertEqual( isinstance(db, DozerDB ), True)

        try:
            db = DozerDB( "Dummy", "RTT", dry="FUBAR" )
        except DozerError: 
            self.epanic("it was intentional, don't panic!")

        try:
            db = DozerDB( "Dummy", "RTT", dry=True)
        except: 
            pass
        self.assertEqual( isinstance(self.db, DozerDB ), True)




    ## += DozerSchema
    # @param self "Me, myself and Irene"
    def test_02_addSchema( self ):
        schema = self.confRTT["dev"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]
        path = schema.pathChunk()
        self.db += schema 
        schema = self.confRTT["bugfix"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]
        self.db += schema

        self.assertEqual( isinstance(self.db, DozerDB), True)

        print path

        schema = self.db.getSchema( path )

        print "*******", schema

        self.assertEqual( isinstance(schema, DozerSchema), True)

        print schema.nightly()


    ## addPayload()
    # @param self "Me, myself and Irene" 
    def test_03_addPayload( self ):
        iov = DozerIOV()
        schema = self.confRTT["bugfix"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]

        today = iov.today()
        schema["testChannelRTT"]["testDataRTT"].setValue(7)
        self.db.addSchemaPayload( schema, iov.lastNDays(6) )
        schema["testChannelRTT"]["testDataRTT"].setValue(6)
        self.db.addSchemaPayload( schema, iov.lastNDays(5) )
        schema["testChannelRTT"]["testDataRTT"].setValue(5)
        self.db.addSchemaPayload( schema, iov.lastNDays(4) )
        schema["testChannelRTT"]["testDataRTT"].setValue(4)
        self.db.addSchemaPayload( schema, iov.lastNDays(3) )
        schema["testChannelRTT"]["testDataRTT"].setValue(3)
        self.db.addSchemaPayload( schema, iov.lastNDays(2) )
        schema["testChannelRTT"]["testDataRTT"].setValue(2)
        self.db.addSchemaPayload( schema, iov.lastNDays(1) )
        schema["testChannelRTT"]["testDataRTT"].setValue(1)
        self.db.addSchemaPayload( schema, today )

        
        channel = schema["testChannelRTT"]
        channel["testDataRTT"].setValue(10)
        since = DozerIOV( iov.lastNDays(7) )
        until = DozerIOV( iov.lastNDays(6))
        self.assertEqual( self.db.addChannelPayload( channel, since, until  ), True )


    ## ls() cd() get()
    # @param self "Me, myself and Irene"
    def test_04_fs( self ):
 
        spath = self.confRTT["bugfix"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"].pathChunk()
        
        dbpath = self.db.pathChunk()
        
        print spath
        print dbpath

        recordSet = self.db.get( dbpath + "/" + spath + "/" + "testChannelRTT" )
        
        print recordSet

        graph = recordSet.getDozerGraph() 

        print graph.plotables()

        graph.trend( "testDataRTT", True, "trend_testDataRTT.png")
        graph.histogram( "testDataRTT", True, "histo_testDataRTT.png" )



    def test_05_evolution( self ):
        
        rtSchema = DozerSchema("bugfix", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "evo" )

        self.db = DozerDB( "DummyTestPackage", "RTT", dry=True)
        
        for i in range(1,6):
            self.db.openDB()
            newChannel = DozerChannel( str(i), str(i) )
            newChannel += DozerData( str(i), "Int" )
            rtSchema += newChannel
            self.db += rtSchema
            self.db.closeDB()
            

 


## test suite execution
if __name__ == "__main__":
    
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DozerDB)      
    unittest.TextTestRunner(verbosity=3).run(suite)
    
