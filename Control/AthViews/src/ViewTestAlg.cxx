///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ViewTestAlg.cxx 
// Implementation file for class ViewTestAlg
// Author: B. Wynne <bwynne@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "ViewTestAlg.h"

// STL includes

// FrameWork includes
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthViews/View.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ViewTestAlg::ViewTestAlg( const std::string& name, 
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
ViewTestAlg::~ViewTestAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ViewTestAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewTestAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewTestAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  auto theStore = Atlas::getExtendedEventContext(getContext()).proxy();
  ATH_MSG_INFO( name() << " running with store " << theStore->name() );

  // Identify if this is a view
  SG::View* theView = dynamic_cast< SG::View* >( theStore );
  if ( theView )
  {
    ATH_MSG_INFO( theView->dump() );
  }
  else
  {
    ATH_MSG_INFO( "This is not an EventView" );
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
