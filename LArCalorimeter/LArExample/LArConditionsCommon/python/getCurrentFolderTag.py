#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import sys

from PyCool import cool
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias

def getCurrentFolderTag(dbname,folderName):
    current=None
    next=None

    #1. Get current and next global tags using resolver class in ~atlcond
    resolver=resolveAlias()
    currentGlobal=resolver.getCurrent().replace("*","ST")
    nextGlobal=resolver.getNext().replace("*","ST")

    #2. Open the DB to resolve this gobal tag for the given folder
    dbSvc = cool.DatabaseSvcFactory.databaseService() 
    db = dbSvc.openDatabase(dbname)
    f=db.getFolder(folderName)
    try:
        current=f.resolveTag(currentGlobal)
    except:
        pass
    if len(nextGlobal)>2:
        # NEXT exists, try to resolve it
        try:
            next=f.resolveTag(nextGlobal)
        except:
            pass
        pass
    
    db.closeDatabase()
    return (current,next)


if __name__=="__main__":
    if len(sys.argv)<3:
        sys.stderr.write("Usage: %s dbname folder\n" %  sys.argv[0].split("/")[-1])
        sys.exit(-1)

    dn=sys.argv[1]
    fn=sys.argv[2]

    currTag=getCurrentFolderTag(dn,fn)[0]
    
    if currTag is None:
        sys.stderr.write("Failed to resolve current folder-level tag for folder %s in db %s\n" % (fn,dn))
        sys.exit(-1)

    print currTag
    
