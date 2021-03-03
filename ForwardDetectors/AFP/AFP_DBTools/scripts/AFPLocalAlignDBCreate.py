#!/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# file   AFPLocalAlign.py
# author Petr Balek <petr.balek@cern.ch>
# date   2021-02-27
#
# brief  A python script that generates COOL database with alignment constants for AFP. A lot of inspiration obtained in InnerDetector/InDetConditions/InDetBeamSpotService/utils/beamSpot_set.py

from __future__ import print_function

import sys
from CoolConvUtilities import AtlCoolLib
from PyCool import cool

# AFPDBBase is no longer needed, partially replaced by AtlCoolLib
#from AFP_DBTools.AFPDBBase import AFPDBRecordBase, AFPDBTableBase

class folderBulk():
    """A class to store database folder, its specification, and its tag at the same place, so they won't be mixed up"""
    def __init__ (self, folder,spec,tag):
        self.folder=folder
        self.spec=spec
        self.tag=tag

def runLBtoDB(run,lb):
    """Turns run number and lumibock into integrer needed by the database"""
    return (run<<32) + lb

def makeFolderAndSpec(db,folderName,tag):
    """A function that will ensure a folder is in the database, it creates it if necessary. Also defines content of the folders. There are two folders: one for local constants and one for global constants."""
    
    spec=cool.RecordSpecification()

    # folder's content partially depend whether we have local or global constants
    if(folderName=="/FWD/AFP/LocalAlignment"):
        spec.extend("layerID",cool.StorageType.Int32)
    elif(folderName=="/FWD/AFP/GlobalAlignment"):
        spec.extend("alignType",cool.StorageType.String255)
    else:
        print ("unknown folder %s, please edit \"makeFolderAndSpec\" function, exiting now" % folderName)
        sys.exit(1)

    # most of the folder's content is the same for both versions
    spec.extend("stationID",cool.StorageType.Int32)

    spec.extend("shiftX",cool.StorageType.Float)
    spec.extend("shiftY",cool.StorageType.Float)
    spec.extend("shiftZ",cool.StorageType.Float)
    spec.extend("alpha",cool.StorageType.Float)
    spec.extend("beta",cool.StorageType.Float)
    spec.extend("gamma",cool.StorageType.Float)

    runLumi=True

    folder=AtlCoolLib.ensureFolder(db,folderName,spec,AtlCoolLib.athenaDesc(runLumi,'AthenaAttributeList'),cool.FolderVersioning.MULTI_VERSION)

    if(folder is None):
        sys.exit(1)
    
    return folderBulk(folder, spec, tag)

def savePayload(folderBlk, stationID, layerID=-1, alignType="None", shiftX=0.0, shiftY=0.0, shiftZ=0.0, alpha=0.0, beta=0.0, gamma=0.0, fromRun=-2, toRun=-1, fromLB=0, toLB=4294967295):
    """A function that saves one slice of the alignment constants to the database. Local constants have to have layerID defined, while alignType is undefined. Global constants have to have alignType defined (tracker/RP/beam/correction) and layerID undefined. Saves a set of constants in IOV from a given run and LB to the given run and LB."""

    # perform some simple check so basic mistakes are avoided
    if(layerID==-1 and alignType=="None"):
        print ("cannot save payload, got layerID=%d and alignType=%s; one of them has to be specified" % layerID, alignType)
        sys.exit(1)
    elif(layerID!=-1 and alignType!="None"):
        print ("cannot save payload, got layerID=%d and alignType=%s; one of them should not be specified" % layerID, alignType)
        sys.exit(1)

    if(stationID<0 or stationID>=4):
        print ("cannot save payload, got stationID=%d, unknown" % stationID)
        sys.exit(1)

    if(fromRun<=0 or toRun<=0):
        print ("cannot save payload, got fromRun=%d and toRun=toRun, they have to be both positive" % fromRun, toRun)
        sys.exit(1)

    # provided set of constants have to be the right one for the provided specification
    payload=cool.Record(folderBlk.spec)

    # station 0 occupies channels 0-3, station 1 occupies channels 4-7, ..., and station 3 occupies channels 12-15
    # if you are insterested in e.g. "RP" constants, look at channels 1, 5, 9, and 13
    channel=0
    if(alignType!="None"):
        channel=stationID*4
        if(alignType=="tracker"):
            channel+=0
        elif(alignType=="RP"):
            channel+=1
        elif(alignType=="beam"):
            channel+=2
        elif(alignType=="correction"):
            channel+=3
        else:
            print ("cannot save payload, got alignType=%s, unknown" % alignType)
            sys.exit(1)
        
        payload['alignType']=alignType
    
    # station 0 again occupies channels 0-3 etc.
    if(layerID!=-1):
        channel=stationID*4
        if(0<=layerID and layerID<4):
            channel+=layerID
        else:
            print ("cannot save payload, got layerID=%d, unknown" % layerID)
            sys.exit(1)

        payload['layerID']=layerID

    # save the payload
    payload['shiftX'], payload['shiftY'], payload['shiftZ'] = shiftX, shiftY, shiftZ
    payload['alpha'], payload['beta'], payload['gamma'] = alpha, beta, gamma    
    payload['stationID']=stationID

    # transform run nr. and LB to the right integers
    since=runLBtoDB(run=fromRun,lb=fromLB)
    until=runLBtoDB(run=toRun,lb=toLB)

    # save constants
    folderBlk.folder.storeObject(since,until,payload,channel,folderBlk.tag)

