# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This module bundles the RTT factory functions.
These functions are written as classes to help write test code.
"""

from Factory_BaseClass      import Factory
from AthenaJobDescriptor    import AthenaJobDescriptor
from AthenaJobDescriptor    import AthenaPilotJobDescriptor
from AthenaJobDescriptor    import AthenaATNJobDescriptor
from Descriptor             import Descriptor
from PseudoDescriptor       import PseudoDescriptor
from RTTSException          import RTTCodingError
from WorkerJobDescriptor    import WorkerJobDescriptor
from WatcherJobDescriptor   import WatcherJobDescriptor
from JobTransformDescriptor import JobTransformDescriptor

from SequentialDescriptor   import SequentialDescriptor
from ParallelDescriptor     import ParallelDescriptor
from ChainJobDescriptor     import ChainJobDescriptor
from exc2string2            import exc2string2

from copy                 import deepcopy

class DescriptorFactory(Factory):
  def __init__(self, argBag, logger):
    Factory.__init__(self,  logger, self.__class__.__name__)
    self.argBag = argBag
    self.dict = {
      'Athena':        AthenaJobDescriptor,
      'AthenaPilot':   AthenaPilotJobDescriptor,
      'AthenaATN':     AthenaATNJobDescriptor,
      'JobTransform':  JobTransformDescriptor, 
      'Sequential':    SequentialDescriptor,
      'Parallel':      ParallelDescriptor,
      'Chain':         ChainJobDescriptor,
      'Pseudo':        PseudoDescriptor
     }

    self.jobSerialNumber = -1
  
  def create(self, selector, argDict={}):
    paths = self.argBag
    if selector in self.dict:
      # limit the information flow,
      # and allow the descriptors to update their own paths object
      # descPaths   = PathsForDescriptors(paths)
      # clonedPaths = deepcopy(descPaths)
      # desc        = self.dict[selector](clonedPaths, self.logger)
      self.jobSerialNumber += 1
      sn = self.jobSerialNumber
      try:
        desc        = self.dict[selector](paths, self.logger, sn, argDict)
      except Exception, e:
        msg = 'Error while instantiating  Descriptor type %s\nException: %s\nTraceback:\n%s' % (str(selector), str(e), exc2string2())
        self.logger.error(msg)
        raise RTTCodingError(msg)
      else:
        return desc
    else:
      msg = 'Unknown Descriptor type %s' % str(selector)
      self.logger.error(msg)
      raise RTTCodingError(msg)

