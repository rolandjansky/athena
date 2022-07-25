#!/bin/env python

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# file   AFPToFMCDBCreate.py
# author Petr Balek <petr.balek@cern.ch>
# date   2022-07-19
#
# brief  A python script that generates COOL database with ToF parameters for AFP. This version of DB is intended to be used in MC simulations. Each tag has one entry with IOV from 0 to infinite. The tags reflect various alignments through data-taking.
# usage  0. setup Athena environment (setupATLAS, asetup ...)
#        1. run this script:
#              python AFPToFMCDBCreate.py
#        2. check the output:
#              AtlCoolConsole.py "sqlite://;schema=ExampleMC.db;dbname=OFLP200"
#        3. check "testAFPDB.py" for more testing


import sys, json, copy
from CoolConvUtilities import AtlCoolLib
from PyCool import cool

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
    """A function that will ensure a folder is in the database, it creates it if necessary. Also defines content of the folders. It contains only a single entry of a long string"""
    
    spec=cool.RecordSpecification()

    # each folder contain just a single entry, this string will be JSON entry
    spec.extend("data",cool.StorageType.String64k)

    # if there are more channels, use "CondAttrListCollection"; if there is only one channel, use "AthenaAttributeList"
    # it's also possible to use "CondAttrListCollection" if there's only one channel
    folder=AtlCoolLib.ensureFolder(db,folderName,spec,AtlCoolLib.athenaDesc(True,'CondAttrListCollection'),cool.FolderVersioning.MULTI_VERSION)

    if(folder is None):
        sys.exit(1)
    
    return folderBulk(folder, folderName, spec, tag)

