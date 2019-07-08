/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBJETHYPO_TRIGBJETBTAGHYPOALGMT_H
#define TRIGBJETHYPO_TRIGBJETBTAGHYPOALGMT_H 1

#include "TrigBjetHypoAlgBaseMT.h"
#include "TrigBjetHypoTool.h"

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

  ToolHandleArray< TrigBjetHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};

 private:
  SG::ReadHandleKey< xAOD::JetContainer > m_jetKey {this,"Jets","SplitJet","Input Jet Collection"};

  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roisKey {this,"RoIsKey","RoIs","Key for RoIs"};
  SG::ReadHandleKey< xAOD::BTaggingContainer> m_bTagKey {this,"BTaggingKey","BTagging","Key for BTagging"};

  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_decisionsKey {this,"DecisionsKey","BjetHypoDecisions","Output key for Btag Kypo Decisions"};
}; 

#endif //> !TRIGBJETHYPO_TRIGBJETHYPOALGMT_H
