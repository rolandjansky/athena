#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys  
import time
import glob
from time import mktime, gmtime
import calendar
from datetime import datetime

import cx_Oracle

from PyCool import cool, coral
from CoolConvUtilities.AtlCoolLib import indirectOpen

from ROOT import *

TAG = 'OflLumi-7TeV-002'

import MyCOOLlib
from MyCOOLlib import *

def GetLastRuns():
    dbSvc=cool.DatabaseSvcFactory.databaseService()
    dbstring="COOLONL_TDAQ/COMP200"
    try:
        db=dbSvc.openDatabase(dbstring)
    except Exception,e:
        print 'Problem opening database',e
        sys.exit(-1)
        print "Opened database",dbstring

    folderlist=db.listAllNodes()
    for i in folderlist: print i+'\n'

    tdaqfolder=db.getFolder('/TDAQ/RunCtrl/SOR_Params')
    taglist=tdaqfolder.listTags()
    print "Tags in folder:"
    for i in taglist: print i

##     limmin = ConvertUTCtoIOV(since)*1000000000
##     limmax = ConvertUTCtoIOV(until)*1000000000
##     chansel = cool.ChannelSelection.all()
##     objs = folder.browseObjects(limmin,limmax,chansel)
##     while objs.goToNext():
##         obj=objs.currentRef()
##         payload=obj.payload()
##         res.append([int(payload['Run']),int(payload['LumiBlock'])])

    print 'time is: ',time.time()
    print 'time for run is',183003 << 32

    # now get the data corresponding to a particular tag and IOV
    # run 5, lumi block 2 - a point in time query
##     myiov=(183003 << 32)+2
##     # use the first tag
##     mytag="RunNumber"
##     print "Getting objects for IOV",myiov,"tag",mytag
##     objs=myfolder.browseObjects(myiov,myiov,cool.ChannelSelection.all(),mytag)
##     # iterate through and print the objects
##     for obj in objs:
##         print obj
    # note you can also use the functions channelId(), payload(), since(),
    # until() of obj to access the corresponding information in detail
    
    
##     limmin = time.time()*1000000000-100000000
##     limmax = time.time()
##     chansel = cool.ChannelSelection(0)
##     objs = tdaqfolder.browseObjects(limmax,limmax,chansel)

##     while objs.goToNext():
##         obj = objs.currentRef()
##         payload = obj.payload()
##         runN = int(payload['RunNumber'])

##     for i in range(183003,183050):
##         print i
##         runiov=i<<32
##         obj=tdaqfolder.findObject(runiov,0)
##         payload=obj.payload()
##         print payload['RunNumber']
##         print payload['RunType']
##         print '''------'''
##         run = i
##         ruiov=run<<32
##         obj=tdaqfolder.findObject(runiov,0)
##         print obj
        
    #run = 183003
    #runiov=run << 32
    
    #obj=tdaqfolder.findObject(runiov,0)
    #print obj
    
    #myfolder=db.getFolder('/TDAQ/RunCtrl/FinalSFOStat/')#SOR_Params')

    #taglist=myfolder.listTags()
    #print "Tags in folder:"
    #for i in taglist: print i+'\n'
    
    #sys.stdout.write('183003;')
    #sys.stdout.write('186216;')
    
#############################################################################
def ConvertIOVtoUTC( iovtime ):
    if len(str(iovtime))>10:
        tsec = int(float(iovtime)/1.E9)
    else:
        tsec = int(iovtime)

    utctime = time.asctime( time.gmtime(tsec) )
    return utctime

#############################################################################
def ConvertUTCtoIOV( utctime ):
    try:
        ts   = time.strptime( utctime + '/UTC','%Y-%m-%d:%H:%M:%S/%Z' )
        tsec = int(calendar.timegm(ts))
        return tsec
    except ValueError:
        print "ERROR in time specification: '%s' (use format: 2007-05-25:14:01:00)" % utctime


