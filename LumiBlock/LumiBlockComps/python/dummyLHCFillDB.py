# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from PyCool import cool
import ROOT
import os



def createSqlite(sqliteName,folderName="/TDAQ/OLC/LHC/FILLPARAMS"):

    dbSvc = cool.DatabaseSvcFactory.databaseService()

    if os.access(sqliteName,os.R_OK):
        print ("UPDATING existing sqlite file %s" % sqliteName)
        db=dbSvc.openDatabase("sqlite://;schema="+sqliteName+";dbname=CONDBR2",False)
    else:
        print("Creating new sqlite file %s" % sqliteName)
        db=dbSvc.createDatabase("sqlite://;schema="+sqliteName+";dbname=CONDBR2")
        pass
       
    spec = cool.RecordSpecification()
    spec.extend("Beam1Bunches",cool.StorageType.UInt32)
    spec.extend("Beam2Bunches",cool.StorageType.UInt32)
    spec.extend("LuminousBunches",cool.StorageType.UInt32)
    spec.extend("BCIDmasks",cool.StorageType.Blob64k)
    descr='<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'

    if db.existsFolder(folderName):
        folder=db.getFolder(folderName)
    else:
        print("Creating COOL folder/tag  %s" % (folderName))
        #folder = db.createFolder(folderName, spec, desc, cool.FolderVersioning.MULTI_VERSION, True)
        folderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, spec)
        folder = db.createFolder(folderName, folderSpec, descr, True)
        pass
      
    return db,folder

def fillFolder(folder,data=[],iovMin=cool.ValidityKeyMin,iovMax=cool.ValidityKeyMax):
    
    nB1=0
    nB2=0
    nColl=0

    for bcid in data:
        if (bcid & 0x1): nB1+=1
        if (bcid & 0x2): nB2+=1
        if (bcid & 0x3 == 0x3): nColl+=1
        pass

    
    payload=cool.Record(folder.payloadSpecification())

    payload['Beam1Bunches']=nB1
    payload['Beam2Bunches']=nB2
    payload['LuminousBunches']=nColl

    btype=getattr(ROOT,"coral::Blob")
    
    bcmask=btype()
    bcmask.resize(3564)

    for i,d in enumerate(data):
        bcmask[i]=d


    payload['BCIDmasks']=bcmask
            

    print("Storing CaloCondBlob object")
    folder.storeObject(iovMin, iovMax, payload, cool.ChannelId(0))

    return
   


def createBCMask1():
    mask=[]
    #pre-fill with zero
    for i in range(0,3564):
        mask.append(0x0)    

    #A train across the wrap-around:
    for i in range (0,25):
        mask[i]=0x3
    
    for i in range (3550,3564):
        mask[i]=0x3

    #A short sequence of bunches that doesn't qualify as train
    for i in range (1000,1030):

        mask[i]=0x3
    return mask
         

def createBCMask2():
    mask=[]
    #pre-fill with zero
    for i in range(0,3564):
        mask.append(0x0)    

    t8b4e=[0x3,0x3,0x3,0x3, 0x3,0x3,0x3,0x3, 0x0,0x0,0x0,0x0]
    
    for i in range(0,20):
        #create a train of 20 8be4 groups start
        mask[100+i*12:100+(i+1)*12]=t8b4e

    return mask




if __name__=="__main__":


    db,folder=createSqlite("test.db")
    
    d1=createBCMask1()
    d2=createBCMask2()
    
    fillFolder(folder,d1,iovMin=cool.ValidityKey(1e6),iovMax=cool.ValidityKey(2e6))
    fillFolder(folder,d2,iovMin=cool.ValidityKey(2e6),iovMax=cool.ValidityKey(3e6))


    db.closeDatabase()
    
   
