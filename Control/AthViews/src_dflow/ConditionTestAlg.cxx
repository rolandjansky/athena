///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ConditionTestAlg.cxx 
// Implementation file for class ConditionTestAlg
// Author: B. Wynne <bwynne@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "ConditionTestAlg.h"

// STL includes

// FrameWork includes
#include "AthenaKernel/ExtendedEventContext.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ConditionTestAlg::ConditionTestAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
}

// Destructor
///////////////
ConditionTestAlg::~ConditionTestAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ConditionTestAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK( m_condKeyTest.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode ConditionTestAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ConditionTestAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  ATH_MSG_INFO( name() << " running with store " << Atlas::getExtendedEventContext(ctx).proxy()->name() );

  // Make a (large) validity range
  EventIDBase begin (0,
           EventIDBase::UNDEFEVT,  // event
           EventIDBase::UNDEFNUM,  // timestamp
           EventIDBase::UNDEFNUM,  // timestamp ns
           0);
  EventIDBase end (10000,
           EventIDBase::UNDEFEVT,  // event
           EventIDBase::UNDEFNUM,  // timestamp
           EventIDBase::UNDEFNUM,  // timestamp ns 
           10000);
  EventIDRange range( begin, end );

  // Write a dummy object with this validity range
  SG::WriteCondHandle< CondDataObj > condHandleTest( m_condKeyTest, ctx );
  ATH_CHECK( condHandleTest.record( range, std::make_unique< CondDataObj >( 987 ) ) );

  return StatusCode::SUCCESS;
}

} //> end namespace AthViews
