/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"

// PerfMonComps includes
#include "PerfMonComps/PerfMonMTAlg.h"

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

  ATH_MSG_INFO("Initialize");

  /// Retrieve the PerfMonMTSvc
  CHECK( m_perfMonMTSvc.retrieve() );

  return StatusCode::SUCCESS;

}

/*
 * Finalize the algorithm
 */
StatusCode PerfMonMTAlg::finalize() {

  ATH_MSG_INFO("Finalize");

  return StatusCode::SUCCESS;

}

/*
 * Execute the algorithm
 */
StatusCode PerfMonMTAlg::execute( const EventContext& /*ctx*/ ) const {

  // Call startAud
  m_perfMonMTSvc->startAud("","");

  // Call stopAud
  m_perfMonMTSvc->stopAud("","");

  return StatusCode::SUCCESS;
}
