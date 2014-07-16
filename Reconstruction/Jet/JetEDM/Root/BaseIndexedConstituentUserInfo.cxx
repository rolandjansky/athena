/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BaseIndexedConstituentUserInfo.cxx

#include "JetEDM/BaseIndexedConstituentUserInfo.h"
#include "JetEDM/LabelIndex.h"

//**********************************************************************

jet::BaseIndexedConstituentUserInfo::BaseIndexedConstituentUserInfo()
: m_idx(0), m_pli(0) { }

//**********************************************************************

jet::BaseIndexedConstituentUserInfo::
BaseIndexedConstituentUserInfo(Index idx, const jet::LabelIndex* pli)
: m_idx(idx), m_pli(pli) { }

//**********************************************************************

const SG::AuxElement* jet::BaseIndexedConstituentUserInfo::element() const {
  return particle();
}

//**********************************************************************

jet::BaseIndexedConstituentUserInfo::Label
jet::BaseIndexedConstituentUserInfo::label() const {
  if ( m_pli == 0 ) return "";
  return m_pli->label(abs(m_idx));
}

//**********************************************************************

jet::BaseIndexedConstituentUserInfo::Index
jet::BaseIndexedConstituentUserInfo::index() const {
  return abs(m_idx);
}

//**********************************************************************

const jet::LabelIndex* jet::BaseIndexedConstituentUserInfo::labelMap() const {
  return m_pli;
}

//**********************************************************************

bool jet::BaseIndexedConstituentUserInfo::isGhost() const {
  // valid m_idx are guaranteed to be != 0 when constructed by LabelIndex
  return m_idx < 0;
}

//**********************************************************************

xAOD::JetConstitScale jet::BaseIndexedConstituentUserInfo::constitScale() const {
  if ( m_pli == nullptr ) return xAOD::CalibratedJetConstituent;
  return m_pli->constitScale( abs(m_idx) );
}

//**********************************************************************
