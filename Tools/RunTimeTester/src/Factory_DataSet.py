# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


"""
This module bundles the RTT factory functions.
These functions are written as classes to help write test code.
"""

from exc2string           import exc2string
from Factory_BaseClass    import Factory
from Factory_IOUtils      import IOUtilsFactory
from RTTSException        import RTTInputError

import os
import time

class DataSetFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)
    
    
    self.dict = {
        'dc2'              : DC2DataSet,
        'bytestream'       : ByteStreamDataSet,
        'jobTransformData' : JobTransformDataSet
        }
    
    
  def create(self, datasetType, physicalName, logicalName, origin):
    "Factory for contructing DataSet objects"
    
    try:
        return self.dict[datasetType]((physicalName, logicalName), origin, self.logger)
    except KeyError:
        pass


# ----------------------------------------------------------------

class DataSet:
  def __init__(self, (physical, logical), origin, logger):
    self.physical = physical
    self.logical  = logical 
    self.type     = None
    self.mtime    = 'Unknown'
    self.size     = 'Unknown'
    self.origin   = origin
    self.exists   = False
    self.logger   = logger
    self.ioutils  = None
    
    #self.setIOutils()
    #if not self.ioutils: return # some datasets have physical path set to Unknown
    #self.setStats()
    
  def setStats(self):
    if not self.ioutils: self.setIOutils()
    
    if self.ioutils and self.ioutils.exists(self.physical):
        self.exists = True
        self.mtime  = self.ioutils.lastmodtime(self.physical)
        self.size   = str(self.ioutils.filesize(self.physical))+'B'

  def exists_(self):
    if not self.ioutils:
      self.logger.error('Factory_Dataset => self.ioutils does not exist, cannot check if dataset exists_()')
      return False
    return self.ioutils.exists(self.physical)
  
  def whichAthenaList(self):
    pass

  def __str__(self):
    return self.physical

  def castorProtocol(self):
    return 'castor:'

  def isCastorBased(self):
    return self.physical.startswith('/castor/cern.ch/')

  def isAFSBased(self):
    return self.physical.startswith('/afs/cern.ch/atlas')

  def setIOutils(self):
    if self.isCastorBased(): self.ioutils = IOUtilsFactory(self.logger).create('castor')
    elif self.isAFSBased(): self.ioutils = IOUtilsFactory(self.logger).create('afs')
    else: self.ioutils = None
  
class JobTransformDataSet(DataSet):
  def __init__(self,(physical, logical), origin, logger):
    DataSet.__init__(self, (physical, logical), origin, logger)
    self.type = 'JobTransform'
      
class ByteStreamDataSet(DataSet):
  def __init__(self, (physical, logical), origin, logger):
    DataSet.__init__(self, (physical, logical), origin, logger)
    self.type = 'ByteStream'

  def whichAthenaList(self):
    return 'ByteStreamInputSvc.FullFileName'

  def castorProtocol(self):
    return 'rfio:'
    
class DC2DataSet(DataSet):
  def __init__(self, (physical, logical), origin, logger):
    DataSet.__init__(self, (physical, logical), origin, logger)
    self.type = 'DC2'
        
  def whichAthenaList(self):
    return 'ServiceMgr.EventSelector.InputCollections'

class DQ2DataSet:
  "dataset from dq2"
  def __init__(self, dsName, files, type, origin):
    self.name  = dsName
    self.files = files
    self.type  = type
    self.logicalFiles  = self.setLogicalFiles()
    self.origin = origin
    
  def __str__(self):
    self.logicalFiles.sort()
    return str(self.logicalFiles)
    
  def setLogicalFiles(self):
    lf = []
    for file in self.files:
      fn =os.path.join(self.name, file)
      lf.append(fn)
    return lf

  def whichAthenaList(self):

    lists ={
      'dc2':        'ServiceMgr.EventSelector.InputCollections',
      'bytestream': 'ByteStreamInputSvc.FullFileName'
      }

    if self.type in lists.keys():
      return lists[self.type]
    else:
      print 'Unkown dataset type!'
      
      
      
