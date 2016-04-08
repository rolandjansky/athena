# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from xml.dom.minidom import parse
from Evaluate import Evaluate
import os.path, string

#root = '/afs/cern.ch/atlas/project/RTT/Results/rel_3/dev/build/i686-slc3-gcc323-opt'

import xml.dom, time
import xml.dom.minidom
def makeDict(parentNode):
    els = [e for e in parentNode.childNodes if e.nodeType == e.ELEMENT_NODE]
    
    dict = {}
    def filldict(e):
        dict[string.ljust(e.tagName.strip(), 30)] = Evaluate('text()', e).strip()#
        
    [filldict(e) for e in els]
    return dict


def RTTSummaryToText(dom):
    
    de = dom.documentElement


    dict = makeDict(Evaluate('overview', de)[0])

    lines = []
    
    line  = '|'+'-'*60 +'|'
    vert  = '|'+' '*60 +'|'
    title = '|'+string.center('RTT Run Summary', 60)+'|'
    tim   = '|'+string.center(time.strftime('%d %B %Y  %H:%M  %Z', time.gmtime()), 60) +'|'
    blank = ' '
    lines.extend([line, vert, title, tim, vert, line, blank, blank])

    [lines.append('%30s    %s'% i) for i in dict.items()]
    lines.extend([blank, blank])

    
    tags = ['nPPSuccessInPackage',
            'nRetriesInPackage',
            'nTimeOutsInPackage',
            'nBatchErrorsInPackage',
            'packageName',
            'nTestsSuccessInPackage',
            'nJobsOpCloseInPackage',
            'nTestsFailureInPackage',
            'containerPackage',
            'nJobsDoneInPackage',
            'nJobsSuccessInPackage',
            'nTestsInPackage',
            'packageTag',
            'nPPFailureInPackage',
            'nJobsFailureInPackage'
            ]

    packageNodes = dom.getElementsByTagName('package')
    precords = []
    total = [string.ljust('totRTT', 30), '-', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    for n in packageNodes:
        record = ( (Evaluate('packageName/text()', n).strip()).ljust(30),
                   Evaluate('phase/text()', n).strip(),
                   int(Evaluate('nJobsTotalPackage/text()', n).strip()),
                   int(Evaluate('nJobsInPackage/text()', n).strip()),
                   int(Evaluate('nJobsSuccessInPackage/text()', n).strip()),
                   int(Evaluate('nJobsFailureInPackage/text()', n).strip()),
                   int(Evaluate('nTimeOutsInPackage/text()', n).strip()),
                   int(Evaluate('nBatchErrorsInPackage/text()', n).strip()),
                   int(Evaluate('nJobsOpCloseInPackage/text()', n).strip()),
                   int(Evaluate('nTestsInPackage/text()', n).strip()),
                   int(Evaluate('nTestsSuccessInPackage/text()', n).strip()),
                   int(Evaluate('nTestsFailureInPackage/text()', n).strip()),
                   int(Evaluate('nJobsDoneInPackage/text()', n).strip()))

        for i in range(len(total[2:])):
            j = i+2
            total[j] += record[j]
                  

        precords.append(record)

    precords.sort()
    records = [tuple(total)]
    records.extend(precords)

    print
    labels = (' ', 'pkge', 'jobs', 'jobs', 'jobs', 'jobs', 'jobs', 'jobs','oper', 'tests','tests', 'tests', 'process')
    lines.append( '%30s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s  %8s' % labels)
    labels = (' ', 'phase', 'total', 'submt','succ','fail', 't_out', 'b_err', 'abort', 'total', 'succ', 'fail', 'complete')
    lines.append('%30s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s  %8s' % labels)

    for r in records:

        lines.append( '%s %5s %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d  %8d' % r)

    result = ''
    for l in lines:
        result += l  + '\n'

    return result


if __name__ == '__main__':


    import sys
    if sys.argv[2] == 'slc4' : conf = 'i686-slc4-gcc34-opt'
    if sys.argv[2] == 'slc3' : conf = 'i686-slc3-gcc323-opt'
    # root = '/afs/cern.ch/atlas/project/RTT/Results/rel_%s/dev/build/i686-slc3-gcc323-opt' % sys.argv[1]
    root = '/afs/cern.ch/atlas/project/RTT/Results/rel_%s/dev/build/%s' % (sys.argv[1], conf)
    sys.path.append('../src')
    fn = os.path.join(root, 'RTTSummary.xml')
    dom = xml.dom.minidom.parse(fn)
    print 'parsed ',fn

    
    print RTTSummaryToText(dom)
