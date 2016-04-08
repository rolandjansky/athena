///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthPrescaler.cxx
// Implementation file for class AthPrescaler
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

#include "AthPrescaler.h"

AthPrescaler::AthPrescaler(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm( name, pSvcLocator ),
  m_pass( 0 ),
  m_seen( 0 )
{
  declareProperty( "PercentPass", m_percentPass=100.0,
                   "Percentage of events that should be passed" );
  m_percentPass.verifier().setBounds( 0.0, 100.0 );
}

AthPrescaler::~AthPrescaler( )
{}

StatusCode
AthPrescaler::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "..." 
                << endreq
                << "initialize ==> pass: " << m_percentPass);
  return StatusCode::SUCCESS;
}

StatusCode
AthPrescaler::execute()
{
  ++m_seen;
  float fraction = (float(100.0) * (float)(m_pass+1)) / (float)m_seen;
  if ( fraction > m_percentPass ) {
    setFilterPassed( false );
    ATH_MSG_INFO ("execute ==> filter failed");
  } else {
    ATH_MSG_INFO ("execute ==> filter passed");
    ++m_pass;
  }
  return StatusCode::SUCCESS;
}

StatusCode
AthPrescaler::finalize()
{
  ATH_MSG_INFO ("finalize : " << endreq
                << "  - total  events: " << m_seen << endreq
                << "  - passed events: " << m_pass);

  return StatusCode::SUCCESS;
}
