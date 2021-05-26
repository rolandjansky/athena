/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOALG_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOALG_H 1

#include <string>
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigMultiVarHypo/tools/RingerSelectorTool.h"
#include "RingerSelectorTools/IAsgRingerSelectorTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "ITrigEgammaFastCaloHypoTool.h"

/**
 * @class TrigEgammaFastCaloHypoAlg
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigEgammaFastCaloHypoAlg : public ::HypoBase {

  public: 

    TrigEgammaFastCaloHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;


  private:

    SG::ReadHandleKey< xAOD::TrigEMClusterContainer > m_clustersKey { this, "CaloClusters", "CaloClusters", "CaloClusters in view" };
    SG::ReadHandleKey<xAOD::TrigRingerRingsContainer> m_ringsKey { this, "RingerKey","HLT_FastCaloRinger","Point to RingerKey"};
    ToolHandleArray< ITrigEgammaFastCaloHypoTool >    m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
    ToolHandleArray<Ringer::IAsgRingerSelectorTool>   m_ringerNNTools{this, "RingerNNSelectorTools", {}, "Ringer Neural Network tools." };
    ToolHandle<ILumiBlockMuTool>                      m_lumiBlockMuTool;
    ToolHandle< GenericMonitoringTool >               m_monTool{ this, "MonTool", "", "Monitoring tool" };
    Gaudi::Property<std::vector<std::string>>         m_pidNames{this, "PidNames", {}, "Ringer pid names"};


    // NOTE: This will be uncommissioned.
    std::vector<Ringer::RingerSelectorTool*>          m_ringerTools;
    Gaudi::Property<std::vector<std::string>>         m_constantsCalibPaths{this, "ConstantsCalibPaths", {}, "Ringer Run2 Constants Calib Paths"};
    Gaudi::Property<std::vector<std::string>>         m_thresholdsCalibPaths{this, "ThresholdsCalibPaths", {}, "Ringer Run2 Thresholds Calib Paths"};
    Gaudi::Property<bool>                             m_useRun3{this, "UseRun3", false, "Use Run 3 NN selection tools."};
 
    
}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGEGAMMAFASTCALOHYPOALG_H
