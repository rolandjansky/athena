/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGISOHPTTRACKTRIGGERHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGISOHPTTRACKTRIGGERHYPOALG_H 1

#include "Gaudi/Property.h"

#include <string>
#include "TrigIsoHPtTrackTriggerHypoTool.h"
#include "DecisionHandling/HypoBase.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"

/**
 * @class TrigIsoHPtTrackTriggerHypoAlg
 * @brief Implements Hypo selection on triggering high pt isolated tracks 
 * @author Ismet Siral <ismet.siral@cern.ch> - University of Oregon
 **/

class TrigIsoHPtTrackTriggerHypoAlg  :  public ::HypoBase 
{ 
 public: 

  TrigIsoHPtTrackTriggerHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute(const EventContext& context) const override;

 private: 
  ToolHandleArray< TrigIsoHPtTrackTriggerHypoTool > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};

  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_trackKey {this,"trackKey","Key for precision tracks"};
  
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGISOHPTTRACKTRIGGERHYPOALG_H
