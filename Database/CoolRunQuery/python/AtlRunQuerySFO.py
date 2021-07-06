#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

from __future__ import with_statement, print_function
from CoolRunQuery.utils.AtlRunQueryCache import Cache
from CoolRunQuery.AtlRunQueryRun import Run

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
            if e[0] is not None:
                res[run] = e[0]
    return res




def GetSFO_lastNruns( cursor, nruns ):
    """returns list of last 'nruns' run numbers, where at least one stream is not calibration"""
    cursor.execute( "SELECT RUNNR,STATE FROM (SELECT UNIQUE RUNNR,STATE FROM SFO_TZ_RUN WHERE STREAMTYPE!='calibration' AND RUNNR < 999999 ORDER BY RUNNR DESC) SFO_TZ_RUN2 WHERE rownum<=:arg_1", arg_1=nruns )
    return cursor.fetchall()



##############################################################
#
# The following function is used to get OKS info
#
##############################################################

def SetOKSLinks( runlist ):
    from CoolRunQuery.utils.AtlRunQueryUtils import coolDbConn
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
        #print (coolDbConn.get_auth('oracle://ATLAS_COOLPROD/ATLAS_COOLOFL_TRIGGER'))
        from CoolRunQuery.AtlRunQuerySFO import SetOKSLinks
        SetOKSLinks([Run(178211)])

    if test_sfo:

        runno = 122050
        import sys
        if len(sys.argv)>1:
            runno = int(sys.argv[1])

        runno = [140541, 140571, 140579, 140592, 140616, 140620,
        140622, 140638, 140670, 140682, 140704, 140737, 140747,
        140748, 140754, 140762, 140765, 140769, 140772, 140776,
        140790, 140794, 140822, 140836, 140842, 140929, 140953,
        140955, 140974, 140975, 141046, 141059, 141066, 141079,
        141109, 141150, 141189, 141192, 141194, 141203, 141209,
        141226, 141234, 141236, 141237, 141238, 141266, 141270,
        141359, 141374, 141387, 141398, 141401, 141403, 141461,
        141473, 141474, 141525, 141527, 141529, 141533, 141534,
        141561, 141562, 141563, 141565, 141599, 141624, 141655,
        141667, 141670, 141688, 141689, 141691, 141695, 141700,
        141702, 141704, 141705, 141706, 141707, 141718, 141721,
        141730, 141746, 141748, 141749, 141755, 141769, 141807,
        141811, 141818, 141841, 141915, 141928, 141976, 141979,
        141994, 141998, 141999, 142042, 142065, 142081, 142091,
        142094, 142111, 142123, 142125, 142128, 142133, 142144,
        142149, 142154, 142155, 142157, 142159, 142161, 142165,
        142166, 142171, 142174, 142183, 142185, 142187, 142189,
        142190, 142191, 142192, 142193, 142194, 142195, 142199,
        142203, 142205, 142210, 142214, 142216, 142240, 142258,
        142259, 142265, 142291, 142301, 142308, 142309, 142310,
        142319, 142356, 142368, 142383, 142390, 142391, 142392,
        142394, 142395, 142397, 142400, 142401, 142402, 142403,
        142404, 142405, 142406, 143019, 143023, 143027, 143033,
        143034, 143131, 143136, 143143, 143163, 143169, 143171,
        143178, 143182, 143185, 143190, 143192, 143198, 143203,
        143204, 143205, 143207, 143210, 143218, 143222, 143225,
        143236, 143242]

        #runno = range(141000,143000)


        from CoolRunQuery.utils.AtlRunQueryUtils import coolDbConn
        connection = coolDbConn.GetSFODBConnection()
        cursor     = connection.cursor()

        # find last N runs
        if False:
            n = 10
            runs = GetSFO_lastNruns( cursor, n )
            print ('Last %i runs:' % n)
            for r in runs:
                print ('... %i' % r)
            sys.exit()

        # retrieve streams
        if True:
            start = time()
            GetSFO_streamsAll( cursor, runno )
            print ("streams",time()-start)
            start = time()
            GetSFO_LBsAll    ( cursor, runno )
            print ("lbs",time()-start)
            start = time()
            GetSFO_NeventsAll( cursor, runno )
            print ("events",time()-start)
            start = time()
            GetSFO_filesAll  ( cursor, runno )
            print ("files",time()-start)
            start = time()
            GetSFO_overlapAll( cursor, runno )
            print ("overlap",time()-start)


        print ("Query execution time: %f sec" % (time()-start))

        cursor.close()
        connection.close()



if __name__ == '__main__':
    main()
