#!/bin/env python
#
# script to turn scaling parameters into a local SQlite DB.
# does not publish directly to COOL.
# updated for COOL 2 (RJH 7/6/07)
#
import sys,traceback
from PyCool import cool,coral

# database and folder name
connect="sqlite://X;schema=mycool.db;dbname=OFLP200"
foldername="/Indet/TrkErrorScaling"
tag="IndetTrkErrorScaling_nominal"
dataset=[ ("PixPhi Barrel",1.0,0.0),
          ("PixEta Barrel",1.0,0.0),
          ("PixPhi Endcap",1.0,0.0),
          ("PixEta Endcap",1.0,0.0),
          ("SCT Barrel",1.0,0.0),
          ("SCT Endcap",1.0,0.0),
          ("TRT Barrel",1.0,0.0),
          ("TRT Endcap",1.0,0.0)
          ]
# first value is factor on error, second uncorrelated noise term
# to error given in mm

dbSvc = cool.DatabaseSvcFactory.databaseService()
try:
    db = dbSvc.openDatabase(connect,False)
except Exception, e:
    print e
    print "Could not connect to the database"
    # try to create it
    try:
        db=dbSvc.createDatabase(connect)
    except Exception, e:
        print e
        print "Could not create the database either"
        sys.exit(2)

# setup folder specification
spec = cool.RecordSpecification()
spec.extend("Name",cool.StorageType.String4k)
spec.extend("Len",cool.StorageType.Int32)
# assume we will not have more than 5 parameters
maxpar=5
spec.extend("par1",cool.StorageType.Double)
spec.extend("par2",cool.StorageType.Double)
spec.extend("par3",cool.StorageType.Double)
spec.extend("par4",cool.StorageType.Double)
spec.extend("par5",cool.StorageType.Double)

# check if folder exists
if (not db.existsFolder(foldername)):
    print "Attempt to create",foldername
    desc='<timeStamp>run-event</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>'
    db.createFolder(foldername,spec,desc,cool.FolderVersioning.MULTI_VERSION,True)
    print 'Folder',foldername,'created OK'

# now write data
folder=db.getFolder(foldername)
chan=0
print "Writing data to",folder,"with tag",tag
for idata in dataset:
    name=idata[0]
    nvals=len(idata)-1
    if (nvals>maxpar):
        print "Maximum of",maxpar,"parameters allowed - additional ones ignored!"
        nvals=maxpar
    print "Write data for name",name,"number of values",nvals,"at channel",chan
    payload=cool.Record(spec)
    payload['Name']=name
    payload['Len']=nvals
    for i in range(0,nvals):
        payload['par'+str(1+i)]=idata[1+i]
    folder.storeObject(cool.ValidityKeyMin,cool.ValidityKeyMax,payload,chan,tag)
    chan+=1
print "All done"
db.closeDatabase()
