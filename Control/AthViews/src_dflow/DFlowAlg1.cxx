///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg1.cxx 
// Implementation file for class DFlowAlg1
// Modifed by bwynne to add simple tests for views
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthExStoreGateExample includes
#include "DFlowAlg1.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DFlowAlg1::DFlowAlg1( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_r_int( "view_start" ),
  m_w_int( "dflow_int" )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "IntFlow", m_w_int, "Data flow of int" );

  declareProperty( "ViewStart", m_r_int, "Seed data of view" ); //implicit DH or whatever
}

// Destructor
///////////////
DFlowAlg1::~DFlowAlg1()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DFlowAlg1::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK( m_r_int.initialize() );
  CHECK( m_w_int.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg1::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg1::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  SG::ReadHandle< int > inputData( m_r_int, ctx );
  if ( !inputData.isValid() )
  {
    ATH_MSG_ERROR( "Failed to retrieve initial view data from store " << inputData.store() );
    return StatusCode::FAILURE;
  }
  int seedData = *inputData;

  SG::WriteHandle< int > outputData( m_w_int, ctx );
  ATH_MSG_INFO("myint handle...");
  ATH_MSG_INFO("name: [" << outputData.name() << "]");
  ATH_MSG_INFO("store [" << outputData.store() << "]");
  ATH_MSG_INFO("clid: [" << outputData.clid() << "]");
  
  outputData.record( CxxUtils::make_unique< int >( seedData ) );

  //redundant check as op = would throw if outputData was not valid (e.g. because if clid/key combo was duplicated)
  if ( outputData.isValid() )
  {
    ATH_MSG_INFO("ptr: " << outputData.cptr());
    ATH_MSG_INFO("val: " << *outputData);
    
    ATH_MSG_INFO("modify myint by value...");

    ATH_MSG_INFO("ptr: " << outputData.cptr());
    ATH_MSG_INFO("val: " << *outputData);
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
