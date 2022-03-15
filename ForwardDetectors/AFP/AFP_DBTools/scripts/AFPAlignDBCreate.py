#!/bin/env python

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# file   AFPAlignDBCreate.py
# author Petr Balek <petr.balek@cern.ch>
# date   2021-02-27
#
# brief  A python script that generates COOL database with alignment constants for AFP. A lot of inspiration obtained in InnerDetector/InDetConditions/InDetBeamSpotService/utils/beamSpot_set.py
# usage  0. setup Athena environment (setupATLAS, asetup ...)
#        1. run this script:
#              python AFPAlignDBCreate.py
#        2. check the output:
#              AtlCoolConsole.py "sqlite://;schema=Example.db;dbname=CONDBR2"
#        3. check "testAFPDB.py" for more testing


from __future__ import print_function

import sys, json, copy
from CoolConvUtilities import AtlCoolLib
from PyCool import cool

# AFPDBBase is no longer needed, partially replaced by AtlCoolLib
#from AFP_DBTools.AFPDBBase import AFPDBRecordBase, AFPDBTableBase

class folderBulk():
    """A class to store database folder, its specification, and its tag at the same place, so they won't be mixed up"""
    def __init__ (self, folder,folderName,spec,tag):
        self.folder=folder
        self.folderName=folderName
        self.spec=spec
        self.tag=tag

def runLBtoDB(run,lb):
    """Turns run number and lumibock into integrer needed by the database"""
    return (run<<32) + lb

def makeFolderAndSpec(db,folderName,tag):
    """A function that will ensure a folder is in the database, it creates it if necessary. Also defines content of the folders. There are two folders: one for local constants and one for global constants. They both contain single entry of a long string"""
    
    spec=cool.RecordSpecification()

    # each folder contain just a single entry, this string will be JSON entry
    spec.extend("data",cool.StorageType.String64k)

    runLumi=True

    # if there are more channels, use "CondAttrListCollection"; if there is only one channel, use "AthenaAttributeList"
    # it's also possible to use "CondAttrListCollection" if there's only one channel
    folder=AtlCoolLib.ensureFolder(db,folderName,spec,AtlCoolLib.athenaDesc(runLumi,'CondAttrListCollection'),cool.FolderVersioning.MULTI_VERSION)

    if(folder is None):
        sys.exit(1)
    
    return folderBulk(folder, folderName, spec, tag)

