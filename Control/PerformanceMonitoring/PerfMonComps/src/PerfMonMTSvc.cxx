/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */

// Framework includes
#include "GaudiKernel/ThreadLocalContext.h"

// PerfMonComps includes
#include "PerfMonMTSvc.h"
#include "PerfMonUtils.h" // borrow from existing code

// Input/Output includes
#include <fstream>
#include <iomanip>

#include "boost/format.hpp"

using json = nlohmann::json; // for convenience

/*
 * Constructor
 */
PerfMonMTSvc::PerfMonMTSvc( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : AthService( name, pSvcLocator ) {

  m_measurement.capture();
  m_snapshotData[0].addPointStart(m_measurement);

  declareProperty( "doEventLoopMonitoring",
                  m_isEventLoopMonitoring = false,
                  "True if event loop monitoring is enable, false o/w."
                  "Event loop monitoring may cause a decrease in the performance"
                  "due to the usage of locks" );
 
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
  if(m_isEventLoopMonitoring)
    ATH_MSG_INFO("----------------------------!!!!!!!!!!!!!!!!!!!!!!-----------------------------");
  
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

    //if (stepName == "Execute")
    if( isLoop() && m_isEventLoopMonitoring == true)
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

    if( isLoop() &&  m_isEventLoopMonitoring == true)
      stopCompAud_MT(stepName, compName);
    else
      stopCompAud_serial(stepName, compName);

  }
}

void PerfMonMTSvc::startSnapshotAud( const std::string& stepName,
                                     const std::string& compName ) {

  // Last thing to be called before the event loop begins
  if( compName == "AthRegSeq" && stepName == "Start") {
    m_measurement.capture();
    m_snapshotData[1].addPointStart(m_measurement);
  }

  // Last thing to be called before finalize step begins
  if ( compName == "AthMasterSeq" && stepName == "Finalize"){
    m_measurement.capture();
    m_snapshotData[2].addPointStart(m_measurement);
  }

}

void PerfMonMTSvc::stopSnapshotAud( const std::string& stepName,
                                    const std::string& compName ) {

  // First thing to be called after the initialize step ends
  if ( compName == "AthMasterSeq" && stepName == "Initialize"){
    m_measurement.capture();
    m_snapshotData[0].addPointStop(m_measurement);
  }


  // First thing to be called after the event loop ends
  if( compName == "AthMasterSeq" && stepName == "Stop") {
    m_measurement.capture();
    m_snapshotData[1].addPointStop(m_measurement);
  }

}


void PerfMonMTSvc::startCompAud_serial( const std::string& stepName,
                                      const std::string& compName) {

  // Current step - component pair. Ex: Initialize-StoreGateSvc 
  PMonMT::StepComp currentState = generate_serial_state (stepName, compName);
  
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
  PMonMT::StepComp currentState = generate_serial_state (stepName, compName);

  m_compLevelDataMap[currentState]->addPointStop(m_measurement);
}

void PerfMonMTSvc::startCompAud_MT(const std::string& stepName,
                                   const std::string& compName) {


  std::lock_guard<std::mutex> lock( m_mutex );
   
  // Get the event number
  int eventNumber = getEventNumber();
  eventCounter(eventNumber);

  PMonMT::StepCompEvent currentState = generate_parallel_state(stepName, compName, eventNumber);

  m_measurement.capture_MT( currentState );    

  m_parallelCompLevelData.addPointStart_MT(m_measurement, currentState);
}

void PerfMonMTSvc::stopCompAud_MT(const std::string& stepName,
                                   const std::string& compName) {


  std::lock_guard<std::mutex> lock( m_mutex );

  // Get the event number
  int eventNumber = getEventNumber();

  PMonMT::StepCompEvent currentState = generate_parallel_state(stepName, compName, eventNumber);
        
  m_measurement.capture_MT( currentState );
    
  m_parallelCompLevelData.addPointStop_MT(m_measurement, currentState);
}

bool PerfMonMTSvc::isLoop(){
  int eventNumber = getEventNumber();
  return (eventNumber > 0) ? true : false;
}

