/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGDEDXTRACKTRIGGERHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGDEDXTRACKTRIGGERHYPOALG_H

#include "Gaudi/Property.h"

#include <string>
#include "TrigdEdxTrackTriggerHypoTool.h"
#include "DecisionHandling/HypoBase.h"

/**
 * @class TrigdEdxTrackTriggerHypoAlg
 * @brief Implements Hypo selection on triggering large dEdx tracks
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
 **/

class TrigdEdxTrackTriggerHypoAlg : public ::HypoBase 
{ 
public: 

   TrigdEdxTrackTriggerHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
   
   virtual StatusCode  initialize() override;
   virtual StatusCode  execute(const EventContext& context) const override;
   
private: 
   ToolHandleArray< TrigdEdxTrackTriggerHypoTool >   m_hypoTools     {this, "HypoTools", {}, "Tools to perfrom selection"};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer > m_dEdxTrkKey    {this, "dEdxTrk",    "HLT_dEdxTrk", ""};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer>  m_dEdxHitKey    {this, "dEdxHit",    "HLT_dEdxHit", ""};
   SG::WriteHandleKey<xAOD::TrigCompositeContainer>  m_HPtdEdxTrkKey {this, "HPtdEdxTrk", "HLT_HPtdEdxTrk", ""};
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGDEDXTRACKTRIGGERHYPOALG_H
