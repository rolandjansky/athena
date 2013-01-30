#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQuerySFO.py
# Project: AtlRunQuery
# Purpose: Utility to retrieve information from SFO DB
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 13, 2008
# ----------------------------------------------------------------
#
# SFO DB schema:
# ==============================================
# 
# Run Table
# 
# "SFOID"        "VARCHAR2(32 Bytes)"
# "RUNNR"        "NUMBER(10,0)"
# "STREAMTYPE"   "VARCHAR2(32 Bytes)"
# "STREAM"       "VARCHAR2(32 Bytes)"
# "STATE"        "VARCHAR2(32 Bytes)" [OPENED,CLOSED,TRANSFERRED]
# "TZSTATE"      "VARCHAR2(32 Bytes)" filled by TZ
# "DSNAME"       "VARCHAR2(256 Bytes)"
# "T_STAMP"      "TIMESTAMP(0)"  "RUN_SEQNUM"    "NUMBER(10,0)"
# 
# Lumiblock Table
# 
# "SFOID"        "VARCHAR2(32 Bytes)"
# "RUNNR"        "NUMBER(10,0)"
# "LUMIBLOCKNR"  "NUMBER(10,0)"
# "STREAMTYPE"   "VARCHAR2(32 Bytes)"
# "STREAM"       "VARCHAR2(32 Bytes)"
# "STATE"        "VARCHAR2(32 Bytes)" [OPENED,CLOSED,TRANSFERRED]
# "TZSTATE"      "VARCHAR2(32 Bytes)" filled by TZ
# "T_STAMP"      "TIMESTAMP(0)"  "LUMIBLOCK_SEQNUM"    "NUMBER(10,0)"
# 
# File Table
# 
# "LFN"          "VARCHAR2(256 Bytes)" filename
# "FILENR"       "NUMBER(10,0)"
# "SFOID"        "VARCHAR2(32 Bytes)"
# "RUNNR"        "NUMBER(10,0)"
# "LUMIBLOCKNR"  "NUMBER(10,0)"
# "STREAMTYPE"   "VARCHAR2(32 Bytes)"
# "STREAM"       "VARCHAR2(32 Bytes)"
# "SFOHOST"      "VARCHAR2(64 Bytes)"
# "TRANSFERSTATE" "VARCHAR2(32 Bytes)" [ONDISK,TRANSFERRED]
# "FILESTATE"    "VARCHAR2(32 Bytes)" [OPENED,CLOSED,DELETED]
# "FILEHEALTH"   "VARCHAR2(32 Bytes)" [SANE,TRUNCATED]
# "TZSTATE"      "VARCHAR2(32 Bytes)" filled by TZ
# "FILESIZE"     "NUMBER(10,0)"
# "GUID"         "VARCHAR2(64 Bytes)"
# "CHECKSUM"     "VARCHAR2(64 Bytes)" Adler-32
# "PFN"          "VARCHAR2(1024 Bytes)" /path/filename on Castor
# "SFOPFN"       "VARCHAR2(1024 Bytes)" /path/filename on SFO
# "NREVENTS"     "NUMBER(10,0)"
# "T_STAMP"      "TIMESTAMP(0)" "ENDTIME"    "TIMESTAMP(0)" UTC time
# "FILE_SEQNUM"  "NUMBER(10,0)"
# 
# Index Table
# 
# "SEQNAME"      "VARCHAR2(64 Bytes)"
# "SEQINDEX"     "NUMBER(10,0)"
# "LASTMOD"      "TIMESTAMP(0)"
#
# Schema of new OVERLAP table:
# 
# "SFOID" 		VARCHAR2(32 BYTE)
# "RUNNR" 		NUMBER(10,0)
# "REFERENCE_STREAM" 	VARCHAR2(128 BYTE)
# "OVERLAP_STREAM" 	VARCHAR2(128 BYTE)
# "OVERLAP_EVENTS"	NUMBER(10,0)
# "OVERLAP_RATIO" 	BINARY_FLOAT
# "TYPE" 			VARCHAR2(32 BYTE) [RAW, FILE]
# "T_STAMP" 		TIMESTAMP (0)
# "OVERLAP_SEQNUM" 	NUMBER(10,0)
# 
# 
# Each row provides the overlap for a stream pair (in a given run and
# SFO). The pair is composed by a reference stream and an overlap stream.
# We can distinguish between two cases:
# 
# a) reference stream = overlap stream
# The row provides the total number of events in the stream
# (OVERLAP_EVENTS) and OVERLAP_RATIO is 1
# 
# b) reference stream != overlap stream
# The row provides the overlap between the two streams (OVERLAP_EVENTS).
# OVERLAP_RATIO is defined as OVERLAP_EVENTS/#(events for reference stream).
# 
# The attached screenshot is an example of the expected result (with two
# streams only).
# 
# 
# The TYPE column is needed to distinguish the source level of the counters:
# 
# RAW: raw streams at the input of the SFO
# FILE: resulting streams after the SFO stream processing
# (inclusive/exclusive, late, ...)
#  

