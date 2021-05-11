/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTAUHYPO_TrigTauCaloHypoAlg_H
#define TRIGTAUHYPO_TrigTauCaloHypoAlg_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTau/TauJetContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"

#include "ITrigTauGenericHypoTool.h"

/**
 * @class TrigTauCaloHypoAlg
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigTauCaloHypoAlg : public ::HypoBase {
 public: 

  TrigTauCaloHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  ToolHandleArray< ITrigTauGenericHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::TauJetContainer > m_tauJetKey { this, "taujets", "taujets", "taujets in view" };
}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGL2TAUCALOHYPOALG_H
