#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryTriggerUtils.py
# Project: AtlRunQuery
# Purpose: Trigger Utility functions for AtlRunQuery
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Oct 27, 2010
# ----------------------------------------------------------------

try:
    set
except NameError:
    from sets import Set
    set = Set

from TrigConfigSvc.TrigConfigSvcUtils import getTriggerDBCursor

__cursor=None
__schema = ""

class TriggerChain:
    def __init__(self, name, counter, lowername="", lowercounter=-1, forshow=False, forselect=False, level=0):
        self.name         = name     
        self.counter      = counter       
        self.lowername    = "" if lowername=="~" else lowername
        self.lowercounter = lowercounter  
        self.forshow      = forshow  
        self.forselect    = forselect
        self.multiseeded  = (self.lowername=="") or (',' in self.lowername)
        self.level        = level
        self.lower        = None
        
    def __repr__(self):
        return "TC:"+self.name




def getUsedTables(output, condition, schemaname, tables):
    schemaname = schemaname.rstrip('.')+'.'
    usedtables = set()
    for o in output:
        usedtables.add(o.split('.')[0])
    for c in condition:
        for p in c.split():
            if '.' in p and not '\'' in p: usedtables.add(p.split('.')[0])
    return ["%s%s %s" % (schemaname,tables[t],t) for t in usedtables]


def executeQuery(cursor, output, condition, schemaname, tables, bindvars=()):
    query = 'select distinct %s from %s where %s' % \
            (', '.join(output),
             ', '.join(getUsedTables(output, condition, schemaname, tables)),
             ' and '.join(condition))
    if len(bindvars)==0:
        cursor.execute(str(query))
    else:
        cursor.execute(str(query),bindvars)

    return cursor.fetchall()


def getL1PskNames(psk):
    global __cursor, __schema
    if not __cursor:
        __cursor,__schema = getTriggerDBCursor("TRIGGERDB")

    if type(psk) == set: psk = list(psk)
    if type(psk) != list: psk = [psk]

    keyslist = ','.join([str(k) for k in psk if k])

    if keyslist=="":
        return {}

    tables = { 'L' : 'L1_PRESCALE_SET' }

    output = [ 'L.L1PS_ID', 'L.L1PS_NAME' ]

    condition = [ "L.L1PS_ID in (%s)" % keyslist ]

    res = executeQuery(__cursor, output, condition, __schema, tables)

    return dict(res)



def getHLTPskNames(psk):
    global __cursor, __schema
    if not __cursor:
        __cursor,__schema = getTriggerDBCursor("TRIGGERDB")

    if type(psk) == set: psk = list(psk)
    if type(psk) != list: psk = [psk]

    if len(psk)==0:
        return {}
    
    tables = { 'L' : 'HLT_PRESCALE_SET' }

    output = ['L.HPS_ID', 'L.HPS_NAME']

    prescales = [str(k) for k in psk if k]

    if len(prescales)==0: return {}

    condition = [ "L.HPS_ID in (%s)" % ','.join(prescales) ]

    res = executeQuery(__cursor, output, condition, __schema, tables)

    return dict(res)


def getSmkNames(smk):
    global __cursor, __schema
    if not __cursor:
        __cursor,__schema = getTriggerDBCursor("TRIGGERDB")

    if type(smk) == set: smk = list(smk)
    if type(smk) != list: smk = [smk]

    if len(smk)==0:
        return {}
    
    tables = { 'S' : 'SUPER_MASTER_TABLE' }

    output = ['S.SMT_ID', 'S.SMT_NAME', 'S.SMT_VERSION', 'S.SMT_COMMENT']

    condition = [ "S.SMT_ID in (%s)" % ','.join([str(k) for k in smk]) ]

    res = executeQuery(__cursor, output, condition, __schema, tables)

    return dict([ (r[0],(r[1],r[2],r[3])) for r in res])

def getHLTMenu(smk):
    global __cursor, __schema
    if not __cursor:
        __cursor,__schema = getTriggerDBCursor("TRIGGERDB")

    output = ['TC.HTC_NAME', 'TC.HTC_CHAIN_COUNTER', 'TC.HTC_LOWER_CHAIN_NAME', 'TC.HTC_L2_OR_EF' ]
    
    tables = {}
    tables['SM']    = 'SUPER_MASTER_TABLE'
    tables['HM']    = 'HLT_MASTER_TABLE'
    tables['M2C']   = 'HLT_TM_TO_TC'
    tables['TC']    = 'HLT_TRIGGER_CHAIN'

    
    condition = [ "SM.SMT_ID = :smk",
                  'SM.SMT_HLT_MASTER_TABLE_ID = HM.HMT_ID',
                  'HM.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID',
                  'M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID' ]
    
    bindvars = { "smk": smk }
    
    res = executeQuery(__cursor, output, condition, __schema, tables, bindvars)

    l2chains = []
    efchains = []

    for r in res:
        if r[3]=='L2':
            l2chains += [TriggerChain(r[0],r[1],r[2], level=2)]
        elif r[3]=='EF':
            efchains += [TriggerChain(r[0],r[1],r[2], level=3)]

    return l2chains, efchains

