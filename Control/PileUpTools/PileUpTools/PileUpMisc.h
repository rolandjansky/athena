/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_PILEUPMISC_H
#define PILEUPTOOLS_PILEUPMISC_H

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"

/// default value for the EventInfoContainer storing subevents for PileUp
const std::string       c_pileUpEventInfoObjName =  "EventInfo";

/// default value for the EventInfoContainer storing subevents for PileUp
const std::string       c_pileUpEventInfoContName =  "PileUpEventInfo";


void addSubEvent( xAOD::EventInfo* targetEv,
                  const xAOD::EventInfo::SubEvent& subev,
                  xAOD::EventInfoContainer* eiContainer,
                  const std::string& eiContKey );

void addSubEvent( xAOD::EventInfo* targetEv,
                  const xAOD::EventInfo* ev2add,
                  int16_t subev_time,
                  xAOD::EventInfo::PileUpType subev_type,
                  xAOD::EventInfoContainer* eiContainer,
                  const std::string& eiContKey );

#endif
