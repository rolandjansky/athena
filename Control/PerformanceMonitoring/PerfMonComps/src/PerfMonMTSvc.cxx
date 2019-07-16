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
  m_snapshotData[0].addPointStart(m_measurement);
 
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
  
  // TODO: Define this array as a class member !
  // Name the steps whose snapshots will be captured as a whole
  //const std::string snapshotStepNames[3] = {"Initialize","Event_loop","Finalize"};
  m_snapshotStepNames.push_back("Initialize");
  m_snapshotStepNames.push_back("Event_loop");
  m_snapshotStepNames.push_back("Finalize");

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
 
  //ATH_MSG_INFO("Post Finalization is captured!");
  m_measurement.capture();
  m_snapshotData[2].addPointStop(m_measurement);

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

    if (stepName == "Execute")
      startCompAud_MT(stepName, compName);
    else
      startCompAud_serial(stepName, compName);
     
  }
}

/*
 * Stop Auditing
 */
void PerfMonMTSvc::stopAud( const std::string& stepName,
                            const std::string& compName ) {

  if( compName != "PerfMonMTSvc" ){
    stopSnapshotAud(stepName, compName);


    if (stepName == "Execute")
      stopCompAud_MT(stepName, compName);
    else
      stopCompAud_serial(stepName, compName);

  }
}

void PerfMonMTSvc::startSnapshotAud( const std::string& stepName,
                                     const std::string& compName ) {

  // Last thing to be called before the event loop begins
  if( compName == "AthRegSeq" && stepName == "Start") {
    //ATH_MSG_INFO("Pre Event Loop is captured!");
    m_measurement.capture();
    m_snapshotData[1].addPointStart(m_measurement);
  }

  // Last thing to be called before finalize step begins
  if ( compName == "AthMasterSeq" && stepName == "Finalize"){
    //ATH_MSG_INFO("Pre Finalization is captured!");
    m_measurement.capture();
    m_snapshotData[2].addPointStart(m_measurement);
  }

  /* Later we may need this
  if ( compName == "AuditorSvc" && stepName == "Finalize"){
    m_measurement.capture();
  } 
  */

}

void PerfMonMTSvc::stopSnapshotAud( const std::string& stepName,
                                    const std::string& compName ) {

  // First thing to be called after the initialize step ends
  if ( compName == "AthMasterSeq" && stepName == "Initialize"){
    //ATH_MSG_INFO("Post Initialization is captured!");
    m_measurement.capture();
    m_snapshotData[0].addPointStop(m_measurement);
  }


  // First thing to be called after the event loop ends
  if( compName == "AthMasterSeq" && stepName == "Stop") {
    //ATH_MSG_INFO("Post Event Loop is captured!");
    m_measurement.capture();
    m_snapshotData[1].addPointStop(m_measurement);
  }

}


