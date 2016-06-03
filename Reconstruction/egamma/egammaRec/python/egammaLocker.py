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
    self.countmsg = 0
  
  def initialize(self):
    self.msg.warning("[%s] does nothing anymore, remove its usage: See https://its.cern.ch/jira/browse/ATLASRECTS-2769?filter=-1", self.name())        
    return StatusCode.Success

  #-------------------------------------------------------

  def execute(self):
    if self.countmsg <10 :
      self.msg.warning("[%s] does nothing anymore, remove its usage: See https://its.cern.ch/jira/browse/ATLASRECTS-2769?filter=-1", self.name())        
      self.countmsg+=1
    return StatusCode.Success
          
  #-------------------------------------------------------
  
  def finalize(self):
    self.msg.warning("[%s] does nothing anymore, remove its usage: See https://its.cern.ch/jira/browse/ATLASRECTS-2769?filter=-1", self.name())        
    return StatusCode.Success
