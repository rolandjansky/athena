# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from Factory_BaseClass    import Factory
from Commander            import LinuxInteractiveCommander
from Commander            import DummyCommander
from Commander            import LSFBatchCommander
from RTTSException        import RTTCodingError
from exc2string2          import exc2string2

import sys


class CommanderFactory(Factory):
  def __init__(self, mode, logger):
    Factory.__init__(self, logger, self.__class__.__name__)

    self.mode      = mode
    
    self.dict = {
      'LinuxInteractive': LinuxInteractiveCommander,
      'LSFBatch':         LSFBatchCommander,
      'QueuedDummy':      DummyCommander,
      'RunningDummy':     DummyCommander,
      'TransparentDummy': DummyCommander,
      'Dummy':            DummyCommander,
      }
    
    
  def create(self):
    'Returns a Commander instance according to mode'
    
    if self.mode in self.dict.keys():
      return self.dict[self.mode]()
    else:
      msg = 'CommanderFactory: Unknown mode: %s, could not instantiate a Commander\nTraceback:\n%s' % (self.mode,
                                                                                                       exc2string2())
      self.logger.error(msg)
      raise RTTCodingError(msg)
      

