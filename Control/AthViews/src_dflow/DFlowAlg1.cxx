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

#include "CxxUtils/make_unique.h"

#include "AthViews/View.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DFlowAlg1::DFlowAlg1( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthViewAlgorithm( name, pSvcLocator ),
  m_r_int( "view_start" ),
  m_w_int( "dflow_int" )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "IntFlow", m_w_int, "Data flow of int" );

  declareProperty( "ViewStart", m_r_int, "Seed data of view" );
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

  if ( !m_r_int.isValid() )
  {
    ATH_MSG_ERROR( "Failed to retrieve initial view data from store " << m_r_int.store() );
    return StatusCode::FAILURE;
  }
  int seedData = *m_r_int;

  ATH_MSG_INFO("myint handle...");
  ATH_MSG_INFO("name: [" << m_w_int.name() << "]");
  ATH_MSG_INFO("store [" << m_w_int.store() << "]");
  ATH_MSG_INFO("clid: [" << m_w_int.clid() << "]");
  
  m_w_int.record( CxxUtils::make_unique< int >( seedData ) );

  //redundant check as op = would throw if m_w_int was not valid (e.g. because if clid/key combo was duplicated)
  if (m_w_int.isValid())
  {
    ATH_MSG_INFO("ptr: " << m_w_int.cptr());
    ATH_MSG_INFO("val: " << *m_w_int);
    
    ATH_MSG_INFO("modify myint by value...");

    ATH_MSG_INFO("ptr: " << m_w_int.cptr());
    ATH_MSG_INFO("val: " << *m_w_int);
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
