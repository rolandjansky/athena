# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This module bundles the RTT factory functions.
These functions are written as classes to help write test code.
"""


# some imports have been placed at end of
# module to prevent circular import problems
from Factory_BaseClass         import Factory
from WatcherJobMinder          import WatcherJobMinder
from ErrorJobMinder            import ErrorJobMinder
from ErrorDescriptor           import ErrorDescriptor
from PseudoJobMinder           import PseudoJobMinder
from LinuxInteractiveJobMinder import LinuxInteractiveJobMinder
from LSFBatchJobMinder         import LSFBatchJobMinder
from ProductionManagerVetoer   import ProductionManagerVetoer
from exc2string2               import exc2string2
from getJobGroupKit            import getJobGroupKit
import sys

from RTTSException             import RTTCodingError

from RTTConstructionFactoryHelpers import ArgBag
from RTTConstructionFactoryHelpers import makeArgBag

# -------------------------------------------------------------------------
# Minder Factory
# -------------------------------------------------------------------------

class MinderFactory(Factory):
  def __init__(self, jobVetoer, scriptWriterFactory, jobGroupDirectoryMakerFactory, jobGroupKits, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
    self.dict = {
      'LinuxInteractive':      LinuxInteractiveJobMinder,
      'Watcher':               WatcherJobMinder,
      'Error':                 ErrorJobMinder,
      'LSFBatch':              LSFBatchJobMinder,
      'ChainJobDescriptor':    ChainJobMinder,
      'SequentialDescriptor':  SequentialMinder,
      'ParallelDescriptor':    ParallelMinder,
      'PseudoDescriptor':      PseudoJobMinder
      }

    self.jobVetoer = jobVetoer
    self.jgDirectoryMakerFactory = jobGroupDirectoryMakerFactory
    self.scriptWriterFactory     = scriptWriterFactory
    self.jobGroupKits            = jobGroupKits
    
  def makeKey(self, desc):
    if desc.jobGroup == "RTTMonitor": return desc.jobGroup
    if desc.constructionError:        return 'Error'
    
    keyByDesc = ['SequentialDescriptor', 'ParallelDescriptor', 'ChainJobDescriptor',
                 'PseudoDescriptor']

    descName = desc.__class__.__name__
    if descName in  keyByDesc: return descName

    if self.jobVetoer.toBeVetoed(desc): 
      desc.errorMessages.append('Production Manager Veto')
      return 'Error'

  
  def createMinder(self, argBag, jDescriptor):
    self.logger.debug("Coming into createMinder: desc of type %s" % jDescriptor.__class__.__name__)
    key = self.makeKey(jDescriptor)
    self.logger.debug("Made key, value is: %s" % key)
    if not key: key = argBag.mode
    self.logger.debug("Now have key, value is: %s" % key)
    if key not in self.dict.keys():
        m = 'MinderFactory: Unknown key for minder type. key: %s ' % key
        self.logger.error(m)
        raise RTTCodingError(m)

    minderClass = self.dict[key]
    msg  = 'Instantiating a  %s from a %s with name %s for package %s' % (minderClass.__name__,
                                                                          jDescriptor.__class__.__name__,
                                                                          jDescriptor.identifiedName,
                                                                          argBag.package)
    self.logger.info(msg)
    labels = ['jobGroup', 'runPath', 'castorSvcClass', 'jobTransformCmd', 'jobOptions']
    descArgsForScripts = makeArgBag(labels, jDescriptor)
    try:
      descArgsForScripts.athenaCommand = jDescriptor.athenaCommand()
    except:
      pass

    # add in things  jDescriptor _might_ have these will typically be collected
    # by minders low in the Minder hierarchy
    labels = ['outChainFiles', 'missingDatasets']
    tempArgBag  = makeArgBag(labels, jDescriptor)
    argBag.__dict__.update(tempArgBag.__dict__)

    try:
      argBag.jobGroupKit = getJobGroupKit(jDescriptor.jobGroup, self.jobGroupKits)
    except Exception, e:
      m  = 'MinderFactory: Error extracting job group kit descriptor '
      m += 'type: %s, job group: %s, error message: %s\nTraceback:%s' % (jDescriptor.__class__.__name__,
                                                                         jDescriptor.jobGroup,
                                                                         str(e),
                                                                         exc2string2())      
      # Not sure if this is really a coding error: might be a configuration problem.
      # In any case, the exception is caught by the calling method
      raise RTTCodingError(m)

    argBag.scriptWriter           = self.scriptWriterFactory.create(descArgsForScripts, self.logger)
    argBag.errorMessages          = jDescriptor.errorMessages
    argBag.resPath                = jDescriptor.resPath
    argBag.runPath                = jDescriptor.runPath
    argBag.identifiedName         = jDescriptor.identifiedName
    argBag.log                    = jDescriptor.log
    argBag.elog                   = jDescriptor.elog

    minderClass                   = self.dict[key]
    argBag.jobGroupDirectoryMaker = self.jgDirectoryMakerFactory.create(jDescriptor.setupRunDir, minderClass.__name__)

    minder = minderClass(argBag, jDescriptor)
    return minder

  def createErrorMinder(self, argBag, jDescriptor):
    desc = ErrorDescriptor(jDescriptor)
    minder = ErrorJobMinder(argBag, desc)
    return minder

  def create(self, argBag, jDescriptor):
    # creating normal minders can fall over,
    # but creating ErrorMidners must be bullet-proof
    # therefore keep very simple!
    try:
      minder = self.createMinder(argBag, jDescriptor)
    except Exception, e:
      msg  = 'Error constructing minder from a Descriptor of type %s, '   % jDescriptor.__class__.__name__
      msg += 'replacing with ErrorMinder:\nException:\n%s\nTraceback\n%s' % (str(e), exc2string2())
      self.logger.error(msg)
      try:
        minder = self.createErrorMinder(argBag, jDescriptor)
      except Exception, e:
        msg  = 'Error constructing ErrorMinder\n Traceback:\n  %s' % exc2string2()
        self.logger.error(msg)
        raise RTTCodingError(msg)
      else:
        minder.errorMessages.append(msg)
        
    self.logger.debug('Returning a minder of type %s' % minder.__class__.__name__)
    return minder

from ContainerMinder           import SequentialMinder
from ContainerMinder           import ParallelMinder
from ContainerMinder           import ChainJobMinder
