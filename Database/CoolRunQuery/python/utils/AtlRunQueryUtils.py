#!/bin/env

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryUtils.py
# Project: AtlRunQuery
# Purpose: Utility functions for AtlRunQuery
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Jan 20, 2009
# ----------------------------------------------------------------
#

from __future__ import with_statement
from .AtlRunQueryTimer import timer

from collections import namedtuple, defaultdict
import sys, os, time, re, calendar
from math import exp,sqrt,pi
from PyCool import cool
from copy import copy, deepcopy
import cx_Oracle
import struct
import urllib

from httplib import HTTP
from urlparse import urlparse

import DQDefects

def checkURL(url):
    p = urlparse(url)
    h = HTTP(p[1])
    h.putrequest('HEAD', p[2])
    h.endheaders()
    if h.getreply()[0] == 200: return 1
    else: return 0

class RunPeriods():
    def findPeriod( self, runno ):
        from CoolRunQuery.AtlRunQueryCOMA import ARQ_COMA
        s = ",".join( ARQ_COMA.get_periods_for_run(runno) )
        return s

class Enumerate(object):
    def __init__(self, names):
        for number, name in enumerate(names.split()):
            setattr(self, name, number)

def runsOnServer():
    hostname = os.getenv('HOSTNAME')
    print "Execution on host: %r" % hostname 
    if not hostname:
        onserver = False
    else:
        onserver = ( re.match('aiatlas.*\.cern\.ch',hostname) != None )
    print "Execution on server: %r" % onserver
    return onserver

def importroot(batch=True):
    import sys
    cmdline_args = sys.argv
    sys.argv[1:]=[]
    if batch:
        sys.argv += ['-b']
    import ROOT
    ROOT.gROOT.SetBatch(batch)
    if batch:
        del sys.argv[1]
    ROOT.gErrorIgnoreLevel = 2000 # to avoid printout when creating images
    sys.argv=cmdline_args
    return ROOT


def prettyNumber( n, width=-1, delim=',',decimal='.' ):
    """Converts a float to a string with appropriately placed commas"""
    if width >= 0: s = "%.*f" % (width, n)
    else: s = str(n)
    dec = s.find(decimal)
    if dec == -1: dec = len(s)
    threes = int((dec-1)/3) 
    for i in xrange(threes):
        loc = dec-3*(i+1)
        s = s[:loc] + delim + s[loc:]
    return s

def durationInSeconds( duration_string ):
    lst = duration_string.split()
    sec = 0
    for l in lst:
        if   's' in l: sec += int(l.replace('s',''))
        elif 'm' in l: sec += int(l.replace('m',''))*60
        elif 'h' in l: sec += int(l.replace('h',''))*3600
        elif 'd' in l: sec += int(l.replace('d',''))*3600*24
        elif 'w' in l: sec += int(l.replace('w',''))*3600*24*7
        elif 'y' in l: sec += int(l.replace('y',''))*3600*24*365
        else:
            print 'Big troubles... in function "AtlRunQueryUtils::durationInSeconds": cannot decode string "%s"' % l
            sys.exit(1)
    return sec

