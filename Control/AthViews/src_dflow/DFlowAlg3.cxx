///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg3.cxx 
// Implementation file for class DFlowAlg3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthExStoreGateExample includes
#include "DFlowAlg3.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthenaKernel/ExtendedEventContext.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DFlowAlg3::DFlowAlg3( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_r_int( "dflow_int" ),
  m_r_ints( "dflow_ints" ),
  m_w_dflowDummy( "dflow_dummy" ),
  m_testUpdate( "testUpdate" )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );


  declareProperty( "RIntFlow", m_r_int, "Data flow of int (read)" );

  declareProperty( "RIntsFlow", m_r_ints, "Data flow of integers (read)" );

  declareProperty( "DFlowDummy", m_w_dflowDummy, "Dummy object to fix dependencies" );

  declareProperty( "TestUpdate", m_testUpdate, "Test update handle" );

}

// Destructor
///////////////
DFlowAlg3::~DFlowAlg3()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DFlowAlg3::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK( m_r_int.initialize() );
  CHECK( m_r_ints.initialize() );
  CHECK( m_w_dflowDummy.initialize() );
  CHECK( m_testUpdate.initialize() );
  CHECK( m_condKeyTest.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg3::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg3::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const EventContext& ctx = getContext();

  SG::ReadHandle< int > inputScalarHandle( m_r_int, ctx );
  ATH_MSG_INFO("================================");
  ATH_MSG_INFO("myint r-handle...");
  ATH_MSG_INFO("name: [" << inputScalarHandle.name() << "]");
  ATH_MSG_INFO("store [" << inputScalarHandle.store() << "]");
  ATH_MSG_INFO("clid: [" << inputScalarHandle.clid() << "]");

  ATH_MSG_INFO("ptr: " << inputScalarHandle.cptr());
  if ( inputScalarHandle.isValid() )
  {
    ATH_MSG_INFO( "val: " << *( inputScalarHandle.cptr() ) );
  }

  SG::ReadHandle< std::vector< int > > inputVectorHandle( m_r_ints, ctx );
  ATH_MSG_INFO("ints r-handle...");
  ATH_MSG_INFO("name: [" << inputVectorHandle.name() << "]");
  ATH_MSG_INFO("store [" << inputVectorHandle.store() << "]");
  ATH_MSG_INFO("clid: [" << inputVectorHandle.clid() << "]");
  ATH_MSG_INFO("cptr: " << inputVectorHandle.cptr());

  // try to modify 'ints' via ReadHandle<>
  // shouldn't compile
#ifdef TRY_COMPILATION_ERRORS
  inputVectorHandle->push_back(666);
#endif

  // create a temporary r-handle
  SG::ReadHandle< std::vector<int> > ints( inputVectorHandle.name() );
  StatusCode sc = ints.setProxyDict( Atlas::getExtendedEventContext(ctx).proxy() );
  if ( !sc.isSuccess() ) ATH_MSG_INFO( "Failed to load view " );
  ATH_MSG_INFO( "temporary r-handle[ints] - size: " << ints->size() );

  // test that inexistant proxies are correctly detected
  ATH_MSG_INFO("-- testing inexistant proxies --");
  {
    SG::ReadHandle<int> o("--r-not-there--");
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }
  {
    SG::WriteHandle<int> o("--w-not-there--");
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }

  // test WVar<T> semantics
  ATH_MSG_INFO("-- testing WVar<T> semantics...");
  {
    SG::WriteHandle<int> o( inputScalarHandle.name() );
    ATH_MSG_INFO("name: " << o.name());
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
    try {
      *o = 42;
      if (o.isValid()) {
        ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
        return StatusCode::FAILURE;
      }
    } catch (std::exception &err) {
      ATH_MSG_INFO("good, caught: [" << err.what() << "]");
    }
    ATH_MSG_INFO("valid:" << o.isValid());
    if (o.isValid()) {
      ATH_MSG_ERROR("should NOT be valid ! [line " << __LINE__ << "]" );
      return StatusCode::FAILURE;
    }
  }

  //Dummy object to fix the data flow
  SG::WriteHandle< int > outputHandle( m_w_dflowDummy, ctx );
  ATH_CHECK( outputHandle.record( std::make_unique<int>(1) ) );

  // Test update handles
  SG::ReadHandle< HiveDataObj > testUpdate( m_testUpdate, ctx );
  ATH_MSG_INFO( "Update handle final: " << testUpdate->val() );

  // Test conditions handles
  SG::ReadCondHandle< CondDataObj > testConditions( m_condKeyTest, ctx );
  ATH_MSG_INFO( "Conditions handle test: " << **testConditions << " from key " << testConditions.fullKey() );

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
