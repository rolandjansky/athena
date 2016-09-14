///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg2_manualViews.cxx 
// Implementation file for class DFlowAlg2_manualViews
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthExStoreGateExample includes
#include "DFlowAlg2_manualViews.h"

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
DFlowAlg2_manualViews::DFlowAlg2_manualViews( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_r_int( "dflow_int" ),
  //m_rw_int( "dflow_int" ),
  m_ints( "dflow_ints" ),
  m_viewName( "view1" ),
  m_r_views( "all_views" )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "RIntFlow", m_r_int, "Data flow of int" );

  /*declareProperty( "RWIntFlow", m_rw_int, "Data flow of int" );*/

  declareProperty( "IntsFlow", m_ints, "Data flow of integers" );

  declareProperty( "ViewName", m_viewName, "Name of event view to use" );

  declareProperty( "AllViews", m_r_views, "All views" );
}

// Destructor
///////////////
DFlowAlg2_manualViews::~DFlowAlg2_manualViews()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DFlowAlg2_manualViews::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg2_manualViews::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg2_manualViews::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  //Use views if told to
  if ( m_viewName != "" )
  {
    //Examine all views
    bool foundView = false;
    for ( SG::View * view : *( m_r_views ) )
    {
      //Find the view by name
      if ( view->name() == m_viewName )
      {
        foundView = true;
        StatusCode sc = m_r_int.setProxyDict( view );
        //sc = m_rw_int.setProxyDict( view );
        sc = m_ints.setProxyDict( view );
        if ( !sc.isSuccess() ) ATH_MSG_ERROR( "Failed to load view " << m_viewName );
        break;
      }
    }
    if ( !foundView ) ATH_MSG_ERROR( "Failed to find view " << m_viewName );
  }

  ATH_MSG_INFO("================================");
  ATH_MSG_INFO("myint r-handle...");
  ATH_MSG_INFO("name: [" << m_r_int.name() << "]");
  ATH_MSG_INFO("store [" << m_r_int.store() << "]");
  ATH_MSG_INFO("clid: [" << m_r_int.clid() << "]");

  ATH_MSG_INFO("ptr: " << m_r_int.cptr());
  if (m_r_int.isValid()) {
    ATH_MSG_INFO("val: " << *(m_r_int.cptr()));
  }

  //UpdateHandles have changed
  /*ATH_MSG_INFO("myint rw-handle...");
  ATH_MSG_INFO("name: [" << m_rw_int.name() << "]");
  ATH_MSG_INFO("store [" << m_rw_int.store() << "]");
  ATH_MSG_INFO("clid: [" << m_rw_int.clid() << "]");
  ATH_MSG_INFO("ptr: " << m_rw_int.ptr());
  if (m_rw_int.isValid())
  {
    ATH_MSG_INFO("val: " << *(m_rw_int.cptr()));
    *m_rw_int += 100;

    ATH_MSG_INFO("val: " << *m_rw_int);
  }
  ATH_MSG_INFO("cptr: " << m_rw_int.cptr());*/

  ATH_MSG_INFO("ints w-handle...");
  m_ints.record( CxxUtils::make_unique< std::vector< int > >() );
  m_ints->push_back( 10 );
  //would be nice if it worked...  if (0 != m_r_int) m_ints->push_back(*m_r_int);
  if ( m_r_int.isValid() ) m_ints->push_back( *m_r_int );
  ATH_MSG_INFO( "size:" << m_ints->size() );
  for ( int i = 0, imax = m_ints->size(); i != imax; ++i )
  {
    ATH_MSG_INFO( "val[" << i << "]= " << m_ints->at( i ) );
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
