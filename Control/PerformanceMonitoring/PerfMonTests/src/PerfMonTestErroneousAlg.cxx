///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestErroneousAlg.cxx 
// Implementation file for class PerfMonTest::ErroneousAlg
// Author: R.Seuster
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// PerfMonTests includes
#include "PerfMonTestErroneousAlg.h"

using namespace PerfMonTest;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ErroneousAlg::ErroneousAlg( const std::string& name, 
			    ISvcLocator* pSvcLocator ) : 
  Algorithm   ( name,    pSvcLocator ),
  m_msg       ( msgSvc(),       name )
{ }

// Destructor
///////////////
ErroneousAlg::~ErroneousAlg()
{ 
  m_msg << MSG::DEBUG << "Calling destructor" << endreq;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ErroneousAlg::initialize()
{
  // configure our MsgStream
  m_msg.setLevel( outputLevel() );

  m_msg << MSG::INFO 
	<< "Initializing " << name() << "..." 
	<< endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode ErroneousAlg::finalize()
{
  m_msg << MSG::INFO 
	<< "Finalizing " << name() << "..." 
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode ErroneousAlg::execute()
{  
  m_msg << MSG::DEBUG << "Executing " << name() << "..." 
	<< endreq;
  
  if ( this->jumpOnUninitializedValue() )
    m_msg << MSG::INFO << " jumpOnUninitializedValue() returned false !" << endreq;
  
  if ( this->invalidRead() )
    m_msg << MSG::INFO << " jumpOnUninitializedValue() returned false !" << endreq;
  
  if ( this->mismatchedFree() )
    m_msg << MSG::INFO << " jumpOnUninitializedValue() returned false !" << endreq;
  
  return StatusCode::SUCCESS;
}

bool ErroneousAlg::jumpOnUninitializedValue()
{
  const unsigned int maximum=8192;
  
  bool someConditions[64*maximum];
  
  unsigned int yesNo[] = { 0, 0};
  
  for ( unsigned int i=0; i<64*maximum; ++i )
    if ( this->shouldIJump(someConditions[i]) )
      yesNo[0]++;
    else
      yesNo[1]++;
  
  m_msg << "Jump, jump not, jump, jump not : " << yesNo[0] << " to " << yesNo[1] << endreq;
  
  return (yesNo[1]==0);
}

bool ErroneousAlg::invalidRead()
{
  const unsigned int maximum=8192;
  double *invalidReadPointer = new double[maximum];
  
  // fill with something
  for ( unsigned int i=1; i<=maximum; ++i )
    invalidReadPointer[i] = double(i);
  m_msg << MSG::INFO << "Found, that last element contains " << invalidReadPointer[maximum] << endreq;
  
  // and delete
  delete [] invalidReadPointer;
  
  // and print out element 10 !
  m_msg << MSG::INFO << "Found, that last element contains " << invalidReadPointer[maximum] << endreq;
  
  return true;
}

bool ErroneousAlg::mismatchedFree()
{
  const unsigned int maximum=8192;
  double *invalidReadPointer = new double[maximum];
  
  // fill with something
  for ( unsigned int i=1; i<maximum; ++i )
    invalidReadPointer[i] = double(i);
  m_msg << MSG::INFO << "Found, that last element contains " << invalidReadPointer[maximum-1] << endreq;
  
  // and delete
  delete invalidReadPointer;
  
  return true;
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

