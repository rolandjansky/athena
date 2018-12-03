# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PyCool import cool
import ROOT
import cppyy
import sys

# Make sure the dictionary is loaded.
ROOT.LArBadChannel


def getMissingFebs(run, tag=None):

    if tag is None:
        sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
        from AtlCoolBKLib import resolveAlias
        resolver=resolveAlias()
        tag=resolver.getCurrent().replace("*","ST")
    
    retval=[]

    dbName="COOLOFL_LAR/CONDBR2"
    fName="/LAR/BadChannelsOfl/MissingFEBs"

    dbSvc = cool.DatabaseSvcFactory.databaseService()
    try:
        db= dbSvc.openDatabase(dbName)
    except Exception,e:
        print "Problems connecting to database:",e
        return None
    

    print "getMissingFEBs: Checking tag",tag,"for run",run
    
    if not db.existsFolder(fName):
        print "ERROR: Folder",fName,"not found in ",dbName
        return None

    f=db.getFolder(fName)
    if not f.existsUserTag(tag):
        try:
            t=f.resolveTag(tag)
        except Exception,e:
            print "ERROR: Tag",tag,"not defined in folder",fName
            print e
            return None
    else:
        t=tag


    cl_LArBadChannelDBTools=ROOT.LArBadChannelDBTools
    cl_AthenaAttributeList=ROOT.AthenaAttributeList    
    ms=cl_LArBadChannelDBTools.getDefaultMsgStream()



    ts1=cool.ValidityKey(1L+(run<<32))
    ts2=cool.ValidityKey(((run+1L)<<32)-1)
    itr=f.browseObjects(ts1,ts2,cool.ChannelSelection(0),t)
    while itr.goToNext():
        obj=itr.currentRef()
        pl=obj.payload()
        #print obj.since(), obj.until(), pl

        al=cl_AthenaAttributeList(pl.attributeList())
        vect_BadFebEntry=cl_LArBadChannelDBTools.readBadFeb(al,ms)

        feblist=set()
        for (id,mf) in vect_BadFebEntry:
            if not mf.good():
                feblist.add(id.get_identifier32().get_compact())

        retval.append((obj.since(), obj.until(),feblist))

    db.closeDatabase()
    return retval


#print getMissingFebs(201489,"COMCOND-BLKPA-006-03")
