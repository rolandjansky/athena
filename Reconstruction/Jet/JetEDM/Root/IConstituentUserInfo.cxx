/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IConstituentUserInfo.cxx

#include "JetEDM/IConstituentUserInfo.h"

using jet::IConstituentUserInfo;
using jet::LabelIndex;

//**********************************************************************

IConstituentUserInfo::Index IConstituentUserInfo::index() const {
  return 0;
}

//**********************************************************************

const LabelIndex* IConstituentUserInfo::labelMap() const {
  return 0;
}

//**********************************************************************

bool IConstituentUserInfo::isGhost() const { 
  return false;
}

//**********************************************************************

xAOD::JetConstitScale IConstituentUserInfo::constitScale() const {
  return xAOD::CalibratedJetConstituent;
}

//**********************************************************************
