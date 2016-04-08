# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
JobMinder that watches other jobs
"""
from Minder                         import Minder
from formatCollection               import formatCollection

# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

class WatcherJobMinder(Minder):
    def __init__(self, argBag, jDescriptor):

        self.jobsToWatch = jDescriptor.confederation
        

        Minder.__init__(self,argBag, jDescriptor, logger)

    # =======================================================
    def removeJob(self, job):
        self.logger.debug('Removed job: '+job.identifiedName)
        self.jobsToWatch.remove(job)

    # =======================================================

    def __str__(self):
        s  = Minder.__str__(self)
        names = [job.identifiedName for job in self.jobsToWatch]
        s += 'job to watch.:      %s' % formatCollection(names)
        return s

    
    def dump(self):
        self.logger.debug('|-------------------------------------------|')
        self.logger.debug('|WatcherJobMinder  dump                     |')
        self.logger.debug('|-------------------------------------------|')
        self.logger.debug(self.__str__()+'\n')
        self.logger.debug(Minder.__str__(self)+'\n')

    def jobIsRunning(self):
        queryOK = True
        isRunning = False
        for j in self.jobsToWatch:
            isRunning = True
            if not j.isDone(): return (queryOK,isRunning)
        return (queryOK,isRunning)
   # -----------------------------------------------
    
    def fromQueued(self):
        return 'running'
            
    # -----------------------------------------------
    
    def fromRunning(self):
        queryOK, isRunning =  self.jobIsRunning()
        
        if isRunning:  return 'running'# no state transition

        self.fromRunningActions()

        if self.doPostProcessing():
            return 'postProcessingQueued'

        return 'done'


    # -----------------------------------------------    

    def fromRunningActions(self):
        'in a method to allow derived class extensions'

        # self.minder.runMoniActions()
        self.runChecks()

        # chainFileCopier present only if running in a chain 
        if self.chainFileCopier:
            self.minder.chainFileCopier.copyFiles()

        # if keep file is also a chain file, this code
        # will silently fail. Intended that the chain
        # file copier is interogated to find new location
        self.copyKeepFiles()

    # -----------------------------------------------
    
    def fromPostProcessingQueued(self):        
        self.fire('postProcessorScript')
        return 'postProcessingRunning'
            
    # -----------------------------------------------
    
    def fromPostProcessingRunning(self):
        queryOK, isRunning =  self.jobIsRunning()
        if isRunning:  return 'postProcessingRunning'# no state transition

        self.minder.readTestResults()
        self.minder.copyKeepFiles() # grab any new keep files

        # make sure ALL files in run and results directory are
        # accessible by all members of the UCL Atlas group
        self.calcPostProcessingResult()

        return 'done'
        
            
