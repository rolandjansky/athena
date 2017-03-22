///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg2.cxx 
// Implementation file for class DFlowAlg2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthExStoreGateExample includes
#include "DFlowAlg2.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "CxxUtils/make_unique.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DFlowAlg2::DFlowAlg2( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_r_int( "dflow_int" ),
  m_ints( "dflow_ints" )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "RIntFlow", m_r_int, "Data flow of int" );

  declareProperty( "IntsFlow", m_ints, "Data flow of integers" );
}

// Destructor
///////////////
DFlowAlg2::~DFlowAlg2()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DFlowAlg2::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK( m_r_int.initialize() );
  CHECK( m_ints.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg2::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg2::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  SG::ReadHandle< int > inputHandle( m_r_int, ctx );
  ATH_MSG_INFO("================================");
  ATH_MSG_INFO("myint r-handle...");
  ATH_MSG_INFO("name: [" << inputHandle.name() << "]");
  ATH_MSG_INFO("store [" << inputHandle.store() << "]");
  ATH_MSG_INFO("clid: [" << inputHandle.clid() << "]");

  ATH_MSG_INFO("ptr: " << inputHandle.cptr());
  if ( inputHandle.isValid() )
  {
    ATH_MSG_INFO("val: " << *( inputHandle.cptr() ) );
  }

  SG::WriteHandle< std::vector< int > > outputHandle( m_ints, ctx );
  ATH_MSG_INFO("ints w-handle...");
  outputHandle.record( CxxUtils::make_unique< std::vector< int > >() );
  outputHandle->push_back( 10 );

  if ( inputHandle.isValid() )
  {
    outputHandle->push_back( *inputHandle );
  }

  ATH_MSG_INFO( "size:" << outputHandle->size() );
  for ( int i = 0, imax = outputHandle->size(); i != imax; ++i )
  {
    ATH_MSG_INFO( "val[" << i << "]= " << outputHandle->at( i ) );
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

} //> end namespace AthViews
