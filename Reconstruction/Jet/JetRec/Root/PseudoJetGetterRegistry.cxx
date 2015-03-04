/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetGetterRegistry.cxx

#include "JetRec/PseudoJetGetterRegistry.h"

PseudoJetGetterRegistry::Map PseudoJetGetterRegistry::m_map;

//**********************************************************************

int PseudoJetGetterRegistry::add(Label lab, const IPseudoJetGetter* ptool) {
  m_map[lab] = ptool;
  return 0;
}

//**********************************************************************

int PseudoJetGetterRegistry::add(const IPseudoJetGetter* ptool) {
  if ( ptool == nullptr ) return 1;
  std::string lab = ptool->label();
  if ( lab.size() == 0 ) return 2;
  m_map[lab] = ptool;
  return 0;
}

//**********************************************************************

bool PseudoJetGetterRegistry::has(Label lab) {
  return m_map.find(lab) != m_map.end();
}

//**********************************************************************

const IPseudoJetGetter* PseudoJetGetterRegistry::find(Label lab) {
  auto ient = m_map.find(lab);
  if ( ient == m_map.end() ) return nullptr;
  return ient->second;
}

//**********************************************************************
