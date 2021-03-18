/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOALGMT_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOALGMT_H 1

#include <string>

#include "DecisionHandling/HypoBase.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

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
  ToolHandleArray< ITrigEgammaFastCaloHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::TrigEMClusterContainer > m_clustersKey { this, "CaloClusters", "CaloClusters", "CaloClusters in view" };
  SG::ReadHandleKey<xAOD::TrigRingerRingsContainer> m_ringsKey { this, "RingerKey","HLT_FastCaloRinger","Point to RingerKey"};
}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGEGAMMAFASTCALOHYPOALG_H
