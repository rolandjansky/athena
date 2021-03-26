///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthEventCounter.cxx
// Implementation file for class AthEventCounter
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

#include "AthEventCounter.h"
#include <cstdlib>

StatusCode
AthEventCounter::initialize()
{
  ATH_MSG_INFO ("initialize ==> Frequency: " << m_frequency);
  return StatusCode::SUCCESS;
}

StatusCode
AthEventCounter::execute(const EventContext& ) const 
{
  int currTotal=(m_total++); 
  if (m_frequency==0) {
    return StatusCode::SUCCESS;
  }

  int rem=std::div(currTotal,m_frequency).rem;

  if (rem==0) {
      ATH_MSG_INFO ("execute ==> seen events: " << currTotal);
  }
  return StatusCode::SUCCESS;
}

StatusCode
AthEventCounter::finalize()
{
  ATH_MSG_INFO ("finalize ==> total events: " << m_total);
  return StatusCode::SUCCESS;
}
