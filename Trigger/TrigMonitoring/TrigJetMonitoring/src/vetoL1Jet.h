/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigger/jFexSRJetRoI.h"
#include "xAODTrigger/jFexLRJetRoI.h"
#include "xAODTrigger/gFexJetRoI.h"

#ifndef TRIGJETMONITORING_L1VETOJET_H
#define TRIGJETMONITORING_L1VETOJET_H
bool vetoJet(const xAOD::jFexSRJetRoI*);
bool vetoJet(const xAOD::jFexLRJetRoI*);
bool vetoJet(const xAOD::gFexJetRoI*);

#endif
