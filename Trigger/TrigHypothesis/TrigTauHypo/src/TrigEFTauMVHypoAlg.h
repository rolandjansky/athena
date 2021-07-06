/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTAUHYPO_TrigEFTauMVHypoAlg_H
#define TRIGTAUHYPO_TrigEFTauMVHypoAlg_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "ITrigEFTauMVHypoTool.h"

class TrigEFTauMVHypoAlg : public ::HypoBase {
 public: 

  TrigEFTauMVHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  ToolHandleArray< ITrigEFTauMVHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::TauJetContainer > m_tauJetKey { this, "taujetcontainer", "taujetcontainer", "taujets in view" };
  

}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGL2TAUCALOHYPOALG_H