class DBConnectionController:
    def __init__(self):
        self.openConn = {}
        self.pw = {}

    def GetDBConn(self, schema, db):
        """for example schema=COOLONL_TRIGGER', db='CONDBR2"""
        if (schema,db) in self.openConn:
            return self.openConn[(schema,db)]
        try:
            if schema=="DEFECTS":
                #from AthenaCommon.Logging import logging # this is needed because of some problem in DQUtils logger
                # the logger there, if called first makes the athena logger crash
                defdb = DQDefects.DefectsDB("COOLOFL_GLOBAL/CONDBR2",tag=db)
                defdb.closeDatabase = lambda: None
                self.openConn[(schema,db)] = defdb
            else:
                readoracle=False # Richard: Tools which expect to read from the real
                                 # data conditions database have to be
                                 # setup in such a way as to ignore SQLite
                                 # replicas have to use 'indirectOpen' with
                                 # the oracle=True argument
                logging=False
                from CoolConvUtilities.AtlCoolLib  import indirectOpen
                self.openConn[(schema,db)] = indirectOpen("%s/%s"%(schema,db),True,readoracle, logging)
        except Exception, e:
            print e
            sys.exit(-1)
        return self.openConn[(schema,db)]

    def get_auth(self,key):
        if key not in self.pw:
            from os import environ as env
            #lookup = XMLReader(env['CORAL_DBLOOKUP_PATH']+"/dblookup.xml")
            #for s in lookup.servicelist.logicalservices:
            #    print "Service",s['name']
            #    for p in s.services:
            #        print "   name",p['name']
            auth = XMLReader(env['CORAL_AUTH_PATH']+"/authentication.xml")
            for c in auth.connectionlist.connections:
                if key!=c['name']: continue
                self.pw[key] = dict([(p['name'],p['value']) for p in c.parameters])
                break
            if key not in self.pw:
                print "Can not authenticate DB",key
                sys.exit(0)
        return self.pw[key]


    def GetAtlasRunDBConnection(self):
        if not 'run' in self.openConn:
            auth = self.get_auth('oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_TRIGGER')
            self.openConn['run'] = cx_Oracle.connect("%s/%s@ATLAS_COOLPROD" % (auth['user'],auth['password']))
        return self.openConn['run']

    def GetSFODBConnection(self):
        if not 'sfo' in self.openConn:
            #auth = self.get_auth('oracle://ATLAS_CONFIG/ATLAS_SFO_T0')
            #self.openConn['sfo'] = cx_Oracle.connect("%s/%s@ATLAS_CONFIG" % (auth['user'],auth['password']))
            with timer("Opening Connection to ATLAS_SFO_T0_R @ ATLAS_CONFIG"):
                self.openConn['sfo'] = cx_Oracle.connect("ATLAS_SFO_T0_R/readmesfotz2008@ATLAS_CONFIG")
        return self.openConn['sfo']

    def GetTier0DBConnection(self):
        if not 'tier0' in self.openConn:
            #auth = self.get_auth('oracle://ATLAS_T0/ATLAS_T0')
            #self.openConn['tier0'] = cx_Oracle.connect("%s/%s@ATLAS_T0" % (auth['user'],auth['password']))
            self.openConn['tier0'] = cx_Oracle.connect("ATLAS_T0_R1/titft0ra@ATLAS_T0")
        return self.openConn['tier0']

    def GetPVSSDBConnection(self):
        if not 'pvss' in self.openConn:
            #auth = self.get_auth('oracle://ATLAS_PVSSPROD/ATLAS_PVSS_READER')
            #self.openConn['pvss'] = cx_Oracle.connect("%s/%s@ATLAS_PVSSPROD" % (auth['user'],auth['password']))
            self.openConn['pvss'] = cx_Oracle.connect("ATLAS_PVSS_READER/PVSSRED4PRO@ATLAS_PVSSPROD")
        return self.openConn['pvss']

    def CloseAll(self):
        for (dbname,dbconn) in self.openConn.items():
            if isinstance(dbconn,cx_Oracle.Connection):
                dbconn.close()
            else:
                dbconn.closeDatabase()


coolDbConn = DBConnectionController()

def addKommaToNumber(no):
    gr = [str(no)]
    if not gr[0].isdigit(): return gr[0]
    while gr[0]:
        gr[0:1]=[gr[0][:-3],gr[0][-3:]]
    return ','.join(gr[1:])

def filesize(no):
    if no == None: return "n.a."
    if no>0x4000000000000: return "%1.1f&nbsp;PB" % (no*1.0/0x4000000000000)
    if no>0x10000000000:   return "%1.1f&nbsp;TB" % (no*1.0/0x10000000000)
    if no>0x40000000:      return "%1.1f&nbsp;GB" % (no*1.0/0x40000000)
    if no>0x100000:        return "%1.1f&nbsp;MB" % (no*1.0/0x100000)
    if no>0x400:           return "%1.1f&nbsp;kB" % (no*1.0/0x400)
    return "%i&nbsp;B" % no


# a helper function to decode range lists and merge the intervals
# par: listOfRanges: [(b1,e1),(b2,e2),....,(bn,en)]  ->   [(b1,e2),....,(bn,en)] if b2-1<=e1
def MergeRanges(listOfRanges):
    listOfRanges.sort()
    newRR=[]
    for rr in listOfRanges:
        if len(newRR)==0 or rr[0]-1>newRR[-1][1]:
            newRR.append(copy(rr))
        else:
            newRR[-1] = [newRR[-1][0], max(rr[1],newRR[-1][1]) ]
    return newRR

