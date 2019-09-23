/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBJETHYPO_TRIGBJETBTAGHYPOALGMT_H
#define TRIGBJETHYPO_TRIGBJETBTAGHYPOALGMT_H 1

#include "TrigBjetHypoAlgBaseMT.h"
#include "TrigBjetBtagHypoTool.h"

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"

class TrigBjetBtagHypoAlgMT : public TrigBjetHypoAlgBaseMT {
 public:
  TrigBjetBtagHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~TrigBjetBtagHypoAlgMT();
  
  virtual StatusCode  initialize();
  virtual StatusCode  execute( const EventContext& context ) const;
  virtual StatusCode  finalize();

 private: 
  TrigBjetBtagHypoAlgMT();

  StatusCode retrieveBtagging( const EventContext&,
			       ElementLinkVector< xAOD::BTaggingContainer >&,
			       const SG::ReadHandleKey< xAOD::BTaggingContainer >&,
			       const TrigCompositeUtils::DecisionContainer*) const;

  virtual StatusCode attachLinkToDecisions( const EventContext&,
                                            const TrigCompositeUtils::DecisionContainer*,
                                            std::vector< TrigCompositeUtils::Decision* >& ) const;

 private:
  ToolHandleArray< TrigBjetBtagHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};

  SG::ReadHandleKey< xAOD::BTaggingContainer> m_bTagKey {this,"BTaggingKey","BTagging","Key for BTagging"};
  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_decisionsKey {this,"DecisionsKey","BjetHypoDecisions","Output key for Btag Kypo Decisions"};
}; 

#endif //> !TRIGBJETHYPO_TRIGBJETHYPOALGMT_H
