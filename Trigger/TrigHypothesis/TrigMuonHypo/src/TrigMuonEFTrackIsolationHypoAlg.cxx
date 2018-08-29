/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthLinks/ElementLink.h"
#include "TrigMuonHypo/TrigMuonEFTrackIsolationHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFTrackIsolationHypoAlg::TrigMuonEFTrackIsolationHypoAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
   ::HypoBase( name, pSvcLocator )
{}


TrigMuonEFTrackIsolationHypoAlg::~TrigMuonEFTrackIsolationHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFTrackIsolationHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
 
  ATH_MSG_INFO( "Initialization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFTrackIsolationHypoAlg::finalize() 
{   
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  ATH_MSG_INFO( "Finalization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFTrackIsolationHypoAlg::execute_r( const EventContext& context ) const
{

  ATH_MSG_DEBUG("StatusCode TrigMuonEFTrackIsolationHypoAlg::execute_r start");

  ATH_MSG_DEBUG("StatusCode TrigMuonEFTrackIsolationHypoAlg::execute_r success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

