#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## use it as such:
## python cnvXML_express.py --lv1xml=... --hltxml=... --ps_name=... --psxml=...
## where lv1xml is the reference lv1 xml file
##       hltxml is the reference hlt xml file
##       ps_name is the prescales file
##       psxml is the output file name suffix (two output files will be generated, one for lvl1 and one for hlt.


#-----------------------------------------------------------------------------
# Create and configure option parser
#

from optparse import OptionParser

p = OptionParser(usage="usage: compareXML.py <XML file 1> <XML file 2> ...", version="1.0")

p.add_option('-f', '--filter', action="store_true", default=False, dest="filter", help="print debug messages")

(options, args) = p.parse_args()

#-----------------------------------------------------------------------------
# Create and configure python logger
#
import logging

def getLog(name, level = logging.INFO):

    f = logging.Formatter("%(message)s")
    h = logging.StreamHandler()
    h.setFormatter(f)
    
    log = logging.getLogger(name)
    log.setLevel(level)
    log.addHandler(h)

    return log

log = getLog('compareXML')

#-----------------------------------------------------------------------------
# Helper class
#
class Trigger:
    def __init__(self):

        self.name   = None
        self.ps     = None
        self.pt     = None
        self.es     = None
        self.has_pt = False
        self.has_es = False

#-----------------------------------------------------------------------------
# XML parsing function
#

trigger_names = []

def readPsFile(fname):
    
    def fill(level):
        items = []
        v = level.findall('signature')
        for x in v:
            t = Trigger()
            t.name = x.findtext('sig_name')
            t.ps   = x.findtext('chain_prescale')
            t.pt   = x.findtext('passthrough')
            t.es   = x.findtext('express_prescale')
            
            items.append(t)

            if t.name not in trigger_names:
                trigger_names.append(t.name)
            
        return items

    import xml.etree.cElementTree as et
    
    xdoc   = et.parse(fname)
    root   = xdoc.getroot()
    levels = root.findall('level')

    items = []
    
    for level in levels:
        items += fill(level)
        
    return items

#-----------------------------------------------------------------------------
# Compare one across many files trigger
#
def IsDiffTrigger(trigger_name, trigger_lists):
    
    if len(trigger_lists) < 2: return

    triggers = [None]*len(trigger_lists)

    for i in range(0, len(trigger_lists)):
        for t in trigger_lists[i]:
            if trigger_name == t.name:
                triggers[i] = t

    is_diff = False

    for i in range(0, len(triggers)):
        t1 = triggers[i]
        
        for j in range(i+1, len(triggers)):
            t2 = triggers[j]

            if t1 == None and t2 == None: continue
            if t1 == None or  t2 == None: break


            if t1.ps != t2.ps: is_diff = True

            if t1.pt != None and t1.pt != None:
                if t1.pt != t2.pt: is_diff = True

            if t1.es != None and t1.es != None:
                if t1.es != t2.es: is_diff = True
            
        if is_diff:
            passFilter = False

            if (float(t1.ps) < 1.1 and float(t2.ps) > 1.0):
                passFilter = True

            if t1.pt != None and t2.pt != None:
            
                if (float(t1.pt) < 1.1 and float(t1.pt) > 0.9) and float(t2.pt) < 0.9:
                    passFilter = True

            name = trigger_name.replace('_', '\\_')

            if name.count('MBTS') or name.count('ZDC') or name.count('LUCID') or name.count('EMPTY'):
                passFilter = False
                
            if passFilter:

                log.debug('Differences for trigger: %s' %trigger_name)
                log.debug('  PS1=%s  PT=%s' %(t1.ps, t1.pt))
                log.debug('  PS2=%s  PT=%s' %(t2.ps, t2.pt))

                if t1.pt == None: t1.pt = 0;
                if t2.pt == None: t2.pt = 0;                

                log.info('%s & %s & %s & %s & %s \\\\' %(name, t1.ps, t2.ps, t1.pt, t2.pt))
                log.info('\\hline')
                
            break

    return is_diff

#-----------------------------------------------------------------------------
# Process inputs
#
import os, sys

if len(args) < 2:
    log.info('Need at least 2 input XML files')
    sys.exit(0)

lists = []

for arg in args:
    if not os.path.isfile(arg):
        log.info('File does not exist: %s' %arg)
        sys.exit(1)
    
    lists += [readPsFile(arg)]

log.info('Comparing %d trigger(s)' %len(trigger_names))

for trigger_name in trigger_names:

    if IsDiffTrigger(trigger_name, lists):
        log.debug('Differences for trigger: %s' %trigger_name)
    
