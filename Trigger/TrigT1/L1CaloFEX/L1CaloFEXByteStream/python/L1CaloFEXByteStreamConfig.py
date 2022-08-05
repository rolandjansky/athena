#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentFactory import CompFactory
from libpyeformat_helper import SourceIdentifier, SubDetector


def eFexByteStreamToolCfg(name, flags, TOBs=True, writeBS=False, xTOBs=False, multiSlice=False, decodeInputs=False):
  tool = CompFactory.eFexByteStreamTool(name)
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
    if TOBs:
      tool.eEMContainerWriteKey  = "L1_eEMRoI"
      tool.eTAUContainerWriteKey = "L1_eTauRoI"
    if xTOBs:
      tool.eEMxContainerWriteKey = "L1_eEMxRoI"
      tool.eTAUxContainerWriteKey = "L1_eTauxRoI"
    if multiSlice:
      tool.eEMSliceContainerWriteKey = "L1_EMxRoIOutOfTime"
      tool.eTAUSliceContainerWriteKey = "L1_TauxRoIOutOfTime"

  if decodeInputs:
    efex_raw_ids = []
    inputId = int(SourceIdentifier(SubDetector.TDAQ_CALO_FEAT_EXTRACT_DAQ, 0x1000))
    for shelf in range(0,2):
      for module in range(0,12):
        efex_raw_ids += [inputId + shelf*0x100 + module*0x010 ]
    tool.ROBIDs += efex_raw_ids
    tool.eTowerContainerWriteKey   = "L1_eTowers"

  return tool


def jFexRoiByteStreamToolCfg(name, flags, writeBS=False, xTOBs=False):
  tool = CompFactory.jFexRoiByteStreamTool(name)
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

 
def gFexByteStreamToolCfg(name, flags, writeBS=False):
  tool = CompFactory.gFexByteStreamTool(name)
  gfex_roi_moduleids = [0x3000]
  tool.ROBIDs = [int(SourceIdentifier(SubDetector.TDAQ_CALO_FEAT_EXTRACT_ROI, moduleid)) for moduleid in gfex_roi_moduleids]
  if writeBS:
    # write BS == read xAOD
    tool.gFexRhoOutputContainerReadKey                  ="L1_gFexRhoRoI"
    tool.gFexSRJetOutputContainerReadKey                ="L1_gFexSRJetRoI"
    tool.gFexLRJetOutputContainerReadKey                ="L1_gFexLRJetRoI"
    tool.gScalarEJwojOutputContainerReadKey             ="L1_gScalarEJwoj"
    tool.gMETComponentsJwojOutputContainerReadKey       ="L1_gMETComponentsJwoj"
    tool.gMHTComponentsJwojOutputContainerReadKey       ="L1_gMHTComponentsJwoj"
    tool.gMSTComponentsJwojOutputContainerReadKey       ="L1_gMSTComponentsJwoj"
    tool.gMETComponentsNoiseCutOutputContainerReadKey   ="L1_gMETComponentsNoiseCut"
    tool.gMETComponentsRmsOutputContainerReadKey        ="L1_gMETComponentsRms"
    tool.gScalarENoiseCutOutputContainerReadKey         ="L1_gScalarENoiseCut"
    tool.gScalarERmsOutputContainerReadKey              ="L1_gScalarERms"
    
    
    tool.gFexRhoOutputContainerWriteKey                 =""
    tool.gFexSRJetOutputContainerWriteKey               =""
    tool.gFexLRJetOutputContainerWriteKey               =""
    tool.gScalarEJwojOutputContainerWriteKey            =""
    tool.gMETComponentsJwojOutputContainerWriteKey      =""
    tool.gMHTComponentsJwojOutputContainerWriteKey      =""
    tool.gMSTComponentsJwojOutputContainerWriteKey      =""
    tool.gMETComponentsNoiseCutOutputContainerWriteKey  =""
    tool.gMETComponentsRmsOutputContainerWriteKey       =""
    tool.gScalarENoiseCutOutputContainerWriteKey        =""
    tool.gScalarERmsOutputContainerWriteKey             =""
  else:
    # read BS == write xAOD
    tool.gFexRhoOutputContainerReadKey                  =""
    tool.gFexSRJetOutputContainerReadKey                =""
    tool.gFexLRJetOutputContainerReadKey                =""
    tool.gScalarEJwojOutputContainerReadKey             =""
    tool.gMETComponentsJwojOutputContainerReadKey       =""
    tool.gMHTComponentsJwojOutputContainerReadKey       =""
    tool.gMSTComponentsJwojOutputContainerReadKey       =""
    tool.gMETComponentsNoiseCutOutputContainerReadKey   =""
    tool.gMETComponentsRmsOutputContainerReadKey        =""
    tool.gScalarENoiseCutOutputContainerReadKey         =""
    tool.gScalarERmsOutputContainerReadKey              =""
    
    
    tool.gFexRhoOutputContainerWriteKey                 ="L1_gFexRhoRoI"
    tool.gFexSRJetOutputContainerWriteKey               ="L1_gFexSRJetRoI"
    tool.gFexLRJetOutputContainerWriteKey               ="L1_gFexLRJetRoI"
    tool.gScalarEJwojOutputContainerWriteKey            ="L1_gScalarEJwoj"
    tool.gMETComponentsJwojOutputContainerWriteKey      ="L1_gMETComponentsJwoj"
    tool.gMHTComponentsJwojOutputContainerWriteKey      ="L1_gMHTComponentsJwoj"
    tool.gMSTComponentsJwojOutputContainerWriteKey      ="L1_gMSTComponentsJwoj"
    tool.gMETComponentsNoiseCutOutputContainerWriteKey  ="L1_gMETComponentsNoiseCut"
    tool.gMETComponentsRmsOutputContainerWriteKey       ="L1_gMETComponentsRms"
    tool.gScalarENoiseCutOutputContainerWriteKey        ="L1_gScalarENoiseCut"
    tool.gScalarERmsOutputContainerWriteKey             ="L1_gScalarERms"

  return tool


def jFexInputByteStreamToolCfg(name, flags, writeBS=False):
  tool = CompFactory.jFexInputByteStreamTool(name)
  jfex_roi_moduleids = [0x2000,0x2010,0x2020,0x2030,0x2040,0x2050]
  tool.ROBIDs = [int(SourceIdentifier(SubDetector.TDAQ_CALO_FEAT_EXTRACT_DAQ, moduleid)) for moduleid in jfex_roi_moduleids]  
  
  #will be needed in the future for jTower container, still not coded
  if writeBS:
    # write BS == read xAOD
    tool.jTowersReadKey   = "L1_jTowers" 

    tool.jTowersWriteKey  =""
  else:
    # read BS == write xAOD
    tool.jTowersReadKey   =""

    tool.jTowersWriteKey  = "L1_jTowers"
    
  return tool
