#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys


from optparse import OptionParser
p = OptionParser(usage="usage: <XML rate file> <XML rate file> ")
p.add_option('--min-ps',    type="float", default=1e-4, dest="min_frac_PS",  help="Minimal fraction on PS")
p.add_option('--min-pt',    type="float", default=1e-4, dest="min_frac_PT",  help="Minimal fraction on PT")
p.add_option('-d','--skip-disabled',  action='store_true', default=False, dest="noshow_disabled",         help="skip disabled triggers")


(options, args) = p.parse_args()

#-----------------------------------------------------------------------------
# XML parsing functions
#

import xml.etree.cElementTree as ET

def readPsFile(fname):
    
    items = {}
    def fill(level):
        v = level.findall('signature')
        for x in v:
            newitem = {
                'PS': x.findtext('chain_prescale'),
                'PT': x.findtext('passthrough'),
                'ES': x.findtext('express_prescale')}
            items[x.findtext('sig_name') ] = newitem
        print 'N items: %d (%s)' % (len(items), level.findtext('lvl_name'))
                          
    doc = ET.parse(fname)
    root = doc.getroot()
    levels = root.findall('level')
    l1_items, l2_chains, ef_chains = [], [], []
    for level in levels:
        lvl_name = level.findtext('lvl_name')
        if lvl_name == 'L1':
            fill(level)
        elif lvl_name == 'L2':
            fill(level)
        elif lvl_name == 'EF':
            fill(level)

    return items


#-----------------------------------------------------------------------------

if len(args) < 2 :
    p.print_help()   
    sys.exit(-1)

ps1= readPsFile(args[0])
ps2= readPsFile(args[1])

fullist=ps1.keys()
fullist.sort()

print "Will show |PS1-PS2|/(PS1+PS2) > %6.3f and |PT1-PT2|/(PT1+PT2) > %6.3f" %(options.min_frac_PS, options.min_frac_PT)
print "\n%30s        [%16s]  /  [%16s] \n"% ( "Item [PS, PT ]", args[0], args[1] )
for item in fullist :
    if item in ps2 :

        def printme (item, mps1, mps2, mpt1, mpt2) :
            print "%30s        [%8.1f,%8.1f]  /  [%8.1f,%8.1f] " %(
                item, mps1, mpt1, mps2, mpt2)
        
        if ps1[item]["PS"] != None : fps1 = float(ps1[item]["PS"])
        else : fps1=-1
        if ps2[item]["PS"] != None : fps2 = float(ps2[item]["PS"])
        else : fps2=-1
        if ps1[item]["PT"] != None : fpt1 = float(ps1[item]["PT"])
        else : fpt1=-1
        if ps2[item]["PT"] != None : fpt2 = float(ps2[item]["PT"])
        else : fpt2=-1

        if options.noshow_disabled and (fps1<1e-4 or fps2<1e-4) :
            continue

        showPS = False
        if options.min_frac_PS > 0  :
            if fps1*fps2 < 0 : showPS= True
            if fps2+fps1 > 0 and abs(fps1-fps2)/(fps1+fps2)>options.min_frac_PS : showPS=True
        else : showPS=True

        showPT = False
        if options.min_frac_PT>0  :
            if fpt1*fpt2 < 0 : showPT= True
            if fpt2+fpt1 > 0 and abs(fpt1-fpt2)/(fpt1+fpt2)>options.min_frac_PT : showPT=True
        else : showPT=True

        
        if showPS or showPT :
            printme(item, fps1, fps2, fpt1, fpt2)


print "Items only in first  PS set "
print [item for item in ps1.keys() if item not in ps2 ]
print "Items only in second PS set "
print [item for item in ps2.keys() if item not in ps1 ]

 
