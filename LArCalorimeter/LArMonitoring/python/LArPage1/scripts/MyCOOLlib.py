#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys  
import time
import glob
from time import mktime, gmtime
import calendar
from datetime import datetime

import cx_Oracle

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen

from ROOT import *

TAG = 'OflLumi-7TeV-002'
#############################################################################
def GetBeamIntensities(runnumber,beam):    
    res = []
    LBtimes = GetLBTimeStamps(runnumber)
    trigDB = indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLONL_TDAQ/COMP200"
        return None
    try:
        folder = trigDB.getFolder('/TDAQ/OLC/LHC/LBDATA')
        limmin = LBtimes[0] *1000000000
        limmax = LBtimes[-1]*1000000000
        chansel = cool.ChannelSelection(0)
        objs = folder.browseObjects(limmin,limmax,chansel)
        while objs.goToNext():
            obj = objs.currentRef()
            payload = obj.payload()
            res.append(float(payload['Beam'+beam+'Intensity']))
    except Exception,e:
        print "ERROR accessing /TDAQ/OLC/LHC/LBDATA"
        print e        
    trigDB.closeDatabase()
    if len(res) < len(LBtimes)-1:
        while len(res) != len(LBtimes)-1:
            res.append(res[-1])
    return res


#############################################################################
def MyDumpFolder(folder, tag):
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

#############################################################################
def GetNEventsCosmicCalo(runnumber):
    nevents = 0.
    return nevents

