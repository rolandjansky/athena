/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGCALOHYPO_TRIGL2CALOLAYERSALG
#define TRIGCALOHYPO_TRIGL2CALOLAYERSALG

#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "DecisionHandling/HypoBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrigCaloHypo/ITrigL2CaloLayersHypoTool.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

/**
 * @class TrigL2CaloLayersAlg
 * @brief Implements LAr layer by layer cut based detection for the new HLT framework
 **/
class TrigL2CaloLayersAlg : public ::HypoBase {
 public: 

  TrigL2CaloLayersAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  SG::ReadHandleKey<xAOD::TrigEMClusterContainer > m_caloContainerKey { this, "TrigClusterContainerKey", "LayersCluster","SG Key of clusters with layers"};
  ToolHandleArray< ITrigL2CaloLayersHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
}; 
#endif
