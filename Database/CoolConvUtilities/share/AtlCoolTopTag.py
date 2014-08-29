#!/usr/bin/env python
# AtlCoolTopTag.py - list the top level tags in a COOL database

import sys
from PyCool import cool
from CoolConvUtilities.AtlCoolLib import transConn

def listTopTag(dbid):
    "List the top-level tags on the given COOL database connection"
    dbSvc=cool.DatabaseSvcFactory.databaseService()
    try:
        db=dbSvc.openDatabase(transConn(dbid))
    except Exception,e:
        print e
        print 'Could not connect to',conn
        return False
    # extract list of tags in top folder
    rfolder=db.getFolderSet('/')
    taglist=rfolder.listTags()
    # initialise dictionary of folder counts per tag
    tagdef={}
    for tag in taglist:
        tagdef[tag]=0
    # extract list of folders, process all first-level folders
    folderlist=db.listAllNodes()
    ntopfolder=0
    for foldername in folderlist:
        if (foldername!='/' and foldername.find('/',1)==-1):
            ntopfolder+=1
            if (db.existsFolder(foldername)):
                lfolder=db.getFolder(foldername)
            else:
                lfolder=db.getFolderSet(foldername)
            # for top-level folder, check all top-level tags to see if they
            # define something in this folder
            for tag in taglist:
                try:
                    rtag=lfolder.resolveTag(tag)
                    tagdef[tag]+=1
                except Exception:
                    pass
    # having gathered all information, print it
    for tag in taglist:
        locked=rfolder.tagLockStatus(tag)
        if (locked==cool.HvsTagLock.UNLOCKED):
            lstat='unlocked'
        elif (locked==cool.HvsTagLock.PARTIALLYLOCKED):
            lstat='partially locked'
        else:
            lstat='locked'
        desc=rfolder.tagDescription(tag)
        print "%s (%i/%i,%s) [%s]" % (tag,tagdef[tag],ntopfolder,lstat,desc)
    return True

if __name__=="__main__":
    if len(sys.argv)!=2:
        print "Usage",sys.argv[0],"<dbconnection>"
        print "Lists the top-level tags on the given COOL database, together with the"
        print "corresponding number of first-level folders which have a tag connected to this top-level tag"
        sys.exit(-1)
    listTopTag(sys.argv[1])
    
