///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ViewTestAlg.cxx 
// Implementation file for class ViewTestAlg
// Author: B. Wynne <bwynne@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "ViewTestAlg.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthViews/View.h"

namespace AthViews {

ViewTestAlg::ViewTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator )
{
}

ViewTestAlg::~ViewTestAlg()
{
}

StatusCode ViewTestAlg::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() << "..." );

  if ( m_output.key() == "" )
  {
    renounce( m_output );
  }
  else
  {
    CHECK( m_output.initialize() );
  }

  return StatusCode::SUCCESS;
}

StatusCode ViewTestAlg::finalize()
{
  ATH_MSG_DEBUG( "Finalizing " << name() << "..." );

  return StatusCode::SUCCESS;
}

StatusCode ViewTestAlg::execute()
{  
  ATH_MSG_DEBUG( "Executing " << name() << "..." );

  auto theStore = Atlas::getExtendedEventContext( getContext() ).proxy();
  ATH_MSG_INFO( name() << " running with store " << theStore->name() );

  // Make (optional) output
  if ( m_output.key() != "" )
  {
    auto output = SG::makeHandle( m_output, getContext() );
    output = std::make_unique<int>( 1 );
  }

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

} //> end namespace AthViews
