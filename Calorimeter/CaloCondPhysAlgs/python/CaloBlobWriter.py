#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import ROOT
from PyCool import cool
import os
#from CaloCondBlobAlgs import CaloCondTools, CaloCondLogger

def CaloCondBlobWriter(spec,valuePairs,defaultValue):

   vec=getattr(ROOT,'vector<float>')
   vecvec=getattr(ROOT,'vector<vector<float> >')

   #Build the defintion vector (1 value per gain)
   gainDefVec = vec()#g.std.vector('float')()
   gainDefVec.push_back(defaultValue)
   defVec = vecvec()
   defVec.push_back(gainDefVec)

   nLArChannels=182468 # Connected LAr cells
   nAllChannels=187652 # Connected LAr + Tile cells

   if len(valuePairs)>nLArChannels: #input data apparently includes also Tile
      nChannels=nAllChannels 
   else:
      nChannels=nLArChannels
      pass

   
   print "Build CaloCondBlob object"
   data = cool.Record( spec )
   blob = data['CaloCondBlob16M']
   #flt = g.CaloCondBlobFlt.getInstance(blob)
   fltClass=getattr(ROOT,'CaloCondBlobFlt')
   flt=fltClass.getInstance(blob)
   flt.init(defVec,nChannels,1)
   
   print "Filling CaloCondBlob object"
   dvec=vec()
   dvec.push_back(defaultValue)

   foundChans=set()


   for (hashid,value) in valuePairs:
      if hashid>=nChannels:
         print "ERROR: Invalid hash id",hashid
         continue
      
      if hashid in foundChans:
         print "WARNING: Channel with hash",hashid,"encountered twice!"
      else:
        foundChans.add(hashid)
        pass
      dvec[0]=value
      flt.setData(hashid,0,dvec)
      pass
   
   if len(foundChans)<nChannels:
      print "WARNING No values found for",nChannels-len(foundChans),"channels. Left at default value",defaultValue
      

   return data



def CaloCondBlobWriterFromFile(spec,filename, defaultvalue): 

   fromfile=[]
   rein=open(filename)
   for line in rein:
      beforecomment=line.split('#')[0].strip()
      if len(beforecomment)==0: continue
      
      tok=beforecomment.split()
      if len(tok)!=2:
         print "ERROR: Unexpected syntax:", line
         continue

      try:
         h=int(tok[0])
         v=float(tok[1])
      except ValueError:
         print "ERROR: Expected numbers, got",tok[0],tok[1] 
         continue

      fromfile.append((h,v))
      pass
   rein.close()
   #print fromfile
   return CaloCondBlobWriter(spec,fromfile,defaultvalue)
   



def createSqlite(sqliteName,folderName,foldertag,iovMin=cool.ValidityKeyMin,iovMax=cool.ValidityKeyMax,inputFileName=None,defaultvalue=1.0):

   dbSvc = cool.DatabaseSvcFactory.databaseService()

   if os.access(sqliteName,os.R_OK):
      print "UPDATING existing sqlite file",sqliteName
      db=dbSvc.openDatabase("sqlite://;schema="+sqliteName+";dbname=CONDBR2",False)
   else:
      print "Creating new sqlite file",sqliteName
      db=dbSvc.createDatabase("sqlite://;schema="+sqliteName+";dbname=CONDBR2")
      
   
   spec = cool.RecordSpecification()
   spec.extend('CaloCondBlob16M', cool.StorageType.Blob16M )

   desc= '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'

   if db.existsFolder(folderName):
      folder=db.getFolder(folderName)
   else:
      print "Creating COOL folder/tag  %s/%s" % (folderName,foldertag)
      #folder = db.createFolder(folderName, spec, desc, cool.FolderVersioning.MULTI_VERSION, True)
      folderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec)
      folder = db.createFolder(folderName, folderSpec, desc, True)
      pass

   if inputFileName is None or len(inputFileName)==0:
      print "No input given. Create empty blob"
      data = cool.Record(spec)
   else:
      data=CaloCondBlobWriterFromFile(spec,inputFileName,defaultvalue)

   print "Storing CaloCondBlob object"
   folder.storeObject(iovMin, iovMax, data, cool.ChannelId(0), foldertag,True)
   
   db.closeDatabase()
