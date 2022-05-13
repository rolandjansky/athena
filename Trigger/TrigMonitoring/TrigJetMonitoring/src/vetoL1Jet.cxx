
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./vetoL1Jet.h"

bool vetoJet(const xAOD::jFexSRJetRoI* j){
  return j->tobWord() == 0;
}

bool vetoJet(const xAOD::jFexLRJetRoI*) {return false;}
bool vetoJet(const xAOD::gFexJetRoI*) {return false;}