int PerfMonMTSvc::getEventNumber(){

  auto ctx = Gaudi::Hive::currentContext();
  int eventNumber = ctx.eventID().event_number();
  return eventNumber;
}

void PerfMonMTSvc::parallelDataAggregator(){

  std::map< PMonMT::StepComp, PMonMT::Measurement >::iterator sc_itr;

  for(auto& sce_itr : m_parallelCompLevelData.m_delta_map ){

    PMonMT::StepComp currentState = generate_serial_state (sce_itr.first.stepName, sce_itr.first.compName);
 
    // If the current state exists in the map, then aggregate it. o/w create a instance for it.
    sc_itr = m_aggParallelCompLevelDataMap.find(currentState);
    if(sc_itr != m_aggParallelCompLevelDataMap.end()){
      m_aggParallelCompLevelDataMap[currentState].cpu_time += sce_itr.second.cpu_time;
      m_aggParallelCompLevelDataMap[currentState].wall_time += sce_itr.second.wall_time;
    }
    else{

      m_aggParallelCompLevelDataMap[currentState] = sce_itr.second;      
    }
      
  } 

}


PMonMT::StepComp PerfMonMTSvc::generate_serial_state( const std::string& stepName,
                                        const std::string& compName){

  PMonMT::StepComp currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
  return currentState;
}

PMonMT::StepCompEvent PerfMonMTSvc::generate_parallel_state( const std::string& stepName,
                                               const std::string& compName,
                                                 const int& eventNumber){

  PMonMT::StepCompEvent currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
  currentState.eventNumber = eventNumber;
  return currentState;
}

std::string PerfMonMTSvc::get_cpu_model_info(){

  std::string cpu_model;

  std::ifstream file("/proc/cpuinfo");
  std::string line;
  if(file.is_open()){

    std::string delimiter = ":";
    while(getline( file, line)){

      std::string key = line.substr(0, line.find(delimiter));
      if(key == "model name	"){
      	cpu_model = line.substr(line.find(delimiter)+1, line.length());
      	break;
      }
    }
    file.close();
    return cpu_model;
  }
  else{
    std::cout << "Unable to open /proc/cpuinfo" << std::endl;
    return "Unable to open /proc/cpuinfo";
  }

}

int PerfMonMTSvc::get_cpu_core_info(){

  int logical_core_num = 0;

  std::ifstream file("/proc/cpuinfo");
  std::string line;
  if(file.is_open()){

    std::string delimiter = ":";
    while(getline( file, line)){

      std::string key = line.substr(0, line.find(delimiter));
      if(key == "processor	"){
      	logical_core_num++;
      }
    }
    file.close();
    return logical_core_num;
  }
  else{
    std::cout << "Unable to open /proc/cpuinfo" << std::endl;
    return -1;
  }
}

void PerfMonMTSvc::eventCounter(int eventNumber){
/*
  if(m_minEventNum > eventNumber)
    m_minEventNum = eventNumber;

  if(m_maxEventNum < eventNumber)
    m_maxEventNum = eventNumber;
*/
  m_eventIds.insert(eventNumber);
}


// Report the results
void PerfMonMTSvc::report(){

  report2Stdout();
  report2JsonFile();

}

void PerfMonMTSvc::report2JsonFile_Summary(nlohmann::json& j){

  // Report snapshot level results
  for(int i = 0; i < 3; i++ ){

    // Clean this part!
    double wall_time = m_snapshotData[i].m_delta_wall;
    double cpu_time =  m_snapshotData[i].m_delta_cpu;

    j["Snapshot_level"][m_snapshotStepNames[i]] = { {"cpu_time", cpu_time}, {"wall_time", wall_time} };

  }

}
void PerfMonMTSvc::report2JsonFile_Serial(nlohmann::json& j){

  // Report component level results
  for(auto& it : m_compLevelDataMap){

    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;
    
    double wall_time = it.second->m_delta_wall;
    double cpu_time = it.second->m_delta_cpu;
    
    // nlohmann::json syntax
    j["Serial_Component_level"][stepName][compName] =  { {"cpu_time", cpu_time}, {"wall_time", wall_time} } ; 

    // Free the dynamically allocated space
    delete it.second;
  }
}