class AFPDBDict():
    """A class to create a dictionary, fill it with zeros in the constructor and to overwrite zeros later"""
    def __init__ (self, folderBlk):
        if(folderBlk.folderName=="/FWD/AFP/ToFParameters/Local"):
            emptydict={"stationID":0, "trainID":-1, "barID":-1, "barWeight":1.0, "barTimeOffset":0.0}
            nchannels=32
        elif(folderBlk.folderName=="/FWD/AFP/ToFParameters/Vertex"):
            emptydict={"stationID":0, "timeGlobalOffset":0.0, "timeOffset":(0.0,0.0,0.0,0.0), "timeSlope":(0.0,0.0,0.0,0.0), "trainEdge":(0.0,0.0,0.0,0.0,0.0)}
            nchannels=2
        else:
            print ("unknown folder %s, please edit \"AFPDBDict\" class constructor, exiting now" % folderBlk.folderName)
            sys.exit(1)
        
        self.mydict={"author":"Petr Balek", 
                     "version":"AFP_DB_v2",
                     "nchannels":nchannels,
                     "data": dict.fromkeys(range(0, nchannels))}
        
        for i in range(0, nchannels):
            self.mydict["data"][i]=copy.deepcopy(emptydict)
            if(folderBlk.folderName=="/FWD/AFP/ToFParameters/Local"):
                self.mydict["data"][i]["stationID"]=(i//16)*3
                self.mydict["data"][i]["trainID"]=(i%16)//4
                self.mydict["data"][i]["barID"]=i%4  
            if(folderBlk.folderName=="/FWD/AFP/ToFParameters/Vertex"):
                self.mydict["data"][i]["stationID"]=i*3
    
    def append(self, stationID, trainID=-1, barID=-1, barWeight=1.0, barTimeOffset=0.0, timeGlobalOffset=0.0, timeOffset=(0.,0.,0.,0.), timeSlope=(0.,0.,0.,0.), trainEdge=(0.,0.,0.,0.,0.)):
        """A function that overwrites one slice of the ToF parameters in the dictionary. Local ToF parameters have to have both trainID and barID defined, Vertex ToF parameters neither."""
                
        # perform some simple check so basic mistakes are avoided
        if(stationID!=0 and stationID!=3):
            print ("cannot save payload, got stationID=%d, unknown" % stationID)
            sys.exit(1)
            
        trainValid=True if 0<=trainID and trainID<4 else False
        barValid = True if 0<=barID  and  barID<4  else  False
        if(trainValid!=barValid):
            # a.k.a. "trainValid xor barValid" 
            print ("cannot save payload, got trainID=%d, barID=%d" % (trainID,barID))
            sys.exit(1)
        
        if trainValid:
            # local ToF parameters
            # station 0 occupies channels 0-16, station 3 occupies channels 17-32
            # train 0 occupies channels 0-3 / 17-20, train 1 occupies channels 4-7 / 21-24, ...
            # bar 0 occupies the first channel of the train, bar 1 occupies the second channel of the train, ...
            channel=(stationID//3)*16+trainID*4+barID

            # overwrite it
            mydict_helper=self.mydict["data"][channel]
            mydict_helper['stationID'], mydict_helper['trainID'], mydict_helper['barID'] = stationID, trainID, barID
            mydict_helper['barWeight'], mydict_helper['barTimeOffset'] = barWeight, barTimeOffset
        else:
            # vertex ToF parameters
            # station 0 occupies channel 0, station 3 occupies channels 1
            channel=stationID//3
            
            # overwrite it
            mydict_helper=self.mydict["data"][channel]
            mydict_helper['stationID']=stationID
            mydict_helper['timeGlobalOffset'], mydict_helper['timeOffset'] = timeGlobalOffset, timeOffset
            mydict_helper['timeSlope'], mydict_helper['trainEdge'] = timeSlope, trainEdge
    
    
    def savePayload(self, folderBlk):
        """A function to transform the dictionary to JSON and save it in IOV from 0 to infinity; technically, upper limits are undefined and the maximum value (until beginning of a next entry) is used."""

        # transform run nr. and LB to the right integers
        since=runLBtoDB(0,0)
        until=cool.ValidityKeyMax

        # provided set of constants have to be the right one for the provided specification
        payload=cool.Record(folderBlk.spec)
        # transform dictionary to JSON
        payload["data"]=json.dumps(self.mydict, indent = 1)
        # save everything (that "0" stands for channel nr. 0, there is only 1 real channel in the DB)
        folderBlk.folder.storeObject(since,until,payload,0,folderBlk.tag)


def main():
    dbFile = "ExampleMC.db"
    dbName = "OFLP200" 
    
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
    folderBlkLoc_ideal_01=makeFolderAndSpec(db,folderName="/FWD/AFP/ToFParameters/Local", tag="AFPMCToFLoc-ideal-01")
    folderBlkVtx_ideal_01=makeFolderAndSpec(db,folderName="/FWD/AFP/ToFParameters/Vertex", tag="AFPMCToFVtx-ideal-01")


    myDict = AFPDBDict(folderBlkLoc_ideal_01)
    myDict.append(stationID=0, trainID=0, barID=0, barWeight=1.0, barTimeOffset=1494.0)
    myDict.append(stationID=0, trainID=0, barID=1, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=0, barID=2, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=0, barID=3, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=1, barID=0, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=1, barID=1, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=1, barID=2, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=1, barID=3, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=2, barID=0, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=2, barID=1, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=2, barID=2, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=2, barID=3, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=3, barID=0, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=3, barID=1, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=3, barID=2, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=0, trainID=3, barID=3, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=0, barID=0, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=0, barID=1, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=0, barID=2, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=0, barID=3, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=1, barID=0, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=1, barID=1, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=1, barID=2, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=1, barID=3, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=2, barID=0, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=2, barID=1, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=2, barID=2, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=2, barID=3, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=3, barID=0, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=3, barID=1, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=3, barID=2, barWeight=1.0, barTimeOffset=1500.0)
    myDict.append(stationID=3, trainID=3, barID=3, barWeight=1.0, barTimeOffset=1500.0)
    myDict.savePayload(folderBlk=folderBlkLoc_ideal_01)


    myDict = AFPDBDict(folderBlk=folderBlkVtx_ideal_01)
    myDict.append(stationID=0, timeGlobalOffset=0.0, timeOffset=(64.,45.,25.,11.), timeSlope=(-7.3,-5.0,-4.0,-4.0), trainEdge=(-18.7,-13.2,-8.1,-4.0,-2.5))
    myDict.append(stationID=3, timeGlobalOffset=5.0, timeOffset=( 2., 9.,14.,12.), timeSlope=( 4.0, 5.0, 3.0, 6.0), trainEdge=(-18.7,-13.2,-8.1,-4.0,-2.5))
    myDict.savePayload(folderBlk=folderBlkVtx_ideal_01)
    
    
    print ("\nClose database")
    db.closeDatabase()


if __name__=="__main__":
    main()
