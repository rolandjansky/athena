# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A class to provide an aml file summary of an RTT run
"""
from BaseXMLConverter2 import BaseXMLConverter
from StringIO          import StringIO
from xml.dom.ext       import PrettyPrint
import os.path, time, copy
from exc2string2       import exc2string2
from Evaluate          import Evaluate
from sortValues        import sortValues

class TestRunXMLConverter(BaseXMLConverter):
    def __init__(self, outputFile, package, packageTag, containerPackage, elementCreator, textNodeCreator,
                 callBackConverter, logger):
        BaseXMLConverter.__init__(self, elementCreator, textNodeCreator, logger)
        
        self.nJobs          = 0     # set in setTestRun
        self.testRun        = None  # set in setTestRun
        self.outputFile     = outputFile
        self.callBack       = callBackConverter
        
        self.minderNodes   = {}
        self.minderDicts   = {}
        
        # temp = StringIO()
        # PrettyPrint (self.node, temp)

        self.overviewInfo = {
        'nJobsTotalPackage':      0, # set in setTestRun
        'nJobsInPackage':         0, # number that have reported
        'nJobsDoneInPackage':     0,
        'nJobsOpCloseInPackage':  0,
        'nJobsSuccessInPackage':  0,
        'nJobsFailureInPackage':  0,
        'nPPSuccessInPackage':    0,
        'nPPFailureInPackage':    0,
        'nTestsInPackage':        0,
        'nTestsSuccessInPackage': 0,
        'nTestsFailureInPackage': 0,
        'nRetriesInPackage':      0,
        'nTimeOutsInPackage':     0,
        'nBatchErrorsInPackage':  0,
        'packageName':            package,
        'packageTag':             packageTag,
        'containerPackage':       containerPackage,
        'phase':                  '', # set in setTesteRun
        'packageStatus':          ''  # set in setTesteRun
        }


        # self.makePackageNode()
        self.updatedMinders = []
        #m = 'TestRunXMLConverter: __init__, self.node:\n%s' % (temp.getvalue())
        #self.logger.debug(m)
        self.hasData = False
        
    def setTestRun(self, tr):
        self.testRun = tr
        self.nJobs                             = len(tr.jobMinders)
        self.overviewInfo['nJobsTotalPackage'] = tr.totalJobs
        self.overviewInfo['phase']             = tr.phase
        self.overviewInfo['packageStatus']     = tr.statusText
        
        self.makePackageNode()
        
    def makePackageNode(self):
        # make a conversion table between attribute names of paths, and
        # element names in the output xml file.
        self.node      = self.elementCreator('package')
        [self.addTextNode(self.node, item) for item in self.overviewInfo.items()]
        if self.testRun.vetoFlag: self.node.appendChild(self.elementCreator('vetoed'))

    def update(self, dict, node):
        name = dict['name']
        self.logger.info('TestRunXMLConverter2::update called by minder %s START' % name)
        self.updatedMinders.append(name)
        
        # self.logger.error('Start TestRunXML update for minder: %s' % name)
        
        if name not in self.minderNodes.keys():
            self.overviewInfo['nJobsInPackage']  += 1    
            self.overviewInfo['nTestsInPackage'] += dict['nTests']

        self.minderNodes[name] = node
        self.minderDicts[name] = dict

        nDone        = len( [1 for d in self.minderDicts.values() if d['done']])
        nOpClose     = len( [1 for d in self.minderDicts.values() if d['operatorCloseDown']])
        nJobsSuccess = len( [1 for d in self.minderDicts.values() if d['jobSuccess']])
        nJobsFailure = len( [1 for d in self.minderDicts.values() if d['jobFailure']])
        nPPSuccess   = len( [1 for d in self.minderDicts.values() if d['ppSuccess']])
        nPPFailure =   len( [1 for d in self.minderDicts.values() if d['ppFailure']])

        temp = [d['nTestsSuccess'] for d in self.minderDicts.values()]
        nTestsSuccess = reduce(lambda x,y: x+y, temp, 0)
        temp = [d['nTestsFailure'] for d in self.minderDicts.values()]
        nTestsFailure = reduce(lambda x,y: x+y, temp, 0)

        self.overviewInfo['nJobsDoneInPackage']      = nDone
        self.overviewInfo['nJobsOpCloseInPackage']   = nOpClose
        self.overviewInfo['nJobsSuccessInPackage']   = nJobsSuccess
        self.overviewInfo['nJobsFailureInPackage']   = nJobsFailure
        self.overviewInfo['nPPSuccessInPackage']     = nPPSuccess
        self.overviewInfo['nPPFailureInPackage']     = nPPFailure
        self.overviewInfo['nTestsSuccessInPackage']  = nTestsSuccess
        self.overviewInfo['nTestsFailureInPackage']  = nTestsFailure

        nto = 0
        for d in  self.minderDicts.values():
            try: # only in the dict for BatchJobMinders
                if d['timedOut']:
                    nto += 1
            except:
                pass
            
        self.overviewInfo['nTimeOutsInPackage']  = nto


        nbe = 0
        for d in  self.minderDicts.values():
            try: # only in the dict for BatchJobMinders
                if d['batchError']:
                    nbe += 1
            except:
                pass
            
        self.overviewInfo['nBatchErrorsInPackage']  = nbe


        mretries = []
        [mretries.append(d['nRetries']) for d in self.minderDicts.values()]
        # print 'TestRunXMLCOnverter2: mretries ' + str(mretries)
        nretries = reduce(lambda x,y: x+y, mretries, 0)

        self.overviewInfo['nRetriesInPackage']  = nretries

        self.overviewInfo['phase']              = self.testRun.phase

        self.overviewInfo['packageStatus']      = self.testRun.statusText

        # maintain the node order in the dom. Required for RTT self tests
        self.makePackageNode()
        sortedNodes = sortValues(self.minderNodes)
        # [self.node.appendChild(node) for node in self.minderNodes.values()]
        [self.node.appendChild(node) for node in sortedNodes]

        self.hasData = True
        self.logger.info('TestRunXMLConverter2::update called by minder %s END' % name)

    def outputToFile(self):
        space = ' '
        h = None
        try:
            h = open(self.outputFile, 'w')
            h.write(self.node.toprettyxml(3*space, '\n'))
            h.close()
        except:
            self.logger.error('Unable to output package info XML file. Traceback: \n%s' % exc2string2())
        if h: h.close()
    
    def finalise(self):
        self.logger.debug('Entering TestRunXMLConverter.finalise')
        self.overviewInfo['packageStatus'] = self.testRun.statusText
        self.makePackageNode()
        [self.node.appendChild(node) for node in self.minderNodes.values()]
        self.logger.debug('Package node updated with final testrun status text')
        updatedMinders = []
        infoTuple = (self.testRun.dataForMonitoring(), self.node.cloneNode(True), updatedMinders)
        self.logger.debug('Calling back...')
        self.callBack.update(infoTuple)
        self.logger.debug('Callback complete. Exiting TestRunXMLConverter.finalise method.')

        
    def pushData(self):
        self.logger.debug('begin %s:pushData() for package: %s' % (self.__class__.__name__, self.testRun.name))
        dict = self.testRun.dataForMonitoring()
        # -------------- debug -------------

        #nMindersFromPackage = int(Evaluate('nJobsInPackage/text()', self.node))
        #nMindersFromMinders = len(Evaluate('minder', self.node))
        # print str(self.node)
        #temp = StringIO()
        #PrettyPrint (self.node, temp)
        #assert nMindersFromPackage == nMindersFromMinders
                                  
        # ----------------------------------
        self.logger.debug('TestRunXMLConverter::callBack: %s, TestRunXMLConverter::hasData: %s' % (str(self.callBack), str(self.hasData)))
        if self.callBack and self.hasData:
            # startTime = time.time()
            updatedMinders = copy.copy(self.updatedMinders)
            self.logger.debug('Pushing TestRun data for package: %s' % self.testRun.name)
            self.callBack.update( (dict, self.node.cloneNode(True), updatedMinders) )
            self.hasData = False
            self.updatedMinders = []
            self.logger.debug('Pushed TestRun data for package: %s' % self.testRun.name)
            self.outputToFile() # write out the node to file

    def __del__(self):
        if self.node: self.node.unlink()
        
class FailureTestRunXMLConverter(TestRunXMLConverter):
    def __init__(self, outputFile, package, packageTag, containerPackage, elementCreator, textNodeCreator,
                 callBackConverter, logger):
        TestRunXMLConverter.__init__(self, outputFile, package, packageTag, containerPackage, elementCreator, textNodeCreator,
                                     callBackConverter, logger)
    def pushData(self):
        self.hasData = True
        TestRunXMLConverter.pushData(self)
