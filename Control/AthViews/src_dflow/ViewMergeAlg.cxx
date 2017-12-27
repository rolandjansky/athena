/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExStoreGateExample includes
#include "ViewMergeAlg.h"
#include "AthViews/ViewHelper.h"

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
ViewMergeAlg::ViewMergeAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_r_ints( "dflow_ints" )
{
}

// Destructor
///////////////
ViewMergeAlg::~ViewMergeAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ViewMergeAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK( m_r_ints.initialize() );
  CHECK( m_w_ints.initialize() );
  CHECK( m_r_views.initialize() );
  
  return StatusCode::SUCCESS;
}

StatusCode ViewMergeAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewMergeAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  //Merge results
  std::vector< int > outputVector;
  SG::ReadHandle< std::vector< SG::View* > > inputViews( m_r_views, ctx );
  CHECK( ViewHelper::MergeViewCollection( *inputViews,  //Vector of views (inside ReadHandle)
				m_r_ints,               //ReadHandleKey to access the views
                                ctx,                    //The context of this algorithm
				outputVector ) );       //Container to merge results into

  //Output the merged data
  SG::WriteHandle< std::vector< int > > outputHandle( m_w_ints, ctx );
  outputHandle.record( CxxUtils::make_unique< std::vector< int > >( outputVector ) );
  if ( !outputHandle.isValid() )
  {
    ATH_MSG_INFO( "Unable to load main event store for output" );
  }

  for ( int const test : outputVector )
  {
    ATH_MSG_INFO( test );
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
