# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "Algorithm to lock egamma containers"
__author__ = "Bruno Lenzi <Bruno.Lenzi@cern.ch>"

from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode
from egammaRec import egammaKeys


class egammaLocker( PyAthena.Alg ):
  "Algorithm to lock egamma containers"
  def __init__(self, name = 'egammaLocker'):
    super(egammaLocker,self).__init__(name = name)
  
  def initialize(self):
    self.msg.info("initializing [%s]", self.name())

    # Retrieve StoreGateSvc
    self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
    if not self.storeGateSvc:
      self.msg.error("Problem retrieving storegate !!")
      return StatusCode.Failure
        
    return StatusCode.Success

  #-------------------------------------------------------
  
  def filterKey(self, key):
    from RecExConfig.RecFlags import rec
    from egammaRec.egammaRecFlags import jobproperties
      
    if (not rec.doTruth() and 'Truth' in key) or \
       (not jobproperties.egammaRecFlags.doEgammaForwardSeeded() and 'Fwd' in key) or\
       (not jobproperties.egammaRecFlags.doEgammaCaloSeeded() and 'Fwd' not in key):
      return False
    return True
  
  def execute(self):    
    for obj, descr in egammaKeys.outputs.iteritems():
      cType, cKey = descr[:2]
      if not self.filterKey(obj) or not cKey in self.storeGateSvc.keys():
        continue
      self.msg.debug('Locking container %s of type %s' % (cKey, cType))
      if self.storeGateSvc.setConst( self.storeGateSvc[cKey] ).isFailure():
        self.msg.error('Problem locking container %s of type %s' % (cKey, cType))
        return StatusCode.Failure
    
    return StatusCode.Success

  #-------------------------------------------------------
  
  def finalize(self):
    self.msg.debug("initializing [%s]", self.name())
    return StatusCode.Success
