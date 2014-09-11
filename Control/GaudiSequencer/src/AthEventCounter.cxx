///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthEventCounter.cxx
// Implementation file for class AthEventCounter
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

#include "AthEventCounter.h"

/**
 ** Constructor(s)
 **/
AthEventCounter::AthEventCounter(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm( name, pSvcLocator ),
  m_skip ( 0 ),
  m_total( 0 )
{
  declareProperty( "Frequency", m_frequency=1,
                   "The frequency with which the number of events should be "
                   "reported. The default is 1, corresponding to every event" );
  m_frequency.verifier().setBounds( 0, 1000 );
}

/**
 ** Destructor
 **/
AthEventCounter::~AthEventCounter( )
{}

StatusCode
AthEventCounter::initialize()
{
  ATH_MSG_INFO ("initialize ==> Frequency: " << m_frequency);
  return StatusCode::SUCCESS;
}

StatusCode
AthEventCounter::execute()
{
  m_total++;
  int freq = m_frequency;
  if ( freq > 0 ) {
    m_skip++;
    if ( m_skip >= freq ) {
      ATH_MSG_INFO ("execute ==> seen events: " << m_total);
      m_skip = 0;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
AthEventCounter::finalize()
{
  ATH_MSG_INFO ("finalize ==> total events: " << m_total);
  return StatusCode::SUCCESS;
}
