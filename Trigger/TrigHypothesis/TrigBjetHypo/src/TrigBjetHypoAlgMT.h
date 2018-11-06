/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGBJETHYPO_TRIGBJETHYPOALGMT_H
#define TRIGBJETHYPO_TRIGBJETHYPOALGMT_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"


#include "TrigBjetHypoTool.h"
#include "DecisionHandling/HypoBase.h"

/**
 * @class Implements b-jet selection for the new HLT framework
 * @brief 
 **/

class TrigBjetHypoAlgMT : public ::HypoBase { 
 public: 

  TrigBjetHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigBjetHypoAlgMT(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigBjetHypoAlgMT();
  ToolHandleArray< TrigBjetHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};

 private:
  SG::ReadHandleKey< xAOD::JetContainer > m_jetKey {this,"Jets","SplitJet","Input Jet Collection"};

  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roisKey {this,"RoIsKey","RoIs","Key for RoIs"};
  SG::ReadHandleKey< xAOD::BTaggingContainer> m_bTagKey {this,"BTaggingKey","BTagging","Key for BTagging"};

  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_decisionsKey {this,"DecisionsKey","BjetHypoDecisions","Output key for Btag Kypo Decisions"};
}; 

#endif //> !TRIGBJETHYPO_TRIGBJETHYPOALGMT_H
