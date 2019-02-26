# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Configuration for the Hive Algorithm Scheduler.
#
# Allows easy low level replacement of the specific Scheduler, without
# requiring clients to know which one is in use.
#
# the AlgScheduler will be setup with the same number of threads
# as are specified on the command line with the "--threads=N" parameter
#
# usage:
#  from AthenaCommon.AlgScheduler import AlgScheduler
# clients can then configure runtime printouts, threadpool, etc:
#  AlgScheduler.ShowDataDependencies( True )
#  AlgScheduler.ShowControlFlow( True )
#  AlgScheduler.ShowDataFlow( True )
#  AlgScheduler.setThreadPoolSize( 7 )
#
# if a specific scheduler lacks that option, a warning message is printed
# clients can also replace the default scheduler with another one
#  from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
#  myScheduler = ForwardSchedulerSvc()
#  AlgScheduler.SetScheduler( myScheduler )
#  AlgScheduler.setThreadPoolSize( 7 )
#
# if this is done, the HiveEventLoopMgr also needs to know about it
#    from AthenaServices.AthenaServicesConf import AthenaHiveEventLoopMgr
#    svcMgr.AthenaHiveEventLoopMgr.SchedulerSvc = AlgScheduler.getScheduler().getName()
#


class AlgScheduler:
    def __init__(self,theSched=None):
        """Setup Algorithm Scheduler"""

        from AppMgr import ServiceMgr as svcMgr
        from Constants import INFO

        from ConcurrencyFlags import jobproperties as jps
        from AthenaCommon.Logging import logging

        self.log = logging.getLogger( 'AlgScheduler' )

        if (theSched is None) :
            from GaudiHive.GaudiHiveConf import AvalancheSchedulerSvc
            svcMgr += AvalancheSchedulerSvc()
            self.SchedulerSvc = svcMgr.AvalancheSchedulerSvc
        else :
            svcMgr += theSched
            self.SchedulerSvc = theSched
            
        self.SchedulerSvc.OutputLevel = INFO
        self.SchedulerSvc.CheckDependencies = True
        self.SchedulerSvc.ThreadPoolSize = jps.ConcurrencyFlags.NumThreads()

        self.log.info("setting up " + self.SchedulerSvc.getFullName() + " with " + str(jps.ConcurrencyFlags.NumThreads()) + " threads")


#
## exchange the current scheduler for another one
    def SetScheduler(self,theSched):
        """setup a different Scheduler"""

        if (self.SchedulerSvc.getFullName() != theSched.getFullName()) :
            self.log.info("replacing " + self.SchedulerSvc.getFullName() 
                          + " with " + theSched.getFullName())
            from AppMgr import ServiceMgr as svcMgr
            svcMgr.remove(self.SchedulerSvc)
            svcMgr += theSched
            self.SchedulerSvc = theSched

#
## change the output level
    def OutputLevel(self,level) :
        self.SchedulerSvc.OutputLevel = level
        
#
## control checking of data deps at beginning of job for unmet input deps
    def CheckDependencies(self,check=True):
        if ( 'CheckDependencies' in self.SchedulerSvc.properties() ):
            self.SchedulerSvc.CheckDependencies = check
        else :
            self.log.warning( self.SchedulerSvc.getFullName() + " has no property \"CheckDependencies\"")

#
## control printout of control flow at beginning of job
    def ShowControlFlow(self,show=True):
        if ( 'ShowControlFlow' in self.SchedulerSvc.properties() ):
            self.SchedulerSvc.ShowControlFlow = show
        else :
            self.log.warning(self.SchedulerSvc.getFullName() + " has no property \"ShowControlFlow\"")

#
## control printout of data flow at beginning of job
    def ShowDataFlow(self,show=True):
        if ( 'ShowDataFlow' in self.SchedulerSvc.properties() ):
            self.SchedulerSvc.ShowDataFlow = show
        else :
            self.log.warning(self.SchedulerSvc.getFullName() + " has no property \"ShowDataFlow\"")

#
## control printout of data dependencies at beginning of job
    def ShowDataDependencies(self,show=True):
        if ( 'ShowDataDependencies' in self.SchedulerSvc.properties() ):
            self.SchedulerSvc.ShowDataDependencies = show
        else :
            self.log.warning(self.SchedulerSvc.getFullName() + " has no property \"ShowDataDependencies\"")

#
## set the DataLoader Algorithm to handle unmet input data deps
    def setDataLoaderAlg(self,dataLoadAlg):
        if ( 'DataLoaderAlg' in self.SchedulerSvc.properties() ):
            self.SchedulerSvc.DataLoaderAlg = dataLoadAlg
        else :
            self.log.warning(self.SchedulerSvc.getFullName() + " has no property \"DataLoaderAlg\"")

#
## enable condition handling
    def EnableConditions(self,enable=True):
        if ( 'EnableConditions' in self.SchedulerSvc.properties() ):
            self.SchedulerSvc.EnableConditions = enable
        else :
            self.log.warning(self.SchedulerSvc.getFullName() + " has no property \"EnableConditions\"")

#
## enable verbose view state logging
    def EnableVerboseViews(self,enable=True):
        if ( 'VerboseSubSlots' in self.SchedulerSvc.properties() ):
            self.SchedulerSvc.VerboseSubSlots = enable
        else :
            self.log.warning(self.SchedulerSvc.getFullName() + " has no property \"VerboseSubSlots\"")

#
## explicitly set the thread pool size
    def setThreadPoolSize(self,tps) :
        self.SchedulerSvc.ThreadPoolSize = tps

#
## get the currently configured scheduler
    def getScheduler(self):
        """Get the Scheduler"""
        return self.SchedulerSvc

AlgScheduler = AlgScheduler()
