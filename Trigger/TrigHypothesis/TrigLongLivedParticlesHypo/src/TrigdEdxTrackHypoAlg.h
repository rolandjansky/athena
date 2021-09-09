/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGDEDXTRACKHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGDEDXTRACKHYPOALG_H

#include <string>

#include "Gaudi/Property.h"
#include "DecisionHandling/HypoBase.h"
#include "TrigdEdxTrackHypoTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

/**
 * @class TrigdEdxTrackHypoAlg
 * @brief Implements Hypo selection on triggering large dEdx tracks
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
 **/

class TrigdEdxTrackHypoAlg : public ::HypoBase 
{ 
public: 

   TrigdEdxTrackHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
   
   virtual StatusCode  initialize() override;
   virtual StatusCode  execute(const EventContext& context) const override;
   
private: 

   ToolHandleArray< TrigdEdxTrackHypoTool >  m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};

   SG::ReadHandleKey< xAOD::TrigCompositeContainer > m_dEdxTrkKey    {this, "dEdxTrk",    "HLT_dEdxTrk", ""};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer>  m_dEdxHitKey    {this, "dEdxHit",    "HLT_dEdxHit", ""};
   SG::WriteHandleKey<xAOD::TrigCompositeContainer>  m_HPtdEdxTrkKey {this, "HPtdEdxTrk", "HLT_HPtdEdxTrk", ""};

   StatusCode createCandidates(const xAOD::TrigCompositeContainer*, const xAOD::TrigCompositeContainer*, xAOD::TrigCompositeContainer*, int&) const;

   ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
   StatusCode doMonitor(const xAOD::TrigCompositeContainer*) const;

   std::vector<float>       m_highdEdxHitDefThres;
   std::vector<std::string> m_highdEdxHitDefNames;
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGDEDXTRACKHYPOALG_H
