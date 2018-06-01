/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGBJETHYPO_TRIGBJETHYPOALG_H
#define TRIGBJETHYPO_TRIGBJETHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"


#include "TrigBjetHypoTool.h"

/**
 * @class Implements b-jet selection for the new HLT framework
 * @brief 
 **/

class TrigBjetHypoAlg
  : public ::AthReentrantAlgorithm
{ 
 public: 

  TrigBjetHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigBjetHypoAlg(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigBjetHypoAlg();
  ToolHandleArray< TrigBjetHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};

 private:
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roisKey {this,"RoIsKey","RoIs","Key for RoIs"};
  SG::ReadHandleKey< xAOD::BTaggingContainer> m_bTagKey {this,"BTaggingKey","BTagging","Key for BTagging"};

  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_decisionsKey {this,"DecisionsKey","BjetHypoDecisions","Output key for Btag Kypo Decisions"};
}; 

#endif //> !TRIGBJETHYPO_TRIGBJETHYPOALG_H
