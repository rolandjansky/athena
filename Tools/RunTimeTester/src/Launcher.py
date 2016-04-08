# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Launchers govern the rate at which jobs are started.
BatchLauncher,       submits all the jobs in rapid succession.
InteractiveLauncher, waits for a job to finish before submitting the next.
"""
from RTTSException      import RTTCodingError
from popen2             import Popen4
import os

from Tools2             import isType

class Launcher:
    def __init__(self, maxActiveJobs, jobs, logger):
        if maxActiveJobs<=0:
            msg = 'Nonsense value for maxActiveJobs: %d' % maxActiveJobs
            logger.error(msg)
            raise RTTCodingError(msg)            

        self.logger        = logger
        self.watchers      = []
        self.workers       = []
        # self.njobs         = 0
        self.nLaunchedJobs = 0

        self.maxActiveJobs = maxActiveJobs

        self.workers   = [job for job in jobs if isType(job, 'WorkerJobMinder')]
        self.watchers  = [job for job in jobs if isType(job, 'WatcherJobMinder')]        
        # self.njobs     = len(self.watchers)

        self.logger.debug('Launcher init: workers %d, watchers %d' % (len(self.workers),len(self.watchers)))
        
    def isWorkerJob(self,job):
        heirarchy = [b.__name__ for b in job.__class__.__bases__]
        heirarchy.append('WorkerJobMinder')
        heirarchy.append('ErrorJobMinder')
        return job.__class__.__name__ in heirarchy

    def isWatcherJob(self,job):
        heirarchy = [b.__name__ for b in job.__class__.__bases__]
        heirarchy.append('WatcherJobMinder')
        return job.__class__.__name__ in heirarchy
        
    def fire(self, job):
        os.chdir(job.runPath)
        job.poll()
        self.nLaunchedJobs=self.nLaunchedJobs+1
        
class ThrottleLauncher(Launcher):

    def __init__(self, maxActiveJobs, jobs, logger):
        Launcher.__init__(self, maxActiveJobs, jobs, logger)
        self.hasBeenLaunched  = []
        self.firstBatchWaits = True

    def launchNextUnlaunchedJob(self):
        
        if len(self.watchers)+ len(self.workers) ==0: return
        
        if self.firstBatchWaits:

            self.firstBatchWaits = False
            count = 0
            for job in self.workers:
                self.fire(job)
                self.hasBeenLaunched.append(job)
                count += 1
                msg = 'first batch, fired '+job.identifiedName
                self.logger.debug(msg)
                if count >= self.maxActiveJobs: return

        # remove finished jobs. Needs two lines: the next line
        # gets messed up in the iteration over changing lists
        # BAD: [self.watchers.remove(job) for  job in self.watchers if job.done]
        doneJobs = [job  for  job in self.workers  if job.isDone()]
        [self.workers.remove(job)  for  job in doneJobs]

        doneJobs = [job  for  job in self.watchers  if job.isDone()]
        [self.watchers.remove(job)  for  job in doneJobs]

        # active jobs are jobs that have been launched, but no yet done.
        nActiveJobs  = len( [m for m in self.workers
                             if m in self.hasBeenLaunched])
        
            
        nWatcherJobs = 0
        for job in self.watchers:
            if not job.isDone(): nWatcherJobs += 1

    
        
        nToFire = min(self.maxActiveJobs-nActiveJobs, len(self.workers)-nActiveJobs)

        if nToFire>0:
            msg =  'ThrottleLauncher: worker jobs max: %d, fired: %d to fire: %d ' % (
                self.maxActiveJobs,
                nActiveJobs,
                nToFire)
        
            self.logger.debug(msg)
        
        if nToFire < 0:
            self.logger.error('Error: n active> maxActive')

        if nToFire > 0:
            for job in self.workers:
                if job not in self.hasBeenLaunched:
                    self.fire(job)
                    self.hasBeenLaunched.append(job)
                    nToFire = nToFire-1
                    if nToFire == 0: break
                
