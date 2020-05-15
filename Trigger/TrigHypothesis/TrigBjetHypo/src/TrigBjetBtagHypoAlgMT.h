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

  virtual StatusCode attachLinksToDecision( const EventContext&,
                                            TrigCompositeUtils::Decision&,
					    int index,
					    int indexPrmVertex = 0 ) const;
  virtual StatusCode monitor_tracks( const ElementLinkVector< xAOD::TrackParticleContainer >& trackELs ) const;

  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
  
 private:
  ToolHandleArray< TrigBjetBtagHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};
  
  //  Gaudi::Property< std::string > m_trackLink {this,"TrackLink","Undefined","Precision Track's link to attach to the output decision"};

  SG::ReadHandleKey< xAOD::BTaggingContainer> m_bTagKey {this,"BTagging","Undefined","Key for BTagging"};
  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_trackKey {this,"Tracks","Undefined","Key for precision tracks, to be linked to output decision"};
  Gaudi::Property< std::string > m_prmVtxLink {this,"PrmVtxLink","Undefined","PrmVtx link to attach to the output decision"};
  SG::ReadHandleKey< xAOD::VertexContainer > m_inputPrmVtx {this,"PrmVtx","Undefined","Primary vertex to be linked to the output decision"};
}; 

#endif

