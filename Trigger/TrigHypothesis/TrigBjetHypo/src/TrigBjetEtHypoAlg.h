/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGBJETHYPO_TRIGBJETETHYPOALG_H
#define TRIGBJETHYPO_TRIGBJETETHYPOALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"


#include "TrigBjetEtHypoTool.h"
#include "DecisionHandling/HypoBase.h"

/**
 * @class Implements b-jet selection for the new HLT framework
 * @brief 
 **/

class TrigBjetEtHypoAlg
  : public ::HypoBase
{ 
 public: 

  TrigBjetEtHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigBjetEtHypoAlg(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigBjetEtHypoAlg();
  ToolHandleArray< TrigBjetEtHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};

 private:
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roisKey {this,"RoIsKey","RoIs","Key for RoIs"};
  SG::ReadHandleKey< xAOD::BTaggingContainer> m_bTagKey {this,"BTaggingKey","BTagging","Key for BTagging"};

  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_decisionsKey {this,"DecisionsKey","BjetHypoDecisions","Output key for Btag Kypo Decisions"};
}; 

#endif //> !TRIGBJETHYPO_TRIGBJETETHYPOALG_H
