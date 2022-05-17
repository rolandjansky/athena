/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITOR_L1MONITORADAPTORS_H
#define TRIGJETMONITOR_L1MONITORADAPTORS_H

#include "xAODTrigger/jFexSRJetRoI.h"
#include "xAODTrigger/jFexLRJetRoI.h"
#include "xAODTrigger/gFexJetRoI.h"
#include "xAODTrigger/JetRoI.h"


inline bool vetoJet(const xAOD::jFexSRJetRoI* j){
  return j->tobWord() == 0;
}
inline float et(const xAOD::jFexSRJetRoI* j){
  return j->et();
}
inline std::string et_label(const xAOD::jFexSRJetRoI*){
  return "et";
}

inline bool vetoJet(const xAOD::jFexLRJetRoI*){
  return false;
}
inline float et(const xAOD::jFexLRJetRoI* j){
  return j->et();
}
inline std::string et_label(const xAOD::jFexLRJetRoI*){
  return "et";
}


inline bool vetoJet(const xAOD::gFexJetRoI*){
  return false;
}
inline float et(const xAOD::gFexJetRoI* j){
  return j->et();
}
inline std::string et_label(const xAOD::gFexJetRoI*){
  return "et";
}


inline bool vetoJet(const xAOD::JetRoI*){
  return false;
}
inline float et(const xAOD::JetRoI* j){
  return j->et8x8();
}
inline std::string et_label(const xAOD::JetRoI*){
  return "et8x8";
}


#endif
