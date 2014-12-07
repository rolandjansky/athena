# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "Algorithm to lock egamma containers"
__author__ = "Bruno Lenzi <Bruno.Lenzi@cern.ch>"

from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode


class egammaLocker( PyAthena.Alg ):
  "Algorithm to lock egamma containers"
  def __init__(self, name = 'egammaLocker', cacheKeys = True, 
               doFinalizeClusters = True, doTruth=False,
               doEgammaForwardSeeded=True, doEgammaCaloSeeded=True,
               outputClusterKey="",
               egammakeys=dict()):
    super(egammaLocker,self).__init__(name = name, cacheKeys = cacheKeys,
                                      doFinalizeClusters = doFinalizeClusters, 
                                      doTruth=doTruth,
                                      doEgammaForwardSeeded=doEgammaForwardSeeded, 
                                      doEgammaCaloSeeded=doEgammaCaloSeeded,
                                      outputClusterKey= outputClusterKey,
                                      egammakeys=egammakeys)             
    self.keys = {}
  
  def initialize(self):
    self.msg.info("initializing [%s]", self.name())

    # Retrieve StoreGateSvc
    self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
    if not self.storeGateSvc:
      self.msg.error("Problem retrieving storegate !!")
      return StatusCode.Failure

    # Retrieve msg stream to finalize clusters
    if self.doFinalizeClusters:
      msgSvc = PyAthena.py_svc('MessageSvc', iface='IMessageSvc')
      self.msgStream = PyAthena.MsgStream(msgSvc, self.name())
        
    return StatusCode.Success

  #-------------------------------------------------------
  
  def filterKey(self, key):
    """Return true if the container associated to the given key should be locked.
    Skip central, forward or truth if not requested"""
      
    if (not self.doTruth and 'Truth' in key) or \
       (not self.doEgammaForwardSeeded and 'Fwd' in key) or\
       (not self.doEgammaCaloSeeded and 'Fwd' not in key):
      return False
    return True

  def getKeys(self):
    "Loop over egammaKeys.outputs and yield (key, type) for those that pass filterKey"
    for obj, descr in self.egammakeys:
      cType, cKey = descr[:2]
      if not self.filterKey(obj) or not cKey in self.storeGateSvc.keys():
        continue
      yield cKey, cType

  def finalizeClusterContainer(self, key):
    "Call CaloClusterStoreHelper.finalizeClusters"
    self.msg.debug('Finalizing cluster container %s' % key)
    x = PyAthena.CaloClusterStoreHelper.finalizeClusters
    try:
      clusterCollection = self.storeGateSvc[key]
    except KeyError:
      self.msg.error('Cluster container %s not in StoreGate' % key)
      return False
    if x(self.storeGateSvc, clusterCollection, key, self.msgStream).isFailure():
      self.msg.error('Could not finalize cluster container %s' % key)
      return False
    return True
  
  def execute(self):
    "Loop over the egamma containers and lock each one"
    # Retrieve the keys of the containers to be locked
    # Only do it the first time if cacheKeys is true
    if not self.cacheKeys or not self.keys:
      self.keys = dict( self.getKeys() )
    
    # Call finalize clusters for cluster collection
    if self.doFinalizeClusters and not self.finalizeClusterContainer(self.outputClusterKey):
      return StatusCode.Failure
    
    for cKey, cType in self.keys.iteritems():
      self.msg.debug('Locking container %s of type %s' % (cKey, cType))
      if self.storeGateSvc.setConst( self.storeGateSvc[cKey] ).isFailure():
        self.msg.error('Problem locking container %s of type %s' % (cKey, cType))
        return StatusCode.Failure
    
    return StatusCode.Success
          
  #-------------------------------------------------------
  
  def finalize(self):
    self.msg.debug("initializing [%s]", self.name())
    return StatusCode.Success
