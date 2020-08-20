#!/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# ----------------------------------------------------------------
# Script : GetNEventsLB.py
# Purpose: Utility to retrieve number of events per LB from SFO DB
# Authors: Andreas Hoecker (CERN)
# Created: Aug 17, 2011
# ----------------------------------------------------------------

from __future__ import print_function

import sys, getopt

def GetSFO_LBNEvents( cursor, runno, streamname ):
    """returns nfiles, fsize, nevents"""
    stype, sep, sname = streamname.partition('_')
    cursor.execute( "SELECT SUM(NREVENTS),LUMIBLOCKNR,COUNT(FILESIZE),SUM(FILESIZE) FROM SFO_TZ_File WHERE RUNNR=:arg_1 and STREAMTYPE=:arg_2 and STREAM=:arg_3 GROUP BY LUMIBLOCKNR ORDER BY LUMIBLOCKNR",
                    arg_1=runno, arg_2=stype, arg_3=sname )
    return cursor.fetchall()

def GetSFO_LBs( cursor, runno ):
    cursor.execute( "SELECT MIN(LUMIBLOCKNR), MAX(LUMIBLOCKNR) FROM SFO_TZ_Lumiblock WHERE RUNNR=:arg_1", arg_1=runno )
    return cursor.fetchone()[0:2]

def OpenSFOConnection():
    import cx_Oracle
    return cx_Oracle.connect("ATLAS_SFO_T0_R/readmesfotz2008@atlr")

def usage():
    print(' ')
    print('Usage: %s [options]' % sys.argv[0])
    print('  -r | --run      : run number (REQUIRED)')
    print('  -s | --stream   : full stream name (REQUIRED)')
    print('  -h | --help     : print this usage message')
    print(' ')
    exit(1)

def main():

    try:
        shortopts  = "r:s:h?"
        longopts   = ["run=", "stream=", "help", "usage"]
        opts, args = getopt.getopt( sys.argv[1:], shortopts, longopts )

    except getopt.GetoptError:
        # print help information and exit:
        print('ERROR: unknown options in argument %s' % sys.argv[1:], file=sys.stderr)
        usage()

    runno   = None
    stream  = None
    for o, a in opts:
        if o in ("-?", "-h", "--help", "--usage"):
            usage()
        elif o in ("-r", "--run"):
            runno = a
        elif o in ("-s", "--stream"):
            stream = a

    if not runno or not stream: usage()
    runno = int(runno)

    connection = OpenSFOConnection()
    cursor     = connection.cursor()

    print('Results for run: %i, stream: "%s"' % (runno, stream))

    # min/max LB number for given run/stream
    # --> this command is slow... don't know why
    # minLB, maxLB = GetSFO_LBs( cursor, runno )
    # print 'LB range: %i -- %i' % (minLB, maxLB)

    # list with
    lblist = GetSFO_LBNEvents( cursor, runno, stream )
    print('First non-zero LB: ' + str(lblist[0][1]))
    print('Last  non-zero LB: ' + str(lblist[-1][1]))

    # access to all LBs
    sumnev    = 0
    sumnfiles = 0
    sumfsize  = 0
    for (nev,lb,nfiles,fsize) in lblist:
        fsize /= 1.e6
        print(' ... Run %i, LB %i has %i events, %i RAW files and %f MB' % (runno,lb,nev,nfiles,fsize))
        sumnev    += nev
        sumnfiles += nfiles
        sumfsize  += fsize
    print('--------------------------------------------------')
    print('Total #events : ', sumnev)
    print('Total #files  : ', sumnfiles)
    print('Total RAW size: ', sumfsize/1000.0, ' GB')

    cursor.close()
    connection.close()

if __name__ == '__main__':
    main()