from __future__ import with_statement
from utils.AtlRunQueryTimer import timer

from utils.AtlRunQueryCache import Cache
from .AtlRunQueryRun import Run

import cx_Oracle
from time import time
from collections import defaultdict

@Cache("cache/sfo","%s_run_%i")
def executeCachedQuery(run,cu,q,cachekey):
    cu.execute(q, run=run)
    return cu.fetchall()

def executeQuery(run,cu,q,cachekey):
    cu.execute(q, run=run)
    return cu.fetchall()



##############################################################

# The following 5 methods are used in the stream selector

##############################################################

def GetSFO_streamsAll( cursor, runlist ):
    res = defaultdict(list)
    q = "SELECT DISTINCT STREAMTYPE, STREAM FROM SFO_TZ_RUN WHERE RUNNR=:run"
    cursor.prepare(q)

    for run in runlist:
        cursor.execute(q,run=run)
        qr = cursor.fetchall()
        for e in qr:
            res[run].append("%s_%s" % (e[0],e[1]))

    return res


def GetSFO_filesAll( cursor, runlist ):
    """returns nfiles, fsize, nevents"""
    res = defaultdict(dict)
    q = "SELECT STREAMTYPE, STREAM, COUNT(FILESIZE), SUM(FILESIZE), SUM(NREVENTS) FROM SFO_TZ_File WHERE RUNNR = :run GROUP BY STREAM, STREAMTYPE"
    cursor.prepare(q)

    for run in runlist:
        if run==Run.runnropen:
            qr = executeQuery(run, cursor, q, cachekey=("filesAll",run))
        else:
            qr = executeCachedQuery(run, cursor, q, cachekey=("filesAll",run))
        for e in qr:
            key = "%s_%s" % (e[0],e[1])
            res[run][key] = e[2:5] # #files, filesize, nevents
    return res



def GetSFO_LBsAll( cursor, runlist ):
    res = defaultdict(dict)
    q = "SELECT STREAMTYPE, STREAM, MIN(LUMIBLOCKNR), MAX(LUMIBLOCKNR), COUNT(DISTINCT LUMIBLOCKNR) FROM SFO_TZ_Lumiblock WHERE RUNNR=:run GROUP BY STREAM, STREAMTYPE"
    cursor.prepare(q)

    for run in runlist:
        if run==Run.runnropen:
            qr = executeQuery(run, cursor, q, cachekey=("LBsAll",run))
        else:
            qr = executeCachedQuery(run, cursor, q, cachekey=("LBsAll",run))
        for e in qr:
            key2 = "%s_%s" % (e[0],e[1])
            res[run][key2] = e[2:5] # minlb,maxlb,nlb
    return res



def GetSFO_NeventsAll( cursor, runlist ):
    # due to the amount of information oracle access is faster than file access
    res = {}
    q = "SELECT STREAMTYPE, STREAM, LUMIBLOCKNR, SUM(NREVENTS) FROM SFO_TZ_File WHERE RUNNR=:run GROUP BY STREAMTYPE, STREAM, LUMIBLOCKNR ORDER BY LUMIBLOCKNR"
    cursor.prepare(q)
    
    for run in runlist:
        if run==Run.runnropen:
            qr = executeQuery(run, cursor, q, cachekey=("NeventsAll",run))
        else:
            qr = executeCachedQuery(run, cursor, q, cachekey=("NeventsAll",run))
        res[run] = defaultdict(list)
        for e in qr:
            key2 = "%s_%s" % (e[0],e[1])
            res[run][key2].append(e[2:4]) # lbNr, NrEv

    return res


def GetSFO_overlapAll( cursor, runlist ):
    """returns number of overlap events"""
    res = {}
    q = "SELECT REFERENCE_STREAM, OVERLAP_STREAM, SUM(OVERLAP_EVENTS) FROM SFO_TZ_Overlap WHERE TYPE='FILE' and RUNNR=:run GROUP BY REFERENCE_STREAM, OVERLAP_STREAM"
    cursor.prepare(q)

    for run in runlist:
        if run==Run.runnropen:
            qr = executeQuery(run, cursor, q, cachekey=("overlapAll",run))
        else:
            qr = executeCachedQuery(run, cursor, q, cachekey=("overlapAll",run))
            
        res[run] = defaultdict(dict)
        for e in qr:
            res[run][e[0]][e[1]] = e[2] # overlap

    return res