def main():
    dbFile = "Example.db"
    dbName = "CONDBR2" 
    
    # remove the old db file so that we can write the new one
    try:
        import os
        os.remove(dbFile)
    except IOError:
        pass

    # get database service and open database
    dbSvc = cool.DatabaseSvcFactory.databaseService()

    # database accessed via physical name
    dbString = "sqlite://;schema=%s;dbname=%s" % (dbFile, dbName)
    try:
        db = dbSvc.createDatabase(dbString)
    except Exception as e:
        print ('Problem creating database', e)
        sys.exit(-1)
    print ("Created database", dbString)
    
    # define the folder in the database, its specification and its tag
    folderBlkLoc=makeFolderAndSpec(db,folderName="/FWD/AFP/LocalAlignment", tag="AFPAlignLoc-00")
    folderBlkGlob=makeFolderAndSpec(db,folderName="/FWD/AFP/GlobalAlignment", tag="AFPAlignGlob-00")
    
    # copy everything from the xml file here
    # https://gitlab.cern.ch/afp/AfpAnalysisToolbox/-/blob/master/AfpAnalysisTools/share/align.xml, commit 3a62232e1e3584704ccbb612f5fdf62821dd6fed from October 10, 2020
    fromRun, toRun = 329484, 348002
    savePayload(folderBlk=folderBlkLoc, stationID=0, layerID=1, shiftX=17.1871e-3,  shiftY=-31.4828e-3, shiftZ=0.0000, alpha=2.8832e-3,beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=0, layerID=2, shiftX=15.2353e-3,  shiftY=-51.4641e-3, shiftZ=0.0000, alpha=3.1571e-3,beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=0, layerID=3, shiftX=0.0000,      shiftY=0.0000,      shiftZ=0.0000, alpha=0.0000,   beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=1, layerID=1, shiftX=137.0504e-3, shiftY=5.5895e-3,  shiftZ=0.0000, alpha=-1.5424e-3,beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=1, layerID=2, shiftX=-81.5657e-3, shiftY=21.9504e-3, shiftZ=0.0000, alpha=-3.4834e-3,beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=1, layerID=3, shiftX=-52.5330e-3, shiftY=-31.8738e-3, shiftZ=0.0000, alpha=0.9359e-3, beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=2, layerID=1, shiftX=148.6858e-3, shiftY=104.3183e-3, shiftZ=0.0000, alpha=-3.3322e-3,beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=2, layerID=2, shiftX=150.8107e-3, shiftY=82.0611e-3, shiftZ=0.0000, alpha=4.2123e-3, beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=2, layerID=3, shiftX=116.9848e-3, shiftY=113.5771e-3, shiftZ=0.0000, alpha=29.2103e-3,beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=3, layerID=1, shiftX=-184.2146e-3,shiftY=55.3273e-3, shiftZ=0.0000, alpha=-0.7655e-3,beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=3, layerID=2, shiftX=-154.2151e-3,shiftY=133.8304e-3, shiftZ=0.0000, alpha=-3.2126e-3,beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkLoc, stationID=3, layerID=3, shiftX=13.2748e-3,  shiftY=138.4570e-3, shiftZ=0.0000, alpha=0.5984e-3, beta=0.0000, gamma=0.0000, fromRun=fromRun, toRun=toRun)

    fromRun, toRun = 203302, 203302
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="RP", shiftX=-2.34, shiftY=-13.22, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="RP", shiftX=-3.68, shiftY=-13.28, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="RP", shiftX=-3.61, shiftY=-10.09, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="RP", shiftX=-2.39, shiftY=-10.53, fromRun=fromRun, toRun=toRun)

    fromRun, toRun = 347955, 348002
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="RP", shiftX=-2.34, shiftY=-13.22, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="RP", shiftX=-3.68, shiftY=-13.28, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="RP", shiftX=-6.46, shiftY=-5.47,  fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="RP", shiftX=-6.79, shiftY=-5.03,  fromRun=fromRun, toRun=toRun)
  
    fromRun, toRun = 329484, 330470
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="beam", shiftX=-1.045, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="beam", shiftX=-0.864, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="beam", shiftX=-1.155, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="beam", shiftX=-0.891, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="RP", shiftX=-3.16, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="RP", shiftX=-4.07, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="RP", shiftX=-4.26, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="RP", shiftX=-2.93, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="correction", shiftX=-0.420, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="correction", shiftX=-0.320, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="correction", shiftX=-0.220, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="correction", shiftX=-0.320, fromRun=fromRun, toRun=toRun)

    fromRun, toRun = 331020, 335302
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="beam", shiftX=-1.045, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="beam", shiftX=-0.864, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="beam", shiftX=-1.155, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="beam", shiftX=-0.891, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="RP", shiftX=-2.65, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="RP", shiftX=-3.57, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="RP", shiftX=-3.76, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="RP", shiftX=-2.43, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="correction", shiftX=-0.420, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="correction", shiftX=-0.320, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="correction", shiftX=-0.220, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="correction", shiftX=-0.320, fromRun=fromRun, toRun=toRun)

    fromRun, toRun = 336288, 341692
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="tracker", shiftX=-0.5, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="beam", shiftX=-1.045, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="beam", shiftX=-0.864, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="beam", shiftX=-1.155, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="beam", shiftX=-0.891, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="RP", shiftX=-2.38, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="RP", shiftX=-3.60, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="RP", shiftX=-3.87, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="RP", shiftX=-2.23, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=0, alignType="correction", shiftX=-0.420, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=1, alignType="correction", shiftX=-0.320, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=2, alignType="correction", shiftX=-0.220, fromRun=fromRun, toRun=toRun)
    savePayload(folderBlk=folderBlkGlob, stationID=3, alignType="correction", shiftX=-0.320, fromRun=fromRun, toRun=toRun)

    print ("\nClose database")
    db.closeDatabase()


if __name__=="__main__":
    main()