void PerfMonMTSvc::startCompAud_serial( const std::string& stepName,
                                      const std::string& compName) {

  // Current step - component pair. Ex: Initialize-StoreGateSvc 
  PMonMT::StepComp currentState;
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

void PerfMonMTSvc::stopCompAud_serial( const std::string& stepName,
                                     const std::string& compName) {

  // Capture the time
  m_measurement.capture();

  // Current step - component pair. Ex: Initialize-StoreGateSvc
  PMonMT::StepComp currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
 
  // We do not need this, because we have just created it in startCompAud_serial
  //m_compLevelDataMap[currentState] = new PMonMT::MeasurementData;
  m_compLevelDataMap[currentState]->addPointStop(m_measurement);

}

void PerfMonMTSvc::startCompAud_MT(const std::string& stepName,
                                   const std::string& compName) {


  std::lock_guard<std::mutex> lock( m_mutex );
   
  // Get the event number
  auto ctx = Gaudi::Hive::currentContext(); 
  int eventNumber = ctx.eventID().event_number();

  // Make generic
  PMonMT::StepCompEvent currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
  currentState.eventNumber = eventNumber;

  m_measurement.capture_MT( currentState );    

  m_parallelCompLevelData.addPointStart_MT(m_measurement, currentState);
}

void PerfMonMTSvc::stopCompAud_MT(const std::string& stepName,
                                   const std::string& compName) {


  std::lock_guard<std::mutex> lock( m_mutex );

  // Get the event number
  auto ctx = Gaudi::Hive::currentContext();
  int eventNumber = ctx.eventID().event_number();

  // Make generic
  PMonMT::StepCompEvent currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
  currentState.eventNumber = eventNumber;
        
  m_measurement.capture_MT( currentState );
    
  m_parallelCompLevelData.addPointStop_MT(m_measurement, currentState);

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
 *   "Snapshot_level" : {
 *     "Initialize" : {
 *       "cpu_time" : cpu_measurement
 *       "wall_time": wall_measurement
 *     },
 *     "Event_loop" : {...},
 *     "Finalize" : {... }
 *   },
 *   "Component_level" : {
 *     "Initialize" : {
 *        "comp1" : {
 *          "cpu_time" : cpu_measurement
 *          "wall_time": wall_measurement
 *        },
 *        "comp2": ...
 *     },
 *      "Start": {...},
 *      "Stop" : {...},
 *      "Finalize": {...}  
 *   }
 * }
 *
 * Output Filename: PerfMonMTSvc_result.json
 */

void PerfMonMTSvc::report2JsonFile(){

  json j;

  // Report snapshot level results
  for(int i = 0; i < 3; i++ ){

    // Clean this part!
    double wall_time = m_snapshotData[i].m_delta_wall;
    double cpu_time =  m_snapshotData[i].m_delta_cpu;

    j["Snapshot_level"][m_snapshotStepNames[i]] = { {"cpu_time", cpu_time}, {"wall_time", wall_time} };

  }
 
  // Report component level results
  for(auto& it : m_compLevelDataMap){

    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;
    
    double wall_time = it.second->m_delta_wall;
    double cpu_time = it.second->m_delta_cpu;
    
    // nlohmann::json syntax
    j["Component_level"][stepName][compName] =  { {"cpu_time", cpu_time}, {"wall_time", wall_time} } ; 

    // Free the dynamically allocated space
    delete it.second;
  }

  std::ofstream o("PerfMonMTSvc_result.json");
  o << std::setw(4) << j << std::endl;

}

void PerfMonMTSvc::report2Stdout(){ 

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("                PerfMonMT Results Summary                ");
  ATH_MSG_INFO("=========================================================");

  ATH_MSG_INFO("Total Wall time in the Initialization is " << m_snapshotData[0].m_delta_wall << " ms ");
  ATH_MSG_INFO("Total CPU  time in the Initialization is " << m_snapshotData[0].m_delta_cpu  << " ms ");
  ATH_MSG_INFO("Average CPU utilization in the Initialization is " <<
               m_snapshotData[0].m_delta_cpu/m_snapshotData[0].m_delta_wall );
  ATH_MSG_INFO("");

  ATH_MSG_INFO("Total Wall time in the event loop is " << m_snapshotData[1].m_delta_wall << " ms ");
  ATH_MSG_INFO("Total CPU  time in the event loop is " << m_snapshotData[1].m_delta_cpu  << " ms ");
  ATH_MSG_INFO("Average CPU utilization in the event loop is " <<
                m_snapshotData[1].m_delta_cpu/m_snapshotData[1].m_delta_wall );
  ATH_MSG_INFO("");

  ATH_MSG_INFO("Total Wall time in the Finalize is " << m_snapshotData[2].m_delta_wall << " ms ");
  ATH_MSG_INFO("Total CPU  time in the Finalize is " << m_snapshotData[2].m_delta_cpu  << " ms ");
  ATH_MSG_INFO("Average CPU utilization in the Finalize is " <<
                m_snapshotData[2].m_delta_cpu/m_snapshotData[2].m_delta_wall );


  ATH_MSG_INFO("=========================================================");

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("               Serial Component Level Monitoring                ");
  ATH_MSG_INFO("=========================================================");

  // Clear! ->
  ATH_MSG_INFO( "Step  CPU  Wall  Component"  );
  for(auto& it : m_compLevelDataMap){
    ATH_MSG_INFO( it.first.stepName << ": " <<  it.second->m_delta_cpu << "  -  "  << it.second->m_delta_wall <<   "     "  <<  it.first.compName  );
  } 
 
  ATH_MSG_INFO("=========================================================");

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("                Event Loop Monitoring                ");
  ATH_MSG_INFO("=========================================================");

  double cpu_sum = 0;
  double wall_sum = 0;

  double thread_id_diff_sum = 0; //should be 0 ideally
 

  for(auto& it : m_parallelCompLevelData.shared_measurement_delta_map){

    PMonMT::StepCompEvent currentState;
    currentState.stepName = it.first.stepName;
    currentState.compName = it.first.compName;
    currentState.eventNumber = it.first.eventNumber;

    double cpu = it.second.first;
    double wall = it.second.second;

    cpu_sum += cpu;
    wall_sum += wall;

    ATH_MSG_INFO( it.first.eventNumber << ": " <<  it.first.stepName << ": " <<  cpu  << "  -  "  << wall  <<   "     "  <<  it.first.compName  );

    //thread_id_diff_sum += it.second.thread_id_delta_map[currentState];


  }
   
  ATH_MSG_INFO("Event loop CPU Sum:  " << cpu_sum );
  ATH_MSG_INFO("Event loop Wall Sum:  " << wall_sum );
  //ATH_MSG_INFO("Thread id Diff Sum:  " << thread_id_diff_sum  );
}

