/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//	====================================================================
//  AthenaConditionStream.cxx
//	--------------------------------------------------------------------
//
//	Package   : AthenaServices
//
//	Author    : SR: for use in non-Event stores
//
//	====================================================================
// STL include files
#include <iostream>
#include <string>

// Framework include files
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaConditionStream.h"

typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;

// Standard Constructor
AthenaConditionStream::AthenaConditionStream(const std::string& name, 
					     ISvcLocator* pSvcLocator)
  : AthenaOutputStream(name, pSvcLocator)
{  
  //bool retrieve = false;
  evtStore().setName ("ConditionStore");
}

// Standard Destructor
AthenaConditionStream::~AthenaConditionStream()   { }

// terminate data writer
StatusCode AthenaConditionStream::finalize() 
{
  // call regular execute to write out events:
  StatusCode status = AthenaOutputStream::execute();

  // summary message:
  ATH_MSG_INFO ("Events output: " << m_events);

  return(status);
}

StatusCode AthenaConditionStream::execute() 
{
  // overloaded to do nothing (called every event)
  return StatusCode::SUCCESS;
}

