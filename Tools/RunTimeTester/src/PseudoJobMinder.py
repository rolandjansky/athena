# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Tracer            import Tracer
from PseudoDescriptor  import PseudoDescriptor
from WorkerJobMinder   import WorkerJobMinder
from RTTSException     import RTTCodingError


class PseudoChainFileCopier:
    def copyFiles(self): return
    
class PseudoJobMinder(WorkerJobMinder, Tracer):
    def __init__(self, argBag, desc):

        commander = argBag.pop('commander')
        argBag.postProcessCommand = 'pseudoPostProcessCommand'
        argBag.submitCommand      = 'pseudoSubmitCommand'

        WorkerJobMinder.__init__(self, argBag, desc)

        self.nRetry       = 0
        self.nIsRunning   = 0
        self.finishedTests = []
        self.chainFileCopier = PseudoChainFileCopier()
        
    def fire(self, rs): print 'script',rs
    def checkJobSubmissionWasOK(self): return True
    def checkForWallTimeTimeOut(self):return False
    def jobIsRunning(self):
        if self.nIsRunning<2:
            self.nIsRunning += 1
            return (True, True)
        
        return  (True, False)
    def audit(self): pass
    def areLogFilesInRunDir(self): return True
    def checkForLogFileTimeOut(self): return False
    def checkLog(self): return 
    def runPostScripts(self): return
    def runMoniActions(self): return
    def runChecks(self): return
    def copyKeepFiles(self): return
    def setProtection(self): return
    def calcPostProcessingResult(self): return
    def retryTest(self):
        self.nRetry += 1
        if self.nRetry>2: return False
        return True
    def doPostProcessing(self): return  False
    def prepareForRetry(self): pass
    def cleanSpace(self): pass
    def stopJob(self): pass
    def handleBatchError(self): pass
    def setDone(self):
        print 'setDone for job ', self.identifiedName
        self.done = True
    def isDone(self):
        return self.done
    def isSuccess(self):
        return True
    # def poll(self):
    #    self.stateEngine.poll()

    def __str__(self):
        s = 'PseudoMinder Dump: empty\n'
        return s
    def dump(self):
        self.logger.debug(self.__str__())

    def readTestResults(self):
        return
    
    def fromQueued(self):
        self.fire('runScript')
        return 'running'

    def fromRunning(self):
        queryOK, isRunning =  self.jobIsRunning()

        if not queryOK:
            msg = 'Unsuccesful query of a job running interactively'
            raise RTTCodingError(msg)

        if isRunning:  return 'running' # no state transition

        self.fromRunningActions()
        
        if self.doPostProcessing():
            return 'postProcessingQueued'
        
        return 'done'

    def fromRunningActions(self):

        self.checkLog()
        
        self.runChecks()
        # chainFileCopier present only if running in a chain 
        if hasattr(self, 'chainFileCopier'):
            self.chainFileCopier.copyFiles()

        # if keep file is also a chain file, this code  will silently fail. Intended that the chain
        # file copier is interogated to find new location
        self.copyKeepFiles()

    def fromPostProcessingQueued(self):
        self.fire('postProcessorScript')
        return 'postProcessingRunning'
            
    def fromPostProcessingRunning(self):
        queryOK, isRunning =  self.jobIsRunning()

        if not queryOK:
            msg = 'Unsuccesful query of a job running interactively'
            raise RTTCodingError(msg)

        if isRunning:  return 'postProcessingRunning'

        self.readTestResults()
        self.copyKeepFiles()
        self.cleanSpace()

        return 'done'
        
    def fromOperatorCloseDown(self): return 'done'
    def fromRetry(self): return 'done'
    def fromBatchError(self): return 'done'
    def fromTimedOut(self): return 'done'
    def fromDone(self):
        self.setDone()
        return 'done'
