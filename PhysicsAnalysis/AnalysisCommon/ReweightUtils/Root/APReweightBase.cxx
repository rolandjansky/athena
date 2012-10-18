/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APReweightBase_cxx
#include "ReweightUtils/APReweightBase.h"

unsigned int APReweightBase::_NID = 0;

APReweightBase::APReweightBase() {
  _ID = _NID;
  ++_NID;
}

APReweightBase::~APReweightBase() { }

unsigned int APReweightBase::GetID() const {
  return _ID;
}