# a helper function to decode range lists and merge the intervals (like above)
# par: listOfRanges: [(b1,e1],(b2,e2],....,(bn,en]]  ->   [(b1,e2],....,(bn,en]] if b2<=e1
def MergeLBRanges(listOfRanges):
    listOfRanges.sort()
    newRR=[]
    for rr in listOfRanges:
        if len(newRR)==0 or rr[0]>newRR[-1][1]:
            newRR.append(copy(rr))
        else:
            newRR[-1] = (newRR[-1][0], max(rr[1],newRR[-1][1]) )
    return newRR

class Matrix(object):

    def __init__(self, cols, rows):
        self.cols = cols
        self.rows = rows
        # initialize matrix and fill with zeroes
        self.matrix = []
        for i in range(rows):
            ea_row = []
            for j in range(cols):
                ea_row.append(0)
            self.matrix.append(ea_row)

    def setitem(self, col, row, v):
        self.matrix[col-1][row-1] = v

    def getitem(self, col, row):
        return self.matrix[col-1][row-1]

    def __repr__(self):
        outStr = ""
        for i in range(self.rows):
            outStr += 'Row %s = %s\n' % (i+1, self.matrix[i])
        return outStr

    def __iter__(self):
        for row in range(self.rows):
            for col in range(self.cols):
                yield (self.matrix, row, col)  

def stringToIntOrTime(s):
    if re.match("^\d+$",s): return int(s)
    # convert the string into seconds since epoch

    # string is meant to be UTC time (hence we need calendar.timegm
    # and not time.mktime to convert the tuple into seconds)
    t = int(1E9*calendar.timegm(time.strptime(s,"%d.%m.%Y")))
    #print "turning",s,"into time",t
    #print "cross-check:",time.strftime("%d.%m.%Y %H:%M:%S",time.gmtime(t/1E9))
    return t

def timeStringToSecondsUTC(t):
    """ convert string into seconds since epoch

    string is meant to be UTC time (hence we need calendar.timegm
    and not time.mktime to convert the tuple into seconds)

    format can be '1.5.2010_14:23:10', '1.5.2010 14:23:10', or '1.5.2010'
    """
    try:
        t = time.strptime(t,"%d.%m.%Y")
    except ValueError:
        try:
            t = time.strptime(t,"%d.%m.%Y %H:%M:%S")
        except:
            t = time.strptime(t,"%d.%m.%Y_%H:%M:%S")

    return int(calendar.timegm(t))

def timeStringToSecondsLocalTime(t):
    """ convert string into seconds since epoch

    string is meant to be in local time (hence we use time.mktime
    and not calender.timegm to convert the tuple into seconds)

    format can be '1.5.2010_14:23:10', '1.5.2010 14:23:10', or '1.5.2010'
    """
    try:
        t = time.strptime(t,"%d.%m.%Y")
    except ValueError:
        try:
            t = time.strptime(t,"%d.%m.%Y %H:%M:%S")
        except:
            t = time.strptime(t,"%d.%m.%Y_%H:%M:%S")

    return int(time.mktime(t))

def secondsToTimeStringUTC(s):
    # convert seconds since epoch into utc
    return time.strftime("%d.%m.%Y %H:%M:%S",time.gmtime(s))


def secondsToTimeStringLocaltime(s):
    # convert seconds since epoch into localtime
    return time.strftime("%d.%m.%Y %H:%M:%S",time.localtime(s))



def GetRanges(rangestr, intRepFnc=stringToIntOrTime, maxval=1<<30):
    if type(rangestr)==list:
        ranges = rangestr
    else:
        if rangestr[:4]=='STR:':
            return [ [rangestr[4:], rangestr[4:]] ]
        ranges = rangestr.split(',')
    listOfRanges = []
    for r in ranges:
        if r == '-':
            listOfRanges += [[0,maxval]]
        elif not ('-' in r or '+' in r):
            # single value
            x = intRepFnc(r)
            listOfRanges += [[x,x]]
        elif r[-1] == '+':
            listOfRanges += [[intRepFnc(r[:-1]),maxval]]
        elif r[-1] == '-':
            listOfRanges += [[0,intRepFnc(r[:-1])]]
        else:
            startend = r.split('-')
            if len(startend)!=2:
                raise RuntimeError, "Range format error '%s'" % r
            listOfRanges += [[intRepFnc(x) for x in startend]]
    return MergeRanges(listOfRanges)




def full_time_string(s,startofday):
    try:
        time.strptime(s,"%d.%m.")
        year=str(time.gmtime().tm_year)
        return s+year+"_00:00:00" if startofday else s+year+"_23:59:59"
    except:
        try:
            time.strptime(s,"%d.%m.%Y")
            return s+"_00:00:00" if startofday else s+"_23:59:59"
        except:
            return s

