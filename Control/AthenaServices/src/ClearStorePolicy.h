/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CLEARSTOREPOLICY_Y
#define CLEARSTOREPOLICY_Y 1

#include "GaudiKernel/MsgStream.h"
#include <string>

struct ClearStorePolicy {
  enum Type {
    BeginEvent = 1L << 0,
    EndEvent   = 1L << 1,
    
    // last !
    None
  };
};

/// returns the enum-version of the policy (by name)
ClearStorePolicy::Type clearStorePolicy( const std::string& policyName,
 					 MsgStream& msg );


#endif
