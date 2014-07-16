/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IndexedConstituentUserInfo.cxx

#include "JetEDM/IndexedConstituentUserInfo.h"
#include "JetEDM/LabelIndex.h"

using jet::IndexedConstituentUserInfo;
typedef IndexedConstituentUserInfo::Label Label;
typedef IndexedConstituentUserInfo::Index Index;
using jet::LabelIndex;
using xAOD::IParticle;

//**********************************************************************

IndexedConstituentUserInfo::IndexedConstituentUserInfo()
: m_ppar(0) { }

//**********************************************************************

IndexedConstituentUserInfo::
IndexedConstituentUserInfo(const IParticle& par, Index idx, const LabelIndex* pli)
: BaseIndexedConstituentUserInfo(idx, pli), m_ppar(&par) { }


//**********************************************************************

const xAOD::IParticle* IndexedConstituentUserInfo::particle() const {
  return m_ppar;
}

//**********************************************************************
