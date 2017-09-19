# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   AthenaCommon/python/ConcurrencyFlags.py
#=======================================================================
""" Flags for concurrency
"""
#
#
__author__  = 'Charles Leggett'
__version__="$Revision: 1.0 $"
__doc__="concurrency flags "




#=======================================================================
# imports
#=======================================================================
from JobProperties import JobProperty, JobPropertyContainer
from JobProperties import jobproperties

class NumProcs(JobProperty):
    """ Flag to indicate number of parallel workers"
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 0
    def _do_action(self):
        import multiprocessing
        if (self.get_Value() == -1):
            self.set_Value( multiprocessing.cpu_count() )
        elif ( self.get_Value() < -1 ) :
            from Logging import log
            log.fatal("nprocs cannot be < -1")
            import sys, ExitCodes
            sys.exit(ExitCodes.CONFIGURATION_ERROR)
        elif (self.get_Value() > multiprocessing.cpu_count()):
            from Logging import log
            log.warning("nprocs is larger than core count [%s]!", 
                        multiprocessing.cpu_count())
            
        return

class NumThreads(JobProperty):
    """ Flag to indicate number of concurrent threads, possibly per worker"
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 0

    def _do_action(self):
        try:
            import GaudiHive
        except ImportError:
            from Logging import log
            log.fatal("GaudiHive not in release - can't use --threads parameter")
            import sys, ExitCodes
            sys.exit(ExitCodes.IMPORT_ERROR)

        if (self.get_Value() < 0):
            from Logging import log
            log.fatal("Number of threads [%s] cannot be negative",self.get_Value())
            import sys, ExitCodes
            sys.exit(ExitCodes.CONFIGURATION_ERROR)

        return


class NumConcurrentEvents(JobProperty):
    """ Flag to indicate number of concurrent events, possibly per worker"
    """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 0

    def _do_action(self):
        try:
            import GaudiHive
        except ImportError:
            from Logging import log
            log.fatal("GaudiHive not in release - can't use --concurrent-events parameter")
            import sys, ExitCodes
            sys.exit(ExitCodes.IMPORT_ERROR)

        if (self.get_Value() < 0):
            from Logging import log
            log.fatal("Number of concurrent events [%s] cannot be negative",self.get_Value())
            import sys, ExitCodes
            sys.exit(ExitCodes.CONFIGURATION_ERROR)

        return


class DebugWorkers(JobProperty):
    """ stops the worker in bootstratp until a SIGUSR1 is recieved. Used as debug hook
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False


class ConcurrencyFlags(JobPropertyContainer):
    """ The global ConcurrencyFlags job property container.
    """
    pass

# add the concurrency flags container to the top container 
jobproperties.add_Container(ConcurrencyFlags)

# I want always the following flags in the Concurrency container  
list_jobproperties = [
    NumProcs,
    NumThreads,
    NumConcurrentEvents,
    DebugWorkers,
]

for i in list_jobproperties:
    jobproperties.ConcurrencyFlags.add_JobProperty(i)

del list_jobproperties

#=======================================================================
