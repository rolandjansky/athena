# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.dom.minidom, sys, getopt, os
#import xml.dom


"""
Replace xpath version Evaluate because I (PS)cant get xpath to work at cern
for the moment (14/6/05).
"""

from xml.dom.minidom     import parse
import os.path
sys.path.append('../src')
from Evaluate import Evaluate
from exc2string2 import exc2string2

#def Evaluate(path, element):
#    paths = path.split('/')
#/afs/cern.ch/atlas/project/RTT/Results/rel_3/dev/build/i686-slc3-gcc323-opt/RTTSummary.xml
#    curGenEls = [element]
#    for p in paths:
#        nextGenEls = []
#        if p == 'text()':
#            texts = []
#            [texts.extend(getText(e)) for e in curGenEls]
#            text = reduce(lambda x,y:x+y, texts,'')
#            return text
#        else:
#            # [nextGenEls.extend(e.getElementsByTagName(p)) for e in curGenEls]
#            [nextGenEls.extend(e.childNodes) for e in curGenEls]
#            nextGenEls = [n for n in nextGenEls if n.nodeName == p]
#            # print p, len(nextGenEls), str([n.nodeName for n in nextGenEls])
#            
#        curGenEls = nextGenEls
#    return curGenEls
#def getText(element):
#    texts = [node.data for node in element.childNodes if node.nodeType == node.TEXT_NODE]
#    text = reduce(lambda x,y:x+y, texts, '')
#    return text
  

def processPackage(node, tagnames):
    packagename =  Evaluate('packageName/text()', node).strip()
    jobs =  Evaluate('minder', node)

    res = '-------- '+packagename.ljust(30)+'-------------\n\n'
    for j in jobs:
        res += '   '+Evaluate('identifiedName/text()', j).strip()+'\n\n'
        texts =  [(tagname.ljust(20),Evaluate(tagname+'/text()', j).strip('\n\n')) for tagname in tagnames]
        for t in texts:
            res +=  '      %20s %s\n' % t
    return res
#print len(states)


def usage():
    print 'Name - dumpMinderTag.py  -- dump minder text for user provided xml tags'
    print 'Synopsis python dumpMinderTag -r release -s slc version -t tags [-f] [-h]'
    print '  -r Mandatory. release number in range [0,6]'
    print '  -s Mandatory. slc3 version (slc3 or slc4)'
    print '  -t mandatory. colon separated list of tags tag1:tag2:...'
    print '  -f write to file to program provided location'
    print '  -h print  this help message and exit'
if __name__ =='__main__':
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'r:s:t:fh')
    except:
        usage()
        sys.exit(2)
        
    cmtTarget = None
    release   = None
    tags      = []
    of        = False
    
    for o,a in opts:
        if o == '-r':
            if a in ['0', '1', '2', '3', '4', '5', '6']:
                release = 'rel_'+a
        if o == '-s':
            if a == 'slc3': cmtTarget = 'i686-slc3-gcc323-opt'
            if a == 'slc4': cmtTarget = 'i686-slc4-gcc34-opt'

        if o == '-t':
            tags = a.split(':')

        if o == '-f':
            of = True
        if o == '-h':
            of = usage()
            sys.exit(0)

    
    if (not cmtTarget or not release or not tags):
        usage()
        sys.exit(2)

    fn='/afs/cern.ch/atlas/project/RTT/Results/%s/dev/build/%s/RTTSummary.xml' % (release, cmtTarget)

    print '\nexamining %s \n' % fn

    if not os.path.exists(fn):
        print 'no summary file %s, exiting' % fn
        sys.exit(0)

    print 'Will look for the tags:'
    for t in tags: print '   ',t
    
    if of:
        of = '/afs/cern.ch/user/r/rtt/Scratch/dumpMinderTag_%s_%s.log' % (release, cmtTarget)
        print 'Output will be written to ', of
        
    try:
        dom = xml.dom.minidom.parse(fn)
    except:
        print '\n\nCould not parse file', fn
        print exc2string2()
        sys.exit(2)
        
    packages = dom.getElementsByTagName('package')

    if of:
        outf = open(of, 'w')
    else:
        outf = sys.stdout
        
    [outf.write(processPackage(p, tags)) for p in packages]
