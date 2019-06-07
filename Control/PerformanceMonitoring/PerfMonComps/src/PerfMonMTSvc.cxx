/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "GaudiKernel/ThreadLocalContext.h"

// PerfMonComps includes
#include "PerfMonComps/PerfMonMTSvc.h"
#include "PerfMonUtils.h" // borrow from existing code

/*
 * Constructor
 */
PerfMonMTSvc::PerfMonMTSvc( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : AthService( name, pSvcLocator ) {
}

/*
 * Query Interface
 */
StatusCode PerfMonMTSvc::queryInterface( const InterfaceID& riid,
                                         void** ppvInterface ) {
  if( !ppvInterface ) {
    return StatusCode::FAILURE;
  }

  if ( riid == IPerfMonMTSvc::interfaceID() ) {
    *ppvInterface = static_cast< IPerfMonMTSvc* >( this );
    return StatusCode::SUCCESS;
  }

  return AthService::queryInterface( riid, ppvInterface );
}

/*
 * Initialize the Service
 */
StatusCode PerfMonMTSvc::initialize() {
  ATH_MSG_INFO("Initialize");

  /// Configure the auditor
  if( !PerfMon::makeAuditor("PerfMonMTAuditor", auditorSvc(), msg()).isSuccess()) {
    ATH_MSG_ERROR("Could not register auditor [PerfMonMTAuditor]!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/*
 * Finalize the Service
 */
StatusCode PerfMonMTSvc::finalize() {
  ATH_MSG_INFO("Finalize");

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("                PerfMonMT Results Summary                ");
  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("Total Wall time in the event loop is " << m_data.m_delta_wall << " ms ");
  ATH_MSG_INFO("Total CPU  time in the event loop is " << m_data.m_delta_cpu  << " ms ");
  ATH_MSG_INFO("Average CPU utilization in the event loop is " <<
                m_data.m_delta_cpu/m_data.m_delta_wall );
  ATH_MSG_INFO("=========================================================");

  return StatusCode::SUCCESS;
}

/*
 * Start Auditing
 */
void PerfMonMTSvc::startAud( const std::string& stepName,
                             const std::string& compName ) {
  // Last thing to be called before the event loop begins
  if( compName == "AthRegSeq" && stepName == "Start") {
    m_measurement.capture();
    m_data.addPointStart(m_measurement);
  }
}

/*
 * Stop Auditing
 */
void PerfMonMTSvc::stopAud( const std::string& stepName,
                            const std::string& compName ) {
  // First thing to be called after the event loop begins
  if( compName == "AthMasterSeq" && stepName == "Stop") {
    m_measurement.capture();
    m_data.addPointStop(m_measurement);
  }
}