def GetTimeRanges(timeranges, intRepFnc=timeStringToSecondsUTC, maxval=1<<30):
    if type(timeranges)==list:
        timeranges = ','.join(timeranges)

    timeranges = timeranges.split(',')
    listOfRanges = []
    listOfRangesHR = []

    for r in timeranges:
        start = 0
        end = 'inf'
        if r == '-':
            listOfRangesHR += [['0','inf']]
        elif not ('-' in r or '+' in r):
            # single value
            start = full_time_string( r, startofday=True )
            end   = full_time_string( r, startofday=False )
        elif r[-1] == '+':
            start = full_time_string( r[:-1], startofday=True )
        elif r[-1] == '-':
            end   = full_time_string( r[:-1], startofday=False )
        else:
            try:
                start,end = r.split('-')
            except:
                raise RuntimeError, "Time format '%s' wrong, should be 'from-until'" % r
            start = full_time_string( start, startofday=True )
            end   = full_time_string( end, startofday=False )

        listOfRangesHR += [[start,end]]
        start = 0 if start==0 else intRepFnc(start)
        end   = maxval if end=='inf' else intRepFnc(end)
        listOfRanges += [[start,end]]

    return MergeRanges(listOfRanges),listOfRangesHR


def SmartRangeCalulator(runlist,singleRuns=False):
    if len(runlist) == 0:
        return []
    if type(runlist[0]) == type([]):
        return runlist
    if singleRuns:
        rr = [[r.runNr,r.runNr] for r in runlist]
    else:
        rr = [[runlist[0].runNr,runlist[-1].runNr]]
    return rr

# code from Eric Torrence
# Optional argument to nval to specify number of values to read
def bConvertList(b, nbyte=1, nval=1):
    # routine to store an unsigned int (1, 2, 4 or 8 byte) in a blob
    packopt=dict([[1,'B'],[2,'H'],[4,'f'],[8,'d']])
    if nbyte in packopt:
        # print 'bConvert - b:[', b[0:nbyte], '] nbyte:', nbyte, ' fmt:', packopt[nbyte], type(b)
        fmt = '%d%s' % (nval, packopt[nbyte])
        ival=struct.unpack(fmt, b[0:nval*nbyte])
    else:
        print 'bConvertList: Unrecognized pack option'
        sys.exit()
    return list(ival)


def unpackRun1BCIDMask(blob, nb1, nb2, nbc):
    size = nb1+nb2+nbc
    a = bConvertList(blob, 2, size)
    beam1 = a[0:nb1]
    beam2 = a[nb1:nb1+nb2]
    coll = a[nb1+nb2:]
    #print 'unpackRun1BCIDMask found:'
    #print ' Beam1:', beam1
    #print ' Beam2:', beam2
    #print ' Coll: ', coll
    return  beam1, beam2, coll


def unpackRun2BCIDMask(blob):
    beam1=[]
    beam2=[]
    coll=[]
    rawData = bConvertList(blob, 1, 3564)
    for i in range(3564):
        val = rawData[i]
        if val & 0x01:
            beam1.append(i)
        if val & 0x02: 
            beam2.append(i)
        if (val & 0x03) == 0x03:
            coll.append(i)
    #print 'unpackRun2BCIDMask found:'
    #print ' Beam1:', beam1
    #print ' Beam2:', beam2
    #print ' Coll: ', coll
    return beam1, beam2, coll



def Poisson( n, mu ):
    # only valid for small mu and integer n
    if   n < 0 : return 0
    else:
        p = exp(-mu)
        for i in xrange(n):
            p *= mu
            p /= float(i+1)
        return p

def ComputePileup( lumi_mb, sigma, nbunch, fOrbit ):
    # sanity
    if nbunch <= 0: return 0

    # compute first probability of interaction    
    nint = lumi_mb*sigma/fOrbit/nbunch
    if nint > 100: return nint

    # small 'nint', compute poisson probabilities
    p = []
    for n in xrange(40):
        p.append(Poisson(n,nint))
        if n > 20 and p[-1] < 1e40: break
    return nint, p

