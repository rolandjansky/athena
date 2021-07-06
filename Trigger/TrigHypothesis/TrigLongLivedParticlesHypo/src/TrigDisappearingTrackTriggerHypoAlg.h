/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKTRIGGERHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKTRIGGERHYPOALG_H

#include "Gaudi/Property.h"

#include <string>
#include "TrigDisappearingTrackTriggerHypoTool.h"
#include "DecisionHandling/HypoBase.h"

/**
 * @class TrigDisappearingTrackTriggerHypoAlg
 * @brief Implements Hypo selection on triggering large Disappearing tracks
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
 **/

class TrigDisappearingTrackTriggerHypoAlg : public ::HypoBase 
{ 
public: 

   TrigDisappearingTrackTriggerHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
   
   virtual StatusCode  initialize() override;
   virtual StatusCode  execute(const EventContext& context) const override;
   
private: 
   ToolHandleArray< TrigDisappearingTrackTriggerHypoTool > m_hypoTools  {this, "HypoTools", {}, "Tools to perfrom selection"};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer > m_DisTrkCandKey    {this, "DisTrkCand",  "HLT_DisTrkCand",   ""};

   SG::WriteHandleKey<xAOD::TrigCompositeContainer>  m_DisTrkBDTSelKey  {this, "DisTrkBDTSel","HLT_DisTrkBDTSel", ""};
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKTRIGGERHYPOALG_H
