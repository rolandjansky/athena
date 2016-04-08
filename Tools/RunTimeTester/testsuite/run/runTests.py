# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from listFiles import listFiles

import sys
import os
import commands
import pprint
import getopt

def optusage():
    print 'usage: python runTests -d [path to dir above testsuites]'
    
def getTopOfSuitesDir():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'd:')
    except getopt.GetoptError:
        optusage()
        sys.exit(2)

    for o,a in opts:
        if o == '-d': return a
        
    optusage()
    sys.exit(2)

class RunTests:
    def __init__(self):
        cwd = os.getcwd()
        sys.path.append(cwd)
        tspath = getTopOfSuitesDir()
        assert(os.path.exists(tspath))
        ls = os.listdir(tspath)
        ls = [os.path.join(tspath,f) for f in ls] 
        dirs = [(os.path.join(cwd, d), []) for d in ls if os.path.isdir(d) and d.find('.svn') == -1]

        toFind = ['TestCase.py', 'RTTSummaryReference.xml']
        for fn in toFind:
            print
            for (dir, missing) in dirs:
                if not os.path.exists(os.path.join(dir,fn)): missing.append(fn)

        fn = 'RTTSummary.xml'
        for (dir, missing) in dirs:
            if len(listFiles(dir, fn)) != 1:
                missing.append(fn)

        self.okdirs = [d for (d, missing) in dirs if len(missing) == 0]
        self.baddirs = [ (d, missing) for (d, missing) in dirs if len(missing) != 0]

    def runTests(self):
        if len(self.baddirs) == 0:
            print 'All directories have valid tests'
            print
        else:
            print
            for (d, missing) in self.baddirs:
                print 'Could not run tests in %s: missing files:' % d
                pprint.pprint(missing)
                print
        
        for d in self.okdirs:
            os.chdir(d)

            s, o = commands.getstatusoutput('python TestCase.py')
            print o


runner = RunTests()
runner.runTests()
