#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryDQUtils.py
# Project: AtlRunQuery
# Purpose: Utility functions for AtlRunQuery
# Authors: Peter Onyisi (Chicago U)
# Created: May 6, 2010
# ----------------------------------------------------------------
#
from __future__ import division, print_function

import sys, time
from functools import reduce
import xml.etree.cElementTree as et
import xmlrpclib
from PyCool import cool
dbSvc = cool.DatabaseSvcFactory.databaseService()

from CoolRunQuery.AtlRunQuerySelectorBase import DataKey

THIRTYTWOMASK=int(2**32-1)
SERVER='http://atlasdqm.cern.ch:8080'

FLAGS_WE_CARE_ABOUT = ['PIXB', 'PIX0', 'PIXEA', 'PIXEC',
                       'SCTB', 'SCTEA', 'SCTEC',
                       'TRTB', 'TRTEA', 'TRTEC','TRTTR'
                       'IDGL', 'IDAL',
                       'EMBA', 'EMBC', 'EMECA', 'EMECC', 'HECA', 'HECC',
                       'FCALA', 'FCALC',
                       'TILBA', 'TILBC', 'TIEBA', 'TIEBC',
                       'MBTSA', 'MBTSC',
                       'MDTBA', 'MDTBC', 'MDTEA', 'MDTEC',
                       'RPCBA', 'RPCBC',
                       'TGCEA', 'TGCEC',
                       'CSCEA', 'CSCEC',
                       'LCD',
                       #'L1CAL',
                       #'L1MUB', 'L1MUE',
                       #'L1CTP',
                       #'TRCAL', 'TRBJT', 'TRBPH', 'TRCOS',
                       #'TRELE', 'TRGAM', 'TRJET', 'TRMET',
                       #'TRMBI', 'TRMUO', 'TRTAU', 'TRIDT',
#                       'EIDB', 'EIDCR', 'EIDE', 'PIDB', 'PIDCR', 'PIDE',
#                       'JETB', 'JETEA', 'JETEC',
#                       'MET', 'METCALO', 'METMUON',
#                       'TAUB', 'TAUCR', 'TAUE',
                       ]

FLAGGROUPS = {
    'PIX': ['PIXB', 'PIX0', 'PIXEA', 'PIXEC'],
    'SCT': ['SCTB', 'SCTEA', 'SCTEC'],
    'TRT': ['TRTB', 'TRTEA', 'TRTEC', 'TRTTR'],
    'LAR': ['EMBA', 'EMBC', 'EMECA', 'EMECC',
            'HECA', 'HECC', 'FCALA', 'FCALC'],
    'EM': ['EMBA', 'EMBC', 'EMECA', 'EMECC'],
    'HEC': ['HECA', 'HECC'],
    'FCAL': ['FCALA', 'FCALC'],
    'TILE': ['TILBA', 'TILBC', 'TIEBA', 'TIEBC'],
    'MDT': ['MDTBA', 'MDTBC', 'MDTEA', 'MDTEC'],
    'RPC': ['RPCBA', 'RPCBC'],
    'TGC': ['TGCEA', 'TGCEC'],
    'CSC': ['CSCEA', 'CSCEC'],
    'MBTS': ['MBTSA', 'MBTSC'],
    'LUCID': ['LCD']
    }

def _intersect(lbr1, lbr2):
    def normalize(lbr):
        return (max(1, lbr[0]), lbr[1] if lbr[1] != -1 else THIRTYTWOMASK)
    tlbr1 = normalize(lbr1)
    tlbr2 = normalize(lbr2)
    rv = (max(tlbr1[0],tlbr2[0]), min(tlbr1[1],tlbr2[1]))
    if rv[1] <= rv[0]:
        return None
    else:
        return rv

lumicache = {}

def lumi(run, lbr):
    if tuple(lbr) in lumicache:
        return lumicache[tuple(lbr)]

    lblbdb = dbSvc.openDatabase('COOLONL_TRIGGER/CONDBR2', True)
    lblb = lblbdb.getFolder('/TRIGGER/LUMI/LBLB')
    lblestonl = lblbdb.getFolder('/TRIGGER/LUMI/LBLESTONL')

    lbs = lblb.browseObjects((run<<32)+lbr[0],
                             (run<<32)+lbr[1]-1,
                             cool.ChannelSelection(0))
    inf = {}
    for obj in lbs:
        lbpy = obj.payload()
        #print ((lbpy['EndTime']-lbpy['StartTime'])/1e9)
        inf[(run, obj.since() & 0xFFFFFFFF)] = (lbpy['EndTime']-lbpy['StartTime'])/1e9
    if obj.since() & 0xFFFFFFFF == lbr[1]:
        print ('Oops: this should not happen, appears to be off-by-one error')
    lbls = lblestonl = lblestonl.browseObjects((run<<32)+lbr[0],
                                               (run<<32)+lbr[1]-1,
                                               cool.ChannelSelection(0))
    infl = {}
    for obj in lbls:
        lblpy = obj.payload()
        infl[(run, obj.since() & 0xFFFFFFFF)] = lblpy['LBAvInstLumi']

    #print (sorted(infl.keys()))
    totlum = 0
    for lb in inf:
        if lb in infl:
            totlum += inf[lb]*infl[lb]
        else:
            print ('Missing run %d, LB %d' % lb)
    lumicache[tuple(lbr)] = totlum
    return totlum

