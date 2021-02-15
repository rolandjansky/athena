# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = "Class / methods to test EGammaAmbiguityTool. Scripts in test/ directory"
__author__ = "Bruno Lenzi"

import os

def printMethod(x): 
  print (x)

def printAmbiguityInfo(egamma, tool, printMethod = printMethod):
  "printAmbiguityInfo(egamma, tool) -> print author and ambiguity result"
  import ROOT
  eg2 = egamma.ambiguousObject()
  electron, photon = (egamma, eg2) if egamma.type() == ROOT.xAOD.Type.Electron \
    else (eg2, egamma)
  
  cluster = egamma.caloCluster()
  vertex  = photon.vertex() if photon else None
  track   = electron.trackParticle() if electron else None
  
  printMethod('Author: %3d ; overlapping: %5s ; new author: %3d ; accept: %s'  %\
    (egamma.author(), 
     bool(eg2),
     tool.ambiguityResolve(cluster, vertex, track),
     tool.accept( egamma ) )
  )

if 'AtlasVersion' in os.environ:
  from AthenaPython import PyAthena
  from AthenaPython.PyAthena import StatusCode
  
  class TestEGammaAmbiguityAlg( PyAthena.Alg ):
    def __init__(self, name = 'TestEGammaAmbiguityAlg'):
      super(TestEGammaAmbiguityAlg,self).__init__(name = name)
    
    def initialize(self):
      self.msg.info("initializing [%s]", self.name())
      self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
      if self.storeGateSvc is None:
        self.msg.error("Problem retrieving storegate !!")
        return PyAthena.StatusCode.Failure
      self.tool = PyAthena.py_tool('EGammaAmbiguityTool', iface='IEGammaAmbiguityTool')
      if not self.tool:
        self.msg.error("Problem retrieving EgammaAmbiguityTool !!")
        return PyAthena.StatusCode.Failure

      return StatusCode.Success
  
    def execute(self):
      for container in 'Photons', 'Electrons':
        self.msg.info('\n*** %s ****', container)
        for obj in self.storeGateSvc[container]:
          printAmbiguityInfo(obj, self.tool, self.msg.info)
    
      return StatusCode.Success
  
    def finalize(self):
      return StatusCode.Success
