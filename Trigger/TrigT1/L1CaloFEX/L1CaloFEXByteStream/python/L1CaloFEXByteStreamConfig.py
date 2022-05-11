#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentFactory import CompFactory
from libpyeformat_helper import SourceIdentifier, SubDetector


def jFexByteStreamToolCfg(name, flags, writeBS=False):
  tool = CompFactory.jFexByteStreamTool(name)
  jfex_roi_moduleids = [0x2000]
  tool.ROBIDs = [int(SourceIdentifier(SubDetector.TDAQ_CALO_FEAT_EXTRACT_ROI, moduleid)) for moduleid in jfex_roi_moduleids]
  if writeBS:
    # write BS == read xAOD
    tool.jJRoIContainerReadKey   ="L1_jFexSRJetRoI"
    tool.jLJRoIContainerReadKey  ="L1_jFexLRJetRoI"
    tool.jTauRoIContainerReadKey ="L1_jFexTauRoI"
    tool.jTERoIContainerReadKey  ="L1_jFexSumETRoI"
    tool.jXERoIContainerReadKey  ="L1_jFexMETRoI"
    
    
    tool.jJRoIContainerWriteKey  =""
    tool.jLJRoIContainerWriteKey =""
    tool.jTauRoIContainerWriteKey=""
    tool.jTERoIContainerWriteKey =""
    tool.jXERoIContainerWriteKey =""
  else:
    # read BS == write xAOD
    tool.jJRoIContainerReadKey   =""
    tool.jLJRoIContainerReadKey  =""
    tool.jTauRoIContainerReadKey =""
    tool.jTERoIContainerReadKey  =""
    tool.jXERoIContainerReadKey  =""
    
    
    tool.jJRoIContainerWriteKey  ="L1_jFexSRJetRoI"
    tool.jLJRoIContainerWriteKey ="L1_jFexLRJetRoI"
    tool.jTauRoIContainerWriteKey="L1_jFexTauRoI"
    tool.jTERoIContainerWriteKey ="L1_jFexSumETRoI"
    tool.jXERoIContainerWriteKey ="L1_jFexMETRoI"
    
  return tool
