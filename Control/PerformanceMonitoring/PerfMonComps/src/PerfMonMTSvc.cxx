/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */

// Framework includes
#include "GaudiKernel/ThreadLocalContext.h"

// PerfMonComps includes
#include "PerfMonComps/PerfMonMTSvc.h"
#include "PerfMonUtils.h" // borrow from existing code

// Input/Output includes
#include <fstream>
#include <nlohmann/json.hpp> 

using json = nlohmann::json; // for convenience

/*
 * Constructor
 */
PerfMonMTSvc::PerfMonMTSvc( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : AthService( name, pSvcLocator ) {

  //ATH_MSG_INFO(" Pre initialization is captured!  ");
  m_measurement.capture();
  m_data[0].addPointStart(m_measurement);
 
}

PerfMonMTSvc::~PerfMonMTSvc(){

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
StatusCode PerfMonMTSvc::finalize(){ 
 
  //ATH_MSG_INFO("Finalize");
  //ATH_MSG_INFO("Post Finalization is captured!");
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
  /*
   * This if statement is temporary. It will be removed.
   * In current implementation the very first thing called is stopAud function
   * for PerfMonMTSvc. There are some components before it. We miss them.
   * It should be fixed.
   */
  if( compName != "PerfMonMTSvc" ){  
    startSnapshotAud(stepName, compName);
    startCompLevelAud(stepName, compName); 
  }
}

/*
 * Stop Auditing
 */
void PerfMonMTSvc::stopAud( const std::string& stepName,
                            const std::string& compName ) {
  if( compName != "PerfMonMTSvc" ){
    stopCompLevelAud(stepName, compName);
    stopSnapshotAud(stepName, compName);
  }
}

void PerfMonMTSvc::startSnapshotAud( const std::string& stepName,
                                     const std::string& compName ) {

  //ATH_MSG_INFO("PerfMonMTSvc::startAud: stepName: " << stepName << ", compName: " << compName);

  // Last thing to be called before the event loop begins
  if( compName == "AthRegSeq" && stepName == "Start") {
    //ATH_MSG_INFO("Pre Event Loop is captured!");
    m_measurement.capture();
    m_data[1].addPointStart(m_measurement);
  }

  // Last thing to be called before finalize step begins
  if ( compName == "AthMasterSeq" && stepName == "Finalize"){
    //ATH_MSG_INFO("Pre Finalization is captured!");
    m_measurement.capture();
    m_data[2].addPointStart(m_measurement);
  }

  /* Later we may need this
  if ( compName == "AuditorSvc" && stepName == "Finalize"){
    m_measurement.capture();
  } 
  */

}

void PerfMonMTSvc::stopSnapshotAud( const std::string& stepName,
                                    const std::string& compName ) {

  //ATH_MSG_INFO("PerfMonMTSvc::stopAud: stepName: " << stepName << ", compName: " << compName);

  // First thing to be called after the initialize step ends
  if ( compName == "AthMasterSeq" && stepName == "Initialize"){
    //ATH_MSG_INFO("Post Initialization is captured!");
    m_measurement.capture();
    m_data[0].addPointStop(m_measurement);
  }


  // First thing to be called after the event loop ends
  if( compName == "AthMasterSeq" && stepName == "Stop") {
    //ATH_MSG_INFO("Post Event Loop is captured!");
    m_measurement.capture();
    m_data[1].addPointStop(m_measurement);
  }

}


void PerfMonMTSvc::startCompLevelAud( const std::string& stepName,
                                      const std::string& compName) {
   
  // Current step - component pair. Ex: Initialize-StoreGateSvc 
  PMonMT::StepCompPair currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;

  // Capture the time
  m_measurement.capture(); 

  /*
   *  Dynamically create a MeasurementData instance for the current step-component pair
   *  This space will be freed after results are reported.
   */   
  m_compLevelDataMap[currentState] = new PMonMT::MeasurementData;
  m_compLevelDataMap[currentState]->addPointStart(m_measurement);

}

void PerfMonMTSvc::stopCompLevelAud( const std::string& stepName,
                                     const std::string& compName) {
  
  // Capture the time
  m_measurement.capture();

  // Current step - component pair. Ex: Initialize-StoreGateSvc
  PMonMT::StepCompPair currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
  
  // We do not need this, because we have just created it in startCompLevelAud
  //m_compLevelDataMap[currentState] = new PMonMT::MeasurementData;
  m_compLevelDataMap[currentState]->addPointStop(m_measurement);

}


// Report the results
void PerfMonMTSvc::report(){

  report2Stdout();
  report2JsonFile();

}

/*
 * JSON Format:
 *
 * {
 *   "Initialize" : {
 *     "comp1" : {
 *       "cpu_time" : cpu_measurement
 *       "wall_time": wall_measurement
 *     },
 *     "comp2": ...
 *   },
 *
 *   "Start": {...},
 *   "Stop" : {...},
 *   "Finalize": {...}
 * }
 *
 * Output Filename: PerfMonMTSvc_result.json
 */

void PerfMonMTSvc::report2JsonFile(){

  json j;

  for(auto& it : m_compLevelDataMap){

    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;
    
    double wall_time = it.second->m_delta_wall;
    double cpu_time = it.second->m_delta_cpu;

    j[stepName][compName] = { {"cpu_time", cpu_time}, {"wall_time", wall_time} }; 

  }

  std::ofstream o("PerfMonMTSvc_result.json");
  o << std::setw(4) << j << std::endl;

}

void PerfMonMTSvc::report2Stdout(){ 

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


  ATH_MSG_INFO("=========================================================");

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("                Component Level Monitoring                ");
  ATH_MSG_INFO("=========================================================");

  // Clear! ->
  ATH_MSG_INFO( "Step  CPU  Wall  Component"  );
  for(auto& it : m_compLevelDataMap){
    ATH_MSG_INFO( it.first.stepName << ": " <<  it.second->m_delta_cpu << "  -  "  << it.second->m_delta_wall <<   "     "  <<  it.first.compName  );
    delete it.second;
  }  

}

