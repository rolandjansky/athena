# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__author__  = 'Javier Montejo'
__version__="$Revision: 1.01 $"
__doc__="Access to Trigger DB and TriggerMenu to read past and future prescales"

import sys
from TriggerMenu.api.TriggerEnums import TriggerPeriod, LBexceptions, TriggerRenaming
from TriggerMenu.api.TriggerPeriodData import TriggerPeriodData

def getRunLBFromU64(runlb):
    run = runlb >> 32
    lb = runlb & ((1L<<32)-1)
    return ( int(run), int(lb) )


def getReadyForPhysicsInRange(period):
    """
    returns all runs in the given period which have the ReadyForPhysics flag set in at least 1 LB
    """

    print "Loading COOL libs..."
    from CoolLumiUtilities.CoolDataReader import CoolDataReader
    print "Done loading libs, starting now ..."

    myReader = CoolDataReader('COOLONL_TDAQ/CONDBR2', '/TDAQ/RunCtrl/DataTakingMode')
    runsWithReady = {}

    firstRun = min([x for x in period.keys()])
    lastRun = max([x for x in period.keys()])+1
    since = ((1L*firstRun) << 32)
    until = ((1L*lastRun) << 32)

    myReader.setIOVRange( since, until )
    myReader.readData()
    
    for obj in myReader.data:
        isReady = (obj.payload()['ReadyForPhysics'] == 1)
        if not isReady:
            continue
        sincerun, sincelb = getRunLBFromU64(obj.since())
        untilrun, untillb = getRunLBFromU64(obj.until())
        if sincerun != untilrun:
            print "WARNING: ready block crosses run boundaries:", sincerun, untilrun
        if sincerun not in period: continue
        if sincerun in runsWithReady:
            runsWithReady[sincerun] += [ (sincelb, untillb) ]
        else:
            runsWithReady[sincerun] = [ (sincelb, untillb) ]

    print runsWithReady

    return runsWithReady


def getKeys( listOfRuns, doPrint = False ):

    from CoolLumiUtilities.CoolDataReader import CoolDataReader

    keysByRun = {}

    mySmkReader = CoolDataReader('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/HLT/HltConfigKeys')
    myL1pskReader = CoolDataReader('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/LVL1/Lvl1ConfigKey')
    myHltpskReader = CoolDataReader('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/HLT/PrescaleKey')
    #myBgskReader = CoolDataReader('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/LVL1/BunchGroupKey')
    
    for run in sorted(listOfRuns):

        listOfReadyBlocks = listOfRuns[run]

        print "Getting keys for run %i, lbs %r" % (run, listOfReadyBlocks)

        since = ((1L*run) << 32) 
        until = ((1L*(run+1)) << 32)

        # super master key
        mySmkReader.setIOVRange( since, until - 1 )
        mySmkReader.readData()
        for obj in mySmkReader.data:
            smk = obj.payload()['MasterConfigurationKey']
            sincerun, sincelb = getRunLBFromU64(obj.since())
            untilrun, untillb = getRunLBFromU64(obj.until())
            keysByRun.setdefault(run,{})['smk'] = smk

        for sincelb, untillb in listOfReadyBlocks:

            since = ((1L*run) << 32) + sincelb
            until = ((1L*run) << 32) + untillb

            # l1 prescale keys
            myL1pskReader.setIOVRange( since, until )
            myL1pskReader.readData()
            for obj in myL1pskReader.data:
                l1psk = obj.payload()['Lvl1PrescaleConfigurationKey']
                sincerun2, sincelb2 = getRunLBFromU64(obj.since())
                untilrun2, untillb2 = getRunLBFromU64(obj.until())
                if sincelb2 == untillb: break
                keysByRun.setdefault(run,{}).setdefault('l1psk',[]).append((l1psk,sincerun2, sincelb2,untilrun2, untillb2-1)) #use same convention as GRL, last lb is included
            
            # hlt prescale keys
            myHltpskReader.setIOVRange( since, until )
            myHltpskReader.readData()
            for obj in myHltpskReader.data:
                hltpsk = obj.payload()['HltPrescaleKey']
                sincerun2, sincelb2 = getRunLBFromU64(obj.since())
                untilrun2, untillb2 = getRunLBFromU64(obj.until())
                if sincelb2 == untillb: break
                keysByRun.setdefault(run,{}).setdefault('hltpsk',[]).append((hltpsk,sincerun2, sincelb2,untilrun2, untillb2-1)) #use same convention as GRL, last lb is included

            ## Bunch group key
            #myBgskReader.setIOVRange( since, until )
            #myBgskReader.readData()
            #for obj in myBgskReader.data:
            #    bgsk = obj.payload()['Lvl1BunchGroupConfigurationKey']
            #    sincerun2, sincelb2 = getRunLBFromU64(obj.since())
            #    untilrun2, untillb2 = getRunLBFromU64(obj.until())
            #    keysByRun.setdefault(run,{}).setdefault('bgsk',[]).append(bgsk)

    if doPrint:
        print keysByRun

    return keysByRun