class AFPDBDict():
    """A class to create a dictionary, fill it with zeros in the constructor and to overwrite zeros later"""
    def __init__ (self, folderBlk):
        if(folderBlk.folderName=="/FWD/Onl/AFP/Align/Local"):
            emptydict={"stationID":0, "layerID":-1, "shiftX":0.0, "shiftY":0.0, "shiftZ":0.0, "alpha":0.0, "beta":0.0, "gamma":0.0}
        elif(folderBlk.folderName=="/FWD/Onl/AFP/Align/Global"):
            emptydict={"stationID":0, "alignType":"None", "shiftX":0.0, "shiftY":0.0, "shiftZ":0.0, "alpha":0.0, "beta":0.0, "gamma":0.0}
        else:
            print ("unknown folder %s, please edit \"AFPDBDict\" class constructor, exiting now" % folderBlk.folderName)
            sys.exit(1)
        
        self.mydict={"author":"Petr Balek", 
                     "version":"AFP_DB_v2",
                     "nchannels":16,
                     "data": dict.fromkeys(range(0, 16))}
        
        for i in range(0, 16):
            self.mydict["data"][i]=copy.deepcopy(emptydict)
            self.mydict["data"][i]["stationID"]=i//4
            if(folderBlk.folderName=="/FWD/Onl/AFP/Align/Local"):
                self.mydict["data"][i]["layerID"]=i%4
            if(folderBlk.folderName=="/FWD/Onl/AFP/Align/Global"):
                if i%4==0: self.mydict["data"][i]["alignType"]="tracker"
                if i%4==1: self.mydict["data"][i]["alignType"]="beam"
                if i%4==2: self.mydict["data"][i]["alignType"]="RP"
                if i%4==3: self.mydict["data"][i]["alignType"]="correction"
    
    def append(self, stationID, layerID=-1, alignType="None", shiftX=0.0, shiftY=0.0, shiftZ=0.0, alpha=0.0, beta=0.0, gamma=0.0):
        """A function that overwrites one slice of the alignment constants in the dictionary. Local constants have to have layerID defined, while alignType is undefined. Global constants have to have alignType defined (tracker/RP/beam/correction) and layerID undefined."""

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

        channel=0
        # station 0 occupies channels 0-3, station 1 occupies channels 4-7, ..., and station 3 occupies channels 12-15
        # if you are insterested in e.g. "beam" constants, look at channels 1, 5, 9, and 13
        if(alignType!="None"):
            channel=stationID*4
            if(alignType=="tracker"):
                channel+=0
            elif(alignType=="beam"):
                channel+=1
            elif(alignType=="RP"):
                channel+=2
            elif(alignType=="correction"):
                channel+=3
            else:
                print ("cannot save payload, got alignType=%s, unknown" % alignType)
                sys.exit(1)
            
            # overwrite it
            self.mydict["data"][channel]['alignType']=alignType

        # station 0 again occupies channels 0-3 etc.
        if(layerID!=-1):
            channel=stationID*4
            if(0<=layerID and layerID<4):
                channel+=layerID
            else:
                print ("cannot save payload, got layerID=%d, unknown" % layerID)
                sys.exit(1)

            # overwrite it
            self.mydict["data"][channel]['layerID']=layerID

        # overwrite it
        mydict_helper=self.mydict["data"][channel]
        mydict_helper['shiftX'], mydict_helper['shiftY'], mydict_helper['shiftZ'] = shiftX, shiftY, shiftZ
        mydict_helper['alpha'], mydict_helper['beta'], mydict_helper['gamma'] = alpha, beta, gamma    
        mydict_helper['stationID']=stationID
        

    def savePayload(self, folderBlk, fromRun=-2, fromLB=0):
        """A function to transform the dictionary to JSON and save it in IOV from a given run and LB; upper limits are undefined and the maximum value (until beginning of a next entry) is used."""
        
        # perform some simple check so basic mistakes are avoided
        if(fromRun<=0):
            print ("cannot save payload, got fromRun=%d, it has to be positive" % fromRun)
            sys.exit(1)

        # transform run nr. and LB to the right integers
        since=runLBtoDB(run=fromRun,lb=fromLB)
        until=cool.ValidityKeyMax

        # provided set of constants have to be the right one for the provided specification
        payload=cool.Record(folderBlk.spec)
        # transform dictionary to JSON
        payload["data"]=json.dumps(self.mydict, indent = 1)
        # save everything (that "0" stands for channel nr. 0, there is only 1 real channel in the DB)
        folderBlk.folder.storeObject(since,until,payload,0,folderBlk.tag)


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
    folderBlkLoc_01=makeFolderAndSpec(db,folderName="/FWD/Onl/AFP/Align/Local", tag="AFPAlignLoc-01")
    folderBlkLoc_02=makeFolderAndSpec(db,folderName="/FWD/Onl/AFP/Align/Local", tag="AFPAlignLoc-02")
    folderBlkGlob_01=makeFolderAndSpec(db,folderName="/FWD/Onl/AFP/Align/Global", tag="AFPAlignGlob-01")
    
    # copy everything from the xml file here
    # https://gitlab.cern.ch/afp/AfpAnalysisToolbox/-/blob/master/AfpAnalysisTools/share/align.xml, commit 3a62232e1e3584704ccbb612f5fdf62821dd6fed from October 10, 2020
    # "toRun" is not used but it's kept here for better clarity 

    # save an empty entry, just to have the same ranges in local and global alignments
    # fromRun, toRun = 203302, 203302
    fromRun = 203302
    myDict = AFPDBDict(folderBlk=folderBlkLoc_01)
    myDict.savePayload(folderBlk=folderBlkLoc_01, fromRun=fromRun)

    # fromRun, toRun = 329484, 348002
    fromRun = 329484
    myDict = AFPDBDict(folderBlkLoc_01)
    myDict.append(stationID=0, layerID=1, shiftX=17.1871e-3,  shiftY=-31.4828e-3, shiftZ=0.0000, alpha=2.8832e-3,beta=0.0000, gamma=0.0000)
    myDict.append(stationID=0, layerID=2, shiftX=15.2353e-3,  shiftY=-51.4641e-3, shiftZ=0.0000, alpha=3.1571e-3,beta=0.0000, gamma=0.0000)
    myDict.append(stationID=0, layerID=3, shiftX=0.0000,      shiftY=0.0000,      shiftZ=0.0000, alpha=0.0000,   beta=0.0000, gamma=0.0000)
    myDict.append(stationID=1, layerID=1, shiftX=137.0504e-3, shiftY=5.5895e-3,  shiftZ=0.0000, alpha=-1.5424e-3,beta=0.0000, gamma=0.0000)
    myDict.append(stationID=1, layerID=2, shiftX=-81.5657e-3, shiftY=21.9504e-3, shiftZ=0.0000, alpha=-3.4834e-3,beta=0.0000, gamma=0.0000)
    myDict.append(stationID=1, layerID=3, shiftX=-52.5330e-3, shiftY=-31.8738e-3, shiftZ=0.0000, alpha=0.9359e-3, beta=0.0000, gamma=0.0000)
    myDict.append(stationID=2, layerID=1, shiftX=148.6858e-3, shiftY=104.3183e-3, shiftZ=0.0000, alpha=-3.3322e-3,beta=0.0000, gamma=0.0000)
    myDict.append(stationID=2, layerID=2, shiftX=150.8107e-3, shiftY=82.0611e-3, shiftZ=0.0000, alpha=4.2123e-3, beta=0.0000, gamma=0.0000)
    myDict.append(stationID=2, layerID=3, shiftX=116.9848e-3, shiftY=113.5771e-3, shiftZ=0.0000, alpha=29.2103e-3,beta=0.0000, gamma=0.0000)
    myDict.append(stationID=3, layerID=1, shiftX=-184.2146e-3,shiftY=55.3273e-3, shiftZ=0.0000, alpha=-0.7655e-3,beta=0.0000, gamma=0.0000)
    myDict.append(stationID=3, layerID=2, shiftX=-154.2151e-3,shiftY=133.8304e-3, shiftZ=0.0000, alpha=-3.2126e-3,beta=0.0000, gamma=0.0000)
    myDict.append(stationID=3, layerID=3, shiftX=13.2748e-3,  shiftY=138.4570e-3, shiftZ=0.0000, alpha=0.5984e-3, beta=0.0000, gamma=0.0000)
    myDict.savePayload(folderBlk=folderBlkLoc_01, fromRun=fromRun)

    # update July 30, 2021
    fromRun = 203302
    myDict = AFPDBDict(folderBlk=folderBlkLoc_02)
    myDict.savePayload(folderBlk=folderBlkLoc_02, fromRun=fromRun)

    fromRun = 329484
    myDict = AFPDBDict(folderBlkLoc_02)
    myDict.append(stationID=0, layerID=1, shiftX=17.1313e-3,  shiftY=-46.7438e-3, shiftZ=0.0000, alpha=2.9785e-3,  beta=0.0000, gamma=0.0000)
    myDict.append(stationID=0, layerID=2, shiftX=15.7960e-3,  shiftY=-53.7707e-3, shiftZ=0.0000, alpha=3.3048e-3,  beta=0.0000, gamma=0.0000)
    myDict.append(stationID=0, layerID=3, shiftX=0.0000,      shiftY=0.0000,      shiftZ=0.0000, alpha=0.0000,     beta=0.0000, gamma=0.0000)
    myDict.append(stationID=1, layerID=1, shiftX=138.0070e-3, shiftY=6.0528e-3,   shiftZ=0.0000, alpha=-1.4420e-3, beta=0.0000, gamma=0.0000)
    myDict.append(stationID=1, layerID=2, shiftX=-80.6720e-3, shiftY=24.1310e-3,  shiftZ=0.0000, alpha=-3.6605e-3, beta=0.0000, gamma=0.0000)
    myDict.append(stationID=1, layerID=3, shiftX=-51.5111e-3, shiftY=-32.9151e-3, shiftZ=0.0000, alpha=1.0762e-3,  beta=0.0000, gamma=0.0000)
    myDict.append(stationID=2, layerID=1, shiftX=149.6927e-3, shiftY=103.5674e-3, shiftZ=0.0000, alpha=-3.9565e-3, beta=0.0000, gamma=0.0000)
    myDict.append(stationID=2, layerID=2, shiftX=144.1316e-3, shiftY=88.0891e-3,  shiftZ=0.0000, alpha=3.3219e-3,  beta=0.0000, gamma=0.0000)
    myDict.append(stationID=2, layerID=3, shiftX=47.8090e-3,  shiftY=153.5737e-3, shiftZ=0.0000, alpha=5.1961e-3,  beta=0.0000, gamma=0.0000)
    myDict.append(stationID=3, layerID=1, shiftX=0.0000,      shiftY=0.0000,      shiftZ=0.0000, alpha=0.0000,     beta=0.0000, gamma=0.0000)
    myDict.append(stationID=3, layerID=2, shiftX=-153.0397e-3,shiftY=132.8483e-3, shiftZ=0.0000, alpha=-3.9787e-3, beta=0.0000, gamma=0.0000)
    myDict.append(stationID=3, layerID=3, shiftX=13.9500e-3,  shiftY=136.9500e-3, shiftZ=0.0000, alpha=0.3829e-3,  beta=0.0000, gamma=0.0000)
    myDict.savePayload(folderBlk=folderBlkLoc_02, fromRun=fromRun)


    # fromRun, toRun = 203302, 203302
    fromRun = 203302
    myDict = AFPDBDict(folderBlk=folderBlkGlob_01)
    myDict.append(stationID=0, alignType="RP", shiftX=-2.34, shiftY=-13.22)
    myDict.append(stationID=1, alignType="RP", shiftX=-3.68, shiftY=-13.28)
    myDict.append(stationID=2, alignType="RP", shiftX=-3.61, shiftY=-10.09)
    myDict.append(stationID=3, alignType="RP", shiftX=-2.39, shiftY=-10.53)
    myDict.savePayload(folderBlk=folderBlkGlob_01, fromRun=fromRun)
  
    # fromRun, toRun = 329484, 330470
    fromRun = 329484
    myDict = AFPDBDict(folderBlk=folderBlkGlob_01)
    myDict.append(stationID=0, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=1, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=2, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=3, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=0, alignType="beam", shiftX=-1.045)
    myDict.append(stationID=1, alignType="beam", shiftX=-0.864)
    myDict.append(stationID=2, alignType="beam", shiftX=-1.155)
    myDict.append(stationID=3, alignType="beam", shiftX=-0.891)
    myDict.append(stationID=0, alignType="RP", shiftX=-3.16)
    myDict.append(stationID=1, alignType="RP", shiftX=-4.07)
    myDict.append(stationID=2, alignType="RP", shiftX=-4.26)
    myDict.append(stationID=3, alignType="RP", shiftX=-2.93)
    myDict.append(stationID=0, alignType="correction", shiftX=-0.420)
    myDict.append(stationID=1, alignType="correction", shiftX=-0.320)
    myDict.append(stationID=2, alignType="correction", shiftX=-0.220)
    myDict.append(stationID=3, alignType="correction", shiftX=-0.320)
    myDict.savePayload(folderBlk=folderBlkGlob_01, fromRun=fromRun)

    # fromRun, toRun = 331020, 335302
    fromRun = 331020
    myDict = AFPDBDict(folderBlk=folderBlkGlob_01)
    myDict.append(stationID=0, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=1, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=2, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=3, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=0, alignType="beam", shiftX=-1.045)
    myDict.append(stationID=1, alignType="beam", shiftX=-0.864)
    myDict.append(stationID=2, alignType="beam", shiftX=-1.155)
    myDict.append(stationID=3, alignType="beam", shiftX=-0.891)
    myDict.append(stationID=0, alignType="RP", shiftX=-2.65)
    myDict.append(stationID=1, alignType="RP", shiftX=-3.57)
    myDict.append(stationID=2, alignType="RP", shiftX=-3.76)
    myDict.append(stationID=3, alignType="RP", shiftX=-2.43)
    myDict.append(stationID=0, alignType="correction", shiftX=-0.420)
    myDict.append(stationID=1, alignType="correction", shiftX=-0.320)
    myDict.append(stationID=2, alignType="correction", shiftX=-0.220)
    myDict.append(stationID=3, alignType="correction", shiftX=-0.320)
    myDict.savePayload(folderBlk=folderBlkGlob_01, fromRun=fromRun)

    # fromRun, toRun = 336288, 341692
    fromRun = 336288
    myDict = AFPDBDict(folderBlk=folderBlkGlob_01)
    myDict.append(stationID=0, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=1, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=2, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=3, alignType="tracker", shiftX=-0.5)
    myDict.append(stationID=0, alignType="beam", shiftX=-1.045)
    myDict.append(stationID=1, alignType="beam", shiftX=-0.864)
    myDict.append(stationID=2, alignType="beam", shiftX=-1.155)
    myDict.append(stationID=3, alignType="beam", shiftX=-0.891)
    myDict.append(stationID=0, alignType="RP", shiftX=-2.38)
    myDict.append(stationID=1, alignType="RP", shiftX=-3.60)
    myDict.append(stationID=2, alignType="RP", shiftX=-3.87)
    myDict.append(stationID=3, alignType="RP", shiftX=-2.23)
    myDict.append(stationID=0, alignType="correction", shiftX=-0.420)
    myDict.append(stationID=1, alignType="correction", shiftX=-0.320)
    myDict.append(stationID=2, alignType="correction", shiftX=-0.220)
    myDict.append(stationID=3, alignType="correction", shiftX=-0.320)
    myDict.savePayload(folderBlk=folderBlkGlob_01, fromRun=fromRun)

    # fromRun, toRun = 347955, 348002
    fromRun = 347955
    myDict = AFPDBDict(folderBlk=folderBlkGlob_01)
    myDict.append(stationID=0, alignType="RP", shiftX=-2.34, shiftY=-13.22)
    myDict.append(stationID=1, alignType="RP", shiftX=-3.68, shiftY=-13.28)
    myDict.append(stationID=2, alignType="RP", shiftX=-6.46, shiftY=-5.47)
    myDict.append(stationID=3, alignType="RP", shiftX=-6.79, shiftY=-5.03)
    myDict.savePayload(folderBlk=folderBlkGlob_01, fromRun=fromRun)

    print ("\nClose database")
    db.closeDatabase()


if __name__=="__main__":
    main()