##############################################################

# The following function is used by the XML file maker

# we store only information about physics streams

##############################################################


def GetSFO_NeventsAllPhysics( cursor, runlist ):
    res = {}
    q = "SELECT STREAM, LUMIBLOCKNR, SUM(NREVENTS) FROM SFO_TZ_File WHERE RUNNR=:run AND STREAMTYPE='physics' GROUP BY STREAM, LUMIBLOCKNR ORDER BY LUMIBLOCKNR"
    cursor.prepare(q)
    
    for run in runlist:
        if run==Run.runnropen:
            qr = executeQuery(run, cursor, q, cachekey=("NeventsAllPhysics",run))
        else:
            qr = executeCachedQuery(run, cursor, q, cachekey=("NeventsAllPhysics",run))
        res[run] = defaultdict(list)
        for e in qr:
            key2 = "physics_%s" % e[0]
            res[run][key2].append(e[1:3]) # lbNr, NrEv

    return res



##############################################################

# The following function is used by the EventSelector

##############################################################

def GetSFO_NphysicseventsAll( cursor, runlist ):
    res = {}
    q = "SELECT SUM(OVERLAP_EVENTS) from SFO_TZ_OVERLAP WHERE RUNNR=:run AND TYPE='EVENTCOUNT' AND REFERENCE_STREAM='physics_EventCount'"
    cursor.prepare(q)
    for run in runlist:
        if run==Run.runnropen or 1: # FIXME problem in cache
            qr = executeQuery(run, cursor, q, cachekey=("SumOverlap",run))
        else:
            qr = executeCachedQuery(run, cursor, q, cachekey=("SumOverlap",run))
        for e in qr:
            if e[0]!=None: res[run] = e[0]
    return res




def GetSFO_lastNruns( cursor, nruns ):
    """returns list of last 'nruns' run numbers, where at least one stream is not calibration"""
    cursor.execute( "SELECT RUNNR,STATE FROM (SELECT UNIQUE RUNNR,STATE FROM SFO_TZ_RUN WHERE STREAMTYPE!='calibration' AND RUNNR < 999999 ORDER BY RUNNR DESC) SFO_TZ_RUN2 WHERE rownum<=:arg_1", arg_1=nruns )
    return cursor.fetchall()

def runInfo(runno, streams):
    if streams==[]:
        print "No information available for run %i" % runno
        return

    print 'Output for run number: %i' % runno
    print '--------------------------------------------------------------------------------'
    print 'Streams:'
    print streams
    print ' '
    print ' '
    for s in streams:
        minlb, maxlb, lbs = GetSFO_LBs( cursor, runno, s )
        print 'For Stream: %25s: min - max LBs: %i - %i \t(Num: %i)' % (s, minlb, maxlb, lbs)

        result = GetSFO_Nevents( cursor, runno, s )
        lbold = -1
        allnev = 0
        for lb,nev in result:
            if lb != lbold:
                if lbold != -1: print s,'\t',lbold,'\t',allnev
                allnev = nev
                lbold = lb
            else:
                allnev += nev
            #for lb in range(minlb,maxlb+1):
            #nev = GetSFO_NeventsPerLB( cursor, runno, s, lb )
            #print '       - LB: %i has %s events' % (lb,nev)

    print ' '
    return
    totnf = totsize = totev = 0
    for s in streams:
        nfiles, size, events = GetSFO_files( cursor, runno, s )
        totnf   += nfiles
        totsize += size
        totev   += events
        print 'Stream %-25s : %i files, %.2f GB, %i events' % (s, nfiles, size/1.0e9, events)
    
    print '--------------------------------------------------------------------------------'
    print 'Total  %-25s : %i files, %.2f GB, %.2f mio events' % (' ', totnf, totsize/1.0e9, totev/1.0e6)
    print '--------------------------------------------------------------------------------'

    print 'Check overlaps for stream pairs'
    for i in range(0,len(streams)):
        nfilesi, sizei, eventsi = GetSFO_files( cursor, runno, streams[i] )
        for j in range(i,len(streams)):
            nfilesj, sizej, eventsj = GetSFO_files( cursor, runno, streams[i] )
            eventsij                = GetSFO_overlap( cursor, runno, streams[i], streams[j] )
            if eventsij:
                print '   [ %28s, %28s ] = %g' % (streams[i], streams[j], eventsij)
            else:
                print '   [ %28s, %28s ] = None' % (streams[i], streams[j])



##############################################################
#
# The following function is used to get OKS info
#
##############################################################