#############################################################################
def GetTotalLuminosity(runnumber,lumiblocks,lumiblocksready):
    if not lumiblocks: return 0.
    if not lumiblocksready: return 0.

    luminosity = 0.
    res = []
    integrated_lumi = 0.
    trigDB=indirectOpen('COOLONL_TRIGGER/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLONL_TRIGGER/COMP200"
        return None
    try:
        folder=trigDB.getFolder('/TRIGGER/LUMI/LBLESTONL')
        #print lumiblocks[0]
        #print lumiblocks[-1]
        limmin=(runnumber << 32)+lumiblocks[0]
        limmax=(runnumber << 32)+lumiblocks[-1]
        chansel=cool.ChannelSelection.all()
        objs = folder.browseObjects(limmin,limmax,chansel)
        tmp=0
        while objs.goToNext():
            obj=objs.currentRef()
            payload=obj.payload()
            #res.append(int(payload['StartTime']/1000000000))
            #tmp=int(payload['EndTime']/1000000000)
            luminosity += payload['LBAvInstLumi']*60.

        #res.append(tmp)
    except Exception,e:
        print "ERROR accessing /TRIGGER/LUMI/LBLESTONL"
        print e        
    trigDB.closeDatabase()

    #print "luminosity",luminosity
    return luminosity


#############################################################################
def GetLBTimeStamps(runnumber):
    res = []
    trigDB=indirectOpen('COOLONL_TRIGGER/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLONL_TRIGGER/COMP200"
        return None
    try:
        folder=trigDB.getFolder('/TRIGGER/LUMI/LBLB')
        limmin=(runnumber << 32)+1
        limmax=(runnumber << 32)+10000
        chansel=cool.ChannelSelection.all()
        objs = folder.browseObjects(limmin,limmax,chansel)
        tmp=0
        while objs.goToNext():
            obj=objs.currentRef()
            payload=obj.payload()
            res.append(int(payload['StartTime']/1000000000))
            tmp=int(payload['EndTime']/1000000000)
        res.append(tmp)
    except Exception,e:
        print "ERROR accessing /TRIGGER/LUMI/LBLB"
        print e        
    trigDB.closeDatabase()
    return res
#############################################################################
def GetRunNumberLumiblock(since,until):
    res = []
    trigDB=indirectOpen('COOLONL_TRIGGER/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLONL_TRIGGER/COMP200"
        return None
    try:
        folder = trigDB.getFolder('/TRIGGER/LUMI/LBTIME')
        limmin = ConvertUTCtoIOV(since)*1000000000
        limmax = ConvertUTCtoIOV(until)*1000000000
        chansel = cool.ChannelSelection.all()
        objs = folder.browseObjects(limmin,limmax,chansel)
        while objs.goToNext():
            obj=objs.currentRef()
            payload=obj.payload()
            res.append([int(payload['Run']),int(payload['LumiBlock'])])
    except Exception,e:
        print "ERROR accessing /TRIGGER/LUMI/LBLB"
        print e        
    trigDB.closeDatabase()
    return res[0],res[-1]
#############################################################################
def GetTDaqOnlineLumi(runnumber,chan,type):
    LBTS = GetLBTimeStamps(runnumber)
    nlb = len(LBTS)-1
    res = [0.*i for i in range(nlb)]
    trigDB = indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLONL_TDAQ/COMP200"
    try:
        folder = trigDB.getFolder('/TDAQ/OLC/LUMINOSITY')
        limmin = LBTS[0]*1000000000
        limmax = LBTS[-1]*1000000000
        chansel = cool.ChannelSelection(chan)
        objs = folder.browseObjects(limmin,limmax,chansel)
        while objs.goToNext():
            obj = objs.currentRef()
            payload = obj.payload()
            if int(payload['RunLB']) > 0:
                lb = int( payload['RunLB'] - (runnumber<<32) ) - 1
                res[lb] = [payload['LBAvInstLum%s'%type] if payload['Valid']&3 == 0 else 0.][0]
    except Exception,e:
        print "ERROR accessing /TDAQ/OLC/LUMINOSITY"
        print e        
    trigDB.closeDatabase()
    return res
#############################################################################
def GetOnlineLumiFromCOOL(runnumber,nlb,channelid):
    res = [0.*i for i in range(nlb)]
    trigDB=indirectOpen('COOLONL_TRIGGER/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLONL_TRIGGER/COMP200"
        return None
    try:
        folder=trigDB.getFolder('/TRIGGER/LUMI/LBLESTONL')
        limmin=(runnumber << 32)+1
        limmax=(runnumber << 32)+10000
        chansel=cool.ChannelSelection(channelid)
        objs = folder.browseObjects(limmin,limmax,chansel)
        lb = 0
        while objs.goToNext():
            obj=objs.currentRef()
            lb = obj.since() - limmin
            payload=obj.payload()
            res[lb] = payload['LBAvInstLumi']
    except Exception,e:
        print "ERROR accessing /TRIGGER/LUMI/LBLESTONL"
        print e        
    trigDB.closeDatabase()
    return res
############################################################################# 
def GetOfflineLumiFromCOOL(runnumber,nlb,channelid):
    res = [0.*i for i in range(nlb)]
    trigDB=indirectOpen('COOLOFL_TRIGGER/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLOFL_TRIGGER/COMP200"
        return None
    try:
        folder=trigDB.getFolder('/TRIGGER/OFLLUMI/LBLESTOFL')
        limmin=(runnumber << 32)+1
        limmax=(runnumber << 32)+10000
        chansel=cool.ChannelSelection(channelid)
        objs = folder.browseObjects(limmin,limmax,chansel,TAG)
        lb = 0
        while objs.goToNext():
            obj=objs.currentRef()
            lb = obj.since() - limmin
            if lb >= nlb:
                break
            payload=obj.payload()
            res[lb] = payload['LBAvInstLumi']
    except Exception,e:
        print "ERROR accessing /TRIGGER/OFLLUMI/LBLESTOFL"
        print e        
    trigDB.closeDatabase()
    return res
#############################################################################
def GetOnlineMuFromCOOL(runnumber,nlb,channelid):
    res = [0.*i for i in range(nlb)]
    trigDB=indirectOpen('COOLONL_TRIGGER/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLONL_TRIGGER/COMP200"
        return None
    try:
        folder=trigDB.getFolder('/TRIGGER/LUMI/LBLESTONL')
        limmin=(runnumber << 32)+1
        limmax=(runnumber << 32)+10000
        chansel=cool.ChannelSelection(channelid)
        objs = folder.browseObjects(limmin,limmax,chansel)
        lb = 0
        while objs.goToNext():
            obj=objs.currentRef()
            lb = obj.since() - limmin
            payload=obj.payload()
            res[lb] = payload['LBAvEvtsPerBX']
    except Exception,e:
        print "ERROR accessing /TRIGGER/LUMI/LBLESTONL"
        print e        
    trigDB.closeDatabase()
    return res
#############################################################################
def GetOfflineMuFromCOOL(runnumber,nlb,channelid):
    res = [0.*i for i in range(nlb)]
    trigDB=indirectOpen('COOLOFL_TRIGGER/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLOFL_TRIGGER/COMP200"
        return None
    try:
        folder=trigDB.getFolder('/TRIGGER/OFLLUMI/LBLESTOFL')
        limmin=(runnumber << 32)+1
        limmax=(runnumber << 32)+10000
        chansel=cool.ChannelSelection(channelid)
        objs = folder.browseObjects(limmin,limmax,chansel,TAG)
        lb = 0
        while objs.goToNext():
            obj = objs.currentRef()
            lb  = obj.since() - limmin
            if lb >= nlb:
                break
            payload = obj.payload()
            res[lb] = payload['LBAvEvtsPerBX']
    except Exception,e:
        print "ERROR accessing /TRIGGER/OFLLUMI/LBLESTOFL"
        print e        
    trigDB.closeDatabase()
    return res
#############################################################################
def GetReadyFlag(runnumber,LBTS):
    res = []
    LBtimes = LBTS#GetLBTimeStamps(runnumber)
    nLB = len(LBtimes)-1
    db  = indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
    folder = "/TDAQ/RunCtrl/DataTakingMode"
    frc = db.getFolder(folder)
    for lb in range(nLB):
        runlb = (runnumber<<32)+lb+1
        try:
            objrc = frc.findObject(runlb,0)
            plrc = objrc.payload()
            atlReady = plrc["ReadyForPhysics"]
        except RuntimeError:
            atlReady = 0
        res.append(int(atlReady))
    db.closeDatabase()
    return res

#############################################################################
## def GetReadyFlag(runnumber,LBTS):
##     res = []
##     LBtimes = LBTS#GetLBTimeStamps(runnumber)
##     nLB = len(LBtimes)-1
##     db  = indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
##     folder = "/TDAQ/RunCtrl/DataTakingMode"
##     frc = db.getFolder(folder)
##     for lb in range(nLB):
##         runlb = (runnumber<<32)+lb+1
##         try:
##             objrc = frc.findObject(runlb,0)
##             plrc = objrc.payload()
##             atlReady = plrc["ReadyForPhysics"]
##         except RuntimeError:
##             atlReady = 0
##         res.append(int(atlReady))
##     db.closeDatabase()
##     return res

#############################################################################
def GetFillTimeStamp(start):
    res = []
    TIMESTAMP = []
    trigDB = indirectOpen('COOLOFL_DCS/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLOFL_DCS/COMP200"
        return None
    try:
        folder = trigDB.getFolder('/LHC/DCS/FILLSTATE')
        limmin = (start - 86400)*1000000000
        limmax = start*1000000000
        chansel = cool.ChannelSelection.all()
        objs = folder.browseObjects(limmin,limmax,chansel)
        while objs.goToNext():
            obj = objs.currentRef()
            payload = obj.payload()
            fill = int(payload['FillNumber'])
            since = (obj.since())/1000000000
            res.append([fill,since])
        for pair in res:
            if pair[0] == res[-1][0]:
                TIMESTAMP.append(pair[1])
    except Exception,e:
        print "ERROR accessing /LHC/DCS/FILLSTATE"
        print e        
    trigDB.closeDatabase()
    return TIMESTAMP[0]
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
#############################################################################
def ConvertIOVtoSQL( iovtime ):
    if len(str(iovtime))>10:
        tsec = int(float(iovtime)/1.E9)
    else:
        tsec = int(iovtime)
    sqltime = time.strftime('%d%m%y%H%M',time.gmtime(tsec))
    return sqltime
#############################################################################
def GetBeamSpotFromCOOL(runnumber,nlb):
    res = [[-999. for i in range(nlb)],
           [-999. for i in range(nlb)],
           [-999. for i in range(nlb)],
           [-999. for i in range(nlb)],
           [-999. for i in range(nlb)],
           [-999. for i in range(nlb)]]
    trigDB=indirectOpen('COOLOFL_INDET/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLONL_TRIGGER/COMP200"
        return None
    try:
        folder=trigDB.getFolder('/Indet/Beampos')
        limmin=(runnumber << 32)+1
        limmax=(runnumber << 32)+10000
        chansel=cool.ChannelSelection(0)
        objs = folder.browseObjects(limmin,limmax,chansel,'IndetBeampos-ES1-UPD2')
        lb = 0
        while objs.goToNext():
            obj=objs.currentRef()
            lb = obj.since() - limmin + 1
            payload=obj.payload()
            res[0][lb] = payload['posX']
            res[1][lb] = payload['posY']
            res[2][lb] = payload['posZ']
            res[3][lb] = payload['sigmaX']
            res[4][lb] = payload['sigmaY']
            res[5][lb] = payload['sigmaZ']
    except Exception,e:
        print "ERROR accessing /Indet/Beampos"
        print e        
    trigDB.closeDatabase()
    return res
#############################################################################
def GetStableBeams(runnumber):
    res = []
    LBtimes = GetLBTimeStamps(runnumber)
    nlb = len(LBtimes)-1
    trigDB = indirectOpen('COOLOFL_DCS/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to COOLOFL_DCS/COMP200"
        return None
    try:
        folder = trigDB.getFolder('/LHC/DCS/FILLSTATE')
        limmin = (LBtimes[0]) *1000000000
        limmax = (LBtimes[-1])*1000000000
        chansel = cool.ChannelSelection.all()
        objs = folder.browseObjects(limmin,limmax,chansel)
        while objs.goToNext():
            obj = objs.currentRef()
            payload = obj.payload()
            iovtime = obj.since()/1000000000
            res.append([iovtime,int(payload['StableBeams'])])
    except Exception,e:
        print "ERROR accessing /LHC/DCS/FILLSTATE"
        print e
    trigDB.closeDatabase()
    sb = [None for i in range(nlb)]
    for p in xrange(len(res)):
        for lb in xrange(nlb):
            if res[p][0] <= LBtimes[lb]:
                sb[lb] = res[p][1]
                continue
    return sb
#############################################################################
def GetNumberOfCollidingBunches(runnumber):
    res = []
    LBtimes = GetLBTimeStamps(runnumber)
    nlb = len(LBtimes)-1
    #trigDB = indirectOpen('COOLOFL_DCS/COMP200',oracle=True)
    trigDB = indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
    if (trigDB is None):
        print "ERROR: Cannot connect to database"
        return None
    try:
        folder = trigDB.getFolder('/TDAQ/OLC/LHC/FILLPARAMS')
        #folder = trigDB.getFolder('/LHC/DCS/FILLSTATE')
        limmin = (LBtimes[0]) *1000000000
        limmax = (LBtimes[-1])*1000000000
        chansel = cool.ChannelSelection.all()
        objs = folder.browseObjects(limmin,limmax,chansel)
        while objs.goToNext():
            obj = objs.currentRef()
            payload = obj.payload()
            iovtime = obj.since()/1000000000
            #res.append([iovtime,int(payload['NumBunchColl'])])
            res.append([iovtime,int(payload['LuminousBunches'])])
    except Exception,e:
        print "ERROR accessing /LHC/DCS/FILLSTATE"
        print e
    trigDB.closeDatabase()
    sb = [0 for i in range(nlb)]
    for p in xrange(len(res)):
        for lb in xrange(nlb):
            if res[p][0] <= LBtimes[lb]:
                sb[lb] = res[p][1]
                continue

    #print "res",res
    #print "sb",sb
    if not sb: return 0
    else: return max(sb)
#############################################################################
def doOracleQuery(runnumber,dpe):
    #
    LBTS = GetLBTimeStamps(runnumber)
    nLB = len(LBTS)-1
    since = LBTS[0]
    until = LBTS[-1]
    #
    args = {'since' : ConvertIOVtoSQL(since),
            'until' : ConvertIOVtoSQL(until),
            'dp'    : dpe}
    #
    if 'ATLLAR' in dpe:
        ev = "atlas_pvsslar.eventhistory"
        el = "atlas_pvsslar.elements"
    elif 'ATLIDE' in dpe:
        ev = "atlas_pvsside.eventhistory"
        el = "atlas_pvsside.elements"
    elif 'ATLGCS' in dpe:
        ev = "atlas_pvssdcs.eventhistory"
        el = "atlas_pvssdcs.elements"
    else:
        print "Could not resolve system name for %s" % dpe
        exit(0)
    #
    conn_str = 'ATLAS_PVSS_READER/pvssred4pro@ATLR'
    cx_oracle_connection = cx_Oracle.Connection(conn_str)
    cursor = cx_Oracle.Cursor(cx_oracle_connection)
    #
    query = """
    select ev.ts, ev.value_number
    from %s ev, %s el
    where el.element_id = ev.element_id
    and ev.ts between to_date(:since, 'ddmmyyhh24mi') and to_date(:until,'ddmmyyhh24mi')
    and el.element_name = :dp
    order by ts
    """ % (ev,el)
    #
    cursor.execute(query,args)
    result = cursor.fetchall()
    #
    data = []
    for row in result:
        utctime = row[0] 
        value = row[1]
        tsec = calendar.timegm(utctime.timetuple())+1e-6*utctime.microsecond
        data.append([tsec,value])
    #
    if len(data) == 0:
        print "No data for dpe : %s" % dpe
    #
    h = TProfile(dpe,dpe,nLB,0,nLB)
    h.SetDirectory(0)
    for i in xrange(len(data)):
        iovtime = data[i][0]
        val = data[i][1]
        if iovtime < since or iovtime > until:
            continue
        for lb in xrange(nLB):
            if iovtime < LBTS[lb+1]:   
                h.Fill(lb,val)
                break
    LBav_data = []
    for bin in xrange(1,h.GetNbinsX()+1):
        LBav_data.append(h.GetBinContent(bin))
    return LBav_data
#############################################################################
def getOracleBaseline(runnumber,dpe):
    #
    LBTS  = GetLBTimeStamps(runnumber)
    since = GetFillTimeStamp(LBTS[0])
    until = since + 3600
    nLB   = len(LBTS)-1
    #
    args = {'since' : ConvertIOVtoSQL(since),
            'until' : ConvertIOVtoSQL(until),
            'dp'    : dpe}
    #
    if 'ATLLAR' in dpe:
        ev = "atlas_pvsslar.eventhistory"
        el = "atlas_pvsslar.elements"
    elif 'ATLIDE' in dpe:
        ev = "atlas_pvsside.eventhistory"
        el = "atlas_pvsside.elements"
    elif 'ATLGCS' in dpe:
        ev = "atlas_pvssdcs.eventhistory"
        el = "atlas_pvssdcs.elements"
    else:
        print "Could not resolve system name for %s" % dpe
        exit(0)
    #
    conn_str = 'ATLAS_PVSS_READER/pvssred4pro@ATLR'
    cx_oracle_connection = cx_Oracle.Connection(conn_str)
    cursor = cx_Oracle.Cursor(cx_oracle_connection)
    #
    query = """
    select ev.ts, ev.value_number
    from %s ev, %s el
    where el.element_id = ev.element_id
    and ev.ts between to_date(:since, 'ddmmyyhh24mi') and to_date(:until,'ddmmyyhh24mi')
    and el.element_name = :dp
    order by ts
    """ % (ev,el)
    #
    cursor.execute(query,args)
    result = cursor.fetchall()
    #
    data = []
    for row in result:
        value = row[1]
        data.append(value)
    #
    if len(data) == 0:
        print "No data for dpe : %s" % dpe
    #
    h = TH1F(dpe,dpe,51000,-10,500)
    h.SetDirectory(0)
    for i in xrange(len(data)):
        h.Fill(data[i])
    #h.Draw()
    #raw_input('...')
    return h.GetMean(),h.GetRMS()
#############################################################################
def get_max_width(table, index):
    """Get the maximum width of the given column index"""
    #
    return max([len(str(row[index])) for row in table])
#############################################################################
def pprint_table(out, table):
    """Prints out a table of data, padded for alignment
    @param out: Output stream (file-like object)
    @param table: The table to print. A list of lists.
    Each row must have the same number of columns. """
    #
    col_paddings = []
    #
    for i in range(len(table[0])):
        col_paddings.append(get_max_width(table, i))
    #
    for row in table:
        # left col
        print >> out, str(row[0]).ljust(col_paddings[0] + 1),
        # rest of the cols
        for i in range(1, len(row)):
            col = str(row[i]).rjust(col_paddings[i] + 2)
            print >> out, col,
        print >> out
#############################################################################
def ATLASLabel(x,y):
    l = TLatex(x,y,'ATLAS')
    l.SetNDC()
    l.SetTextFont(72)
    #l.Draw()
    #delx = 0.115*696*gPad.GetWh()/(472*gPad.GetWw())
    p = TLatex(x+0.117,y,'Preliminary')
    p.SetNDC()
    p.SetTextFont(42)
    #p.Draw()
    return p,l
############################################################################# 
if __name__=='__main__':
    #
    # print getOracleBaseline(177539,'ATLLARHV6:M185.C0.R.IMeas')
    #
    # print GetNumberOfCollidingBunches(177968)
    #
    ready = GetReadyFlag(182013)
    #print ready








"""
    n_coll = {}
    n_lb = {}
    since = {}
    until = {}
    peak_lumi = {}
    peak_mu = {}
    #
    files = glob.glob('./trees/*')
    files.sort()
    for file in files:
        #
        run = int(file[8:14])
        #
        n_coll[run] = GetNumberOfCollidingBunches(run)
        LBTS = GetLBTimeStamps(run)
        n_lb[run] = len(LBTS)-1
        since[run] = ConvertIOVtoUTC(LBTS[0])
        until[run] = ConvertIOVtoUTC(LBTS[-1])
        peak_lumi[run] = round(max(GetOnlineLumiFromCOOL(run,n_lb[run],0)),1)
        peak_mu[run] = round(max(GetOnlineMuFromCOOL(run,n_lb[run],0)),1)
    #
    table = []
    table.append(['Run','Start (UTC)','End (UTC)','# LB','# Colliding Bunches','Peak Lumi (10^30 cm2s-1)','Peak mu'])
    for k in sorted(n_lb.iterkeys()):
        #
        table.append([k,since[k],until[k],n_lb[k],n_coll[k],peak_lumi[k],peak_mu[k]])
        #
    out = sys.stdout
    pprint_table(out, table)
"""
