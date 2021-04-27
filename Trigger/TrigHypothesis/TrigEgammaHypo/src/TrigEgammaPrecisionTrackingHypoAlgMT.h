/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONTRACKINGHYPOALGMT_PRECISION_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONTRACKINGHYPOALGMT_PRECISION_H 1

#include <string>
#include "DecisionHandling/HypoBase.h"
#include "ITrigEgammaPrecisionTrackingHypoTool.h"

/**
 * @class TrigEgammaPrecisionTrackingHypoAlgMT
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigEgammaPrecisionTrackingHypoAlgMT : public ::HypoBase {
  public: 

    TrigEgammaPrecisionTrackingHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;

  private: 
    ToolHandleArray< ITrigEgammaPrecisionTrackingHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
      

}; 

#endif 