def getHLTPrescalesRun2(connection,psk):
    """returns set name, prescale and passthrough 
    values for a given HLT prescale key 
    @connection - connection string, e.g. TRIGGERDB
    @psk - HLT prescale key
    @return (ps name, [('L2/EF',chainId,prescale,pass-through),...])
    """

    res = queryHLTPrescaleTableRun2(connection,psk)
    prescales = {r[0]:r[3] for r in res if r and r[1]=='Prescale'}
    rerun     = {r[0]:r[3] for r in res if r and r[1]=='ReRun'}
    for x in prescales:
        if x not in rerun: rerun[x] = False
    return {x: (prescales[x],rerun[x]) for x in prescales}

def queryHLTPrescaleTableRun2(connection,psk):

    from TrigConfigSvc.TrigConfigSvcUtils import getTriggerDBCursor, executeQuery

    global cursor, schemaname
    cursor,schemaname = getTriggerDBCursor(connection)

    output = [ "PS.HPR_CHAIN_COUNTER", "PS.HPR_TYPE", "PS.HPR_CONDITION" , "PS.HPR_VALUE"]

    tables = {}
    tables['PS'] = 'HLT_PRESCALE' 

    condition = [ "PS.HPR_PRESCALE_SET_ID = :psk" ]

    bindvars = { "psk": psk }

    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)

    return res


def fillHLTmap( info, hltMap_prev , lbCount, run, grlblocks):
    from TrigConfigSvc.TrigConfigSvcUtils import getL1Items, getL1Prescales
    from AthenaCommon.Logging import logging

    log = logging.getLogger( "TrigConfigSvcUtils.py" )
    log.setLevel(logging.ERROR) #avoid the spam from TrigConfigSvcUtils

    items = getL1Items('TRIGGERDB', info['smk']) # returs map item name => CTP ID
    chainsHLT = getChainsWithL1seed('TRIGGERDB', info['smk']) # returns map HLT ID => (HLT name, L1 seed)
    chainsHLT = {k:v for (k,v) in chainsHLT.iteritems() if "L1" in v[1]}

    tmphltList = []
    for lbrange in info['hltpsk']:
        lbstart, lbend = lbrange[2], lbrange[4]
        if lbend ==-1: lbend = 2000
        hltprescales = getHLTPrescalesRun2('TRIGGERDB', lbrange[0])
        tmphltList.append(( lbstart, lbend,hltprescales) )

    tmpl1List = []
    for lbrange in info['l1psk']:
        lbstart, lbend = lbrange[2], lbrange[4]
        if lbend ==-1: lbend = 2000
        l1psname, l1prescales = getL1Prescales('TRIGGERDB', lbrange[0])
        l1prescales    = {l1name: l1prescales[int(l1id)] for (l1name, l1id) in items.iteritems()}
        tmpl1List.append(( lbstart, lbend,l1prescales) )

    #merge the lb ranges of HLT and L1
    hltindex, l1index = 0,0
    mergedList = []
    while hltindex < len(tmphltList) and l1index < len(tmpl1List) :
        if tmphltList[hltindex][1] == tmpl1List[l1index][1]:
            lbstart, lbend = max(tmphltList[hltindex][0],tmpl1List[l1index][0]), tmphltList[hltindex][1]
            mergedList.append((lbstart, lbend,tmphltList[hltindex][2],tmpl1List[l1index][2]))
            hltindex += 1
            l1index += 1
        elif tmphltList[hltindex][1] > tmpl1List[l1index][1]:
            lbstart, lbend = max(tmphltList[hltindex][0],tmpl1List[l1index][0]), tmpl1List[l1index][1]
            mergedList.append((lbstart, lbend,tmphltList[hltindex][2],tmpl1List[l1index][2]))
            l1index += 1
        else:
            lbstart, lbend = max(tmphltList[hltindex][0],tmpl1List[l1index][0]), tmphltList[hltindex][1]
            mergedList.append((lbstart, lbend,tmphltList[hltindex][2],tmpl1List[l1index][2]))
            hltindex += 1

    hltMap = {}
    for lbstart, lbend, hltprescales, l1prescales in mergedList:
        lboverlap = max([min(lbend,grllbend) - max(lbstart,grllbstart) for (grllbstart,grllbend) in grlblocks])+1
        if lboverlap <= 0:
            #print "Rejected:",(lboverlap, lbstart, lbend, grlblocks)
            continue
        if run in LBexceptions.exceptions:
            if any([lbstart>=exc_start and lbstart<=exc_end for exc_start, exc_end in LBexceptions.exceptions[run]]): continue
            if any([lbend>=exc_start and lbend<=exc_end for exc_start, exc_end in LBexceptions.exceptions[run]]): continue

        #print "Accepted:",(lboverlap, lbstart, lbend, grlblocks)
        lbCount += lboverlap
        for hltid, (hltps, hltrerun) in hltprescales.iteritems():
            if hltid not in chainsHLT: continue
            if hltps < 1: hltps = 1e99
            l1seeds = chainsHLT[hltid][1]
            l1ps = 1e99
            for l1seed in l1seeds.split(","): #protect 'L1_MU20,L1_MU21'
                if l1seed not in l1prescales and len(l1seeds) > 10: continue #Protection against buggy HLT_noalg_Standby
                tmpl1ps = l1prescales[l1seed] 
                if tmpl1ps < 1: tmpl1ps = 1e99
                l1ps = min(l1ps, tmpl1ps)
            
            #if hltps*l1ps!=1 and chainsHLT[hltid][0]=="HLT_mu60_0eta105_msonly": #muon primary since 2015 as standard candle to find problematic LBs
            #    print "WARNING: Prescaled HLT_mu60_0eta105_msonly",l1ps,hltps,lbstart, lbend, grlblocks

            if hltps*l1ps < 1e99: efflb = lboverlap/(hltps*l1ps)
            else:                 efflb = 0
            if not chainsHLT[hltid][0] in hltMap: hltMap[chainsHLT[hltid][0]] = [l1seeds, 0, hltrerun>0]
            hltMap[chainsHLT[hltid][0]][1] += efflb
    
    for hlt,(l1,efflb,rerun) in hltMap_prev.iteritems():
        if hlt in hltMap: 
            hltMap[hlt][1] += efflb
            hltMap[hlt][2] |= rerun
        else: hltMap[hlt] = [l1, efflb,rerun]

    return hltMap, lbCount


