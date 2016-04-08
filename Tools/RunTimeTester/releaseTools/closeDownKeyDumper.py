# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.dom, time
import xml.dom.minidom

import sys
sys.path.append('../src')
from Evaluate import Evaluate
fn = '/afs/cern.ch/atlas/project/RTT/Results/closeDownFile.xml'
dom = xml.dom.minidom.parse(fn)

de = dom.documentElement
keys = dom.getElementsByTagName('key')
for k in keys:
    els = [e for e in k.childNodes if e.nodeType == e.ELEMENT_NODE]

    dict = {}
    def filldict(e):
        dict[e.tagName] = Evaluate('text()', e)#
        
    [filldict(e) for e in els]

    key = u'startTime'
    # convert to human readable format
    try:
        dict[key] = time.strftime('%y/%m/%d %H:%M',
                                  time.localtime(float(dict[key])))
    except Exception.KeyError, e:
        print 'no time to convert'
    except:
        print 'unknown error converting time', e.__class__.__name__

    print    
    print '--------------------------'
    print
    for i in dict.items():
        print '%30s    %s'% i
