#!/bin/env python

# Taken from InnerDetector/InDetRecTools/TRT_ElectronPidTools/DatabaseTools/WritePyCoolAll.py
# https://twiki.cern.ch/twiki/bin/view/Atlas/ConditionsSimpleExample

import sys
from PyCool import cool

def main():
    dbFile = "LinkMasking.db"
    dbName = "CONDBR2" 
    folderName ="/purple/pants"
    
    fieldNames = ["lastProbedState"]
    fieldTypes = [cool.StorageType.Bool]

    # remove the old db file so that we can write the new one
    try:
        import os
        os.remove(dbFile)
    except:
        pass

    # get database service and open database
    dbSvc = cool.DatabaseSvcFactory.databaseService()

    # database accessed via physical name
    dbString = "sqlite://;schema=%s;dbname=%s" % (dbFile, dbName)
    try:
        db = dbSvc.createDatabase(dbString)
    except Exception, e:
        print 'Problem creating database', e
        sys.exit(-1)
    print "Created database", dbString
    
    # setup folder 
    spec = cool.RecordSpecification()
    spec.extend(fieldNames[0], fieldTypes[0])

    # folder meta-data - note for Athena this has a special meaning
    desc = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'
    
    # create the folder - single version
    # last argument is createParents - if true, automatically creates parent folders if needed
    # note this will not work if the database already exists - delete mycool.db first
    folderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, spec)
    folder = db.createFolder(folderName, folderSpec, desc, True)
        
    # now fill in some data - create a record and fill it
    data = cool.Record(spec)

    dataDict = {167772160:False, # barrel_ec 0 layer_disk 0 eta_module -6 phi_module 0 side 0
                167773184:True,  # barrel_ec 0 layer_disk 0 eta_module -6 phi_module 0 side 1
                167774208:True,  # barrel_ec 0 layer_disk 0 eta_module -5 phi_module 0 side 0
                167775232:True,  # barrel_ec 0 layer_disk 0 eta_module -5 phi_module 0 side 1
                167776256:False, # barrel_ec 0 layer_disk 0 eta_module -4 phi_module 0 side 0
                }
    
    for waferID, lastProbedState in dataDict.items():
        print "\nChannel", waferID
        print "lastProbedState is", lastProbedState
        data[fieldNames[0]] = lastProbedState
        print "Will store this object for channel", waferID, data
        folder.storeObject(0, cool.ValidityKeyMax, data, waferID)

    print "\nClose database"
    db.closeDatabase()

if __name__=="__main__":
    main()
