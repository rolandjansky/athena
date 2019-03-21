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
  AthAlgorithm( name,    pSvcLocator )
{ }

// Destructor
///////////////
ErroneousAlg::~ErroneousAlg()
{ 
  ATH_MSG_DEBUG ( "Calling destructor" ) ;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ErroneousAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." ) ;
  return StatusCode::SUCCESS;
}

StatusCode ErroneousAlg::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." ) ;

  return StatusCode::SUCCESS;
}

StatusCode ErroneousAlg::execute()
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." ) ;
  
  if ( this->jumpOnUninitializedValue() )
    ATH_MSG_INFO ( " jumpOnUninitializedValue() returned false !" ) ;
  
  if ( this->invalidRead() )
    ATH_MSG_INFO ( " jumpOnUninitializedValue() returned false !" ) ;
  
  if ( this->mismatchedFree() )
    ATH_MSG_INFO ( " jumpOnUninitializedValue() returned false !" ) ;
  
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
  
  //m_msg << "Jump, jump not, jump, jump not : " << yesNo[0] << " to " << yesNo[1] << endreq;
  
  return (yesNo[1]==0);
}

bool ErroneousAlg::invalidRead()
{
  const unsigned int maximum=8192;
  double *invalidReadPointer = new double[maximum];
  
  // fill with something
  for ( unsigned int i=1; i<=maximum; ++i )
    invalidReadPointer[i] = double(i);
  ATH_MSG_INFO ( "Found, that last element contains " << invalidReadPointer[maximum] ) ;
  
  // and delete
  delete [] invalidReadPointer;
  
  // and print out element 10 !
  ATH_MSG_INFO ( "Found, that last element contains " << invalidReadPointer[maximum] ) ;
  
  return true;
}

bool ErroneousAlg::mismatchedFree()
{
  const unsigned int maximum=8192;
  double *invalidReadPointer = new double[maximum];
  
  // fill with something
  for ( unsigned int i=1; i<maximum; ++i )
    invalidReadPointer[i] = double(i);
  ATH_MSG_INFO ( "Found, that last element contains " << invalidReadPointer[maximum-1] ) ;
  
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

