# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#*************************************************
#Author monica.verducci@cern.ch
#python script to create the MDT DQ folders in sqlite files and fill in it
#during the postprocessing algorithm
#************************************************


def createMDTConditionDBDead():
       import sys, os
       from PyCool import cool,coral

       # get database service and open database
       dbSvc=cool.DatabaseSvcFactory.databaseService()
       # database accessed via physical name

       dbstring='sqlite://;schema=MDTDQMFOFFLINE_DEAD.db;dbname=MDT_DQA'

       if os.access('MDTDQMFOFFLINE_DEAD.db', os.R_OK):              
            try:
                  os.unlink('MDTDQMFOFFLINE_DEAD.db')
                  #print 'Trying to Delete'
            except Exception, e:
                  print 'Unable to remove existing db file, reason', e
       try:
             db=dbSvc.createDatabase(dbstring)
       except Exception,e:
             print 'Problem creating database',e
             return
       #print "Created database",dbstring

       spec2=cool.RecordSpecification()
       spec2.extend("Chamber_Name",cool.StorageType.String255)
       spec2.extend("Dead_multilayer",cool.StorageType.String255)
       spec2.extend("Dead_layer",cool.StorageType.String255)
       spec2.extend("Dead_mezz",cool.StorageType.String255)
       spec2.extend("Dead_asd",cool.StorageType.String255)
       spec2.extend("Dead_tube",cool.StorageType.String4k)


       desc="<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>"

       # create the folder - single version
       # last argument is createParents - if true, automatically creates parent
       # folders if needed
       # note this will not work if the database already exists - delete mycool.db first
       # Deprecated/dropped:  myfolder=db.createFolder("/OFFLINE/DQMFOFFLINE/DQMFOFFLINE_DEAD",spec2,desc,cool.FolderVersioning.MULTI_VERSION,True)
       folderSpec=cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec2)
       myfolder=db.createFolder("/OFFLINE/DQMFOFFLINE/DQMFOFFLINE_DEAD",folderSpec,desc,True)
       db.closeDatabase()

def createMDTConditionDBNoisy():
       import sys, os
       from PyCool import cool,coral

       # get database service and open database
       dbSvc=cool.DatabaseSvcFactory.databaseService()
       # database accessed via physical name

       dbstring='sqlite://;schema=MDTDQMFOFFLINE_NOISY.db;dbname=MDT_DQA'

       if os.access('MDTDQMFOFFLINE_NOISY.db', os.R_OK):
            try:
                  os.unlink('MDTDQMFOFFLINE_NOISY.db')
            except Exception, e:
                  print 'Unable to remove existing db file, reason', e
       try:
             db=dbSvc.createDatabase(dbstring)
       except Exception,e:
             print 'Problem creating database',e
             return
       #print "Created database",dbstring

       spec2=cool.RecordSpecification()
       spec2.extend("Chamber_Name",cool.StorageType.String255)
       spec2.extend("Noisy_multilayer",cool.StorageType.String255)
       spec2.extend("Noisy_layer",cool.StorageType.String255)
       spec2.extend("Noisy_mezz",cool.StorageType.String255)
       spec2.extend("Noisy_asd",cool.StorageType.String255)
       spec2.extend("Noisy_tube",cool.StorageType.String4k)
       
       desc="<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>"

       # create the folder - single version
       # last argument is createParents - if true, automatically creates parent
       # folders if needed
       # note this will not work if the database already exists - delete mycool.db first
       # Deprecated/dropped:  myfolder=db.createFolder("/OFFLINE/DQMFOFFLINE/DQMFOFFLINE_NOISY",spec2,desc,cool.FolderVersioning.MULTI_VERSION,True)
       folderSpec=cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec2)
       myfolder=db.createFolder("/OFFLINE/DQMFOFFLINE/DQMFOFFLINE_NOISY",folderSpec,desc,True)
       db.closeDatabase()
