# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/bin/env python
from PyCool import cool
from string import *

def getOFCFlavor():
    onlineOFCFolder="/LAR/ElecCalibFlat/OFC"

    offlineOFCFolders=["/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples1phase","/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples1phase"]
    

    dbSvc = cool.DatabaseSvcFactory.databaseService()
    try:
        db= dbSvc.openDatabase("COOLONL_LAR/CONDBR2")
    except Exception,e:
        print "ERROR: Problems connecting to database:",e
        return None

    if db.existsFolder(onlineOFCFolder):
        try:
            pFldr=db.getFolder(onlineOFCFolder)
            currObj=pFldr.findObject(cool.ValidityKeyMax-1,0) #Check only high gain
            pl=currObj.payload()
            curr_nsamples=pl["nSamples"]
        except Exception,e:
            print "Failed to extract data from online OFC folder"
            print e
            curr_nsamples=None
            pass
    else:
        print "ERROR: Folder",onlineOFCFolder,"doesn't exist!"
        curr_nsamples=None
        pass
    
    db.closeDatabase()

    OFCOptions=[]
    currOFCOption=None

    db=dbSvc.openDatabase("COOLOFL_LAR/CONDBR2")
    #db=dbSvc.openDatabase("sqlite://;schema=freshConstants_ofl.db;dbname=CONDBR2")

    currentOFCOption=None
    for offlineOFCFolder in offlineOFCFolders:
        if db.existsFolder(offlineOFCFolder):
            try:
                pFldr=db.getFolder(offlineOFCFolder)
                ofctaglist=pFldr.listTags()
            except Exception,e:
                print "Failed to extract data from offline database"
                print e
                pass
        
            for t in ofctaglist:
                if t.find("UPD3")!=-1:
                    OFCOptions+=[(offlineOFCFolder,t),]

            if curr_nsamples is not None:
                if offlineOFCFolder.endswith("%isamples1phase" %  curr_nsamples):
                    try:
                        currentOFCOption=(offlineOFCFolder,pFldr.resolveTag("LARCALIB-RUN2-00"))
                    except:
                        print "Could not resolve global tag in folder ",offlineOFCFolder
                        pass
                    pass
                pass #end if "%isamples1phase
            pass #end if curr_nsamples is not None: 
        else: 
            print "Folder",offlineOFCFolder,"does not exist"
            pass
        pass


    print "Current number of samples:", curr_nsamples

    print "OFC flavor for production [%s]: " % currentOFCOption[1]

    if "mu" in currentOFCOption[1]:
        return 20
    else:   
        return 0

    
        


if __name__=="__main__":

    ofcForOnline=getOFCFlavor()
    if ofcForOnline is None:
        print "Failed to get OFC/Shapes for online database"
        
    else: 
        print "Mu: ",ofcForOnline
    