def GetDQEfficiency( rundict ):

    runset = set()
    for runnum in rundict.keys():
        runset.add(runnum)

    s = xmlrpclib.ServerProxy(SERVER)
    flaginfo = s.get_dqmf_summary_flags_lb({'run_list': list(runset)}, FLAGS_WE_CARE_ABOUT, 'SHIFTOFL')
    #print (flaginfo)
    record = {}
    for flag in FLAGS_WE_CARE_ABOUT:
        record[flag] = []
    for run in runset:
        if str(run) not in flaginfo:
            print ('%s not in flaginfo' % run)
            del rundict[run]
            continue
        for sublb in rundict[run]:
            for flag, periods in flaginfo[str(run)].items():
                for period in periods:
                    ip = _intersect(period, sublb)
                    if ip is not None:
                        #print (run, flag, ip, period[2])
                        #print (lumi(run, ip))
                        record[flag].append((ip[1]-ip[0], period[2], lumi(run, ip)))


    totallum = 0
    for run in rundict:
        for pair in rundict[run]:
            totallum += lumi(run, pair)
    print ('Total lumi:', totallum)

    flagsum = {}

    for flag in FLAGS_WE_CARE_ABOUT:
        flagsum[flag] = {}
        lr = record[flag]
        cols = set([x[1] for x in lr])
        accounted = 0
        for col in cols:
            llum = sum([x[2] for x in lr if x[1] == col])
            accounted += llum
            print (flag, col, llum, '%.2f%%' % (llum/totallum*100))
            flagsum[flag][col] = (llum/totallum*100)
        if abs(accounted-totallum) > 1e-8:
            print (flag, 'n.a.', totallum-accounted, '%.2f%%' % ((1-accounted/totallum)*100))
            flagsum[flag]['n.a.'] = ((1-accounted/totallum)*100)

    def _sum(d1, d2):
        rv = {}
        for key in set(d1.keys() + d2.keys()):
            rv[key] = d1.get(key, 0) + d2.get(key, 0)
        #print (rv)
        return rv

    for flagtop, flaggroup in FLAGGROUPS.items():
        vals = reduce(_sum, [flagsum[f] for f in flaggroup], {})
        print (flagtop)
        for typ in vals:
            print ('  %s: %.2f%%' % (typ, vals[typ]/len(flaggroup)))

def MakeDQeffHtml( dic, dicsum, datapath ):
    s = ''

    
    # run and time ranges
    runs   = dic[DataKey('Run')]
    times  = dic[DataKey('Start and endtime')]

    starttime = float(times[-1].partition(',')[0])
    endtime   = float(times[0].partition(',')[2])

    s = '<table style="color: #777777; font-size: 85%; margin-left: 14px" cellpadding="0" cellspacing="3">\n'
    # runs in reverse order
    s += '<tr><td><i>Number of runs selected:</i></td><td>&nbsp;&nbsp;%g</td><td></td></tr>\n' % (len(runs))
    s += '<tr><td><i>First run selected:</i></td><td>&nbsp;&nbsp;%s</td><td>&nbsp;&nbsp;(%s)</td></tr>\n' % (runs[-1], time.strftime("%a %b %d, %Y, %X",time.gmtime(starttime)))
    s += '<tr><td><i>Last run selected:</i></td><td>&nbsp;&nbsp;%s</td><td>&nbsp;&nbsp;(%s)</td></tr>\n'  % (runs[0], time.strftime("%a %b %d, %Y, %X",time.gmtime(endtime)))
    s += '</table>\n'
    
    # --------------------------------------------------------------------------------------
    # run summary table    
    s += '<p></p>\n'
    s += '<table style="margin-left: 14px">\n'
    s += '<tr><td><b>Run / Event Summary</b></td></tr>\n'
    s += '</table>\n'

    return s

if __name__ == '__main__':

    p = et.parse(sys.argv[1])

    rundict = {}
    for lbc in p.getiterator('LumiBlockCollection'):
        runnum = int(lbc.find('Run').text)
        #print ('Run', runnum)
        rundict[runnum] = []
        for lbr in lbc.findall('LBRange'):
            rundict[runnum].append([int(lbr.get('Start')),
                                    int(lbr.get('End'))+1])
            #print ('  LBs %s-%s' % (lbr.get('Start'), lbr.get('End')))

