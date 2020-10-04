# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Import message level flags.
from GaudiKernel.Constants import WARNING
#                                (VERBOSE,
#                                   DEBUG,
#                                   INFO,
#                                   WARNING,
#                                   ERROR,
#                                   FATAL,)

def getConfiguredTLASelector(jetPtThreshold=20000, maxNJets=-1, inputJetsKey="HLT_xAOD__JetContainer_a4tcemsubjesISFS", outputJetsKey="HLT_xAOD__JetContainer_a4tcemsubjesISFS_TLA", outputLevel=WARNING) :

  
  from AthenaConfiguration.ComponentFactory import CompFactory
  from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
  from TrigEDMConfig.TriggerEDMRun3 import recordable

  myTrigHLTJetTLASelector = conf2toConfigurable( CompFactory.TrigHLTJetTLASelector() )
  myTrigHLTJetTLASelector.jetPtThreshold = jetPtThreshold 
  myTrigHLTJetTLASelector.maxNJets = maxNJets 
  #check that the collection is suitable:
  outputJetsName = recordable(outputJetsKey)
  myTrigHLTJetTLASelector.TLAInputJetCollection = inputJetsKey 
  myTrigHLTJetTLASelector.TLAOutputJetCollection = outputJetsName
  myTrigHLTJetTLASelector.OutputLevel = outputLevel
 
  return myTrigHLTJetTLASelector
