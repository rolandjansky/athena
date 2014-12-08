/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define APReweightBase_cxx
#include "ReweightUtils/APReweightBase.h"

unsigned int APReweightBase::_NID = 0;

APReweightBase::APReweightBase()
  : _scale(0),
    _isTrig(0),
    _isQuiet(0),
    _syst_uncert_global(0),
    _empty_weight(0)
{
  _ID = _NID;
  ++_NID;
}

APReweightBase::~APReweightBase() { }

unsigned int APReweightBase::GetID() const {
  return _ID;
}
