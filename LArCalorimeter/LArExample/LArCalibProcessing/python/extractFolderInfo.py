#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from PyCool import cool


def extractFolderInfo(dbname,globaltag="",checkFolders=[],runnumber=cool.ValidityKeyMax>>32,selection=""):
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    try:
        db= dbSvc.openDatabase(dbname)
    except Exception,e:
        print "Problems connecting to database:",e
        return None

    #Set up Channel Selection:
    sel=None
    print selection
    ss=selection.split(",")
    for s in ss:
        if len(s)==0:continue
        idx=s.find(":");
        if idx==-1:
            c1=cool.ChannelId(int(s))
            c2=c1
        else:
            c1=cool.ChannelId(int(s[:idx]))
            c2=cool.ChannelId(int(s[1+idx:]))
        #print "ExtractFolderInfo: Add channel range",c1,"to",c2
        if sel is None:
            sel=cool.ChannelSelection(c1,c2,cool.ChannelSelection.sinceBeforeChannel);
        else:
            sel.addRange(c1,c2);
                               
    if sel is None:
        print "ExtractFolderInfo: No COOL channel selection given, work on all channels"
        sel=cool.ChannelSelection.all()
        
        
    folderInfoList=list()

    nodelist=db.listAllNodes()
    for fn in nodelist:
        if not db.existsFolder(fn): continue
        if len(checkFolders):
            takeFolder=False
            for cf in checkFolders:
                if cf[-1] is "/": cf=cf[0:-1]
                idx=len(cf)
                if fn.startswith(cf) and (idx>=len(fn) or fn[idx]=="/"):
                    takeFolder=True
                    break                 
            if not takeFolder: continue
            
        f=db.getFolder(fn)
        print "Analyzing",fn
        #Parse Folder description:
        #Model:
        #<timeStamp>run-lumi</timeStamp><symlinks>ILArAutoCorr</symlinks><key>LArAutoCorr</key><addrHeader><address_header service_type="71" clid="255786016" /></addrHeader><typeName>LArAutoCorrComplete</typeName>  -
        # Get Type
        descr=f.description()
        i1=descr.find("<typeName>")+len("<typeName>")
        i2=descr.find("</typeName>",i1)
        if (i1==-1 or i2==-1):
            print "ERROR could not get typename of object stored in folder",fn
            continue
        typename=descr[i1:i2]

        
        i1=descr.find("<key>")+len("<key>")
        i2=descr.find("</key>",i1)
        if (i1==-1 or i2==-1):
            print "ERROR could not get SG key of object stored in folder",fn
            continue
        key=descr[i1:i2]


        #Loop over all tags
        
        tagList=f.listTags()
        if len(tagList)==0:
            tagList=[""]
        elif len(tagList)>1 and globaltag!="":
            try:
                tagList=[f.resolveTag(globaltag)]
            except Exception:
                print "Hierachical tag", globaltag,"not defined in folder",fn
                return None

        for t in tagList:
            itr=f.findObjects(runnumber<<32,sel,t)
            minIOV=cool.ValidityKeyMax>>32
            maxIOV=cool.ValidityKeyMin>>32
            #Loop over all objects to get IOV
            while itr.goToNext():
                obj=itr.currentRef()
                #payload=obj.payload()
                since=obj.since()>>32
                until=obj.until()>>32
                #print fn,t,since,until
                if minIOV>since: minIOV=since
                if maxIOV<until: maxIOV=until
            itr.close()

            #Form tuple:
            folderinfo=(fn,typename,key,t,minIOV,maxIOV)
            folderInfoList+=[folderinfo]

    db.closeDatabase()
    return folderInfoList



#x=extractFolderInfo("sqlite://;schema=/scratch/wlampl/DBwrites/Sep13AllCalo_v2/freshConstants.db;dbname=COMP200")

#for i in x:
#    print i
