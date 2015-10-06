/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ClearStorePolicy.h"

ClearStorePolicy::Type clearStorePolicy( const std::string& policyName,
					 MsgStream& msg )
{
  if ( "BeginEvent" == policyName )      { return ClearStorePolicy::BeginEvent;
  } else if ( "EndEvent" == policyName ) { return ClearStorePolicy::EndEvent;
  } else {
    msg << MSG::WARNING
    	<< "Unknown policy [" << policyName << "] for the 'clear-store' "
    	<< "policy !! We'll use 'BeginEvent' instead !!"
    	<< endreq;
    return ClearStorePolicy::BeginEvent;
  }
}
