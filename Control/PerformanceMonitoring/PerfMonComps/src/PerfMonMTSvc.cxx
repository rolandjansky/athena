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

  ATH_MSG_INFO(" Pre initialization is captured!  ");
  m_measurement.capture();
  m_data[0].addPointStart(m_measurement);
 
}

PerfMonMTSvc::~PerfMonMTSvc(){

  ATH_MSG_INFO("Destructor!!!");

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
  /*
  // Get the IncidentSvc to capture the the moment when all components halt
  ServiceHandle < IIncidentSvc  > incSvc( "IncidentSvc", this->name() );
  if( !incSvc.retrieve().isSuccess() ){

    ATH_MSG_ERROR("Could not retrieve the IncidentSvc!");
    return StatusCode::FAILURE;

  }
 
  incSvc->addListener( this, IncidentType::SvcPostFinalize );  
  */
  return StatusCode::SUCCESS;
}

/*
 * Finalize the Service
 */
StatusCode PerfMonMTSvc::finalize(){ 
  ATH_MSG_INFO("Finalize");
  ATH_MSG_INFO("Post Finalization is captured!");
  m_measurement.capture();
  m_data[2].addPointStop(m_measurement);

  report();
  return StatusCode::SUCCESS;
}

/*
 * Start Auditing
 */
void PerfMonMTSvc::startAud( const std::string& stepName,
                             const std::string& compName ) {
  
  ATH_MSG_INFO("PerfMonMTSvc::startAud: stepName: " << stepName << ", compName: " << compName);
  m_measurement.capture(); // just to debug 
 
  // Last thing to be called before the event loop begins
  if( compName == "AthRegSeq" && stepName == "Start") {
    ATH_MSG_INFO("Pre Event Loop is captured!");
    m_measurement.capture();
    m_data[1].addPointStart(m_measurement);
  }

  // Last thing to be called before finalize step begins
  if ( compName == "AthMasterSeq" && stepName == "Finalize"){
    ATH_MSG_INFO("Pre Finalization is captured!");
    m_measurement.capture();
    m_data[2].addPointStart(m_measurement);
  }

  // Just to debug: Understand how much time we miss (AuditorSvc finalizes last)
  if ( compName == "AuditorSvc" && stepName == "Finalize"){
    ATH_MSG_INFO("AUDITOR SERVICE IS CAPTURED!");
    m_measurement.capture();
  }
  

}

/*
 * Stop Auditing
 */
void PerfMonMTSvc::stopAud( const std::string& stepName,
                            const std::string& compName ) {
  ATH_MSG_INFO("PerfMonMTSvc::stopAud: stepName: " << stepName << ", compName: " << compName);
  m_measurement.capture(); // just to debug 

  // First thing to be called after the initialize step ends
  if ( compName == "AthMasterSeq" && stepName == "Initialize"){
    ATH_MSG_INFO("Post Initialization is captured!");
    m_measurement.capture();
    m_data[0].addPointStop(m_measurement);
  }


  // First thing to be called after the event loop ends
  if( compName == "AthMasterSeq" && stepName == "Stop") {
    ATH_MSG_INFO("Post Event Loop is captured!");
    m_measurement.capture();
    m_data[1].addPointStop(m_measurement);
  }

  // Just to debug: How much time passes in PerfMonMTSvc configuration
  if ( compName == "PerfMonMTSvc" && stepName == "Initialize"){
    ATH_MSG_INFO("PerfMonMTSvc IS CAPTURED!");
    m_measurement.capture();
  }

}
/*
void PerfMonMTSvc::handle( const  Incident& incident ){

  // When all finalize steps of all components end, create report
  if(incident.type() == IncidentType::SvcPostFinalize ){
    ATH_MSG_INFO("POST FINALIZATION IS CAPTURED!");
    m_measurement.capture();
    m_data[2].addPointStop(m_measurement);
    report();
  }

} 
*/
// Report the results
void PerfMonMTSvc::report(){

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("                PerfMonMT Results Summary                ");
  ATH_MSG_INFO("=========================================================");

  ATH_MSG_INFO("Total Wall time in the Initialization is " << m_data[0].m_delta_wall << " ms ");
  ATH_MSG_INFO("Total CPU  time in the Initialization is " << m_data[0].m_delta_cpu  << " ms ");
  ATH_MSG_INFO("Average CPU utilization in the Initialization is " <<
                m_data[0].m_delta_cpu/m_data[0].m_delta_wall );
  ATH_MSG_INFO("");

  ATH_MSG_INFO("Total Wall time in the event loop is " << m_data[1].m_delta_wall << " ms ");
  ATH_MSG_INFO("Total CPU  time in the event loop is " << m_data[1].m_delta_cpu  << " ms ");
  ATH_MSG_INFO("Average CPU utilization in the event loop is " <<
                m_data[1].m_delta_cpu/m_data[1].m_delta_wall );
  ATH_MSG_INFO("");

  ATH_MSG_INFO("Total Wall time in the Finalize is " << m_data[2].m_delta_wall << " ms ");
  ATH_MSG_INFO("Total CPU  time in the Finalize is " << m_data[2].m_delta_cpu  << " ms ");
  ATH_MSG_INFO("Average CPU utilization in the Finalize is " <<
                m_data[2].m_delta_cpu/m_data[2].m_delta_wall );
  ATH_MSG_INFO("");

  ATH_MSG_INFO("=========================================================");

}
