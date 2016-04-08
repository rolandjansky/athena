# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This module bundles the RTT factory functions.
These functions are written as classes to help write test code.
"""

from exc2string                   import exc2string

from Factory_BaseClass            import Factory
from JobGroupDirectoryMaker       import JobGroupDirectoryMaker, JobGroupDirectoryMakerWatcher
from RTTSException                import RTTCodingError
from exc2string2                  import exc2string2
# -------------------------------------------------------------------------
# DirectoryMaker Factory
# -------------------------------------------------------------------------

class JobGroupDirectoryMakerFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
    
    self.dict = {
      'WorkerJobMinder':           JobGroupDirectoryMaker,
      'LSFBatchJobMinder':         JobGroupDirectoryMaker,
      'PBSJobMinder':              JobGroupDirectoryMaker,
      'LinuxInteractiveJobMinder': JobGroupDirectoryMaker,
      'ErrorJobMinder':            JobGroupDirectoryMaker,
      'PseudoJobMinder':           JobGroupDirectoryMaker,
      'ChainJobMinder':            JobGroupDirectoryMaker,
      'SequentialMinder':          JobGroupDirectoryMaker,
      'ParallelMinder':            JobGroupDirectoryMaker,
      'WatcherJobMinder':          JobGroupDirectoryMakerWatcher,
      }
    
    
  def create(self, descSetUpRunDirFn, minderClassName):
    
    if minderClassName in self.dict.keys():
      return self.dict[minderClassName](descSetUpRunDirFn, self.logger)
    try:
      directoryMaker = self.dict[minderClassName](descSetUpRunDirFn, self.logger)
    except KeyError:
      msg  = 'JobGroupDirectoryMakerFactory: key not in dictionary: %s' % minderClassName
      raise RTTCodingError(msg)
    except Exception, e:
      'Error constructing DirectoryMaker from minder type %s \nException: %s\Traceback:\n%s' % (minderClassName,
                                                                                                str(e),
                                                                                                exc2string2()
                                                                                                )
  
