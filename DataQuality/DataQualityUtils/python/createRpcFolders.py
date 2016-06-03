# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#*************************************************
# Python to create RPC SQLite Folder to be filled with RPC alibration results
# author Michele Bianco michele.bianco@le.infn.it, Gabriele Chiodini gabriele.chiodini@le.infn.it                          
#and Angelo Guida angelo.guida@le.infn.it
# 08/April/2009
#************************************************


def createRPCDQMFDB():
      import sys, os
      from PyCool import cool,coral
      
      # get database service and open database
      dbSvc=cool.DatabaseSvcFactory.databaseService()
      # database accessed via physical name
      dbstring='sqlite://;schema=RPCDQMFOFFLINE.db;dbname=RPC_DQA'
      if os.access('RPCDQMFOFFLINE.db', os.R_OK):
            try:
                  os.unlink('RPCDQMFOFFLINE.db')
            except Exception, e:
                  print 'Unable to remove existing db file, reason', e
      try:
            db=dbSvc.createDatabase(dbstring)
      except Exception,e:
            print 'Problem creating database',e
            return
      print "Created database",dbstring

      spec2=cool.RecordSpecification()
      spec2.extend("recEta",cool.StorageType.String4k)
      spec2.extend("detEta",cool.StorageType.String4k)
      spec2.extend("recPhi1",cool.StorageType.String4k)
      spec2.extend("recPhi2",cool.StorageType.String4k)
      spec2.extend("detPhi1",cool.StorageType.String4k)
      spec2.extend("detPhi2",cool.StorageType.String4k)

      # folder meta-data - note for Athena this has a special meaning
      desc="<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>"
      # create the folder - single version
      # last argument is createParents - if true, automatically creates parent
      # folders if needed
      # note this will not work if the database already exists - delete mycool.db first
      # Deprecated/dropped:  myfolder=db.createFolder('/OFFLINE/OFFLINE_DQMF',spec2,desc,cool.FolderVersioning.MULTI_VERSION,True)
      folderSpec=cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec2)
      myfolder=db.createFolder('/OFFLINE/OFFLINE_DQMF',folderSpec,desc,True)
      db.closeDatabase()


def createRPCConditionDB():
       import sys, os
       from PyCool import cool,coral

       # get database service and open database
       dbSvc=cool.DatabaseSvcFactory.databaseService()
       # database accessed via physical name
       dbstring='sqlite://;schema=RPCConditionDB.db;dbname=RPC_DQA'
       if os.access('RPCConditionDB.db', os.R_OK):
            try:
                  os.unlink('RPCConditionDB.db')
            except Exception, e:
                  print 'Unable to remove existing db file, reason', e
       try:
             db=dbSvc.createDatabase(dbstring)
       except Exception,e:
             print 'Problem creating database',e
             return
       print "Created database",dbstring

       spec2=cool.RecordSpecification()
       spec2.extend("PanelRes",cool.StorageType.String255)
       spec2.extend("StripStatus",cool.StorageType.String4k)

       desc="<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>"

       # create the folder - single version
       # last argument is createParents - if true, automatically creates parent
       # folders if needed
       # note this will not work if the database already exists - delete mycool.db first
       # Deprecated/dropped:  myfolder=db.createFolder('/OFFLINE/FINAL',spec2,desc,cool.FolderVersioning.MULTI_VERSION,True)
       folderSpec=cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec2)
       myfolder=db.createFolder('/OFFLINE/FINAL',folderSpec,desc,True)
       db.closeDatabase()