def getChainsWithL1seed(connection, smk):
    '''copy of getChains but retrieving also the L1 seed and assuming always run2
         https://gitlab.cern.ch/atlas/athena/blob/master/Trigger/TrigConfiguration/TrigConfigSvc/python/TrigConfigSvcUtils.py#L611
    '''
    from TrigConfigSvc.TrigConfigSvcUtils import getTriggerDBCursor, executeQuery
    cursor,schemaname = getTriggerDBCursor(connection)
         
    output = ['TC.HTC_ID', 'TC.HTC_CHAIN_COUNTER', 'TC.HTC_NAME', 'TC.HTC_LOWER_CHAIN_NAME']
    tables = {}
    tables['SM']    = 'SUPER_MASTER_TABLE'
    tables['M2C']     = 'HLT_TM_TO_TC'
    tables['TC']    = 'HLT_TRIGGER_CHAIN'
    tables['MT']    = 'HLT_MASTER_TABLE'

    condition = [ "SM.SMT_ID = :smk",
                    'SM.SMT_HLT_MASTER_TABLE_ID = MT.HMT_ID',
                    'MT.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID',
                    'M2C.HTM2TC_TRIGGER_CHAIN_ID = TC.HTC_ID' ]
    
    bindvars = { "smk": smk }
    
    res = executeQuery(cursor, output, condition, schemaname, tables, bindvars)
    
    chainsef = {}
    for x in res:
        if len(x)!=4: continue #protect against HLT_noalg_bkg_L1Bkg and similars
        chainsef[x[1]] = (x[2],x[3])

    return chainsef



def getHLTmap_fromDB(period, customGRL):
    ''' Return a map of HLT chain: (L1 seed, active LBs, is-rerun) for a given period
    '''
    
    triggerPeriod = TriggerPeriodData( period, customGRL ).grl
    if not triggerPeriod: return {},0
    runsWithReadyForPhysics = getReadyForPhysicsInRange(triggerPeriod)
    keys = getKeys( runsWithReadyForPhysics)
    
    hltMap = {}
    lbCount = 0
    for run in keys:
        print "Filling run:",run
        hltMap, lbCount = fillHLTmap( keys[run], hltMap, lbCount , run, triggerPeriod[run])

    return hltMap, lbCount