## #############################################################################
#def FunctionTest(since,until):
#    print 'test3'
##     dbSvc=cool.DatabaseSvcFactory.databaseService()
##     dbstring="COOLONL_TDAQ/COMP200"
##     try:
##         db=dbSvc.openDatabase(dbstring)
##     except Exception,e:
##         print 'Problem opening database',e
##         sys.exit(-1)

##     print "Opened database",dbstring
    
##     res = []

##     trigDB=indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
##     if (trigDB is None):
##         print "ERROR: Cannot connect to COOLONL_TDAQ/COMP200"
##         return None
##     try:
##         folder = trigDB.getFolder('/TDAQ/RunCtrl/SOR_Params')
##         limmin = ConvertUTCtoIOV(since)*1000000000
##         limmax = ConvertUTCtoIOV(until)*1000000000
##         print 'limmin: ',limmin
##         print 'limmax: ',limmax
##         print cool.ValidityKeyMin,cool.ValidityKeyMax
##         chansel = cool.ChannelSelection.all()
##         objs = folder.browseObjects(cool.ValidityKeyMin,cool.ValidityKeyMax,cool.ChannelSelection.all())
##         #objs = folder.browseObjects(limmin,limmax,chansel)
##         while objs.goToNext():
##             obj=objs.currentRef()
##             payload=obj.payload()
##             res.append([int(payload['RunNumber']),int(payload['RunType'])])
##             print 'RunNumber::::: ',payload['RunNumber']
##     except Exception,e:
##         print "ERROR accessing /TDAQ/RunCtrl/SOR_Params"
##         print e
        
##     trigDB.closeDatabase()
##     return res[0],res[-1]



def runsFromTime(time1=cool.ValidityKeyMin,time2=cool.ValidityKeyMax):
    """Query /TDAQ/RunCtrl/LB_Params to get list of runs/LBs in time range,
    returning a map of run numbers to LB_Params objects"""
    
    trigDB=indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLONL_TDAQ/COMP200"
        return None
    
    folderLB_Params=trigDB.getFolder('/TDAQ/RunCtrl/LB_Params')
    print 'opened the folder'
    print 'time1',time1
    print 'time2',time2
    time1 = ConvertUTCtoIOV("2011-06-01:14:01:00")*1000000000
    time2 = ConvertUTCtoIOV("2011-06-03:09:01:00")*1000000000
    print 'time1 after',time1
    print 'time2 after',time2
    itr=folderLB_Params.browseObjects(time1,time2,cool.ChannelSelection.all())
    print 'size of iterator: ',str(itr).len()
    srun=-1
    runlist={}
    while itr.goToNext():
        print 'looping on objets'
        obj=itr.currentRef()
        # ignore openended IOVs which start before the time interval
        # these will be unclosed LB records from old runs
        if (obj.until()<cool.ValidityKeyMax or obj.since()>=time1):
            payload=obj.payload()
            run=payload['RunNumber']
            print 'RUN:::::',run
            lb=payload['LuminosityBlock']
            if (run!=srun):
                # seeing a new run - store old one if needed
                if (srun>-1):
                    runlist[srun]=LBParams(srun,slbmax,sstart,send)
                    srun=run
                    slbmax=lb
                    sstart=obj.since()
                    send=obj.until()
                else:
                    if (lb>slbmax): slbmax=lb
                    if (obj.until()>send): send=obj.until()
                    if (obj.since()<sstart): sstart=obj.since()
    itr.close()
    # store last run
    if (srun>-1):
        runlist[srun]=LBParams(srun,slbmax,sstart,send)
        print "Run list from LB_Params has %i entries" % len(runlist)
    return runlist
        
#GetLastRuns()
print 'test2'
runsFromTime()

#FunctionTest("2011-06-01:14:01:00","2011-06-15:14:01:00")
#GetRunNumberLumiBlock()#"2011-06-01:14:01:00","2011-06-15:14:01:00")