def getL1Menu(smk):
    global __cursor, __schema
    if not __cursor:
        __cursor,__schema = getTriggerDBCursor("TRIGGERDB")

    output = ['TI.L1TI_NAME', 'TI.L1TI_CTP_ID' ]
    
    tables = {}
    tables['SM']    = 'SUPER_MASTER_TABLE'
    tables['M']     = 'L1_MASTER_TABLE'
    tables['M2I']   = 'L1_TM_TO_TI'
    tables['TI']    = 'L1_TRIGGER_ITEM'

    
    condition = [ "SM.SMT_ID = :smk",
                  'SM.SMT_L1_MASTER_TABLE_ID = M.L1MT_ID',
                  'M.L1MT_TRIGGER_MENU_ID = M2I.L1TM2TI_TRIGGER_MENU_ID',
                  'M2I.L1TM2TI_TRIGGER_ITEM_ID = TI.L1TI_ID' ]

    bindvars = { "smk": smk }
    
    res = executeQuery(__cursor, output, condition, __schema, tables, bindvars)

    items = 256*[None]

    for r in res:
        items[r[1]] = TriggerChain(r[0],r[1])
    return items

def getL1Prescales(l1prescalekey):
    global __cursor, __schema
    if not __cursor:
        __cursor,__schema = getTriggerDBCursor("TRIGGERDB")
    
    tables = { 'L' : 'L1_PRESCALE_SET' }
    
    output = ['L.L1PS_VAL%i' % i for i in xrange(1,257)]
    
    condition = [ "L.L1PS_ID = :psk" ]
    
    bindvars = { "psk": l1prescalekey }

    res = executeQuery(__cursor, output, condition, __schema, tables, bindvars)
    
    return res[0][0:256]


def getHLTPrescales(hltprescalekey):
    global __cursor, __schema
    if not __cursor:
        __cursor,__schema = getTriggerDBCursor("TRIGGERDB")
    
    tables = { 'H' : 'HLT_PRESCALE_SET',
               'P' : 'HLT_PRESCALE'
               }
    
    output = [ 'P.HPR_L2_OR_EF', 'P.HPR_CHAIN_COUNTER', 'P.HPR_PRESCALE', 'P.HPR_PASS_THROUGH_RATE' ]
    
    condition = [ "P.HPR_PRESCALE_SET_ID = :psk", "P.HPR_L2_OR_EF not like 'express'" ]
    
    bindvars = { "psk": hltprescalekey }

    res = executeQuery(__cursor, output, condition, __schema, tables, bindvars)

    l2ps = {}
    efps = {}
    for r in res:
        if r[0]=='L2':   l2ps[r[1]] = ( float(r[2]), float(r[3]) )
        elif r[0]=='EF': efps[r[1]] = ( float(r[2]), float(r[3]) )

    return l2ps, efps


def getRandom(smk):
    global __cursor, __schema
    if not __cursor:
        __cursor,__schema = getTriggerDBCursor("TRIGGERDB")

    if smk=='n.a.':
        return ('n.a.','n.a.')

    tables = { 'S' : 'SUPER_MASTER_TABLE',
               'M' : 'L1_MASTER_TABLE',
               'R' : 'L1_RANDOM'
               }
    
    output = [ 'R.L1R_RATE1', 'R.L1R_RATE2' ]
    
    condition = [ "S.SMT_ID=:smk", "S.SMT_L1_MASTER_TABLE_ID=M.L1MT_ID", "M.L1MT_RANDOM_ID=R.L1R_ID" ]
    
    bindvars = { "smk": smk }

    res = executeQuery(__cursor, output, condition, __schema, tables, bindvars)

    return res[0]



if __name__=="__main__":
    #print getL1PskNames([2517, 2518, 2284, 2536])

    #print getHLTPskNames([2517, 2530, 2537, 2491])
    
    #print getSmkNames([931, 932, 933])

    #print getL1Menu(931)

    #print getHLTMenu(931)

    #print getL1Prescales(2517)

    #print getHLTPrescales(2530)

    print getRandom(1038)
