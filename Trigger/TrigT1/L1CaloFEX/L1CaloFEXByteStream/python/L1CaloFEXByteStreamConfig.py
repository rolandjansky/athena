#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentFactory import CompFactory
from libpyeformat_helper import SourceIdentifier, SubDetector


def eFexByteStreamToolCfg(name, flags, writeBS=False, xTOBs=False, multiSlice=False):
  tool = CompFactory.eFexByteStreamTool(name)
  tool.ConvertExtendedTOBs = xTOBs
  tool.MultiSlice = multiSlice
  efex_roi_moduleids = [0x1000,0x1100]
  tool.ROBIDs = [int(SourceIdentifier(SubDetector.TDAQ_CALO_FEAT_EXTRACT_ROI, moduleid)) for moduleid in efex_roi_moduleids]
  if writeBS:
    # write BS == read xAOD
    tool.eEMContainerReadKey   = "L1_eEMxRoI"  if xTOBs else "L1_eEMRoI"
    tool.eTAUContainerReadKey  = "L1_eTauxRoI" if xTOBs else "L1_eTauRoI"
    tool.eEMContainerWriteKey  = ""
    tool.eTAUContainerWriteKey = ""
  else:
    # read BS == write xAOD
    tool.eEMContainerReadKey   = ""
    tool.eTAUContainerReadKey  = ""
    tool.eEMContainerWriteKey  = "L1_eEMxRoI"  if xTOBs else "L1_eEMRoI"
    tool.eTAUContainerWriteKey = "L1_eTauxRoI" if xTOBs else "L1_eTauRoI"

  return tool


def jFexByteStreamToolCfg(name, flags, writeBS=False, xTOBs=False):
  tool = CompFactory.jFexByteStreamTool(name)
  tool.ConvertExtendedTOBs = xTOBs
  jfex_roi_moduleids = [0x2000]
  tool.ROBIDs = [int(SourceIdentifier(SubDetector.TDAQ_CALO_FEAT_EXTRACT_ROI, moduleid)) for moduleid in jfex_roi_moduleids]
  if writeBS:
    # write BS == read xAOD
    tool.jJRoIContainerReadKey   = "L1_jFexSRJetxRoI" if xTOBs else "L1_jFexSRJetRoI"
    tool.jLJRoIContainerReadKey  = "L1_jFexLRJetxRoI" if xTOBs else "L1_jFexLRJetRoI"
    tool.jTauRoIContainerReadKey = "L1_jFexTauxRoI"   if xTOBs else "L1_jFexTauRoI"
    tool.jEMRoIContainerReadKey  = "L1_jFexFwdElxRoI" if xTOBs else "L1_jFexFwdElRoI"
    tool.jTERoIContainerReadKey  = "L1_jFexSumETxRoI" if xTOBs else "L1_jFexSumETRoI"
    tool.jXERoIContainerReadKey  = "L1_jFexMETxRoI"   if xTOBs else "L1_jFexMETRoI"

    tool.jJRoIContainerWriteKey  =""
    tool.jLJRoIContainerWriteKey =""
    tool.jTauRoIContainerWriteKey=""
    tool.jEMRoIContainerWriteKey =""
    tool.jTERoIContainerWriteKey =""
    tool.jXERoIContainerWriteKey =""
  else:
    # read BS == write xAOD
    tool.jJRoIContainerReadKey   =""
    tool.jLJRoIContainerReadKey  =""
    tool.jTauRoIContainerReadKey =""
    tool.jEMRoIContainerReadKey  =""
    tool.jTERoIContainerReadKey  =""
    tool.jXERoIContainerReadKey  =""

    tool.jJRoIContainerWriteKey  = "L1_jFexSRJetxRoI" if xTOBs else "L1_jFexSRJetRoI"
    tool.jLJRoIContainerWriteKey = "L1_jFexLRJetxRoI" if xTOBs else "L1_jFexLRJetRoI"
    tool.jTauRoIContainerWriteKey= "L1_jFexTauxRoI"   if xTOBs else "L1_jFexTauRoI"
    tool.jEMRoIContainerWriteKey = "L1_jFexFwdElxRoI" if xTOBs else "L1_jFexFwdElRoI"
    tool.jTERoIContainerWriteKey = "L1_jFexSumETxRoI" if xTOBs else "L1_jFexSumETRoI"
    tool.jXERoIContainerWriteKey = "L1_jFexMETxRoI"   if xTOBs else "L1_jFexMETRoI"
    
  return tool
