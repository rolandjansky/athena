/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MULTIVARHYPO_TRIGL2CALORINGERHYPOALG_H
#define MULTIVARHYPO_TRIGL2CALORINGERHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"
#include "AthViews/View.h"

#include "TrigL2CaloRingerHypoToolMT.h"



class TrigL2CaloRingerHypoAlgMT
  : public ::HypoBase
{ 
 public: 

  TrigL2CaloRingerHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigL2CaloRingerHypoAlgMT(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r(const EventContext& context) const override;

 
 private: 
  TrigL2CaloRingerHypoAlgMT();
  ToolHandleArray< TrigL2CaloRingerHypoToolMT > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};


  SG::ReadHandleKey< ViewContainer > m_views {this, "Views", "Unspecified", "Views to read electrons from" };
  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_decisionsKey {this, "RNNOutDecisions", "RNNOutDecisions", "Output decisions"};
  //SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_RNNOutdecisionsKey {this, "RNNOutDecisions", "RNNOutDecisions", "Output decisions"};

  SG::ReadHandleKey<xAOD::TrigRNNOutputContainer> m_outputKey {
      this,
      "RNNOutputContainer",
      "CaloRNNOutput",
      ""
  };



}; 
//DECLARE_ALGORITHM_FACTORY( TrigL2CaloRingerHypoAlgMT )

#endif //> !MULTIVARHYPO_TRIGL2CALORINGERHYPOALG_H
