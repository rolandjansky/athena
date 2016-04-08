# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
TestRun subclasses threading.Thread to allow concurrent running of instances.

02/02/09 out of date documentation

1. Submit the jobs. One JobMinder per job is created. JobMinders incoporate the
   JobDescriptor, and then monitor the progress of the job.class 

2. A poll command is periodically sent to the JobMinders. These check their job status.
   This status is interogated, and html pages are updated with new status info.

3. After all jobs have completed perform cleanup activities
"""
#----- Python modules -----

import time, gc

#----- RTT modules -----

from Factory_Launcher    import launcherFactory
from Tools               import sendMail
from Tools2              import deSpamifyEmailAddress

from MethodTimer         import MethodTimer

from memstat             import memstat
from exc2string2         import exc2string2
from RTTSException       import RTTCodingError
import threading
from MethodTracer        import MetaTracer

# uncomment to monitor all method calls (shows args + return values)
# __metaclass__ = MetaTracer
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------

from TestRun import TestRun

class UnifiedConfigurationTestRun(TestRun):
    
    def __init__(self, minders, packageName, closeDownKey, closeDownKeyMaker,
                 mode, devRun, mailer, logger, statusText=''):
        # the summaryCallBack object passed in is the TesterXMLConverter.

        statusText = 'initialising'
        TestRun.__init__(self, minders, packageName, closeDownKey, closeDownKeyMaker,
                         logger, statusText)

        self.mode   = mode
        self.devRun = devRun
        self.mailer = mailer

# -------------------------------------------------------------------------

    def removeBadMinders(self):
        msg  = 'package = '+str(self.name)
        msg += ' No. of jobs before removing bad jobs '+str(len(self.jobMinders))
        self.logger.debug(msg)
        
        # [m.makeJobGroupDirs() for m in self.jobMinders]
        jobsToRemove = [m for m in self.jobMinders if m.removeMe]

        lminders = len(self.jobMinders)
        ltoremove = len(jobsToRemove)
        [m.unlink() for m in jobsToRemove]
        [self.jobMinders.remove(m) for m in jobsToRemove]
        
        assert (lminders-ltoremove-len(self.jobMinders)==0)
        
        msg  = 'package = '+str(self.name)
        msg += ' No. of jobs after removing bad jobs '+str(len(self.jobMinders))
        self.logger.info(msg)
        
# ------------------------------------------------------------------------- 

    def run(self):
        """split the jobs into various phases. Each phase runs to completion
         before starting the next one."""
        self.logger.info("TestRun run method starts")

        self.statusText = 'running'

        rttPilotJobs = [j for j in self.jobMinders if j.rttPilotJob]
        
        atnPilotJobs = [j for j in self.jobMinders if j.rttATNJob]
                             
        rttMainJobs  = [j for j in self.jobMinders if (j not in atnPilotJobs and j not in rttPilotJobs)]

        phases = [('atnPilotJobPhase', atnPilotJobs), ('rttPilotJobPhase', rttPilotJobs), ('rttMainJobPhase',rttMainJobs)]
        for phaseName, phaseJobs in phases:
            self.phase = phaseName
            m = "TestRun runMinder method starts for phase %s" % phaseName
            self.logger.info(m)
            self.runMinders(phaseJobs)
            [self.jobMinders.remove(m) for m in phaseJobs] # allow garbage collection
            [m.unlink() for m in phaseJobs]
            self.xmlConverter.pushData()

        self.mailer.sendMailToPackageDevelopers()

        self.statusText = 'done'
        self.logger.debug('status text set to done')
        self.xmlConverter.finalise()
        self.logger.debug('converter update method called.')

# -------------------------------------------------------------------------

    def runMinders(self, jobMinders):

        # make a list of jobs which do not have self.done = True
        msg = "TestRun job launching starts - active jobs: %d, jobs this phase %d" % (len(self.jobMinders), len(jobMinders))
        self.logger.debug(msg)
        s = 'Jobs:\n'
        for j in jobMinders: s += '%s:%s\n' % (j.__class__.__name__, j.identifiedName)
        self.logger.debug(s)

        
        launcher = launcherFactory(self.mode, jobMinders, self.logger)


        sleepSeconds = 300
        if self.devRun: sleepSeconds = 3 # for development purposes, shorten sleeptime
        

        msg = '%s entering poll loop: no of jobs: %d sleep time %d s' % (self.name,
                                                                         len(jobMinders),
                                                                         sleepSeconds
                                                                         )
        self.logger.debug(msg) # goes to screen

            
        while jobMinders:
            self.pollLoopBody(jobMinders, launcher)

            # self.printMethodTimes()

            # don't take this out!
            time.sleep(sleepSeconds)

        self.logger.info('TestRun exited poll loop, leaving TestRun run() method.')
            
    # -------------------------------------------------------------------------

    def pollLoopBody(self, jobMinders, launcher):

            if self.operatorCloseDownTest():
                self.logger.info('TestRun %s detected operator close down signal' % self.name)
                [m.queueOperatorCloseDown() for m in jobMinders]
                
            launcher.launchNextUnlaunchedJob()

            try:
                [m.poll() for m in jobMinders]
            except:
                m = 'Error polling minder: %s ' % exc2string2()
                self.logger.error(m)
                self.logger.error(memstat())
                self.logger.error('Garbage:\n%s' % gc.garbage)
                self.logger.error('Collecting garbage...')
                gc.collect()
                self.logger.error('...Collected garbage')
                raise RTTCodingError(m)
                
                
            # the following print is done once when the minder goes to done
            # the following is debug
            # [m.printMethodTimes() for m in self.jobsActive]


            toRemove  = [m for m in jobMinders if m.isDone()]
            self.mailer.collectStats(toRemove) # info gathering for email sending purposes
            [m.printMethodTimes() for m in toRemove]
            [m.unlink() for m in toRemove]
            [jobMinders.remove(m) for m in toRemove]
            self.doneJobs       += len(toRemove)
            self.successfulJobs += len([j for j in toRemove if j.isSuccess()])
            
            self.xmlConverter.pushData()