def getHLTmap_fromTM(period, release):
    ''' Return a map of HLT chain: (L1 seed, active LBs, is-rerun) for a given period
        Only "Future" periods make sense here
        The format is the same as for TriggerDBAccess for compatibility but rerun is always false
    '''

    from os import getenv
    asetupversion = getenv('AtlasVersion','')
    forceRel21 = not (asetupversion.startswith("21.1") or asetupversion.startswith("22."))
    if (forceRel21 or release) and release!="current":
        sys.path.insert(0, getMenuPathFromRelease(release))
        import Physics_pp_v7
    else:
        from TriggerMenu.menu import Physics_pp_v7
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    
    Physics_pp_v7.setupMenu()
    if not period & TriggerPeriod.future: return {}, 0
    maxlumi = 20000
    if   period & TriggerPeriod.future1p8e34: maxlumi = 17000
    elif period & TriggerPeriod.future2e34:   maxlumi = 20000
    else: print "Warning non-recongnized future",period

    hltMap = {}
    dummyfutureLBs = 1e6
    for prop in dir(TriggerFlags):
        if prop[-5:]!='Slice': continue
        sliceName=prop
        m_slice=getattr(TriggerFlags,sliceName)
        for m in m_slice.signatures():
            #['mu28_ivarmedium', 'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
            hltname = 'HLT_'+m[0]
            l1seed  = m[1]
            comment = m[4][0]
            ps = 0
            if maxlumi <= 20000 and 'Primary:20000' in comment: ps = 1
            if maxlumi <= 17000 and 'Primary:17000' in comment: ps = 1
            hltMap[hltname] = (l1seed, dummyfutureLBs*ps, False)  #hasRerun=False
        
    return hltMap, dummyfutureLBs

def getMenuPathFromRelease(release):
    if release: #already format-proofed in TriggerAPI
        return "/cvmfs/atlas.cern.ch/repo/sw/software/21.1/AthenaP1/%s/InstallArea/x86_64-slc6-gcc62-opt/python/TriggerMenu/menu"%release
    #21.1.50 contains the final menu, no need to find the last release
    return "/cvmfs/atlas.cern.ch/repo/sw/software/21.1/AthenaP1/21.1.50/InstallArea/x86_64-slc6-gcc62-opt/python/TriggerMenu/menu"


def getHLTlist(period, customGRL, release):
    ''' For a given period it returns: [HLT chain, L1 seed, average livefraction, active LB, is-rerun], total LB
        The average livefraction is an approximation weighting the PS by number of lumiblocks.
        *** Don't use this number in analysis!!! ***
        For "future" periods, the average livefraction is 1 for items flagged as primary in TM and 0 for non-primaries
    '''
    if not period & TriggerPeriod.future or TriggerPeriod.isRunNumber(period): 
        hltmap, totalLB = getHLTmap_fromDB(period, customGRL)
    else:
        hltmap, totalLB = getHLTmap_fromTM(period, release)
    
    hltlist = cleanHLTmap(hltmap, totalLB)
    return (hltlist, totalLB)

def cleanHLTmap(hltmap, totalLB):

    from copy import deepcopy
    for  name, (l1seed, activeLB, hasRerun) in deepcopy(hltmap).iteritems(): #since it will modify on the fly
        for pair in TriggerRenaming.pairs:
            if name==pair[0] and     pair[1] in hltmap: hltmap[pair[1]][1] += activeLB
            #if name==pair[0] and not pair[1] in hltmap: hltmap[pair[1]]     = [l1seed, activeLB, hasRerun]
            if name==pair[1] and     pair[0] in hltmap: hltmap[pair[0]][1] += activeLB
            #if name==pair[1] and not pair[0] in hltmap: hltmap[pair[0]]     = [l1seed, activeLB, hasRerun]

    vetoes = ['calib','noise','noalg','satmon','peb']
    hltlist = [(name, l1seed, activeLB/totalLB, activeLB, hasRerun) for name, (l1seed, activeLB, hasRerun) in hltmap.iteritems() if not any(v in name for v in vetoes)]
    return hltlist

def test():
    print getHLTlist(TriggerPeriod.y2017,None)

if __name__ == "__main__":
    sys.exit(test())