void PerfMonMTSvc::report2JsonFile_Parallel(nlohmann::json& j){

  for(auto& it : m_aggParallelCompLevelDataMap){

    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;

    double wall_time = it.second.wall_time;
    double cpu_time = it.second.cpu_time;

    j["Parallel_Component_level"][stepName][compName] = { {"cpu_time", cpu_time}, {"wall_time", wall_time} } ; 

  }
}


void PerfMonMTSvc::report2JsonFile(){

  json j;

  report2JsonFile_Summary(j);
  report2JsonFile_Serial(j);

  if(m_isEventLoopMonitoring)
    report2JsonFile_Parallel(j);
 

  std::ofstream o("PerfMonMTSvc_result.json");
  o << std::setw(4) << j << std::endl;

}




void PerfMonMTSvc::report2Stdout(){ 
  
  report2Stdout_Serial();

  if(m_isEventLoopMonitoring)
    report2Stdout_Parallel();

  report2Stdout_Summary();
  report2Stdout_CpuInfo();
/*
  //ATH_MSG_INFO( "Max/Min:  " << m_maxEventNum << " "  <<  m_minEventNum);
  ATH_MSG_INFO( "Number of Events processed so far:  " << m_eventIds.size() );
  for(auto it : m_eventIds)
    ATH_MSG_INFO( "Event ID: " << it );
*/
  
  /*
  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("                Event Loop Monitoring                ");
  ATH_MSG_INFO("=========================================================");

  double cpu_sum = 0;
  double wall_sum = 0;

 

  for(auto& it : m_parallelCompLevelData.m_delta_map){

    PMonMT::StepCompEvent currentState = generate_parallel_state(it.first.stepName, it.first.compName, it.first.eventNumber);

    double cpu = it.second.cpu_time;
    double wall = it.second.wall_time;

    cpu_sum += cpu;
    wall_sum += wall;

    ATH_MSG_INFO("     " <<  it.first.eventNumber << ":     " <<  it.first.stepName << ": " <<  cpu  << "  -  "  << wall  <<   "     "  <<  it.first.compName  );

  }

  ATH_MSG_INFO("Event loop CPU Sum:  " << cpu_sum );
  ATH_MSG_INFO("Event loop Wall Sum:  " << wall_sum );
  */
}

