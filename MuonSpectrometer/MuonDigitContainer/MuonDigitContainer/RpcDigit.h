/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcDigit.h

#ifndef RpcDigitUH
#define RpcDigitUH

// David Adams
// November 2001
//
// RPC digitization. Holds a channel ID.

#include <iosfwd>
#include "MuonDigitContainer/MuonDigit.h"
#include "MuonIdHelpers/RpcIdHelper.h"

class RpcDigit : public MuonDigit {

private:  // data

  // Time.
  float m_time;

public:  // functions

  // Default constructor.
  RpcDigit();

  // Full constructor from Identifier.
  RpcDigit(const Identifier& id, float time);

  // Is this a valid digit?
  bool is_valid(const RpcIdHelper * rpcHelper) const;

  // Return the Time.
  float time() const { return m_time; }

};

#endif
