# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Factory_BaseClass                 import Factory
from LinuxInteractiveMinderStateEngine import LinuxInteractiveMinderStateEngine
from WatcherMinderStateEngine          import WatcherMinderStateEngine
from ErrorMinderStateEngine            import ErrorMinderStateEngine
from BatchMinderStateEngine            import BatchMinderStateEngine
from ContainerMinderStateEngine        import ContainerMinderStateEngine
from ChainMinderStateEngine            import ChainMinderStateEngine

from exc2string2                       import exc2string2
from RTTSException                     import RTTCodingError

class MinderStateEngineFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
    self.dict = {
      'LinuxInteractiveJobMinder': LinuxInteractiveMinderStateEngine,
      'WatcherJobMinder':          WatcherMinderStateEngine,
      'ErrorJobMinder':            ErrorMinderStateEngine,
      'LSFBatchJobMinder':         BatchMinderStateEngine,
      'ContainerMinder':           ContainerMinderStateEngine,
      'SequentialMinder':          ContainerMinderStateEngine,
      'ParallelMinder':            ContainerMinderStateEngine,
      'ChainJobMinder':            ChainMinderStateEngine,
      'PseudoJobMinder':           BatchMinderStateEngine
      }

  def create(self, minder, state='queued'): 

    key = minder.__class__.__name__

    try:
      engine =  self.dict[key](minder, state)
    except:
      msg = 'MinderStateEngineFactory: Cannot create a State engine for minder type %s:\nTraceback:\n%s' % (key, exc2string2())
      print msg
      raise RTTCodingError(msg)

    return engine
