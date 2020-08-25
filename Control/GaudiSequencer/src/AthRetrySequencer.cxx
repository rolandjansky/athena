///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthRetrySequencer.cxx 
// Implementation file for class AthRetrySequencer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// GaudiSequencer includes
#include "AthRetrySequencer.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthRetrySequencer::AthRetrySequencer( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) : 
  ::AthSequencer( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  declareProperty("MaxRetries", 
                  m_maxRetries = 5, 
                  "maximum number of times to retry running the subsequence "
                  "inside an event before giving up (-1 for infinity)" );

}

// Destructor
///////////////
AthRetrySequencer::~AthRetrySequencer()
{}

StatusCode AthRetrySequencer::execute( const EventContext& ctx ) const
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  int iloop = 0;

  while (iloop < m_maxRetries || m_maxRetries == -1) {
    iloop += 1;
    this->AthSequencer::resetExecuted( ctx );
    if (this->AthSequencer::execute( ctx ).isSuccess() &&
        this->execState(ctx).filterPassed()) {
      return StatusCode::SUCCESS;
    }
  }
  ATH_MSG_WARNING("maximum number of retries reached (" 
                  << m_maxRetries << ") => Giving up!");

  return StatusCode::FAILURE;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