void PerfMonMTSvc::report2Stdout_Summary(){

  using boost::format;
  using boost::io::group;


  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;
  std::cout << "PerfMonMTSvc                               PerfMonMT Results Summary                                " << std::endl;
  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;


  std::cout << format( "PerfMonMTSvc %|5t|%1% %|60t|%2% %|65t|%3% \n") % "Total CPU time in the Initialization is:" % m_snapshotData[0].m_delta_cpu % "ms";
  std::cout << format( "PerfMonMTSvc %|5t|%1% %|60t|%2% %|65t|%3% \n") % "Total Wall time in the Initialization is:" % m_snapshotData[0].m_delta_wall % "ms";

  std::cout << "PerfMonMTSvc\n"; 


  std::cout << format( "PerfMonMTSvc %|5t|%1% %|60t|%2% %|65t|%3% \n") % "Total CPU time in the Event Loop is:" % m_snapshotData[1].m_delta_cpu % "ms";
  std::cout << format( "PerfMonMTSvc %|5t|%1% %|60t|%2% %|65t|%3% \n") % "Total Wall time in the Event Loop is:" % m_snapshotData[1].m_delta_wall % "ms";

  std::cout << "PerfMonMTSvc\n";

  std::cout << format( "PerfMonMTSvc %|5t|%1% %|60t|%2% %|65t|%3% \n") % "Total CPU time in the Finalize is:" % m_snapshotData[2].m_delta_cpu % "ms";
  std::cout << format( "PerfMonMTSvc %|5t|%1% %|60t|%2% %|65t|%3% \n") % "Total Wall time in the Finalize is:" % m_snapshotData[2].m_delta_wall % "ms";
 
  std::cout << "PerfMonMTSvc\n";
  

  std::cout << format( "PerfMonMTSvc %|5t|%1% %|60t|%2% \n") % "Number of Events processed so far:" %  m_eventIds.size();

  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;

/*
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
*/
}
void PerfMonMTSvc::report2Stdout_Serial(){
/*
  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("               Serial Component Level Monitoring                ");
  ATH_MSG_INFO("=========================================================");
  
  // Clear! ->
  ATH_MSG_INFO( "Step  CPU  Wall  Component"  );
  for(auto& it : m_compLevelDataMap){
    ATH_MSG_INFO( it.first.stepName << ": " <<  it.second->m_delta_cpu << "  -  "  << it.second->m_delta_wall <<   "     "  <<  it.first.compName  );
  } */

  using boost::format;
  using boost::io::group;

  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;
  std::cout << "PerfMonMTSvc                           Serial Component Level Monitoring                            " << std::endl;
  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;
  std::cout << "PerfMonMTSvc Step             CPU Time            Wall Time           Component" << std::endl;

  for(auto& it : m_compLevelDataMap){

    std::cout <<  format("PerfMonMTSvc %|5t|%1% %|30t|%2% %|50t|%3% %|70t|%4% \n") % it.first.stepName % it.second->m_delta_cpu % it.second->m_delta_wall % it.first.compName;

    //std::cout <<  format("PerfMonMTSvc %1% %|30t|%2% %|50t|%3% \n") % it.first.stepName % it.second->m_delta_cpu % it.second->m_delta_wall;

    //ATH_MSG_INFO(format("%1%, %|10t|%2%, %|10t|%3%, %|10t|%4% ") % it.first.stepName % it.second->m_delta_cpu % it.second->m_delta_wall % it.first.compName);

  }
 
  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;
  //ATH_MSG_INFO("=========================================================");
  
}
void PerfMonMTSvc::report2Stdout_Parallel(){

  parallelDataAggregator();

  using boost::format;
  using boost::io::group;

  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;
  std::cout << "PerfMonMTSvc                           Aggregated Event Loop Monitoring                             " << std::endl;
  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;
  std::cout << "PerfMonMTSvc Step             CPU Time            Wall Time           Component" << std::endl;
              // "PerfMonMTSvc Execute          1.57695             4                   BeginIncFiringAlg"
  

  for(auto& it : m_aggParallelCompLevelDataMap){

    std::cout <<  format("PerfMonMTSvc %|5t|%1% %|30t|%2% %|50t|%3% %|70t|%4% \n") % it.first.stepName % it.second.cpu_time % it.second.wall_time % it.first.compName;
    
  }
  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;

/*
  ATH_MSG_INFO("=========================================================");

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("               Aggregated Event Loop Monitoring                ");
  ATH_MSG_INFO("=========================================================");

  for(auto& it : m_aggParallelCompLevelDataMap){

    ATH_MSG_INFO( it.first.stepName << ": " <<  it.second.cpu_time << "  -  "  << it.second.wall_time <<   "     "  <<  it.first.compName  );
    
  }

  ATH_MSG_INFO("=========================================================");
*/
}

void PerfMonMTSvc::report2Stdout_CpuInfo(){

  using boost::format;
  using boost::io::group;


  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;
  std::cout << "PerfMonMTSvc                                   System Information                                   " << std::endl;
  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;


  std::cout << format( "PerfMonMTSvc %|5t|%1% %|59t|%2% \n") % "CPU Model:" % get_cpu_model_info();  
  std::cout << format( "PerfMonMTSvc %|5t|%1% %|60t|%2% \n") % "Number of Logical Cores:" % get_cpu_core_info();

  std::cout << "PerfMonMTSvc =======================================================================================" << std::endl;

/*
  ATH_MSG_INFO("=========================================================");

  ATH_MSG_INFO("=========================================================");
  ATH_MSG_INFO("                  System Information                     ");
  ATH_MSG_INFO("=========================================================");

  ATH_MSG_INFO("CPU Model:" << get_cpu_model_info());
  ATH_MSG_INFO("Number of Logical Cores:" << get_cpu_core_info());

  ATH_MSG_INFO("=========================================================");
*/
}
