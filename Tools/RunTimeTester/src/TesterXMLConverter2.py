# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A class to provide an xml file summary of an RTT run
"""
from BaseXMLConverter2     import BaseXMLConverter
from exc2string2           import exc2string2
from Tools                 import changePerm
from Evaluate              import Evaluate
from MemoryUsageDiagnostic import MemoryUsageDiagnostic
from memstat               import memstat
from RTTSummaryToText      import RTTSummaryToText
from getTimes              import getTimes
from StringIO              import StringIO
from xml.dom.ext           import PrettyPrint


import os, time, commands, gc


import threading

summaryDocumentLock = threading.Lock()

# from MethodTracer        import MetaTracer
# __metaclass__ = MetaTracer


def countOpenFiles():
    basedir = '/proc/self/fd'
    nfiles = len(os.listdir(basedir))
    listfiles =  commands.getoutput('ls -o %s' %basedir)
    return (nfiles, listfiles)



class TesterXMLConverter(threading.Thread, BaseXMLConverter):
    def __init__(self, argBag, createElement, createTextNode):
        
        logger = argBag.pop('logger')
        BaseXMLConverter.__init__(self, createElement, createTextNode, logger)

        threading.Thread.__init__(self)

        self.node               = argBag.pop('node') # node must be passed in for this subclass
        self.devRun             = argBag.pop('devRun')
        self.startTime1         = argBag.pop('startTime1')
        self.startTime2         = argBag.pop('startTime2')
        self.nicosDateFormatted = argBag.pop('nicosDateFormatted')
        self.nicosDate          = argBag.pop('nicosDate')
        self.resultsDirs        = argBag.pop('resultsDirs')
        self.RTTSrcDir          = argBag.pop('RTTSrcDir')
        self.RTTLibDir          = argBag.pop('RTTLibDir')
        self.topProject         = argBag.pop('topProject')
        self.otherProject       = argBag.pop('otherProject')
        self.cmtConfig          = argBag.pop('cmtConfig')
        self.originalBranch     = argBag.pop('originalBranch')
        self.site               = argBag.pop('site')
        self.release            = argBag.pop('release')
        self.releaseType        = argBag.pop('releaseType')
        self.runType            = argBag.pop('runType')
        self.legals             = argBag.pop('legals')
        self.RTTSummary         = argBag.pop('RTTSummary')

        self.__dict__.update(argBag.__dict__)
        
        self.testRunNodes  = {}
        self.testRunDicts  = {}

        self.nlock = 0

        self.packagesToProcess = {}
        self.packageLocks      = {}
        self.exit              = False

        self.testRunsAwaitingLock = []

        self.pollSleepTime = 60
        if self.devRun: self.pollSleepTime = 3
        
    def setTester(self, tester):
        self.tester = tester
        
        self.initOverviewData()
        overviewNode = self.makeOverviewNode() #None->node
        self.node.appendChild(overviewNode)
        

        
    def makeOverviewNode(self):
        node = self.elementCreator('overview')
        [self.addTextNode(node, item) for item in self.overviewData.items()]
        return node
    
    def initOverviewData(self):

        def getVersion(path):
            version = 'unknown'
            vpath = os.path.join(str(path), '../cmt/version.cmt')
            try:
                f = open(vpath, 'r')
                version = f.read()
                f.close()
            except:
                m='Error reading version from %s\n%s'%(vpath, exc2string2())
                self.logger.error(m)
            return version



        self.overviewData = {
            'startTime1':         self.startTime1,
            'startTime2':         self.startTime2,
            'latestUpdateTime1':  self.startTime1,
            'latestUpdateTime2':  self.startTime2,
            'nicosDate1':         self.nicosDateFormatted,
            'nicosDate2':         self.nicosDate,
            'resBaseDir':         self.resultsDirs['base'],
            'resReleaseDir':      self.resultsDirs['packages'],
            'RTTSrcVersion':      getVersion(self.RTTSrcDir),
            'RTTLibVersion':      getVersion(self.RTTLibDir),
            'endTime1':           self.tester.endTime1,
            'endTime2':           self.tester.endTime2,
            'nPackages':          len(self.tester.testRuns),
            'statusText':      "%s %s" % (self.tester.statusText,
                                          self.tester.statusTextTime),
            'topProject':         self.topProject,
            'otherProject':       self.otherProject,
            'cmtConfig':          self.cmtConfig,
            'originalBranch':     self.originalBranch,
            'site':               self.site,
            'release':            self.release,
            'releaseType':        self.releaseType,
            'runType':            self.runType,
            'nPackagesAllJobsOK': 0

            }        


        # add the release name to the overview data
        if self.legals.isNightlyInRelease(self.overviewData['release']):
            self.overviewData['releaseName'] = os.path.join(self.overviewData['release'],
                                                            self.overviewData['originalBranch'])
        else:
            self.overviewData['releaseName'] = self.overviewData['release']

    def dynamicOverviewData(self):        
        self.overviewData['nPackages']       = len(self.tester.testRuns)
        self.overviewData['statusText']      = self.getOverviewStatusText()
        self.overviewData['latestUpdateTime1'], self.overviewData['latestUpdateTime2'] = getTimes()

        dicts = self.testRunDicts.values()

        def getJobCounts(dict):
            nSucc = dict['nSucc']
            nJobs = dict['nJobs']
            if nJobs == nSucc: return 1
            return 0

        nAllJobsOK = 0
        for dict in dicts: nAllJobsOK += getJobCounts(dict)

        self.overviewData['nPackagesAllJobsOK'] = nAllJobsOK

    def getOverviewStatusText(self):        
        dicts = self.testRunDicts.values()

        def getJobCounts(dict):
            nSucc = dict['nSucc']
            nDone = dict['nDone']
            nJobs = dict['nJobs']
            return (nSucc, nDone, nJobs)

        counts = [getJobCounts(dict) for dict in dicts]
        sDone = sJobs = sSucc = 0
        for nSucc,nDone,nJobs in counts:
            sDone += nDone
            sJobs += nJobs
            sSucc += nSucc
        
        ftime, utime = getTimes()
        return  "%s: jobs succ/done/tot: %i/%i/%i" % (ftime, sSucc, sDone, sJobs)

    def run(self):

        while True:

            self.processPackages()

            if self.exit:
                self.processPackages()
                self.logger.debug('TesterXMLConverter2 exit')
                return

            time.sleep(self.pollSleepTime)
            
    def processPackages(self):
        
        kkeys = self.packagesToProcess.keys()
        self.logger.debug('start %d packages to process %s' % (len(kkeys),
                                                                   str(kkeys)))

        for package in kkeys:
            try:
                self.processPackage(package)
                self.logger.info('Processed package %s' % (package))
            except:
                self.logger.error('Error processing package %s\nTraceback:\n %s' % (package, exc2string2()))

                        
        self.logger.debug('end %d packages to process %s' % (len(kkeys),
                                                             str(kkeys)))

    def processPackage(self, package):
            
        updatedMinders = []
        lock = self.packageLocks[package]
        # testruns add info to a list - need to know all updated minders, but only
        # the last node and dictionary supersede the earlier ones.
        
        
        ##################locked code###################
        lock.acquire(True)
        infoList = self.packagesToProcess[package]
        self.logger.debug('package %s inflList length %d' % (package, len(infoList)))
        [updatedMinders.extend(pUpdatedMinders) for (dict, node, pUpdatedMinders) in infoList]
        dict, node, unusedM = infoList[-1]
        #[node.unlink() for (dict, node, pUpdatedMinders) in infoList[:-1]]
        del self.packagesToProcess[package]
        lock.release()
        ##################locked code###################
            
        self.updateTestRunDict(dict, node)
        #self.updateOverview()
        self.storeXMLData()

        # ensure each minder is named only once
        temp = {}
        for m in updatedMinders: temp[m] = True
        updatedMinders = temp.keys()
        
        # self.tester.updateMonitors(package, updatedMinders)

    def update(self, info):
        # receives information as a dict and as an XML node
        # keep track of the current package values in dictionaries


        # self.logger.debug('memory usage from TesterXMLConverter.update\n %s' % memstat())
        package = info[0]['name']
        self.logger.debug('TesterXMLConverter update called by %s START' % package )
        
        lock = self.packageLocks.setdefault(package, threading.Lock())


        self.testRunsAwaitingLock.append(package)
        self.logger.debug('start %d packages awaiting locks %s' % (len(self.testRunsAwaitingLock), str(self.testRunsAwaitingLock)))


        # lock the code that updates the dictionaries that hold the data to process for each package.
        # the different packages write to different dictionary entries.
        # the lock prevents a package from updating a dictionary entry while the entry is being processed.
        lock.acquire(True)
        self.packagesToProcess.setdefault(package, []).append(info)
        
        self.testRunsAwaitingLock.remove(package)
        self.logger.debug('end %d packages awaiting locks %s' % (len(self.testRunsAwaitingLock), str(self.testRunsAwaitingLock)))
        
        lock.release()
        self.logger.debug('TesterXMLConverter update called by %s END' % package )
        

    def updateTestRunDict(self, dict, node):
        self.logger.debug('updating for package %s ' % dict['name'])
        pName = dict['name']
        self.testRunNodes[pName] = node
        self.testRunDicts[pName] = dict

    def updateOverview(self):
        self.dynamicOverviewData()
        return self.makeOverviewNode()

    def storeXMLData(self):
        """ updates the summaryDocument (in memory)
        updates the XML files, updatesd the monitors
        """

        #self.logger.error('xml before clear:\n%s'%self.tester.summaryDocument.toxml())
        nover = Evaluate('overview', self.node)
        npack = Evaluate('package', self.node)
        self.logger.debug('No of overview, package nodes before clearDOM(): %d %d' % (len(nover),len(npack)))
        self.clearDOM()
        nover = Evaluate('overview', self.node)
        npack = Evaluate('package', self.node)
        self.logger.debug('No of overview, package nodes after  clearDOM(): %d %d' % (len(nover),len(npack)))
        #self.logger.debug('xml after clear:\n%s'%self.tester.summaryDocument.toxml())

        nodes = [self.updateOverview()]
        nodes.extend(self.testRunNodes.values())
        [self.node.appendChild(n) for n in nodes]
        
        nover = Evaluate('overview', self.node)
        npack = Evaluate('package', self.node)
        self.logger.debug('No of overview, package nodes after readding: %d %d' % (len(nover),len(npack)))

        md = MemoryUsageDiagnostic(self.logger)
        md.execute()
        #  nopen, listopen = countOpenFiles()
        # self.logger.debug('no of open files %d ' % nopen)
        # self.logger.debug('no of open files %d\nopen files:\n%s' % countOpenFiles())
        
        self.doXMLDataStorage()



    def doXMLDataStorage(self):


    
        def storeIt(fn):
            name = fn.__name__
            self.logger.debug('about to call %s' % name)
            try:
                fn()
                return ''
            except:
                return exc2string2()

        fns = [self.xmlToFile, self.overviewNodeToFile, self.xmlToText]

        errs = [(f.__name__, storeIt(f)) for f in fns]

        [self.logger.error('Error executing %s\n%s: ' % (fname,  tb))
         for fname, tb in errs if tb != '']

        
    def finalise(self):
        """Called from Tester.finalise(). Updates the summary document overview data with the
        endTimes newly created in that method. Stores the new XML and calls Tester.updateMonitors().
        """

        self.processPackages()
        endTime1, endTime2 = getTimes()

        self.overviewData['endTime1']          = endTime1
        self.overviewData['endTime2']          = endTime2
        self.overviewData['latestUpdateTime1'] = endTime1
        self.overviewData['latestUpdateTime2'] = endTime2
        self.storeXMLData()

    def terminateThread(self): self.exit = True

    # --------------------------------------------------------------------------

    def xmlToText(self):

        of = open(os.path.join(self.resultsDirs['packages'], 'RTTSummary.txt'), 'w')
        of.write( RTTSummaryToText(self.tester.summaryDocument))
        of.close


    def xmlToFile(self):
        temp = StringIO() 
        PrettyPrint(self.tester.summaryDocument.documentElement, temp)

        of = open(self.RTTSummary, 'w')        
        of.write(temp.getvalue())
        # of.write(self.tester.summaryDocument.toprettyxml('   '))
        # of.write(self.tester.summaryDocument.toprettyxml('   ','\n'))
        # of.write(self.tester.summaryDocument.toxml('utf-8'))
        of.close

    def overviewNodeToFile(self):
        of = open(os.path.join(os.path.dirname(self.RTTSummary), 'overview.xml'), 'w')
        of.write(self.updateOverview().toprettyxml('   ','\n'))
        of.close

    def clearDOM(self):
        
        nodesToRemove = Evaluate('overview', self.node) # do not remove the RTTSummary/ATNSummary/overview
        nodesToRemove.extend(self.node.getElementsByTagName('package'))

        try:
            xmlstr = self.node.toxml()
        except:
            self.logger.error('Clear dom 1, could not convert doc to string. self.node exists: %s' % self.node)
        [self.node.removeChild(n) for n in nodesToRemove]
        try:
            xmlstr = self.node.toxml()
        except:
            self.logger.error('Clear dom 2, could not convert doc to string')
        # [n.unlink() for n in nodesToRemove]
        self.logger.debug('Garbage collection is enabled?: %s' %  str(gc.isenabled()))
        gc.collect()
        self.logger.debug('Garbage from clearDom: %s' % str(gc.garbage))
        #for g in gc.garbage:
        #    m = '\nreferents:\n%s\n\nreferrers:\n%s' % (str(gc.get_referents(g)), str(gc.get_referrers(g)))
        #    self.logger.error(m)
        
        try:
            xmlstr = self.node.toxml()
        except:
            self.logger.error('Clear dom 3, could not convert doc to string')
        
    def printMethodTimes(self):
        print '--TesterXMLConverter--\n%s' % self.formatMethodTimes()
                                                  
