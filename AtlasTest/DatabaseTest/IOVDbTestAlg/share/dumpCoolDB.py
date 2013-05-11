#!/bin/env python
import sys
from PyCool import cool,coral

def dumpFolder(folder, tag):
    nobjs = folder.countObjects( cool.ValidityKeyMin,
                            cool.ValidityKeyMax,
                            cool.ChannelSelection.all(),
                            tag)

    print "number of objects", nobjs

    objs = folder.browseObjects( cool.ValidityKeyMin,
                            cool.ValidityKeyMax,
                            cool.ChannelSelection.all(),
                            tag)
    i = 0
    while objs.goToNext():
        obj = objs.currentRef()
        print "Found object", i,
        print "since [r,l]: [", obj.since() >> 32,',',obj.since()%0x100000000,']',
        print "until [r,l]: [", obj.until() >> 32,',',obj.until()%0x100000000,']',
        print "payload", obj.payload(),
        print "chan",obj.channelId() 
        i += 1
        
    objs.close()



if __name__ == "__main__":

    # get database service and open database
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    # database accessed via physical name
    dbstring="sqlite://;schema=mytest.db;dbname=TESTCOOL"
    # dbstring="sqlite://;schema=SimParams.db;dbname=SIMPARAM"
    # dbstring="sqlite://;schema=mytest.db;dbname=TESTCOOL"
    try:
        db = dbSvc.openDatabase(dbstring,False)
    except Exception,e:
        print 'Problem opening database',e
        sys.exit(-1)

    print "Opened database",dbstring

    # Loop over folders
    folders = db.listAllNodes()
    for ff in folders:
        # Get Folder
        try:
            f = db.getFolder(ff)
            print "Analysing Folder " + str(ff)
        except:
            print "Skipping " + str(ff)
            continue

        # dump empty tag
        print "for untagged folder ",
        dumpFolder(f, "")


        # get tags
        tags  = f.listTags()
        print "for tags ",
        for tag in tags: print tag

        for tag in tags:
            dumpFolder(f, tag)

    # finish
    db.closeDatabase()


