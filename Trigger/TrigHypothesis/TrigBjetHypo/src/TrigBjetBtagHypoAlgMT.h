
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBJETHYPO_TRIGBJETBTAGHYPOALGMT_H
#define TRIGBJETHYPO_TRIGBJETBTAGHYPOALGMT_H 1

#include "TrigBjetHypoAlgBaseMT.h"
#include "TrigBjetBtagHypoTool.h"

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

class TrigBjetBtagHypoAlgMT : public TrigBjetHypoAlgBaseMT {
 public:
  TrigBjetBtagHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual StatusCode  initialize();
  virtual StatusCode  execute( const EventContext& context ) const;

 private: 
  TrigBjetBtagHypoAlgMT();

  // online monitoring 
  virtual StatusCode monitor_jets( const ElementLinkVector<xAOD::JetContainer >& jetELs ) const ;
  virtual StatusCode monitor_tracks( const ElementLinkVector< xAOD::TrackParticleContainer >& trackELs ) const;
  virtual StatusCode monitor_flavor_tag_variable( const ElementLinkVector< xAOD::BTaggingContainer >& bTaggingEL, const std::string& var_name) const;
  virtual StatusCode monitor_btagging( const ElementLinkVector< xAOD::BTaggingContainer >& bTaggingEL ) const;
  
 private:
  ToolHandleArray< TrigBjetBtagHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
  
  SG::ReadHandleKey< xAOD::JetContainer > m_bTaggedJetKey {this,"BTaggedJetKey","","Key for b-tagged jets"};
  SG::ReadHandleKey< xAOD::BTaggingContainer> m_bTagKey {this,"BTaggingKey","","Key for BTagging"};
  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_trackKey {this,"TracksKey","","Key for precision tracks"};
  SG::ReadHandleKey< xAOD::VertexContainer > m_inputPrmVtx {this,"PrmVtxKey","","Key for Primary vertex collection for monitoring"};

  Gaudi::Property< std::string > m_bTaggingLink {this,"BTaggingLink","Unspecified","b-Tagging Link name in navigation (output)"};
  Gaudi::Property< std::string > m_prmVtxLink {this,"PrmVtxLink","Unspecified","Vertex Link name in navigation (input)"};

}; 

#endif

