#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys, getopt
from PyCool import cool,coral
from CoolConvUtilities.AtlCoolLib import indirectOpen
import MyCOOLlib
from MyCOOLlib import *
import os
import string
from ROOT import *
from ROOT import gStyle, TCanvas, TString, TPad
import time
import urllib2,sys
import xmlrpclib

def getPage(url):
  httpHeaders={'Accept':'text/plain'}
  textData=None
  request=urllib2.Request(url, textData, httpHeaders)
  u=urllib2.urlopen(request)
  textString = u.read()
  return textString

def getPageMain():
  url='http://atlas-service-db-runlist.web.cern.ch/atlas-service-db-runlist/cgi-bin/latestRun.py'
  last_run = getPage(url)
  return last_run

def GetSFO_LBNEvents( cursor, runno, streamname ):
    """returns nfiles, fsize, nevents"""
    stype, sep, sname = streamname.partition('_')
    cursor.execute( "SELECT SUM(NREVENTS),LUMIBLOCKNR FROM SFO_TZ_File WHERE RUNNR=:arg_1 and STREAMTYPE=:arg_2 and STREAM=:arg_3 GROUP BY LUMIBLOCKNR ORDER BY LUMIBLOCKNR",
                    arg_1=runno, arg_2=stype, arg_3=sname )
    return cursor.fetchall()

def GetSFO_LBs( cursor, runno ):
    cursor.execute( "SELECT MIN(LUMIBLOCKNR), MAX(LUMIBLOCKNR) FROM SFO_TZ_Lumiblock WHERE RUNNR=:arg_1", arg_1=runno )
    return cursor.fetchone()[0:2]

def OpenSFOConnection():
    import cx_Oracle
    return cx_Oracle.connect("ATLAS_SFO_T0_R/readmesfotz2008@atlr")

def GetEvents(runnumber):

    events = 0

    connection = OpenSFOConnection()
    cursor     = connection.cursor()

    lblist = GetSFO_LBNEvents( cursor, runnumber, "physics_CosmicCalo" )
    if len(lblist)==0:
        return
    else:
        for i in range(0,len(lblist)):
            events += lblist[i][0]
    
    cursor.close()
    connection.close()

    return events


def RunInfo(run, folder, tag):

    run = int(run)
    LBTS = MyCOOLlib.GetLBTimeStamps(run)
    #print 'run is',run
    info={}
    info[0]=run
    info[1]=time.asctime(time.gmtime(LBTS[0]+2.*3600.))#start of run
    info[2]=time.asctime(time.gmtime(LBTS[len(LBTS)-1]+2.*3600.))#end of run
    info[3]=time.asctime(time.gmtime(LBTS[len(LBTS)-1]+2.*3600.+48.*3600.))#end of calib loop
    nLB = len(LBTS)-1
    info[4]=nLB#number of lumiblocks
    atlaslumi = MyCOOLlib.GetOnlineLumiFromCOOL(run,nLB,0)
    atlasready=MyCOOLlib.GetReadyFlag(run,LBTS)
    readylist=[lb+1 for lb in xrange(len(atlasready)) if atlasready[lb]>0 ]
    nLBReady = len(readylist)
    info[5]=nLBReady#number of lumiblocks atlas ready
    info[6]=readylist[0]#from
    info[7]=readylist[int(nLBReady)-1]#to
    nBunches = MyCOOLlib.GetNumberOfCollidingBunches(run) ## Cette ligne a un bug
    info[8]=nBunches#number of colliding bunches
    PeakLumi = 0.
    for lb in atlaslumi: 
        if lb > PeakLumi: PeakLumi=lb
        #print "Peak lumi:",PeakLumi,"e30 cm-2.s-1"
    info[9]=PeakLumi#peak luminosity
    LBs = [lb for lb in xrange(len(LBTS))]
    lumi = GetTotalLuminosity(run,LBs,readylist)
    info[10]=lumi#total integrated lumi
    info[11]=GetEvents(run)#number of cosmiccalo events

    #print info
    return info
  
if __name__ == "__main__":

    run = sys.argv[1]
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    dbstring="COOLONL_TDAQ/COMP200"
    db=indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
    
    if (db is None):
        print "ERROR: Cannot connect to COOLONL_TDAQ/COMP200"
        sys.exit(-1)
    
    folderLB_Params=db.getFolder('/TDAQ/RunCtrl/SOR_Params')
    ThisRunInfo = RunInfo(run, folderLB_Params, "")
    print ThisRunInfo
    #tags_Params  = folderLB_Params.listTags()
    #for tag_Params in tags_Params: dumpFolder(f, tag)

