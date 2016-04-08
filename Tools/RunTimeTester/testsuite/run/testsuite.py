# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from utility         import timeit, SysPath, SysModules, output
from summaryFile     import RTTSummaryFile
from xml.dom.minidom import Document

import os
import sys

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================

class Status:
    PENDING       = 'pending'
    RUNNING_INIT  = 'running_init'
    RUNNING_JOB   = 'running_job'
    RUNNING_TESTS = 'running_tests'
    DONE          = 'done'

class TestSuite:
    def __init__(self, path):
        self.name         = os.path.basename(path)
        self.syspath      = SysPath()
        self.sysmodules   = SysModules()
        self.testsuitedir = path  # base dir in which all test stuff sits
        self.tests        = []
        self.status       = Status.PENDING
        self.xmlFileDump  = 'testsuite.status.xml'
        self.listeners    = []
        self.tester       = None
        self.dumpXML()
        
    def register(self, callback):
        self.listeners.append(callback)
        
    def inform(self):
        [listener() for listener in self.listeners]

    def info(self):
        if not os.path.exists(os.path.join(self.testsuitedir, '__init__.py')):
            return 'No __init__.py file found in %s, no test description available.' % self.name

        self.syspath += self.testsuitedir
        import __init__
        info = __init__.__doc__
        
        self.syspath -= self.testsuitedir

        self.sysmodules -= '__init__'

        return info

    def run(self):
        print
        print
        print '********************************************'
        print 'Launching testsuite: %s' % self.name
        print '********************************************'
        print
        print
        self.syspath += self.testsuitedir

        os.chdir(self.testsuitedir)
        self.runInit()
        os.chdir(self.testsuitedir)
        self.runJob()
        #os.chdir(self.testsuitedir)
        #self.runTests()
        os.chdir(self.testsuitedir)
        self.syspath -= self.testsuitedir
        self.status = Status.DONE        
        self.dumpXML()


    @timeit
    def runInit(self):
        self.status = Status.RUNNING_INIT
        self.dumpXML()
        import __init__
        initcode = None
        try:
            initcode = __init__.run
        except AttributeError:
            print '>>>> No init code to run in __init__.py'
            return
            
        print '>>>> Running testsuite initialise code'
        initcode()
        self.sysmodules -= '__init__'

    @timeit
    def runJob(self):
        self.status = Status.RUNNING_JOB
        self.dumpXML()

	# Output RTT run logs to testsuite dir
        # this is really ugly.....
        # RTTRunner gets the log dir from an envirnment variable in __init__
        # but the testsuites are threaded, and all the threads are busy modifying the
        # (global) environment variable RTTLogDir. Here I make the distasteful action
        # of resetting the logdir in RTTDRunner after instantiation.
        # old...






        from RTTRunner_NoEnv import Runner
        os.environ['RTTCfgFile'] = os.path.normpath(os.path.join(self.testsuitedir, 'cfg/cfg.xml'))
        print
        print '>>>>> Launching job using cfg file: %s ....' % os.environ['RTTCfgFile']
        print
        RTTSrcDir   = os.environ['RTTSrcDir']
        RTTShareDir = os.environ['RTTShareDir']
        RTTCfgFile  = os.path.normpath(os.path.join(self.testsuitedir, 'cfg/cfg.xml'))
        RTTLogDir   = os.path.join(self.testsuitedir, 'logs')
        useDB       = os.environ.has_key('RTTFactoryUseDB')
        runner = Runner(RTTSrcDir, RTTShareDir, RTTCfgFile, RTTLogDir, useDB, self.name)
        runner.logdir = os.path.join(self.testsuitedir, 'logs')
        self.tester = runner.makeTester()        
        self.dumpXML()
        runner.runTester(self.tester)

        print '>>>>> Finished running job from config file %s ....' % runner.topLevelRTTconfFileLocation

        print
        self.sysmodules -= 'RTTRunner' # remove from sys.modules

    def loadTests(self):
        import tests
        self.tests = [t(self.tester) for t in tests.run() if t.__name__ != 'ExampleTest']

    @timeit
    def runTests(self):
        print
        print '>>>>> Launching post-RTT run tests.py...'

        testsScript = os.path.join(self.testsuitedir, 'tests.py')
        if not os.path.exists(testsScript):
            print 'No tests were found in %s, nothing to do.' % self.testsuitedir
            return

        self.status = Status.RUNNING_TESTS
        self.dumpXML()
        
        self.loadTests()

        for test in self.tests:
            print 'Running tests.%s' % test.__class__.__name__
            test.run()
            print str(test.stats)
        print
        print ' >>>>> Finished running tests.py'

        self.dumpXML()
        self.sysmodules -= 'tests' # remove from sys.modules
        
    def dumpXML(self):
        doc  = Document()
        e = doc.createElement
        t = doc.createTextNode        
        tsEl   = e('testsuite')
        tsnEl  = e('testsuitename')
        tsnEl.appendChild(t(self.name))
        statEl = e('testsuitestatus')
        statEl.appendChild(t(self.status))
        tsEl.appendChild(tsnEl)
        tsEl.appendChild(statEl)
        
        # Report on the job(s)
        if self.tester:
            if hasattr(self.tester, 'xmlConverter') and os.path.exists(self.tester.xmlConverter.RTTSummary):
                h = open(self.tester.xmlConverter.RTTSummary, 'r')
                xmlfile = h.read()
                h.close()      
                summFileEl = e('rttSummaryFile')

                summFilePathEl = e('pathToSummaryFile')
                
                resdir = os.path.abspath(os.path.join(self.testsuitedir, 'Results'))
                relativeSummFilePath = os.path.abspath(self.tester.xmlConverter.RTTSummary).split(resdir)[1].strip()
                if relativeSummFilePath.startswith('/'): relativeSummFilePath = relativeSummFilePath[1:]
                summFilePathEl.appendChild(t(relativeSummFilePath))
                summFileEl.appendChild(summFilePathEl)

                # summFileContentsEl = e('summaryFileContents')
                # summFileContentsEl.appendChild(t(xmlfile))
                # summFileEl.appendChild(summFileContentsEl)

                tsEl.appendChild(summFileEl)
        
        # Report on the tests
        if self.tests:
            testsEl = e('tests')
            for test in self.tests:
                testsEl.appendChild(test.stats.toxml())
            tsEl.appendChild(testsEl)

        doc.appendChild(tsEl)
        output(doc.toprettyxml('  '), os.path.join(self.testsuitedir, self.xmlFileDump))
        self.inform() # update any listeners for state changes

# ==========================================================================================
# ==========================================================================================
# ==========================================================================================


