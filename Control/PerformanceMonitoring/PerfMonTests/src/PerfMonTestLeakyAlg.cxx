///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestLeakyAlg.cxx 
// Implementation file for class PerfMonTest::LeakyAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

// PerfMonTests includes
#include "PerfMonTestLeakyAlg.h"

using namespace PerfMonTest;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
LeakyAlg::LeakyAlg( const std::string& name, 
		  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm   ( name,    pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "LeakSize",
                   m_leakSize = 10,
                   "Size of 'Leak' objects to be leaked each event" );

  declareProperty( "NbrLeaks",
                   m_nbrLeaks = 1,
                   "Number of 'Leak' objects to be leaked each event" );
}

// Destructor
///////////////
LeakyAlg::~LeakyAlg()
{ 
  ATH_MSG_DEBUG ( "Calling destructor" ) ;
  // finally deleting our leaked objects...
  for ( std::list<Leak*>::iterator i = m_leaks.begin(), iEnd = m_leaks.end();
	i != iEnd;
	++i ) {
    delete *i; *i = 0;
  }
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode LeakyAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." ) ;

  // Get pointer to StoreGateSvc and cache it :
  ATH_CHECK( evtStore().retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode LeakyAlg::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." ) ;
  return StatusCode::SUCCESS;
}

StatusCode LeakyAlg::execute()
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." ) ;

  if ( 0 == m_leakSize ) {
    return StatusCode::SUCCESS;
  }

  for ( int ileak = 0; ileak < m_nbrLeaks; ++ileak) {
    Leak * leak = new Leak;
    leak->m_data.reserve( m_leakSize );
    for ( std::size_t i = 0; i != static_cast<std::size_t>(m_leakSize); ++i ) {
      leak->m_data.push_back( i );
    }

    m_leaks.push_back( leak );
  }

  return StatusCode::SUCCESS;
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