def Pileup( args ):
    # orbit revolution frequency
    fOrbit = 11245.511 # [Hz] given by: 40.0790e6 Hz (bunch frequency) / 3564 (nbunches)

    if len(args) < 3:
        return '<font color="red">ERROR: need at least 3 arguments for pileup calculation: inst.-lumi cross-section-in-mb nbunch [nevents]</font>'
    try:    
        fstarg = float(args[0])
        if fstarg < 1e20:
            # input expects first two arguments to be number of protons and transverse beam size
            nprotons = fstarg
            sigtrans = float(args[1]) # given in microns
            sigtrans *= 1e-4          # transform to cm
            
            sigma    = float(args[2])
            nbunch   = float(args[3])
            lumi     = nprotons**2 * fOrbit * nbunch / (4.0 * pi * sigtrans**2)
            nevents  = 1
            if len(args) == 5: nevents = float(args[4])
        else:
            # input expects first argument to be luminosity in cm-2s-1
            lumi     = float(args[0])
            sigma    = float(args[1])
            nbunch   = float(args[2])
            nevents  = 1
            if len(args) == 4: nevents = float(args[3])

        # compute pileup
        lumi_mb = lumi/1.e27  # luminosity in mb-1s-1
        pint, plist = ComputePileup( lumi_mb, sigma, nbunch, fOrbit )

        # create output string        
        s = ' '
        s += '<table class="datasettable" style="font-size: 140%">'
        s += '<tr><td>Instantaneous luminosity :</td><td> %g cm&minus;2s&minus;1 (= %g mb&minus;1s&minus;1)</td></tr>' % (lumi, lumi_mb)
        s += '<tr><td>Inelastic cross section :</td><td> %g mb</td></tr>' % sigma
        s += '<tr><td>Number of colliding bunches :</td><td> %g</td></tr>' % nbunch
        s += '<tr><td colspan="2"><hr style="width:100%; #999999; background-color: #999999; height:1px; margin-left:0px; border:0"></td></tr>'
        s += '<tr><td>Inelastic interaction rate:</td><td>%g Hz</td></tr>' % (lumi_mb*sigma)
        s += '<tr><td>Average number of interactions per crossing:&nbsp;&nbsp;</td><td> %g</td></tr>' % pint
        s += '</table>'
        s += '<hr style="width:100%; #999999; background-color: #999999; height:0px; border:0">\n<p>\n'
        if not plist:        
            p = plist[0]
            s += 'Very large pileup probability (assume Gaussian distribution): %g +- %g' % (p, sqrt(p))
        else:
            s += '<table class="pileuptable">'
            s += '<tr><th>Num. of interactions per filled bunch crossing</th><th>Probability per filled bunch crossing</th><th>Probability per triggered minimum bias event*</th>'
            if nevents>1:
                s += '<th>Expected number of events in sample*</th>'
            s += '</tr>'

            pref = 1.0-plist[0] # probability for a zero bias trigger to have triggered an event
            psum = 0
            for i,p in enumerate(plist):
                if i>=1:
                    s += '<tr><td>&gt;= %i</td><td>%g</td><td>%g</td>' % (i,1.0-psum,(1.0-psum)/pref)
                    if nevents > 1:
                        nevexp = (1.0-psum)/pref*nevents
                        s += '<td> %g</td>' % (nevexp)
                    s += '</tr>'
                if p < 1e-15: break
                psum += p                
            s += '</table><p></p>'
            s += '<font size=-2>*assuming 100% trigger efficiency for inelastic events</font><br>'

        return s

    except ValueError:
        return '<font color="red">ERROR: only numerical arguments allowed</font>'


def get_run_range(start,end=None):
    runs = get_runs(start,end)
    if len(runs) == 0: return None
    if len(runs) == 1: return (runlist[0],runlist[0])
    return (runs[0],runs[-1])

def get_runs_last_dt(last):
    lt = last.replace('last','').strip()
    nsec = durationInSeconds( lt )
    start = time.gmtime( time.mktime(time.gmtime()) - nsec  )
    return get_runs(time.strftime("%d.%m.%Y_%H:%M:%S",start))

