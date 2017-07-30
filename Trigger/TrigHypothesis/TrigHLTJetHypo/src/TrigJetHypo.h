/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHLTJETHYPO_TRIGJETHYPO_H
#define TRIGHLTJETHYPO_TRIGJETHYPO_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigHLTJetHypo/ITrigHLTJetHypoTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
/**
 * @class Implements jet selection for the new HLT framework
 * @brief 
 **/

using namespace TrigCompositeUtils;


class TrigJetHypo
  : public ::AthReentrantAlgorithm
{ 
 public: 

  TrigJetHypo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigJetHypo(); 

  StatusCode  initialize() override;
  StatusCode  execute_r( const EventContext& context ) const override;
  StatusCode  finalize() override;


 private: 

  ToolHandleArray<ITrigHLTJetHypoTool> m_hypoTools;
  SG::ReadHandleKey<xAOD::JetContainer> m_jetsKey;
  SG::WriteHandleKey<DecisionContainer> m_decisionsKey;
 
}; 


#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOALG_H
