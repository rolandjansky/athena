#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Shaun Roe, 4 February 2015

from IBLTotParser import *
from PyCool import cool
import sys, os

def upload(connection, mydata, folderPath):
  """Upload a dictionary of channelNum:string4kData to a local sqlite db
  Shaun Roe February 9 2015
  """
  db=None
  creation=False
  dbSvc=cool.DatabaseSvcFactory.databaseService()
  try:
    db=dbSvc.openDatabase(connection,False)
  except Exception,e:
    print 'Problem opening database',e
    print '..will try to create one instead'
    try:
      db=dbSvc.createDatabase(connection)
      creation=True
    except Exception,e:
      print 'Problem creating database',e
      return -1
      
  print "Opened database",connection
  #all the data are simple strings, type String4k
  dataType=cool.StorageType.String4k
  # setup a folder payload specification
  spec=cool.RecordSpecification()
  spec.extend("data",cool.StorageType.String4k)
  desc='<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'
  myfolder=None
  if creation: 
    myfolder=db.createFolder(folderPath,spec,desc,cool.FolderVersioning.MULTI_VERSION,True)
  else:
    myfolder=db.getFolder(folderPath)
  #iovMin=cool.ValidityKeyMin
  iovMin=(222222 << 32)
  iovMax=cool.ValidityKeyMax
  uploadCount=0
  try:
    for hashId in mydata:
      payload=cool.Record(spec)
      payload['data'] = mydata[hashId]
      print mydata[hashId]
      channelID = cool.ChannelId(hashId)
      myfolder.storeObject(iovMin,iovMax,payload,channelID)
      uploadCount+=1
  except Exception,e:
    print "Problem uploading ", e
    db.closeDatabase()
    return 0
  db.closeDatabase()
  return uploadCount
  

def main():
  """'main()' command to make an SQLite db from a pixel calibration text file.
  The calibration file format is as sent by
  Tayfun Ince, but the identifier format may be either the svn format one (link below) or
  that prepared by Geraldine Conti. Different parsers are supplied in the IBLTotParser
  module to cope with either format. 
  Note that as of Feb 13 2015, this has *only* been tested on a calibration file 
  containing IBL data, not a full calibration set.
  The data were uploaded using the following procedure:
  **FIRST set up an Athena release **
  To generate a consistent 2048-module set, I downloaded a known tag to a local 
  sqlite file:
  AtlCoolCopy.exe ATLAS_COOLOFL_PIXEL/OFLP200 sqlite://;schema=mycool.db;dbname=OFLP200 -tag PixCalib-SIM-RUN12-000-01 -folder /PIXEL/PixCalib -run 222222 -ot HEAD -create 
  This was checked with an sqlite browser.
  Run this command with
  python EnterCalibrationToDb.py
  and then I checked the sqlite file again.
  The final upload to the central db was performed with:
  AtlCoolCopy.exe "sqlite://;schema=mycool.db;dbname=OFLP200" "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_PIXEL;dbname=OFLP200;user=<uname>;password=<password>" -folder /PIXEL/PixCalib -tag HEAD -ot PixCalib-SIM-RUN12-IBL-02
  ..and checked with coolcherrypy:
  http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_PIXEL/OFLP200/PIXEL/PixCalib/timespan/954444812386304-954444812386306/tag/PixCalib-SIM-RUN12-IBL-02/channels/420
  Fares checked this and created two new tags:
  PixCalib-SIM-RUN12-000-02
  PixCalib-DATA-RUN2-IBL-00
  """
  connection="sqlite://;schema=mycool.db;dbname=OFLP200"
  calibrationFile="IBLCalibrationData.txt" #Calibration file from Tayfun Ince
  hashIdFile="gcontiIdTable.txt" #Identifier file provided by Geraldine Conti
  svnFilename="table_Run2.txt" # ...or... the in-svn identifier file from Fares Djama
  # https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/trunk/share/table_Run2.txt
  mydata=parseFiles(calibrationFile,svnFilename)
  #mydata=IBLTot(calibrationFile, hashIdFile)
  folderPath="/PIXEL/PixCalib"
  numUploads= upload(connection, mydata, folderPath)
  print "Number of entries: ",numUploads
  if numUploads==0:
    return -1
  return 0

if __name__=="__main__":
  sys.exit(main())
  
  
  
