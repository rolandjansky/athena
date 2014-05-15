/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcDigit.cxx

#include "MuonDigitContainer/RpcDigit.h"

//**********************************************************************
// Member functions.
//**********************************************************************

// Default constructor.

RpcDigit::RpcDigit()
: MuonDigit(), m_time() { }
 
//**********************************************************************

// Full constructor from Identifier.

RpcDigit::RpcDigit(const Identifier& id, float time)
: MuonDigit(id), m_time(time) { }

//**********************************************************************

// Validity check.

bool RpcDigit::is_valid(const RpcIdHelper * rpcHelper) const {
  return (rpcHelper->valid(m_muonId));
}

