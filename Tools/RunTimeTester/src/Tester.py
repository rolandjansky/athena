# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A Giant class which does everything in its constructor !??!
Does roughly the following:

1. Reads the RTT configuration file, which is used to set up a large number
   of paths and other variables

2. Uses factory method to set up a Commander (mode specific commands)

3. Deletes existing  run directories

4. Reads the Jobs configuration file - this sets up a list of Job Descriptors
   (one JobDescriptor per job)
   JobDescriptors contain info about a job. They also create a run directory for
   thir job.

5. Submit the jobs. One JobMinder per job is created. JobMinders incoporate the
   JobDescriptor, and then monitor the progress of the job.

6. A poll command is periodically sent to the JobMinders. These check their job status.
   This status is interrogated, and html pages are updated with new status info.

"""
#----- Python modules -----

import os, os.path, time, sys
import threading
import logging

#----- RTT modules -----

from exc2string2                import exc2string2

from setMaxMem                  import setMaxMem
from getTimes                   import getTimes
# -------------------------------------------------------------------------
# from MethodTracer        import MetaTracer
# __metaclass__ = MetaTracer
# -------------------------------------------------------------------------


def getHost():    
    try:
        host =  os.environ['HOST']
    except:
        try:
            host =  os.environ['HOSTNAME']
        except:
            host = 'UNKNOWN'
    return host

class Tester:
    
    def __init__(self, argBag):

        self.xmlConverter = argBag.pop('xmlConverter')
        self.logger       = argBag.pop('logger')
        self.startTime2   = argBag.pop('startTime2')
        self.testRuns     = argBag.pop('testRuns')


        self.__dict__.update(argBag.__dict__)
         
        self.endTime1           = 'n/a'
        self.endTime2           = 'n/a'
        self.statusText         = 'Initialising RTT run'
        self.statusTextTime, j  = getTimes()


        self.xmlConverter.setTester(self)

        self.logger.info('maximum process memory set to %s' % setMaxMem)
        self.logger.info('Running on host %s' % getHost())
        
        self.logger.info('---RTT initialisation phase---')
        self.configureMonitorMinders()
        

        self.statusText = '---End of RTT initialisation phase---'
        self.logger.info(self.statusText)
        
# ------------------------------------------------------------------------
# End of __init__
# -------------------------------------------------------------------------

    def configureMonitorMinders(self):
        """
        Pass references to all non-RTTmonitor job minders to each  monitor
        minder.
        """
        
        # pass a list of all non-rttMonitor jobs to the monitor descriptors
        # for monitoring
        minders = []
        [minders.extend(tr.jobMinders) for tr in self.testRuns]
        monis  = [m for m in minders if m.jobGroup == "RTTMonitor"]
        others = [m for m in minders if m.jobGroup != "RTTMonitor"]

        m  = 'Number of monitoring minders '
        m += '%d number non moni %d' %(len(monis), len(others))
        self.logger.debug(m)
        [moni.setJobs(others) for moni in monis]
        

    
#-------------------------------------------------------------------------
#----  EXECUTE -----------------------------------------------------------
#-------------------------------------------------------------------------

    def execute(self):
        #-----------------------------------------------
        # Loop through and execute the run method for
        # each test run object
        #-----------------------------------------------
        statusText =  '---Tester  execute phase starts ---'
        self.logger.info(statusText)

        # start the xmlConverter looking for testrun reports
        self.xmlConverter.start() # start the thread

        
        self.executeTestRuns()

        # close down the XMLCOnverter
        
        self.logger.info('Waiting for xml converter thread to terminate')
        self.xmlConverter.terminateThread()
        self.xmlConverter.join()
        self.xmlConverter.finalise()
        statusText =  '---Tester  execute phase ends ---'
        self.logger.info(statusText)
        
        #------------------------------------------
        # Output run summariser files
        #------------------------------------------
        statusText =  '---Tester  monitoring output starts ---'
        self.logger.info(statusText)
        self.doMonitoringOutput()
        statusText =  '---Tester  monitoring output ends ---'
        self.logger.info(statusText)
        
# -------------------------------------------------------------------------

    def doMonitoringOutput(self):
        
        # self.doRunSummariserOutput()

        #------------------------------------------
        # create Monitoring Plots
        #------------------------------------------
        self.doTimingOutput()

# -------------------------------------------------------------------------

    def executeTestRuns(self):

        ntr = 0
        for testRun in self.testRuns:
            try:
                
                m = 'Starting test run %s with %d jobs' % (
                    testRun.name,
                    len(testRun.jobMinders)
                    )
                self.logger.debug(m)
                
                testRun.start() # start the run method in a new thread
                self.logger.debug("Started  testrun thread %i %s" %
                             (ntr, str(testRun.name)))
                ntr+=1
            except Exception, e:
                msg  = 'TestRun %s raised exception on thread start. Traceback:\n %s\n%s' % (str(testRun.name),
                                                                                             str(e),
                                                                                             exc2string2())
                self.logger.warning(msg)

        # wait for al the test runs to complete
        [self.logger.debug('Thread running for TestRun %s' % str(tr.name)) for tr in self.testRuns]
        [tr.join() for tr in self.testRuns]
        [self.logger.debug('Thread terminated for TestRun %s' % str(tr.name)) for tr in self.testRuns]

#-------------------------------------------------------------------------
#-------------------------------------------------------------------------

    def doTimingOutput(self): [tr.printMethodTimes() for tr in self.testRuns]
    # def doTimingOutput(self): pass
    
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
# ----  FINALISE ----------------------------------------------------------
# -------------------------------------------------------------------------

    def finalise(self):
        self.endTime1, self.endTime2 = getTimes()
        statusText = '---finalise - starts  ---'
        self.logger.info(statusText)
        statusText = '---finalise - ends  ---'
        self.logger.info(statusText)
        self.logger.info('RTT Tester close down...')
        logging.shutdown()
        # sys.exit()

