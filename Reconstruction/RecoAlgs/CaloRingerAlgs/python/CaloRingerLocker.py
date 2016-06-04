# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "Algorithm to lock CaloRinger containers (based on egammaLocker by Bruno Lenzi)"
__author__ = "Werner Freund <wsfreund@cern.ch>"

from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode


class CaloRingerLocker( PyAthena.Alg ):
  "Algorithm to lock Ringer containers"
  def __init__(self, name = 'CaloRingerLocker', cacheKeys = True, 
               doElectron = True, 
               doPhoton = True,
               EvtStoreName = 'StoreGateSvc',
               MetaDataStoreName = 'MetaDataStore',
               CaloRingerDict = dict(),
               CaloRingerMetaDict = dict()):

    super(CaloRingerLocker, self).__init__(name = name, cacheKeys = cacheKeys,
                                           doElectron = doElectron,
                                           doPhoton = doPhoton,
                                           EvtStoreName = EvtStoreName,
                                           MetaDataStoreName = MetaDataStoreName,
                                           CaloRingerDict = CaloRingerDict,
                                           CaloRingerMetaDict = CaloRingerMetaDict) 
    self.keys = {}
 
  def initialize(self):
    self.msg.info("initializing [%s]", self.name())

    self.metaDataStore = PyAthena.py_svc(self.MetaDataStoreName, False)
    if not self.metaDataStore:
      self.msg.error("Problem retrieving MetaDataStore !!")
      return StatusCode.Failure

    self.msg.debug("MetaDict = %s", self.CaloRingerMetaDict)
    self.msg.debug("EvtStoreDict = %s", self.CaloRingerDict)
    self.msg.debug("doElectron = %r", self.doElectron)
    self.msg.debug("doPhoton = %r", self.doPhoton)
    self.msg.debug("MetaDataStoreName = %s", self.MetaDataStoreName)
    self.msg.debug("EvtStoreName = %s", self.EvtStoreName)
    self.msg.debug("CacheKeys = %s", self.cacheKeys)

    # We can lock the MetaData containers, they are allocated and filled during
    # xAODRingSetConfWriter initialize:
    self.lockMetaDict()

    # Retrieve StoreGateSvc
    self.storeGateSvc = PyAthena.py_svc(self.EvtStoreName, False)
    if not self.storeGateSvc:
      self.msg.error("Problem retrieving StoreGate !!")
      return StatusCode.Failure

    return StatusCode.Success

  #-------------------------------------------------------
  def filterKey(self, key):
    """Return true if the container associated to the given key should be locked.
    Skip central, forward or truth if not requested"""
      
    if (not self.doElectron and 'Electron' in key) or \
        (not self.doPhoton and 'Photon' in key):
      return False
    return True

  def getEventKeys(self):
    "Loop over event dict items and yield (key, type) for those that pass filterKey"
    for cType, cKeys in self.CaloRingerDict.items():
      for cKey in cKeys:
        if not self.filterKey(cKey) or not cKey in self.storeGateSvc.keys():
          continue
        yield cKey, cType

  def getMetadataKeys(self):
    "Loop over event dict items and yield (key, type) for those that pass filterKey"
    for cType, cKeys in self.CaloRingerMetaDict.items():
      for cKey in cKeys:
        if not self.filterKey(cKey) or not cKey in self.metaDataStore.keys():
          continue
        yield cKey, cType

  def execute(self):
    "Loop over the EvtStore Ringer keys and lock each one"
    sc = self.lockEventDict()
    return sc

  def lockEventDict(self):
    "Loop over the event containers and lock each one"
    # Retrieve the keys of the containers to be locked
    # Only do it the first time if cacheKeys is true
    self.msg.debug('Entering execute.')

    sc = StatusCode.Success

    if not self.cacheKeys or not self.keys:
      self.keys = dict( self.getEventKeys() )
      self.msg.debug( 'Keys are: %r', self.keys)

    for cKey, cType in self.keys.items():
      self.msg.debug('Locking container %s of type %s' % (cKey, cType))
      if self.storeGateSvc.setConst( self.storeGateSvc[cKey] ).isFailure():
        self.msg.error('Problem locking container %s of type %s' % (cKey, cType))
        # Flag that a failure happened, but keep looping to try to lock other
        # containers
        sc = StatusCode.Failure
    
    return sc

  def lockMetaDict(self):
    "Loop over the Metadata containers and lock each one"
    # Retrieve the keys of the containers to be locked
    # Only do it the first time if cacheKeys is true
    self.msg.info('Locking Ringer Configuration Metadata.')

    sc = StatusCode.Success

    metaKeys = dict( self.getMetadataKeys() )

    for cKey, cType in metaKeys.items():
      self.msg.debug('Locking Metadata container %s of type %s' % (cKey, cType))
      if self.metaDataStore.setConst( self.metaDataStore[cKey] ).isFailure():
        self.msg.error('Problem locking Metadata container %s of type %s' % (cKey, cType))
        # Flag that a failure happened, but keep looping to try to lock other
        # containers
        sc = StatusCode.Failure
    
    return sc
  #-------------------------------------------------------

  def finalize(self):
    self.msg.debug("finalizing [%s]", self.name())
    return StatusCode.Success

