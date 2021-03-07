/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONETCUTHYPOALGMT_PRECISION_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONETCUTHYPOALGMT_PRECISION_H 1

#include <string>

#include "DecisionHandling/HypoBase.h"

#include "ITrigEgammaPrecisionEtcutHypoTool.h"

/**
 * @class TrigEgammaPrecisionEtcutHypoAlgMT
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigEgammaPrecisionEtcutHypoAlgMT : public ::HypoBase {
 public: 

  TrigEgammaPrecisionEtcutHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  ToolHandleArray< ITrigEgammaPrecisionEtcutHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
      

}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGPRECISIONCUTHYPOALG_H
