#!/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

# Taken from InnerDetector/InDetRecTools/TRT_ElectronPidTools/DatabaseTools/WritePyCoolAll.py

import sys

from AFP_DBTools.AFPDBBase import AFPDBRecordBase, AFPDBTableBase

from PyCool import cool

class AFPLocalAlignRecord(AFPDBRecordBase):
    def __init__ (self):
        self.reset()

    def reset (self):
        self.stationID=-1
        self.layerID=-1
        
        self.shiftX=0.1
        self.shiftY=0.4
        self.shiftZ=0.6

        self.alpha=0.
        self.beta=0.
        self.gamma=0.
        
class AFPLocalAlignRecordHandler:
    def __init__ (self):
        self.reset()

    def reset (self):
        self.iovSet=False
        self.anglesSet=False
        self.shiftSet=False
        self.idSet=False        # true if stationID and laeryID were set

        self.record = AFPLocalAlignRecord()
        


class AFPLocalAlignmentFiller (AFPDBTableBase):
    """A class that creates a conditions database with AFP internal alignment."""

    def __init__ (self):
        super(AFPLocalAlignmentFiller, self).__init__()

        self.folderName="/FWD/AFP/LocalAlignment/SiliconPlanes"
        self.tag="TestTag-00"

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
    
    filler = AFPLocalAlignmentFiller()
    for statID in range (0, 4):
        for layID in range (0, 4):
            rec = AFPLocalAlignRecord()
            rec.stationID = statID
            rec.layerID = layID
            fstatID = float(statID)
            flayID = float(layID)
            rec.shiftX = fstatID + (flayID/10)
            rec.shiftY = -fstatID - (flayID/10)
            rec.shiftZ = +fstatID - (flayID/10)
            rec.alpha = -fstatID + (flayID/10)

            filler.records.append(rec)
    
    print (filler.serialiseTable())

    filler.iovStartRun = 305359
    filler.iovStartLumiBlock = 0

    filler.iovEndRun = 305360
    filler.iovEndLumiBlock = 0

    filler.folderName = "/FWD/AFP/LocalAlignment/SiliconPlanes"
    filler.createFolder(db)

    filler.saveToDB()
    

    print ("\nClose database")
    db.closeDatabase()


if __name__=="__main__":
    main()
