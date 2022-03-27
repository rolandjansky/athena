#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# MagFieldUtils.py
# Routines to read magnetic field information from COOL
# Richard Hawkings 25/9/08

from __future__ import print_function

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen
from os import environ

#Cache for run/LB time stamp (avoid multiple DB lookup)
global _timeForLB
_timeForLB=dict()

class MagFieldDCSInfo:
    "Wrapper class to hold magnetic field current info from DCS data and the filename tag for the SOR"
    def __init__(self,solC,solSetC,torC,torSetC,fnt):
        "Initialise given values for solenoid & toroid acual and set currents and filename tag"
        self._solC=solC
        self._solSetC=solSetC
        self._torC=torC
        self._torSetC=torSetC
        self._fileNameTag=fnt

    def solenoidCurrent(self):
        "Return the solenoid current"
        return self._solC

    def solenoidSetCurrent(self):
        "Return the requested solenoid current (set point)"
        return self._solSetC

    def toroidCurrent(self):
        "Return the toroid current"
        return self._torC

    def toroidSetCurrent(self):
        "Return the requested toroid current (set point)"
        return self._torSetC

    def fileNameTag(self):
        "Return the Filename tag for this run"
        return self._fileNameTag


def getFieldForRun(run,readOracle=True,quiet=False,lumiblock=None):
    "Get the magnetic field currents (MagFieldDCSInfo) for a given run"
    # access the TDAQ schema to translate run number into timestamp
    # and get the filename tag

    if "DBRELEASE" in environ:
        print ("Running in DBRelease, forcing readOracle to False")
        readOracle=False
    
    newdb=(run>=236107)
    if not quiet:
        print ("Reading magnetic field for run %i, forceOracle %s CONDBR2 %s" % (run,readOracle,newdb))
    # setup appropriate connection and folder parameters
    if newdb:
        dbname='CONDBR2'
        sorfolder='/TDAQ/RunCtrl/SOR'
        fntname='T0ProjectTag'
    else:
        dbname='COMP200'
        sorfolder='/TDAQ/RunCtrl/SOR_Params'
        fntname='FilenameTag'

    tdaqDB=indirectOpen('COOLONL_TDAQ/%s' % dbname,oracle=readOracle)
    if (tdaqDB is None):
        print ("MagFieldUtils.getFieldForRun ERROR: Cannot connect to COOLONL_TDAQ/%s" % dbname)
        return None
    sortime=0
    try:
        tdaqfolder=tdaqDB.getFolder(sorfolder)
        runiov=run << 32
        obj=tdaqfolder.findObject(runiov,0)
        payload=obj.payload()
        sortime=payload['SORTime']
        fnt=payload[fntname]
    except Exception as e:
        print ("MagFieldUtils.getFieldForRun ERROR accessing folder %s" % sorfolder)
        print (e)
    tdaqDB.closeDatabase()
    
    # if lumiblock is specifed, actually want the start time of the LB
    if lumiblock is not None:
        if not quiet:
            print ("Reading specific timestamp for lumiblock %i" % lumiblock)
        
        lbtime=getTimeForLB(run,lumiblock,readOracle)
        if (lbtime==0 and lumiblock>1):
            # sometimes fails as last LB is missing in LBLB - try previous
            print ("MagFieldUtils.getFieldForRun WARNING: Cannot find LB %i, trying %i" % (lumiblock,lumiblock-1))
            lbtime=getTimeForLB(run,lumiblock-1,readOracle)
        if (lbtime==0):
            print ("MagFieldUtils.getFieldForRun WARNING: Cannot find LB %i, fall back on SOR time" % lumiblock)
        if (lbtime>0):
            # use this time instead of SORtime
            if not quiet:
                print ("Lumiblock starts %i seconds from start of run" % int((lbtime-sortime)/1.E9))
            sortime=lbtime
        else:
            print ("MagFieldUtils.getFieldForRun ERROR accessing /TRIGGER/LUMI/LBLB")
            print ("Fall back on SOR time from %s" % sorfolder)
            lbtime=sortime
    
    # if we do not have a valid time, exit
    if (sortime==0): return None
    
    # now having got the start of run timestamp, lookup the field info in DCS
    dcsDB=indirectOpen('COOLOFL_DCS/%s' % dbname,oracle=readOracle)
    if (dcsDB is None):
        print ("MagFieldUtils.getFieldForRun ERROR: Cannot connect to COOLOFL_DCS/%s" % dbname)
        return None
    data=None
    try:
        # map of expected channel names to slots in data[] variable
        # follows original order from run1/COMP200
        # has changed in CONDBR2, but use of named channels recovers this
        currentmap={'CentralSol_Current':0,'CentralSol_SCurrent':1,
                    'Toroids_Current':2,'Toroids_SCurrent':3}
        dcsfolder=dcsDB.getFolder('/EXT/DCS/MAGNETS/SENSORDATA')
        objs=dcsfolder.findObjects(sortime,cool.ChannelSelection.all())
        data=[-1.,-1.,-1.,-1.]
        for obj in objs:
            chan=obj.channelId()
            channame=dcsfolder.channelName(chan)
            if channame in currentmap.keys():
                data[currentmap[channame]]=obj.payload()['value']
    except Exception as e:
        print ("MagFieldUtils.getFieldForRun ERROR accessing /EXT/DCS/MAGNETS/SENSORDATA")
        print (e)
    dcsDB.closeDatabase()
    # if problem accessing folder, exit
    if data is None:
        return None
    # return a MagFIeldDCSInfo object containing the result
    return MagFieldDCSInfo(data[0],data[1],data[2],data[3],fnt)

