#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryTier0.py
# Project: AtlRunQuery
# Purpose: Utility to retrieve information from Tier0 DB
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 4, 2009
# ----------------------------------------------------------------
#
# Tier-0 Schema (dataset class only)
# ==============================================
#
# http://hoecker.home.cern.ch/hoecker/Tier0DatasetSchema.png
#

import cx_Oracle
from time import time

def GetTier0_allDatasets( cursor, runlist, dsnamepattern = [] ):
    res = {}
    pos = 0
    blocksize = 200
    # build dataset selection string
    dsselstring = ''
    for p in dsnamepattern:
        p = p.replace('*','%')
        dsselstring += "LOWER(DATASETNAME) like LOWER('%s') OR " % p
    if dsselstring: dsselstring = 'and (' + dsselstring[:len(dsselstring)-4] + ')'

    # do selection
    while pos<len(runlist):
        cursor.execute("SELECT DISTINCT RUNNR,DATASETNAME,TYPE,PSTATES,DDM,NFILES,TOTSIZE,TOTEVENTS,CREATIONTIME FROM dataset WHERE RUNNR in (%s) and TYPE!='LOG' %s and not DATASETNAME like '%%.LOG%%'"% (','.join([str(i) for i in runlist[pos:pos+blocksize]]), dsselstring))
        r = cursor.fetchall()
        for e in r:
            res.setdefault(e[0],[]).append((e[1],e[2],e[3],e[4],e[5],e[6],e[7],e[8]))
        pos += blocksize

    return res

def GetTier0_datasetsAndTypes( cursor, runlist ):
    res = {}
    pos = 0
    blocksize = 200
    while pos<len(runlist):
        cursor.execute("SELECT DISTINCT RUNNR,DATASETNAME,TYPE,PSTATES FROM dataset WHERE RUNNR in (%s) and TYPE!='LOG' and not DATASETNAME like '%%.LOG%%'"% ','.join([str(i) for i in runlist[pos:pos+blocksize]]))
        r = cursor.fetchall()
        for e in r:
            res.setdefault(e[0],[]).append((e[1],e[2],e[3]))
        pos += blocksize

    return res