def SetOKSLinks( runlist ):
    from utils.AtlRunQueryUtils import coolDbConn
    conn = coolDbConn.GetAtlasRunDBConnection()
    cursor = conn.cursor()
    query = "select ConfigSchema,ConfigData from ATLAS_RUN_NUMBER.RunNumber where RunNumber=:run"
    cursor.prepare(query)
    for run in runlist:
        cursor.execute(query, run=run.runNr)
        re = cursor.fetchall()
        run.addResult('oks',re[0])

def main():
    test_oks = True
    test_sfo = False

    if test_oks:
        #from os import environ as env
        #from CoolRunQuery.AtlRunQueryUtils import coolDbConn
        #coolDbConn.get_auth('oracle://atlr/rn_r') # only in /afs/cern.ch/atlas/project/tdaq/databases/.coral/authentication.xml
        #print coolDbConn.get_auth('oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_TRIGGER')
        from CoolRunQuery.AtlRunQuerySFO import SetOKSLinks
        SetOKSLinks([Run(178211)])

    if test_sfo:

        runno = 122050
        import sys
        if len(sys.argv)>1:
            runno = int(sys.argv[1])

        runno = [140541L, 140571L, 140579L, 140592L, 140616L, 140620L,
        140622L, 140638L, 140670L, 140682L, 140704L, 140737L, 140747L,
        140748L, 140754L, 140762L, 140765L, 140769L, 140772L, 140776L,
        140790L, 140794L, 140822L, 140836L, 140842L, 140929L, 140953L,
        140955L, 140974L, 140975L, 141046L, 141059L, 141066L, 141079L,
        141109L, 141150L, 141189L, 141192L, 141194L, 141203L, 141209L,
        141226L, 141234L, 141236L, 141237L, 141238L, 141266L, 141270L,
        141359L, 141374L, 141387L, 141398L, 141401L, 141403L, 141461L,
        141473L, 141474L, 141525L, 141527L, 141529L, 141533L, 141534L,
        141561L, 141562L, 141563L, 141565L, 141599L, 141624L, 141655L,
        141667L, 141670L, 141688L, 141689L, 141691L, 141695L, 141700L,
        141702L, 141704L, 141705L, 141706L, 141707L, 141718L, 141721L,
        141730L, 141746L, 141748L, 141749L, 141755L, 141769L, 141807L,
        141811L, 141818L, 141841L, 141915L, 141928L, 141976L, 141979L,
        141994L, 141998L, 141999L, 142042L, 142065L, 142081L, 142091L,
        142094L, 142111L, 142123L, 142125L, 142128L, 142133L, 142144L,
        142149L, 142154L, 142155L, 142157L, 142159L, 142161L, 142165L,
        142166L, 142171L, 142174L, 142183L, 142185L, 142187L, 142189L,
        142190L, 142191L, 142192L, 142193L, 142194L, 142195L, 142199L,
        142203L, 142205L, 142210L, 142214L, 142216L, 142240L, 142258L,
        142259L, 142265L, 142291L, 142301L, 142308L, 142309L, 142310L,
        142319L, 142356L, 142368L, 142383L, 142390L, 142391L, 142392L,
        142394L, 142395L, 142397L, 142400L, 142401L, 142402L, 142403L,
        142404L, 142405L, 142406L, 143019L, 143023L, 143027L, 143033L,
        143034L, 143131L, 143136L, 143143L, 143163L, 143169L, 143171L,
        143178L, 143182L, 143185L, 143190L, 143192L, 143198L, 143203L,
        143204L, 143205L, 143207L, 143210L, 143218L, 143222L, 143225L,
        143236L, 143242L]

        #runno = range(141000,143000)


        from utils.AtlRunQueryUtils import coolDbConn
        connection = coolDbConn.GetSFODBConnection()
        cursor     = connection.cursor()

        # find last N runs
        if False:
            n = 10
            runs = GetSFO_lastNruns( cursor, n )
            print 'Last %i runs:' % n
            for r in runs:
                print '... %i' % r
            sys.exit()

        # retrieve streams
        if True:
            start = time()
            streams = GetSFO_streamsAll( cursor, runno )
            print "streams",time()-start
            start = time()
            lbs     = GetSFO_LBsAll    ( cursor, runno )
            print "lbs",time()-start
            start = time()
            nev     = GetSFO_NeventsAll( cursor, runno )
            print "events",time()-start
            start = time()
            files   = GetSFO_filesAll  ( cursor, runno )
            print "files",time()-start
            start = time()
            over    = GetSFO_overlapAll( cursor, runno )
            print "overlap",time()-start


        print "Query execution time: %f sec" % (time()-start)

        cursor.close()
        connection.close()



if __name__ == '__main__':
    main()
