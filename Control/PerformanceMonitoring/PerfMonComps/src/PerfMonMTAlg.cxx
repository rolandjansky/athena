/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"

// PerfMonComps includes
#include "PerfMonMTAlg.h"

/*
 * Constructor
 */
PerfMonMTAlg::PerfMonMTAlg( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : AthReentrantAlgorithm( name, pSvcLocator ),
    m_perfMonMTSvc( "PerfMonMTSvc", name ) {

}

/*
 * Initialize the algorithm
 */
StatusCode PerfMonMTAlg::initialize() {

  ATH_MSG_DEBUG("Initialize");

  /// Retrieve the PerfMonMTSvc
  CHECK( m_perfMonMTSvc.retrieve() );

  return StatusCode::SUCCESS;

}

/*
 * Execute the algorithm
 */
StatusCode PerfMonMTAlg::execute( const EventContext& /*ctx*/ ) const {

  // Do event level monitoring on check points
  m_perfMonMTSvc->eventLevelMon();

  return StatusCode::SUCCESS;
}
