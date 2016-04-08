# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from xml.dom.minidom import parse
import os.path

root = '/afs/cern.ch/atlas/project/RTT/Results/rel_3/dev/build/i686-slc3-gcc323-opt'
import xml.dom, time
import xml.dom.minidom

import sys
sys.path.append('../src')
from Evaluate import Evaluate
#from xml.xpath import Evaluate
fn = os.path.join(root, 'RTTSummary.xml')
dom = xml.dom.minidom.parse(fn)
print 'parsed file:', fn

def makeDict(parentNode):
    els = [e for e in parentNode.childNodes if e.nodeType == e.ELEMENT_NODE]
    
    dict = {}
    def filldict(e):
        dict[e.tagName.strip()] = Evaluate('text()', e).strip()#
        
    [filldict(e) for e in els]
    return dict



de = dom.documentElement


dict = makeDict(Evaluate('overview', de)[0])
print    
print '---------overview-----------------'
print
for i in dict.items():
    print '%30s    %s'% i



packageNodes = dom.getElementsByTagName('package')
for n in packageNodes:
    name = Evaluate('packageName/text()', n).strip()
    print '--------------- %s ------------' % name

    dict = makeDict(n)
    print
    items = (dict.items())
    items.sort()
    for i in items:
        print '%30s    %s'% i


    minders = Evaluate('minder',n)
    items = []
    mindermat = {}
    for m in minders:
        minderdat = {}
        minderdat['name']      = (Evaluate('identifiedName/text()',m)).strip()
        id                     = (Evaluate('jobID/text()', m)).strip()
        minderdat['id']        = id
        minderdat['history']   = (Evaluate('stateHistory/text()', m)).strip()
        minderdat['state']     = (Evaluate('state/text()', m)).strip()
        minderdat['status']    = (Evaluate('status/text()', m)).strip()
        minderdat['results']   = (Evaluate('resultsPath/text()', m)).strip()
        mindermat[id] = minderdat

    ids = mindermat.keys()
    ids.sort()
    print 
    for id in ids:
        md = mindermat[id]
        print '\n   %s:' % md.pop('name')
        items = mindermat[id].items()
        items.sort()
        for i in items:
            print '     %10s:   %s' % i
