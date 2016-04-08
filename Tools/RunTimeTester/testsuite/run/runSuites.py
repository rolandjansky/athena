# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, os.path, sys, time
import popen2
import string

def nowSecs():
    return time.strftime('%d %b %H:%M:%S', time.localtime())

    
if not 'RTTSrcDir' in os.environ.keys():
    print 'Please set up the RTTxxx env vars.'
    sys.exit(1)

sys.path.append(os.path.abspath(os.path.dirname(__file__)))
sys.path.append(os.environ['RTTSrcDir'])

from testsuite import TestSuite
from utility import output

# ====================================================
# ====================================================
# ====================================================
# global logger used by many objects: RTTxxx.log
from LoggerSetup import setupLoggers
import logging
setupLoggers(os.environ['RTTLogDir'])


def printJobStatus(d):
    print '\n---------- Job Status ---------'
    for k in d.keys():
        jk = string.ljust(k, 40)
        print '%s:     %s  %s' %  (jk, d[k][0], d[k][1])
class TestFramework:
    def __init__(self):
        self.testIds = [int(id) for id in sys.argv[1:]]
        self.findTestSuites()
        self.chooseTestSuites()
        self.runTestSuites()

    def findTestSuites(self):
        self.basepath = os.path.dirname(os.path.abspath(__file__))
        suitesdir = os.path.normpath(os.path.join(self.basepath, '../testsuites') )
        testsuites = [os.path.join(suitesdir, suite) for suite in os.listdir(suitesdir) if not suite.startswith('.svn')]
        self.testsuites = [TestSuite(t) for t in testsuites if os.path.isdir(t)]

    def askUserForTests(self):
        for i,suite in enumerate(self.testsuites):
            info = suite.info()
            testsuitename = suite.name
            print '%d) %s: \n\t%s' % (i, testsuitename, info)

        bad_choice = True
        testIds = []
        while bad_choice:
            question  = '\nWhich testsuites should be run?\n'
            question += '[where q = quit; * = run all testsuites; otherwise a whitespace-separated list of integers (order entered = run order)]: \n'
            question += '> '
            ans = raw_input(question)

            if ans.lower().strip()=='q':
                sys.exit(0)
                
            # Pick all testsuites
            if ans.strip()=='*':
                bad_choice = False
                testIds = range(len(self.testsuites))
                continue

            # Pick a subset of the testsuites
            try:
                testIds = [int(i) for i in ans.split()]
            except:
                pass
            else:
                if not [t for t in testIds if t not in range(len(self.testsuites))]: bad_choice = False

        return testIds

    def chooseTestSuites(self):
        if self.testIds:
            testIds = self.testIds
        else:
            testIds = self.askUserForTests()

        self.testsuites = [self.testsuites[id] for id in testIds]

    def runTestSuites(self):
        """
        Kludge in running testsuistes as a sub process.
        Testsuite objects have already been made,and used to allow test selection.
        Having done this, the objects are dumped, and new ones are started up in
        sub shells.
        """
        
        self.testsuitesToXML()
        

        tss = []
        jobStatus = {}
        for t in self.testsuites:
            d = t.testsuitedir
            runner = os.path.join(self.basepath, 'testSuiteRunner.py')
            tdir  = os.path.join(d, 'testsuite.out')
            cmd = 'python %s %s>& %s' % (runner, d,tdir)
            #print 'about to popen the cmd: %s' % cmd
            tss.append((t.name, popen2.Popen3(cmd)))
            jobStatus[t.name] = ('running', nowSecs())
        ntests = len(tss)
        printJobStatus(jobStatus)

        while tss:
            toRemove = [p for p in tss if p[1].poll() != -1]
            if toRemove:
                [tss.remove(p) for p in toRemove]
                for p in toRemove:
                    jobStatus[p[0]] = ('completed', nowSecs())

                printJobStatus(jobStatus)
            time.sleep(10)

        print 'all %d tests have completed' % ntests
        
    def testsuitesToXML(self):
        from xml.dom.minidom import Document
        doc = Document()
        e = doc.createElement
        t = doc.createTextNode
    
        tssEl = e('current_testsuites')

        for suite in self.testsuites:
            tsEl   = e('testsuite')
            tspEl  = e('testsuitepath') 
            tspEl.appendChild(t(os.path.abspath(os.path.join(self.basepath, '../testsuites/%s' % suite.name, suite.xmlFileDump))))
            tsnEl  = e('testsuitename')
            tsnEl.appendChild(t(suite.name))

            statEl = e('testsuitestatus')
            statEl.appendChild(t(suite.status))
            tsEl.appendChild(statEl)
            tsEl.appendChild(tsnEl)
            tsEl.appendChild(tspEl)
            tssEl.appendChild(tsEl)
        doc.appendChild(tssEl)

        outpath = os.path.abspath(os.path.join(self.basepath, '../web/current.testsuites.xml'))
        output(doc.toprettyxml('  '), outpath)

# ----------------------------------------------------
# ----------------------------------------------------
# ----------------------------------------------------

if __name__ == '__main__':
    TestFramework()
