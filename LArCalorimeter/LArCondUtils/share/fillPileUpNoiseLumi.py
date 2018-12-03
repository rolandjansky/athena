#!/bin/env python

from PyCool import cool
from time import asctime,gmtime


def createFolder(db,name):
    desc='<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'
    spec = cool.RecordSpecification()
    spec.extend('LBAvInstLumi',cool.StorageType.Float)
    spec.extend('Valid',cool.StorageType.UInt32)
    folderSpec=cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec)
    #return db.createFolder(name,spec,desc,cool.FolderVersioning.MULTI_VERSION, True)
    return db.createFolder(name,folderSpec,desc, True)
 

def fillPileUpNoiseLumi(db,tag,dataIn, folderName="/CALO/Ofl/Noise/PileUpNoiseLumi"):

    data=0.
    data=dataIn

    if db.existsFolder(folderName):
        folder = db.getFolder(folderName)
    else:
        #create new folder
        print "Folder", folderName,"does not exit yet. Creating it now." 
        folder=createFolder(db,folderName)

    since=cool.ValidityKeyMin
    until=cool.ValidityKeyMax

    payload = cool.Record(folder.payloadSpecification())
    payload["LBAvInstLumi"]=data
    payload["Valid"]=0
    folder.storeObject(since,until,payload,cool.ChannelId(0),tag)
    folder.storeObject(since,until,payload,cool.ChannelId(1),tag)

    return 0


if __name__=='__main__':
    import os,sys,getopt

    if len(sys.argv)<2 or sys.argv[1]=="-h" or sys.argv[1]=="--help":
        print "Usage:"

        print "fillPileUpNoiseLumi.py lumi"
        sys.exit(-1)


    from LArConditionsCommon.getCurrentFolderTag import getCurrentFolderTag
    (current,next)=getCurrentFolderTag("COOLOFL_CALO/CONDBR2","/CALO/Ofl/Noise/PileUpNoiseLumi")
    if current is None:
        print "Failed to get CURRENT folder level tag!"
        sys.exit(-1)
    
    try:
     float(sys.argv[1])
    except ValueError:
     print "Argument is not a float"   

    newlumi=float(sys.argv[1])

    sqlitename="PileUpNoiseLumi.db"
    if os.access(sqlitename,os.F_OK):
            print "ERROR: File",sqlitename,"exists already. Please delete!"
            sys.exit(-1)

    print "Using folder-level tag:",current
    connect="sqlite://;schema="+sqlitename+";dbname=CONDBR2"
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    try:
        db = dbSvc.openDatabase(connect,False)
    except Exception:
        print "Database does not exist, try to create it ..."
        try:
            db=dbSvc.createDatabase(connect)
        except Exception,e:
            print e
            print "Could not create database"
            sys.exit(-1)
        pass
    pass

    data=fillPileUpNoiseLumi(db,current,newlumi)
    sys.exit(0)
