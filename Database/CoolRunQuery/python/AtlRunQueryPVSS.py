#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryPVSS.py
# Project: AtlRunQuery
# Purpose: Utility to retrieve information from PVSS DB (DCS quantities)
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Mar 27, 2010
# ----------------------------------------------------------------

import cx_Oracle
from time import time

def GetPVSS_BPMs( cursor, iovmin, iovmax ):

    # fetch the event history first
    selcmd = "SELECT 'EVENTHISTORY_'||lpad (archive#,8,'0') from ( select archive#, start_time, end_time from atlas_pvssDCS.arc_archive A where A.group_name = 'EVENT' and (A.end_time > to_date('%s','DD-MM-YYYY HH24:MI:SS') or A.end_time is null) intersect select archive#, start_time, end_time from atlas_pvssDCS.arc_archive A where A.group_name = 'EVENT' and A.start_time < to_date('%s','DD-MM-YYYY HH24:MI:SS') )" % (iovmin, iovmax)
    cursor.execute( selcmd )
    eventhistory = cursor.fetchone()[0]

    # perform the actual query
    res = {}
    bpmvars = ['ATLGCSLHC:BPMSW_B1_distanceIP.position.horizontal', 'ATLGCSLHC:BPMSW_B1_distanceIP.position.vertical',
               'ATLGCSLHC:BPMSW_B2_distanceIP.position.horizontal', 'ATLGCSLHC:BPMSW_B2_distanceIP.position.vertical']

    for var in bpmvars:
        selcmd = "SELECT ROUND(A.VALUE_NUMBER,8), TO_CHAR(A.TS, 'DD-MM-YYYY HH24:MI:SS:FF3') FROM ATLAS_PVSSDCS.%s A,ATLAS_PVSSDCS.ELEMENTS B WHERE A.ELEMENT_ID = B.ELEMENT_ID and  ROWNUM <= 15000 AND  A.ELEMENT_ID = ( SELECT ELEMENT_ID FROM ATLAS_PVSSDCS.ELEMENTS WHERE  ELEMENT_NAME LIKE  '%s') AND TS BETWEEN TO_DATE('%s','DD-MM-YYYY HH24:MI:SS' ) AND TO_DATE('%s','DD-MM-YYYY HH24:MI:SS' ) AND A.VALUE_NUMBER <> 'NaN' AND A.VALUE_NUMBER <> BINARY_FLOAT_INFINITY ORDER BY B.ELEMENT_NAME, A.TS" % (eventhistory, var, iovmin, iovmax)
        cursor.execute( selcmd )
        res[var] = cursor.fetchall()

    return res

