# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import os, time,string, sys

from RunTimer                       import *
from exc2string2                    import exc2string2
from RetryTester                    import RetryTester
from WorkerJobMinder                import WorkerJobMinder
from ShellCommand                   import ShellCommand
from RTTpath                        import RTTpath


# -------------------------------------------------------------------------
from MethodTracer        import MetaTracer
# uncomment to monitor all method calls (shows args + return values)
# __metaclass__ = MetaTracer
            
class BatchJobMinder(WorkerJobMinder):
    
    def __init__ (self, argBag, jDescriptor):
        WorkerJobMinder.__init__(self, argBag, jDescriptor)

        
        self.logger.debug("initialising BatchJobMinder for job %s" % self.identifiedName)    

        log  = str(RTTpath(self.runPath, self.log))
        sp   = self.shareArea
        rp   = self.runPath
        # self.retryTester = RetryTester(log, elog, logger, sp, rp)
        self.retryTester  = RetryTester(log, self.logger, sp, rp)
        self.retryCount   =  0
        self.retryMax     =  5
        
        self.pid          =  0
        self.timedOut     = False
        self.jobIsInBatch = False
        self.logFileTimer = None
        self.wallTimer    = None
        self.batchError   = False

        self.wallTimeTimeout = 72*60*60 # 72 hours
        self.logFileTimeout  = 1*60*60 # 1

        self.suggestedBatchWallTime = jDescriptor.suggestedBatchWallTime

    # ------------------------------------------------------------------------

    def handleBatchError(self):
        self.batchError = True

    # ------------------------------------------------------------------------

    def stopJob(self):
        pass

    # -------------------------------------------------------------------------

    def checkQueue(self):
        #check if queue exists        
        pass

    # -------------------------------------------------------------------------

    def jobIsRunning(self):
        'Create the RunTimer if job is no longer in batch queues.'
        

        # self.jobIsInBatch is set true in fire()
        queryOK = True
        if not self.jobIsInBatch: return (queryOK, self.jobIsInBatch)

        # if the job is flagged as being in the batch queue, check if this is still the case.
        # flag failure of the interogation of the batch system.
        queryOK, inBatch = self.isJobStillInBatch()
        if not queryOK: return (queryOK, None)

        
        if inBatch: # job is still in batch
            self.logger.debug('Job %s is running in batch' % str(self.pid))
            return (queryOK, True)
        else: # job no longer seen in batch system
            m = 'Job %s is not visible in batch, setting a logFile timer for %d secs' % (self.pid,
                                                                                         self.logFileTimeout)
            self.logger.info(m)
            self.logFileTimer = RunTimer(self.logFileTimeout, self.logger)
            self.jobIsInBatch = False
            return (queryOK, False)
            
    # -------------------------------------------------------------------------
    
    def batchReplyProvesJobIsInQueue(self,reply): pass
    # -------------------------------------------------------------------------
    
    def batchReplyProvesJobIsAbsentFromQueue(self,reply): pass
    # -------------------------------------------------------------------------

    def sleepTime(self,attempt):
        if attempt < 1: return 2
        if attempt == 1: return 2
        if attempt == 2: return 8
        if attempt == 3: return 10

        return 30

    # -------------------------------------------------------------------------
    
    def isJobStillInBatch(self):
        'Returns True is job still in batch, False otherwise.'
        isFound = True
        maxTry  = 5
        attempt = -1
        queryStat  = False
        
        bQuery = self.batchQuery()
        while True:
            attempt += 1
                    
            if attempt==maxTry:
                # give up on this job
                msg  = 'Abandoning attempt to query batch sys after %d attempts %s ' % (attempt,
                                                                                        self.identifiedName)
                self.logger.error(msg)
                queryStat = False
                isFound   = False
                return (queryStat, isFound)


            bReply = ShellCommand([bQuery], self.logger, 100, None, False).getReply()

            if not self.batchQueryReplyFormatOK(bReply):

                msg  = 'Attempt number: %d\nBad reply: command = %s; reply = %s; extra info =%s ' % (
                    attempt,
                    bQuery,
                    bReply,
                    self.getExtraInfoOnBatchQueryCommand())
                self.logger.warning(msg)                    
                stime = self.sleepTime(attempt)
                m = 'sleep for %d seconds before next attempt to interrogate batch system with %s.' % (stime, bQuery)
                self.logger.debug(m)
                time.sleep(stime)
                continue

            if self.batchReplyProvesJobIsInQueue(bReply):
                queryStat = True
                isFound   = True
                self.logger.debug('Job proved to be in queue')
                return (queryStat,isFound)
            elif self.batchReplyProvesJobIsAbsentFromQueue(bReply):
                self.logger.debug('Job proved not to be in queue')
                queryStat = True
                isFound   = False
                return (queryStat,isFound)
            else:
                msg = 'Cannot determine if job is in queue from reply: %s, will resissue query' % bReply
                stime = self.sleepTime(attempt)
                m = 'sleep for %d seconds before next attempt to interrogate batch system with %s.' % (stime, bQuery)
                self.logger.debug(m)
                time.sleep(stime)
                continue
    
    # -------------------------------------------------------------------------

    def getExtraInfoOnBatchQueryCommand(self): 
        extraInfoCmd = ['echo "Echoing PATH variable"',
                        'echo $PATH',
                        'echo ""',
                        'which %s' % self.batchQueryCommand()]
        
        try:
            sc = ShellCommand(extraInfoCmd, self.logger)
        except Exception, e:
            reply = ['Exception was thrown trying to use ShellCommand to gather extra information on bjobs.']
            reply.append(exc2string2())
            reply.append(str(e))
        else:
            reply = sc.getReply()


        replyString = ''
        for line in reply:
            replyString += line + '\n'
        return replyString

    # -------------------------------------------------------------------------
    
    def areLogFilesInRunDir(self, fn):        
        files = os.listdir(self.runPath)

        if fn in files:
            m = "Found the log file %s in rundir %s" % (fn, self.runPath)            
            self.logger.debug(m)

            return True

        m = "Log file %s NOT found in rundir %s" % (fn, self.runPath)            
        self.logger.debug(m)

        return False

    # -------------------------------------------------------------------------

    def retryTest(self):

        if not self.logResult == 'error': return False
        
        if self.retryTester.shouldRetry():
            m ='retryTest() %d %d' % (self.retryCount,
                                      self.retryMax)
            self.logger.debug(m)
            if self.retryCount < self.retryMax:
                self.retryCount += 1
                return True
            
        return False

    # -------------------------------------------------------------------------

    def prepareForRetry(self):

        logs = [str(RTTpath(self.runPath, self.log))]
        
        for log in logs:
            if os.path.exists(log):
                dest = log+'_'+str(self.errorStateCurRetry)
                m = 'State retry: moving file %s to %s' % (log, dest)
                self.logger.debug(m)
                os.rename(log, dest)
                self.makeKeepFileEntry(os.path.basename(dest),
                                       "Log file leading to a retry")

        self.logResult = 'unavailable'
        self.logFileTimer  = None
        self.wallTimer     = None

    # -------------------------------------------------------------------------

    def checkForWallTimeTimeOut(self): return self.checkForTimeOut(self.wallTimer)

    # -------------------------------------------------------------------------

    def checkForLogFileTimeOut(self): return self.checkForTimeOut(self.logFileTimer)

    # -------------------------------------------------------------------------

    def checkForTimeOut(self, timer):

        if not timer: return False
        
        if not timer.timeOut():
            self.timedOut = False
            return False
        else:
            self.timedOut = True
            return True
            
    # -------------------------------------------------------------------------

    def fire(self, what):

        if not self.checkQueue(): return False

        self.logFileTimer = None # remove any time from a previous script
        self.wallTimer    = RunTimer(self.wallTimeTimeout, self.logger)
        self.jobIsInBatch = True
        
        WorkerJobMinder.fire(self, what)

        self.setPid()
        # time.sleep(10) # let the jobs reach the batch queues
        self.logger.debug('Minder: pid = %s' % self.pid)

    # -------------------------------------------------------------------------

    def dataForMonitoring(self):
        dict = WorkerJobMinder.dataForMonitoring(self)
        dict['timedOut']   = self.timedOut
        dict['batchError'] = self.batchError
        return dict
    # ------------------------------------------------------------------------
    
    def status(self):
        
        states  = {
                   'timedOut':          self.timedOut,
                   'batchError':        self.batchError
                   }
        for s in states.keys():
            if states[s]:  return s

        # if self.logResult == 'unavailable': return self.stateEngine.state.state
        if self.logResult == 'unavailable': return self.currentState()
        return self.logResult

    # -------------------------------------------------------------------------

    def __str__(self):
        s  = '|-------------------------------------------|\n'
        s += '|                                           |\n'
        s += '|           Batch  Minder  dump           |\n'
        s += '|                                           |\n'
        s += '|-------------------------------------------|\n'
        s += WorkerJobMinder.__str__(self)+'\n'
        return s

    # -------------------------------------------------------------------------
    
    def dump(self):
        self.logger.debug(self.__str__())

    # -------------------------------------------------------------------------

    def fromQueued(self):
        self.fire('runScript')

        if not self.checkJobSubmissionWasOK(): return 'batchError'
        return 'running'


    # -------------------------------------------------------------------------

    def fromRunning(self):
        "Running is complete if the log files have returned"

        id  = self.pid
        name = self.identifiedName

        if self.checkForWallTimeTimeOut():
            self.diskspace('fromRunning, job has wall timed out')
            self.logger.warning('BatchMinderStateEngine: wall time timeout %s %s' % (name, id) )
            return 'timedOut'

        # query if job is in batch queue
        queryOK, inBatch = self.jobIsRunning()

        # give up if the query failed
        if not queryOK:
            self.diskspace('fromRunning, job has batch error')
            self.logger.debug('BatchMinderJob::fromRunning(), batch system failed to return intelligible response, abandon: %s %s' % (name, id) )
            return 'batchError'

        # if the job is still running, no state transition.
        if inBatch:
            self.logger.debug('Job is found to be in batch.')
            try:
                self.audit()
            except:
                self.logger.error('Exception during auditing: %s' % exc2string2())
            return 'running'
        
        self.logger.debug('Job not seen in batch, checking for log file')

        # Has the batch job log file come back to the run dir?
        if not self.areLogFilesInRunDir(self.log):
            self.logger.debug('Log file not found in run dir.')
            if not self.checkForLogFileTimeOut(): return 'running'
            self.logger.warning('Time out has occured, never found job log file.')
            self.diskspace('Batch log absent, timing out')
            return 'timedOut'

        self.logger.debug('Found batch log file in run dir.')

        self.runningActions() # must perform log checks before retry test.

        if self.retryTest():
            self.diskspace('Going to retry job')
            self.logger.debug('Conditions for retry are met, retrying...')
            return 'retry'

        if self.doPostProcessing():
            self.diskspace('Job done, going to postprocessing')
            self.logger.debug('Job has postprocessing, will do that')
            return 'postProcessingQueued'

        self.logger.debug('No postprocessing for this job, cleaning up.')
        self.diskspace('Job done, going to clean space')
        self.cleanSpace()
        self.diskspace('Job done, clean space is done.')
        return 'done'

    # -------------------------------------------------------------------------

    def archive(self):
        self.logger.debug('Going to archive...')
        [a.archive(self.runPath) for a in self.archivers]   
        self.logger.debug('Finished archiving...')

        # Pop files that were archived from the keepfile dict
        [self.keepFilesToVeto.extend(a.copiedFiles()) for a in self.archivers]

    # -------------------------------------------------------------------------

    def fromOperatorCloseDown(self):
       self.stopJob()
       self.setOperatorCloseDown()
       return 'done'
   
    # -------------------------------------------------------------------------

    def fromPostProcessingQueued(self):
        self.fire('postProcessorScript')
        return 'postProcessingRunning'

    # -------------------------------------------------------------------------

    def fromPostProcessingRunning(self):

        if self.checkForWallTimeTimeOut():
            self.logger.warning('Postprocessing walltime timeout')
            self.diskspace('Wall timeout for postprocessing')
            return 'timedOut'

        queryOK, isRunning =  self.jobIsRunning()
        if isRunning:  
            return 'postProcessingRunning'

        # is the post processir log file visible?
        if not self.areLogFilesInRunDir('postProcessor.log'):
            id  = self.pid
            name = self.identifiedName
            self.logger.debug('No post processing log file seen yet')
            if not self.checkForLogFileTimeOut(): 
                return 'postProcessingRunning'

            self.logger.warning('Post processing log file never found, timing out.')
            self.diskspace('Postprocessing timeout, batch log absent')
            return 'timedOut'
        
        self.readTestResults()

        self.diskspace('Postprocessing done, about to archive.')

        # NOTE!!!!
        # Archiving MUST happen before copyKeepFiles to prevent file duplication.        
        self.archive() # Only for batch jobs, not linux interactive        
        self.diskspace('Postprocessing done, archiving done.')
        
        self.copyKeepFiles() # grab any new keep files

        self.diskspace('Postprocessing done, about to clean space.')        
        self.cleanSpace()
        self.diskspace('Postprocessing done, cleanup done')
        return 'done'


    # -------------------------------------------------------------------------

    def fromRetry(self):

        # the presence of the log files signals the end of the running
        # phase - remove the files,and ensure they are copied to the
        # results web pages.

        self.prepareForRetry()
        return 'queued'
        

    # -------------------------------------------------------------------------

    def fromBatchError(self):
        self.handleBatchError()
        return 'done'

    # -------------------------------------------------------------------------

    def fromTimedOut(self):
        'A marker state'

        self.logger.info('Job timed out, remove from batch %s %s' % (self.pid, self.identifiedName)) 
        self.stopJob()
        return 'done'
        
    # -------------------------------------------------------------------------

    def fromDone(self):
        self.xmlConverter.makeSummaryReport()
        self.setDone()
        return 'done'

    # -------------------------------------------------------------------------

    def runningActions(self):
        """Archiving MUST happen before copyKeepFiles to prevent file duplication."""

        self.audit()
        dirList = os.listdir(self.runPath)
        self.logger.debug('Run dir contents at the end of running state %s' % str(dirList))
        self.checkLog()

        self.runChecks()

        # Calculate disk space usage by the job, probably at a maximum just before archiving
        self.diskspace('Job done, about to archive')

        # NOTE!!!!
        # Archiving MUST happen before copyKeepFiles to prevent file duplication.
        self.archive() # Only for batch jobs, not linux interactive   
        self.diskspace('Job done, archiving is done.')
        self.copyKeepFiles()
        self.diskspace('Job done, keepfiles copied.')

    # -------------------------------------------------------------------------
    # methods to be supplied by subclasses
    # -------------------------------------------------------------------------

    def batchQuery(self):
        raise NotImplementedError()

    # -------------------------------------------------------------------------

    def batchQueryReplyFormatOK(self, bReply):
        raise NotImplementedError()
    # -------------------------------------------------------------------------

    def batchQueryCmd(self):
        raise NotImplementedError()

    # -------------------------------------------------------------------------

    def checkJobSubmissionWasOK(self):
        raise NotImplementedError()

    # -------------------------------------------------------------------------

    def setPid(self):
        raise NotImplementedError()

    # -------------------------------------------------------------------------

