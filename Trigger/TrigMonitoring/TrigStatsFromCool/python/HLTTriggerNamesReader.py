#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from collections import namedtuple
from PyCool import cool
from CoolRunQuery.AtlRunQueryTimer import timer

def getDataPeriods():
    runlistdir = '/afs/cern.ch/atlas/www/GROUPS/DATAPREPARATION/DataPeriods'
    d = {}
    for p in range(ord('A'),ord('J')):
        fh = open('%s/data10_7TeV.period%c.runs.list' % (runlistdir,chr(p)) )
        lines = fh.readlines()
        d[chr(p)] = [int(l.rstrip('\n')) for l in lines]
        fh.close()
    return d


def getSMK(runlist):
    from CoolConvUtilities.AtlCoolLib  import indirectOpen
    conn = indirectOpen( "COOLONL_TRIGGER/COMP200", True, True, False)
    folder = conn.getFolder("/TRIGGER/HLT/HltConfigKeys")

    smk = {}

    for run in runlist:
        runlb = ( run<<32 )
        obj = folder.findObject(runlb,0)
        smk[run] = obj.payload()['MasterConfigurationKey']

    return smk


def executeQuery(cursor, output, condition, schemaname, tables, bindvars=(), order=None):
    from TrigConfigSvc.TrigConfigSvcUtils import getUsedTables

    s_output = ', '.join(output)
    s_tables = ', '.join(getUsedTables(output, condition, schemaname, tables))
    s_condition = ' and '.join(condition)
    s_order = 'order by %s' % order if order else ''

    
    query = 'select distinct %s from %s where %s %s' % \
            (s_output, s_tables, s_condition, s_order )
    #print (str(query))
    if len(bindvars)==0:
        cursor.execute(str(query))
    else:
        cursor.execute(str(query),bindvars)
    return cursor.fetchall()


def getTriggerNames(smks, pattern=None):

    if type(smks)==int:  smks=set([smks])
    if type(smks)==list: smks=set(smks)

    from TrigConfigSvc.TrigConfigSvcUtils import getTriggerDBCursor
    cursor,schemaname = getTriggerDBCursor("TRIGGERDB")

    if not '.' in schemaname: schemaname += '.'

    tn = {}

    tables = {
        'SM' : 'SUPER_MASTER_TABLE',
        'HM' : 'HLT_MASTER_TABLE',
        'TM' : 'HLT_TRIGGER_MENU',
        'M2C': 'HLT_TM_TO_TC',
        'TC' : 'HLT_TRIGGER_CHAIN'
        }

    condition = [
        'SM.SMT_ID = :smk',
        'SM.SMT_HLT_MASTER_TABLE_ID = HM.HMT_ID',
        'HM.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID',
        'M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID'
        ]

    if pattern:
        condition += [ "TC.HTC_NAME like '%%%s%%'" % pattern ]

    output = ['TC.HTC_NAME', 'TC.HTC_L2_OR_EF', 'TC.HTC_CHAIN_COUNTER']

    order = 'TC.HTC_L2_OR_EF desc, TC.HTC_NAME'

    TC = namedtuple('TC','name level counter')

    for smk in smks:
        bindvars = { "smk": smk }

        res = executeQuery(cursor, output, condition, schemaname, tables, bindvars, order)

        tn[smk] = [TC._make(r) for r in res]

    return tn

    

if __name__=="__main__":
    dp= getDataPeriods()

    #smkByRun = getSMK(dp['G'])

    #smks = set(smkByRun.values())

    print getTriggerNames(910,'EF_tauNo')
