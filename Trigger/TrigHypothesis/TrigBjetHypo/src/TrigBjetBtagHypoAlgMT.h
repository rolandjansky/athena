/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  
  virtual StatusCode  initialize();
  virtual StatusCode  execute( const EventContext& context ) const;

 private: 
  TrigBjetBtagHypoAlgMT();

  virtual StatusCode attachLinksToDecision( const EventContext&,
                                            TrigCompositeUtils::Decision&,
					    int index,
					    int indexPrmVertex = 0 ) const;
  
 private:
  ToolHandleArray< TrigBjetBtagHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};
  
  //  Gaudi::Property< std::string > m_trackLink {this,"TrackLink","Undefined","Precision Track's link to attach to the output decision"};

  SG::ReadHandleKey< xAOD::BTaggingContainer> m_bTagKey {this,"BTagging","Undefined","Key for BTagging"};
  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_trackKey {this,"Tracks","Undefined","Key for precision tracks, to be linked to output decision"};
}; 

#endif

