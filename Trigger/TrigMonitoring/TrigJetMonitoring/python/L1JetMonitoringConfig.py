# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

class L1JetMonAlg():
  def __init__(self,name,jetcoll,triggerChain=''):
    self.name           = name
    self.L1JetContainer = jetcoll
    self.TriggerChain   = triggerChain

  def toAlg(self,monhelper):
    from AthenaConfiguration.ComponentFactory import CompFactory
    alg = monhelper.addAlgorithm(CompFactory.TrigL1JetMonitorAlgorithm, self.name)
    jetcontainer       = self.L1JetContainer
    triggerChain       = self.TriggerChain
    alg.L1JetContainer = jetcontainer
    alg.TriggerChain   = triggerChain
    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool
    myGroup = monhelper.addGroup(alg,'TrigL1JetMonitor','HLT/JetMon/L1/')
    # Configure histograms
    Path  = jetcontainer+'/'
    Path += 'NoTriggerSelection/' if triggerChain == '' else triggerChain+'/'
    myGroup.defineHistogram('et8x8',title='et8x8',path=Path,xbins=400,xmin=0.0,xmax=400.0)
    myGroup.defineHistogram('eta',title='eta',path=Path,xbins=50,xmin=-5,xmax=5)
    myGroup.defineHistogram('phi',title='phi',path=Path,xbins=50,xmin=-3.3,xmax=3.3)
    return alg