def getTimeForLB(run,LB,readOracle=False):
    "Return the time a specific run/LB, given the folder, or 0 for bad/no data"
    if LB is None: 
        LB=0

    runiov=(run << 32)+LB

    if runiov in _timeForLB:
        print ("getTimeForLB: Returning cached time for run %i, LumiBlock %i " % (run,LB))
        return _timeForLB[runiov]

    if (run>=236107):
        dbname="CONDBR2"
    else:
        dbname="COMP200"

    #print ("Querying DB for time of run %i LB %i" % (run,LB))

    try:
        trigDB=indirectOpen('COOLONL_TRIGGER/%s' % dbname,oracle=readOracle)
        if (trigDB is None):
            print ("MagFieldUtils.getTimeForLB ERROR: Cannot connect to COOLONL_TDAQ/%s" % dbname)
            return 0

        lblbfolder=trigDB.getFolder('/TRIGGER/LUMI/LBLB')
        obj=lblbfolder.findObject(runiov,0)
        payload=obj.payload()
        lbtime=payload['StartTime']
        _timeForLB[runiov]=lbtime
        trigDB.closeDatabase()
        return lbtime
    except Exception as e:
        print ("MagFieldUtils.getTimeForLB WARNING: accessing /TRIGGER/LUMI/LBLB for run %i, LB %i" % (run,LB))
        print (e)
        return 0


# command line driver for convenience
if __name__=='__main__':
    import sys
    if len(sys.argv)<2:
        print ("Syntax",sys.argv[0],'<run>')
        sys.exit(-1)
    run=int(sys.argv[1])
    lumiblock=None
    if len(sys.argv)>2:
        lumiblock=int(sys.argv[2])
    magfield=getFieldForRun(run,lumiblock=lumiblock)
    print ("Magnetic field information for run %i" % run)
    if (magfield is not None):
        print ("Solenoid current %8.2f (requested %8.2f)" % (magfield.solenoidCurrent(),magfield.solenoidSetCurrent()))
        print ("Toroid   current %8.2f (requested %8.2f)" % (magfield.toroidCurrent(),magfield.toroidSetCurrent()))
        print ("Filename Tag: %s" % (magfield.fileNameTag()))
    else:
        print ("Not available")