def get_runs(start,end=None):
    """start and end are given in the format '1.5.2010_14:23:10', '1.5.2010 14:23:10', or '1.5.2010'"""

    co = coolDbConn.GetAtlasRunDBConnection()
    cu = co.cursor()

    #available:
    # NAME,RUNNUMBER,STARTAT,DURATION,CREATEDBY,HOST,PARTITIONNAME,CONFIGSCHEMA,CONFIGDATA,COMMENTS
    records = 'RUNNUMBER'

    t = time.gmtime( timeStringToSecondsUTC(start) )
    starttime = time.strftime("%Y%m%dT%H%M%S",t)

    if not end:
        q = "SELECT %s FROM ATLAS_RUN_NUMBER.RUNNUMBER WHERE STARTAT>'%s' ORDER BY RUNNUMBER desc" % (records,starttime)
    else:
        t = time.gmtime( timeStringToSecondsUTC(end) )
        endtime = time.strftime("%Y%m%dT%H%M%S",t)
        q = "SELECT %s FROM ATLAS_RUN_NUMBER.RUNNUMBER WHERE STARTAT>'%s' and STARTAT<'%s' ORDER BY RUNNUMBER" % (records,starttime,endtime)

    cu.arraysize=100
    cu.execute(q)

    res = cu.fetchall()
    return [r[0] for r in res]


def get_run_range2(start,end=None):
    """start and end are given in the format '1.5.2010_14:23:10', '1.5.2010 14:23:10', or '1.5.2010'"""

    co = coolDbConn.GetAtlasRunDBConnection()
    cu = co.cursor()

    #available:
    # NAME,RUNNUMBER,STARTAT,DURATION,CREATEDBY,HOST,PARTITIONNAME,CONFIGSCHEMA,CONFIGDATA,COMMENTS

    t = time.gmtime( timeStringToSecondsUTC(start) )
    sstart = time.strftime("%Y%m%dT%H%M%S",t)

    # last run that started before the begin of the range
    subq = "SELECT MAX(RUNNUMBER) FROM ATLAS_RUN_NUMBER.RUNNUMBER WHERE STARTAT<'%s'" % sstart
    q = "select RUNNUMBER,STARTAT,DURATION from ATLAS_RUN_NUMBER.RUNNUMBER where RUNNUMBER=(%s)" % subq
    cu.execute(q)
    try:
        run1,startat,duration = cu.fetchone()
        # note that duration is not exact, but always a bit larger than the length of the run
    
        endat = calendar.timegm( time.strptime(startat,"%Y%m%dT%H%M%S") ) + duration
        if end < timeStringToSecondsUTC(start):
            q = "SELECT MIN(RUNNUMBER) FROM ATLAS_RUN_NUMBER.RUNNUMBER WHERE STARTAT>'%s'" % sstart
            cu.execute(q)
            try:
                run1 = cu.fetchone()[0]
            except:
                run1 = None
    except:
        run1 = None

    run2 = None
    if end != None:
        # last run that started before the end of the range
        t = time.gmtime( timeStringToSecondsUTC(end) )
        endtime = time.strftime("%Y%m%dT%H%M%S",t)
        q = "SELECT MAX(RUNNUMBER) FROM ATLAS_RUN_NUMBER.RUNNUMBER WHERE STARTAT<'%s'" % endtime
        cu.execute(q)
        run2 = cu.fetchone()[0]

    return (run1,run2)



class XMLReader(object):
    class XMLElement:
        def __init__(self,element):
            self.element = element
            self.tag = element.tag
            self.attributes = dict(element.items())
            self.children = element.getchildren()
            self.readchildren()

        def items(self):
            return self.attributes.items()
        def __str__(self):
            return "<%s %s>" % (self.tag, " ".join(['%s="%s"' % x for x in self.items()]))
        def __repr__(self):
            return self.tag
        def __getitem__(self,k):
            if not k in self.attributes:
                raise KeyError, "'%s'. XML element '%s' has attributes %s" % (k,self.tag, self.attributes.keys())
            return self.attributes[k]

        def readchildren(self):
            self.childtags = []
            self._childtagdict = {}
            for c in self.children:
                self._childtagdict.setdefault(c.tag,[]).append(XMLReader.XMLElement(c))
                if not c.tag in self.childtags: self.childtags += [c.tag]
            for t in self.childtags:
                self.__dict__['%ss'%t] = self._childtagdict[t]
                if len(self._childtagdict[t])==1:
                    self.__dict__['%s'%t] = self._childtagdict[t][0]

        def __getattr__(self,name):
            raise AttributeError, "'%s'. XML element '%s' has tags %s" % (name,self.tag, ["%ss" % t for t in self.childtags])


    def __init__(self,filename):
        import xml.etree.cElementTree as ET
        self.doc = ET.parse(filename)
        root = XMLReader.XMLElement(self.doc.getroot())
        self.__filename = filename
        self.__root = root
        self.__dict__[root.tag] = root

    def __getattr__(self,name):
        raise AttributeError, "'%s'. XML document '%s' has root tag '%s'" % (name,self.__filename, self.__root.tag)
