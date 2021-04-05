/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOALGMT_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOALGMT_H 1

#include <string>

#include "DecisionHandling/HypoBase.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

#include "TrigMultiVarHypo/tools/RingerSelectorTool.h"
#include "RingerSelectorTools/IAsgRingerSelectorTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"

#include "ITrigEgammaFastCaloHypoTool.h"

/**
 * @class TrigEgammaFastCaloHypoAlgMT
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigEgammaFastCaloHypoAlgMT : public ::HypoBase {
 public: 

  TrigEgammaFastCaloHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private:

  Ringer::RingerSelectorTool                  m_ringerTool_vLoose;
  Ringer::RingerSelectorTool                  m_ringerTool_Loose;
  Ringer::RingerSelectorTool                  m_ringerTool_Medium;
  Ringer::RingerSelectorTool                  m_ringerTool_Tight;

  ToolHandle<ILumiBlockMuTool>                m_lumiBlockMuTool;
  ToolHandleArray< ITrigEgammaFastCaloHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
  
  Gaudi::Property<std::string>      m_constantsCalibPath_vLoose{this, "ConstantsCalibPath_vLoose", "RingerSelectorTools/TrigL2_20180903_v9/TrigL2CaloRingerElectronVeryLooseConstants.root", 
                                                                                                                                        "Ringer Run2 Constants Calib Path for vLoose tune"};
  Gaudi::Property<std::string>      m_constantsCalibPath_Loose{this, "ConstantsCalibPath_Loose", "RingerSelectorTools/TrigL2_20180903_v9/TrigL2CaloRingerElectronLooseConstants.root", 
                                                                                                                                         "Ringer Run2 Constants Calib Path for Loose tune"};
  Gaudi::Property<std::string>      m_constantsCalibPath_Medium{this, "ConstantsCalibPath_Medium", "RingerSelectorTools/TrigL2_20180903_v9/TrigL2CaloRingerElectronMediumConstants.root", 
                                                                                                                                        "Ringer Run2 Constants Calib Path for Medium tune"};
  Gaudi::Property<std::string>      m_constantsCalibPath_Tight{this, "ConstantsCalibPath_Tight", "RingerSelectorTools/TrigL2_20180903_v9/TrigL2CaloRingerElectronTightConstants.root", 
                                                                                                                                         "Ringer Run2 Constants Calib Path for Tight tune"};  
  
  Gaudi::Property<std::string>      m_thresholdsCalibPath_vLoose{this, "ThresholdsCalibPath_vLoose", "RingerSelectorTools/TrigL2_20180903_v9/TrigL2CaloRingerElectronVeryLooseThresholds.root", 
                                                                                                                                          "Ringer Run2 Thresholds Calib Path for vLoose tune"};
  Gaudi::Property<std::string>      m_thresholdsCalibPath_Loose{this, "ThresholdsCalibPath_Loose", "RingerSelectorTools/TrigL2_20180903_v9/TrigL2CaloRingerElectronLooseThresholds.root", 
                                                                                                                                           "Ringer Run2 Thresholds Calib Path for Loose tune"};
  Gaudi::Property<std::string>      m_thresholdsCalibPath_Medium{this, "ThresholdsCalibPath_Medium", "RingerSelectorTools/TrigL2_20180903_v9/TrigL2CaloRingerElectronMediumThresholds.root", 
                                                                                                                                           "Ringer Run2 Thresholds Calib Path for Medium tune"};
  Gaudi::Property<std::string>      m_thresholdsCalibPath_Tight{this, "ThresholdsCalibPath_Tight", "RingerSelectorTools/TrigL2_20180903_v9/TrigL2CaloRingerElectronTightThresholds.root", 
                                                                                                                                           "Ringer Run2 Thresholds Calib Path for Tight tune"};
      
  SG::ReadHandleKey< xAOD::TrigEMClusterContainer > m_clustersKey { this, "CaloClusters", "CaloClusters", "CaloClusters in view" };
  SG::ReadHandleKey<xAOD::TrigRingerRingsContainer> m_ringsKey { this, "RingerKey","HLT_FastCaloRinger","Point to RingerKey"};
}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGEGAMMAFASTCALOHYPOALG_H
