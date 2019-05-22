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

  return StatusCode::SUCCESS;

}

/*
 * Start Auditing
 */
void PerfMonMTSvc::startAud( const std::string& stepName,
                             const std::string& compName ) {

  // Just for testing!!!
  if( compName != "HelloWorld") return;

  ATH_MSG_INFO("Starting Auditing " << stepName << " " << compName);
  const EventContext ctx = Gaudi::Hive::currentContext();
  if(ctx.valid()) {
    ATH_MSG_INFO("Current event number is " << ctx.evt() <<
                 " slot number is " << ctx.slot());
  }

  // Print the CPU information
  ATH_MSG_INFO("START :: Current thread's CPU clock time is " << PMonMT::get_thread_cpu_time() << " [ms]");
  ATH_MSG_INFO("START :: Current std::clock time is " << PMonMT::get_process_cpu_time() << " [ms]");

  // Capture here - NOT THREAD SAFE!!! - NOT COMPONENT AWARE!!!
  m_measurement.capture();
}

/*
 * Stop Auditing
 */
void PerfMonMTSvc::stopAud( const std::string& stepName,
                            const std::string& compName ) {
  // Just for testing!!!
  if( compName != "HelloWorld") return;

  ATH_MSG_INFO("Stopping Auditing " << stepName << " " << compName);

  // Print the CPU information
  ATH_MSG_INFO("STOP :: Current thread's CPU clock time is " << PMonMT::get_thread_cpu_time() << " [ms]");
  ATH_MSG_INFO("STOP :: Current std::clock time is " << PMonMT::get_process_cpu_time() << " [ms]");

  // Capture here - NOT THREAD SAFE!!! - NOT COMPONENT AWARE!!!
  double delta_cpu = PMonMT::get_thread_cpu_time() - m_measurement.cpu_time;
  ATH_MSG_INFO("DCPU :: " << delta_cpu << " [ms]");
}
