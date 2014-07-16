/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SimpleConstituentUserInfo.cxx

#include "JetEDM/SimpleConstituentUserInfo.h"

using jet::SimpleConstituentUserInfo;
typedef SimpleConstituentUserInfo::Label Label;
using xAOD::IParticle;

//**********************************************************************

SimpleConstituentUserInfo::SimpleConstituentUserInfo()
: m_ppar(0) { }

//**********************************************************************

SimpleConstituentUserInfo::
SimpleConstituentUserInfo(const IParticle& par, Label lab)
: m_ppar(&par), m_label(lab) { }

//**********************************************************************

const xAOD::IParticle* SimpleConstituentUserInfo::particle() const {
  return m_ppar;
}

//**********************************************************************

Label SimpleConstituentUserInfo::label() const {
  return m_label;
}

//**********************************************************************
