/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONTRACKINGHYPOALG_PRECISION_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONTRACKINGHYPOALG_PRECISION_H 1

#include <string>
#include "DecisionHandling/HypoBase.h"
#include "ITrigEgammaPrecisionTrackingHypoTool.h"

/**
 * @class TrigEgammaPrecisionTrackingHypoAlg
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigEgammaPrecisionTrackingHypoAlg : public ::HypoBase {
  public: 

    TrigEgammaPrecisionTrackingHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;

  private: 
    ToolHandleArray< ITrigEgammaPrecisionTrackingHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
    SG::ReadHandleKey< xAOD::CaloClusterContainer > m_clustersKey { this, "CaloClusters", "CaloClusters", "CaloClusters in previous views" };       

}; 

#endif 
