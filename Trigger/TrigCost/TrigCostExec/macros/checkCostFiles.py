#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import re
import sys
import math
import time
import datetime

from optparse import OptionParser

p = OptionParser(usage="usage: <path:ROOT file directory>", version="0.1")

p.add_option('--pref',         type='string', default=None,          dest='pref')
p.add_option('--post',         type='string', default=None,          dest='post')
p.add_option('--outdir',       type='string', default=None,          dest='outdir')
p.add_option('-o',             type='string', default=None,          dest='output')
p.add_option('-c',             type='string', default=None,          dest='cfile')
p.add_option('--hkey',         type='string', default=None,          dest='hkey')
p.add_option('--fkey',         type='string', default=None,          dest='fkey')
p.add_option('--nfile', '-n',  type='int',    default=None,          dest='nfile')

p.add_option('--debug', '-d',   action='store_true', default=False,   dest='debug')
p.add_option('--stack',         action='store_true', default=False,   dest='stack')
p.add_option('--wait',          action='store_true', default=False,   dest='wait')
p.add_option('--wait-last',     action='store_true', default=False,   dest='wait_last')
p.add_option('--save',          action='store_true', default=False,   dest='save')
p.add_option('--zoom',          action='store_true', default=False,   dest='zoom')
p.add_option('--logy',          action='store_true', default=False,   dest='logy')
p.add_option('--no-title',      action='store_true', default=False,   dest='no_title')
p.add_option('--no-fill',       action='store_true', default=False,   dest='no_fill')
p.add_option('--no-norm',       action='store_true', default=False,   dest='no_norm')

(options, args) = p.parse_args()  

import ROOT
import PyCintex

if not options.wait and not options.wait_last:
    ROOT.gROOT.SetBatch(True)

from TrigCostBase.TrigCostBaseConfig import getLog

log = getLog(__name__)

#-------------------------------------------------------------------------
class Event:
    """Event data"""
        
    def __init__(self, enum, sec, nsec):
        self.enum = enum
        self.sec  = sec
        self.nsec = nsec

#-------------------------------------------------------------------------
class LumiBlock:
    """Lumi block data and list of events"""
        
    def __init__(self, lb):
        self.lb     = lb
        self.events = []
        self.elist  = []

    def AddEventNumber(self, en):
        self.events.append(en)

    def AddEvent(self, event):
        self.elist.append(event)

    def GetEventTimeSet(self):
        times = set()

        for e in self.elist:
            times.add('%s %s' %(e.sec, e.nsec))

        return times

#-------------------------------------------------------------------------
def doFile(rpath, lbs):

    timeStart = time.time()

    rfile = ROOT.TFile(rpath, 'READ')

    if not rfile:
        return 0

    rtree = rfile.Get('event')

    if not rtree:
        return 0
    
    nevent = rtree.GetEntries()
    
    for i in range(0, rtree.GetEntries()):
        rtree.GetEntry(i)
               
        d = rtree.data

        get_lb = getattr(rtree.data, 'getLumi',  None)
        get_en = getattr(rtree.data, 'getEvent', None)
        
        lb = int(get_lb())
        en =     get_en()
        
        if options.debug:
            print 'Run=%s LB=%s Event=%s T=%s %s' %(d.getRun(), lb, en, d.getSec(), d.getNanoSec())

        try:
            block = lbs[lb]
        except KeyError:
            lbs[lb] = LumiBlock(lb)
            block = lbs[lb]
            
        block.AddEvent      (Event(en, d.getSec(), d.getNanoSec()))
        block.AddEventNumber(en)

        if i > 0 and i % 10000 == 0:
            print i

    log.info('%s: nevent=%d, timer=%.1fs' %(os.path.basename(rpath), nevent, time.time()-timeStart))

    rfile.Close()
    return nevent

#-------------------------------------------------------------------------
def doDir(dpath):

    timeStart = time.time()

    ifile  = 0
    nevent = 0
    lumib  = {}

    for entry in os.listdir(dpath):
        path = '%s/%s' %(dpath.rstrip('/'), entry)
            
        if not os.path.isfile(path):
            continue

        if options.fkey and re.search(options.fkey, entry) == None:
            continue
                
        if options.nfile and ifile >= options.nfile:
            break

        ifile  += 1
        nevent += doFile(path, lumib)

    log.info('%s: timer=%.1fs' %(dpath, time.time()-timeStart))
    log.info('   Processed %d file(s)'  %ifile)
    log.info('   Processed %d LB(s)'    %len(lumib))
    log.info('   Processed %d event(s)' %nevent)

    return lumib

#-------------------------------------------------------------------------
def compareData(lbs1, lbs2):

    miss_lbs = []
    pass_lbs = []
    pass_evt = {}

    comm_lbs = []
    comm_evt = 0

    all_lbs = list(set(lbs1.keys() + lbs2.keys()))

    for lb in all_lbs:
        if lb not in lbs1:
            miss_lbs += [lb]
            continue

        if lb not in lbs2:
            miss_lbs += [lb]
            continue

        comm_lbs += [lb]

        b1 = lbs1[lb]
        b2 = lbs2[lb]

        s1 = set(b1.events)
        s2 = set(b2.events)

        t1 = b1.GetEventTimeSet()
        t2 = b2.GetEventTimeSet()

        iset = t1.intersection(t2)
        
        if len(iset) > 0:
            pass_evt[lb] = iset
            comm_evt += len(iset)

        if len(b1.events) != len(b2.events):
            miss_lbs += [lb]
            continue

        if len(s1.difference(s2)) == 0:
            pass_lbs += [lb]
            continue
        else:
            miss_lbs += [lb]
            log.warning('compareData - lb=%d has same number of events but different event numbers' %lb)
            continue

    print 'Matched LBs: \n%s' %(pass_lbs) 
    print 'Missing LBs: \n%s' %(miss_lbs)
    print 'Common  LBs: \n%s' %(comm_lbs)
    print 'Common event:%s'   %(comm_evt)

    ofile = open('common_events.txt', 'w')
    
    for lb, iset in pass_evt.iteritems():        
        for evt in iset:
            ofile.write('%d %s\n' %(lb, evt))
        
    ofile.close()

#-------------------------------------------------------------------------
if __name__ == '__main__':

    ROOT.gSystem.Load('libTrigMonitoringEventDict')

    #
    # Initialize ROOT and open input file
    #
    data = []

    for arg in args:
        if os.path.isdir(arg):
            data += [doDir(arg)]

    if len(data) == 2:
        compareData(data[0], data[1])



     
            
