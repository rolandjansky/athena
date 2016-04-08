# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os.path, string, time, xml.dom.minidom, sys, getopt
from getRTTSummaryAsDOM import getRTTSummaryAsDOM
sys.path.append('../src')
from Evaluate import Evaluate

def badJobsFromRTTSummary(dom):
    
    de = dom.documentElement

    packages = de.getElementsByTagName('package')
    npackages = 0
    nbadjobs  = 0
    badpackages = {}
    for p in packages:
        minders = p.getElementsByTagName('minder')
        minders = [m for m in minders if len(Evaluate('errors/text()',m))>0]
        if not minders: continue
        packageName = Evaluate('packageName/text()', p)
        
        mdata = [(Evaluate('identifiedName/text()', m), Evaluate('errors/text()', m)) for m in minders]

        npackages += 1
        nbadjobs += len(mdata)
        badpackages[packageName] = len(mdata)
        
        print
        print 'package -------------------', packageName 
        print
        for d in mdata: print '   minder: %s\n      %s', d


    sortedItems = badpackages.items()
    sortedItems.sort()

    sortedItems = [(i.strip(), j) for (i,j) in sortedItems]
    print
    print 'No of packages with bad jobs:', npackages
    print 'No of bad jobs:              ', nbadjobs
    print
    print 'No of bad jobs by package:   '
    print
    for i in sortedItems:
        print '%20s %10d' % i

def usage():
    print
    print 'BadJobsFromRTTSummary -- checks for jobs with error messages in RTTSummary'
    print
    print 'usage: python BadJobsFromRTTSummary [options]'
    print
    print 'Options'
    print '   -b branch - one of ["dev", "bugfix", "val"]'
    print '   -c cmtconfig - one of ["i686-slc4-gcc34-opt", "i686-slc3-gcc323-opt"]'
    print '   -r release - one of [0-6]'
    print '   -h print this message and exit'
    return
    
        

if __name__ == '__main__':

    try:
        opts, args = getopt.getopt(sys.argv[1:], 'b:c:r:h')
        print opts
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    branch    = None
    config    = None
    release   = None

    configDict = {'slc4': 'i686-slc4-gcc34-opt',
                  'slc3': 'i686-slc3-gcc323-opt'}

    print opts 
    for o, a in opts:
        if o == '-b':
            
            branch = a
            if branch not in ['bugfix', 'dev', 'val']:

                print 'bad branch:', b
                
                usage()
                sys.exit(2)
                
        if o == '-r':
            
            release = a
            if release not in ['0', '1', '2', '3', '4', '5', '6']:

                print 'bad release:', release
                
                usage()
                sys.exit(2)
                
        if o == '-c':
            
            config = a
            if config not in configDict:

                print 'bad cmtconfig:', config
                
                usage()
                sys.exit(2)
            config = configDict[config]

    
    import sys
    # if sys.argv[2] == 'slc4' : conf = 'i686-slc4-gcc34-opt'
    # if sys.argv[2] == 'slc3' : conf = 'i686-slc3-gcc323-opt'
    # root = '/afs/cern.ch/atlas/project/RTT/Results/rel_%s/dev/build/i686-slc3-gcc323-opt' % sys.argv[1]
    root = '/afs/cern.ch/atlas/project/RTT/Results/rel_%s/%s/build/%s' % (release, branch, config)
    fn = os.path.join(root, 'RTTSummary.xml')
    print fn
    dom = xml.dom.minidom.parse(fn)
    print 'parsed ',fn

    
    print badJobsFromRTTSummary(dom)
