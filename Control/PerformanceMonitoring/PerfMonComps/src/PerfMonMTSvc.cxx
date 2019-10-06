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

    /*
    if( isLoop() &&  m_isEventLoopMonitoring == true)
      //stopCompAud_MT(stepName, compName);
    else
      stopCompAud_serial(stepName, compName);
    */

    if(!( isLoop() &&  m_isEventLoopMonitoring == true))
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


  std::lock_guard<std::mutex> lock( m_mutex_capture );

  /*
    eventID is the event id of the event. It doesnt have to be start from 0 and increment accordingly.
    eventCount is the number of events which started to execute so far. It starts from 0 and inrement accordingly.
  */
  int eventID = getEventID();
  int eventCount = eventCounter(eventID);

  if(isCheckPoint(eventID, eventCount)){
    m_measurement.capture_MT( eventCount ); 
    m_eventLevelData.record_MT(m_measurement, eventCount);
  }
}

bool PerfMonMTSvc::isCheckPoint(int eventID, int eventCount){
  // Since we are capturing the beginning of 0th, kth, 2kth... event, we check if it's beginning or not with the following line.
  bool isBeginning = (m_eventIDsSeenSoFar.find(eventID) == m_eventIDsSeenSoFar.end());
  m_eventIDsSeenSoFar.insert(eventID); // not clear
  return ( (eventCount % m_nThreads == 0) && (isBeginning) ) ? true : false;
}

// Report the results
void PerfMonMTSvc::report() {

  report2Stdout();
  report2JsonFile();

}


void PerfMonMTSvc::report2Stdout() { 

  report2Stdout_Description();

  report2Stdout_Time_Serial();
  
  // If Event Loop Monitoring option is ON
  if(m_isEventLoopMonitoring)
    report2Stdout_Time_Parallel();

  if(isDirectoryExist("/proc")){
    report2Stdout_Mem_Serial();
    if(m_isEventLoopMonitoring)
      report2Stdout_Mem_Parallel();
  }    
  else{
    ATH_MSG_INFO("There is no /proc/ directory in this machine, therefore memory monitoring is failed!");
  }


  report2Stdout_Summary();
  report2Stdout_CpuInfo();
}

void PerfMonMTSvc::report2Stdout_Description() const {
 
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("                                 PerfMonMTSvc Report                                   ");
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("*** Important information is presented below ******************************************");
  ATH_MSG_INFO("*** Full output is inside: PerfMonMTSvc_result.json ***********************************");
  ATH_MSG_INFO("*** In order to make plots out of results run the following commands:");
  ATH_MSG_INFO("*** $ get_files PerfMonMTSvc_plotter.py");
  ATH_MSG_INFO("*** $ python PerfMonMTSvc_plotter PerfMonMTSvc_result.json");
  ATH_MSG_INFO("=======================================================================================");

}

void PerfMonMTSvc::report2Stdout_Time_Serial() { 

  using boost::format;

  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("                             CPU & Wall Time Monitoring                                ");
  ATH_MSG_INFO("                                   (Serial Steps)                                      ");
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("Step             CPU Time [ms]       Wall Time [ms]      Component");

  divideData2Steps_serial(); 
  for(auto vec_itr : m_stdoutVec_serial){
    // Sort the results
    std::vector<std::pair<PMonMT::StepComp , PMonMT::MeasurementData*>> pairs;
    for (auto itr = vec_itr.begin(); itr != vec_itr.end(); ++itr)
      pairs.push_back(*itr);

    sort(pairs.begin(), pairs.end(), [=](std::pair<PMonMT::StepComp , PMonMT::MeasurementData*>& a, std::pair<PMonMT::StepComp , PMonMT::MeasurementData*>& b)
    {
      return a.second->m_delta_cpu + a.second->m_delta_wall > b.second->m_delta_cpu + b.second->m_delta_wall;
    }
    ); 
    for(auto it : pairs){

      ATH_MSG_INFO(format("%1% %|17t|%2% %|37t|%3% %|57t|%4%") % it.first.stepName % it.second->m_delta_cpu % it.second->m_delta_wall % it.first.compName);      

    }
    ATH_MSG_INFO("=======================================================================================");

  }
  
}
void PerfMonMTSvc::report2Stdout_Time_Parallel() {

  using boost::format;
 
  ATH_MSG_INFO("                             CPU & Wall Time Monitoring                                ");
  ATH_MSG_INFO("                                    (Event Loop)                                       ");
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("Event CheckPoint             CPU Time [ms]");

  for(auto it : m_eventLevelData.m_parallel_delta_map){
    ATH_MSG_INFO(format("%1%  %|29t|%2$.2f ") % it.first % it.second.cpu_time );
  }
  ATH_MSG_INFO("=======================================================================================");

}
void PerfMonMTSvc::report2Stdout_Mem_Serial() { 

  using boost::format;

  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("                                  Memory Monitoring                                    ");
  ATH_MSG_INFO("                                   (Serial Steps)                                      ");
  ATH_MSG_INFO("                                     Units: kB                                         ");
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("Step           Vmem      Rss       Pss       Swap      Component");
                //Initialize     6144      6184      6184      0         AthDictLoaderSvc 

  
  for(auto vec_itr : m_stdoutVec_serial){
    // Sort the results
    std::vector<std::pair<PMonMT::StepComp , PMonMT::MeasurementData*>> pairs;
    for (auto itr = vec_itr.begin(); itr != vec_itr.end(); ++itr)
      pairs.push_back(*itr);

    sort(pairs.begin(), pairs.end(), [=](std::pair<PMonMT::StepComp , PMonMT::MeasurementData*>& a, std::pair<PMonMT::StepComp , PMonMT::MeasurementData*>& b)
    {
      return a.second->m_memMon_delta_map["vmem"] + a.second->m_memMon_delta_map["rss"] + a.second->m_memMon_delta_map["pss"] + a.second->m_memMon_delta_map["swap"] > \
             b.second->m_memMon_delta_map["vmem"] + b.second->m_memMon_delta_map["rss"] + b.second->m_memMon_delta_map["pss"] + b.second->m_memMon_delta_map["swap"];
    }
    ); 
    for(auto it : pairs){

      ATH_MSG_INFO(format("%1% %|15t|%2% %|25t|%3% %|35t|%4% %|45t|%5% %|55t|%6%") % it.first.stepName \
                                                                                   % it.second->m_memMon_delta_map["vmem"]    \
                                                                                   % it.second->m_memMon_delta_map["rss"]     \
                                                                                   % it.second->m_memMon_delta_map["pss"]     \
                                                                                   % it.second->m_memMon_delta_map["swap"]    \
                                                                                   % it.first.compName);      

    }
    ATH_MSG_INFO("=======================================================================================");

  }  
}

void PerfMonMTSvc::report2Stdout_Mem_Parallel(){
  
  using boost::format;
 
  ATH_MSG_INFO("                                  Memory Monitoring                                    ");
  ATH_MSG_INFO("                                    (Event Loop)                                       ");
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("Event CheckPoint           Vmem      Rss       Pss       Swap      ");


  for(auto it : m_eventLevelData.m_parallel_delta_map){
    ATH_MSG_INFO(format("%1% %|27t|%2% %|37t|%3% %|47t|%4% %|57t|%5%")             % it.first \
                                                                                   % it.second.mem_stats["vmem"]    \
                                                                                   % it.second.mem_stats["rss"]     \
                                                                                   % it.second.mem_stats["pss"]     \
                                                                                   % it.second.mem_stats["swap"]);
  }
  ATH_MSG_INFO("=======================================================================================");


  return;  
}

void PerfMonMTSvc::report2Stdout_Summary() {

  using boost::format;
 
  ATH_MSG_INFO("                              PerfMonMT Results Summary                                ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "CPU time" % "@Initialize:" % scaleTime(m_snapshotData[0].m_delta_cpu));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Wall time" % "@Initialize:" % scaleTime(m_snapshotData[0].m_delta_wall));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Virtual Memory Size" % "@Initialize:" % scaleMem(m_snapshotData[0].m_memMon_delta_map["vmem"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Resident Set Size(Rss)" % "@Initialize:" % scaleMem(m_snapshotData[0].m_memMon_delta_map["rss"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Proportional Set Size(Pss)" % "@Initialize:" % scaleMem(m_snapshotData[0].m_memMon_delta_map["pss"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Swap Size" % "@Initialize:" % scaleMem(m_snapshotData[0].m_memMon_delta_map["swap"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Effective CPU Utilization" % "@Initialize:" % (m_snapshotData[0].m_delta_cpu/m_snapshotData[0].m_delta_wall));


  ATH_MSG_INFO("");

  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "CPU time" % "@Event Loop:" % scaleTime(m_snapshotData[1].m_delta_cpu));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Wall time" % "@Event Loop:" % scaleTime(m_snapshotData[1].m_delta_wall));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Virtual Memory Size" % "@Event Loop:" % scaleMem(m_snapshotData[1].m_memMon_delta_map["vmem"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Resident Set Size(Rss)" % "@Event Loop:" % scaleMem(m_snapshotData[1].m_memMon_delta_map["rss"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Proportional Set Size(Pss)" % "@Event Loop:" % scaleMem(m_snapshotData[1].m_memMon_delta_map["pss"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Swap Size" % "@Event Loop:" % scaleMem(m_snapshotData[1].m_memMon_delta_map["swap"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Effective CPU Utilization" % "@Event Loop:" % (m_snapshotData[1].m_delta_cpu/m_snapshotData[1].m_delta_wall));

  ATH_MSG_INFO("");

  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "CPU time" % "@Finalize:" % scaleTime(m_snapshotData[2].m_delta_cpu));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Wall time" % "@Finalize:" % scaleTime(m_snapshotData[2].m_delta_wall));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Virtual Memory Size" % "@Finalize:" % scaleMem(m_snapshotData[2].m_memMon_delta_map["vmem"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Resident Set Size(Rss)" % "@Finalize:" % scaleMem(m_snapshotData[2].m_memMon_delta_map["rss"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Proportional Set Size(Pss)" % "@Finalize:" % scaleMem(m_snapshotData[2].m_memMon_delta_map["pss"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Swap Size" % "@Finalize:" % scaleMem(m_snapshotData[2].m_memMon_delta_map["swap"]));
  ATH_MSG_INFO(format( "%1% %|30t|%2% %|55t|%3% ") % "Effective CPU Utilization" % "@Finalize:" % (m_snapshotData[2].m_delta_cpu/m_snapshotData[2].m_delta_wall));

  ATH_MSG_INFO("");

  ATH_MSG_INFO(format( "%1% %|30t|%2% ") % "Max Vmem: " % scaleMem(m_measurement.vmemPeak));
  ATH_MSG_INFO(format( "%1% %|30t|%2% ") % "Max Rss: " % scaleMem(m_measurement.rssPeak));
  ATH_MSG_INFO(format( "%1% %|30t|%2% ") % "Max Pss: " % scaleMem(m_measurement.pssPeak));

  ATH_MSG_INFO("");

  
  ATH_MSG_INFO(format( "%1% %|55t|%2% ") % "Number of Events processed:" %  m_eventIDsSeenSoFar.size());
  ATH_MSG_INFO(format( "%1% %|55t|%2$.2f ms ") % "CPU Usage per Event:" %  (m_snapshotData[1].m_delta_cpu / m_eventIDsSeenSoFar.size()));
  ATH_MSG_INFO(format( "%1% %|55t|%2% ") % "Events per second:" %  (m_eventIDsSeenSoFar.size() / m_snapshotData[1].m_delta_wall  ));

  ATH_MSG_INFO(format( "%1% %|55t|%2% ") % "Number of Threads in the air:" %  (int) m_nThreads);

  ATH_MSG_INFO("=======================================================================================");

}

void PerfMonMTSvc::report2Stdout_CpuInfo() const {

  using boost::format;

  ATH_MSG_INFO("                                  System Information                                   ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format( "%1% %|54t|%2% ") % "CPU Model:" % get_cpu_model_info());
  ATH_MSG_INFO(format( "%1% %|55t|%2% ") % "Number of Logical Cores:" % get_cpu_core_info());

  ATH_MSG_INFO("=======================================================================================");
}


void PerfMonMTSvc::report2JsonFile() {

  json j;

  report2JsonFile_Summary(j);
  report2JsonFile_Time_Serial(j);

  if(isDirectoryExist("/proc")){
    report2JsonFile_Mem_Serial(j);
    //if(m_isEventLoopMonitoring)
      //report2JsonFile_Mem_Parallel(j);
  }
  
  //if(m_isEventLoopMonitoring)
    //report2JsonFile_Time_Parallel(j);
 

  std::ofstream o("PerfMonMTSvc_result.json");
  o << std::setw(4) << j << std::endl;

}

void PerfMonMTSvc::report2JsonFile_Summary(nlohmann::json& j) const {

  // Report snapshot level results
  for(int i = 0; i < 3; i++ ){

    // Clean this part!
    double wall_time = m_snapshotData[i].m_delta_wall;
    double cpu_time =  m_snapshotData[i].m_delta_cpu;

    j["Snapshot_level"][m_snapshotStepNames[i]] = { {"cpu_time", cpu_time}, {"wall_time", wall_time} };

  }

}
void PerfMonMTSvc::report2JsonFile_Time_Serial(nlohmann::json& j) const {

  // Report component level time measurements in serial steps
  for(auto& it : m_compLevelDataMap){

    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;
    
    double wall_time = it.second->m_delta_wall;
    double cpu_time = it.second->m_delta_cpu;
    
    // nlohmann::json syntax
    j["TimeMon_Serial"][stepName][compName] =  { {"cpu_time", cpu_time}, {"wall_time", wall_time} } ; 

  }
}
/*
void PerfMonMTSvc::report2JsonFile_Time_Parallel(nlohmann::json& j) const {

  // Report component level time measurements in parallel steps
  for(auto& it : m_aggParallelCompLevelDataMap){

    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;

    double wall_time = it.second.wall_time;
    double cpu_time = it.second.cpu_time;

    j["TimeMon_Parallel"][stepName][compName] = { {"cpu_time", cpu_time}, {"wall_time", wall_time} } ; 

  }
}
*/
void PerfMonMTSvc::report2JsonFile_Mem_Serial(nlohmann::json& j) const{
  
  // Report component level memory measurements in serial steps
  for(auto& it : m_compLevelDataMap){

    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;
    
    long vmem = it.second->m_memMon_delta_map["vmem"];
    long rss = it.second->m_memMon_delta_map["rss"];
    long pss = it.second->m_memMon_delta_map["pss"];
    long swap = it.second->m_memMon_delta_map["swap"];
    
    // nlohmann::json syntax
    j["MemMon_Serial"][stepName][compName] =  { {"vmem", vmem}, {"rss", rss}, {"pss", pss}, {"swap", swap} } ; 

    // Free the dynamically allocated space
    delete it.second;
  }
}
/*
void PerfMonMTSvc::report2JsonFile_Mem_Parallel(nlohmann::json& j){
  
  // Report component level memory measurements in parallel steps
  for(auto& it : m_aggParallelCompLevelDataMap){

    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;

    long vmem = it.second.mem_stats["vmem"];
    long rss = it.second.mem_stats["rss"];
    long pss = it.second.mem_stats["pss"];
    long swap = it.second.mem_stats["swap"];

    j["MemMon_Parallel"][stepName][compName] = { {"vmem", vmem}, {"rss", rss}, {"pss", pss}, {"swap", swap} } ; 

  }
}
*/
// const?
bool PerfMonMTSvc::isLoop() {
  
  int eventID = getEventID();
  return (eventID >= 0) ? true : false;
  
  /*
  m_eventId = getEventID(); // data race?
  if(m_eventId >= 0){
    eventCounter(m_eventId);
    return true;
  }
  else{
    return false;
  }
  */
}

int PerfMonMTSvc::getEventID() const {

  auto ctx = Gaudi::Hive::currentContext();
  int eventID = ctx.eventID().event_number();
  return eventID;
}

PMonMT::StepComp PerfMonMTSvc::generate_serial_state( const std::string& stepName,
                                                      const std::string& compName) const {

  PMonMT::StepComp currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
  return currentState;
}

void PerfMonMTSvc::divideData2Steps_serial(){
  for(auto it : m_compLevelDataMap){

    if(it.first.stepName == "Initialize")
      m_compLevelDataMap_ini[it.first] = it.second;
    if(it.first.stepName == "Start")
      m_compLevelDataMap_start[it.first] = it.second;
    if(it.first.stepName == "Stop")
      m_compLevelDataMap_stop[it.first] = it.second;
    if(it.first.stepName == "Finalize")
      m_compLevelDataMap_fin[it.first] = it.second; 
    if(it.first.stepName == "preLoadProxy")
      m_compLevelDataMap_plp[it.first] = it.second;
    if(it.first.stepName == "Callback")
      m_compLevelDataMap_cbk[it.first] = it.second;

  }
  m_stdoutVec_serial.push_back(m_compLevelDataMap_ini);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_start);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_stop);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_fin);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_plp);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_cbk);
}

std::string PerfMonMTSvc::scaleTime(double timeMeas){

  std::ostringstream ss;
  ss << std::fixed;
  ss << std::setprecision(2);

  double result = 0;

  std::string significance[5] = {"ms", "seconds", "mins", "hours", "days"};
  int scaleFactor = 0;

  if(timeMeas > 1000*60*60*24){
    int dayCount = timeMeas/(1000*60*60*24);
    timeMeas = std::fmod(timeMeas,(1000*60*60*24)); 
    result += dayCount;
    scaleFactor++;
  }
  if(timeMeas > 1000*60*60){
    int hourCount = timeMeas/(1000*60*60);
    timeMeas = std::fmod(timeMeas,(1000*60*60));
    result += hourCount*1.e-3;
    scaleFactor++;
  }
  if(timeMeas > 1000*60){
    int minCount = timeMeas/(1000*60);
    timeMeas = std::fmod(timeMeas,(1000*60));
    result += minCount*1.e-6;
    scaleFactor++;
  }
  if(timeMeas > 1000){
    int secCount = timeMeas/1000;
    timeMeas = std::fmod(timeMeas,1000);
    result += secCount*1.e-9;
    scaleFactor++;
  }
  if(timeMeas >= 0){
    result += timeMeas*1.e-12;
    scaleFactor++;
  }
  result = result * std::pow(1000,(5 - scaleFactor));

  ss << result;
  std::string stringObj = ss.str() + " " + significance[scaleFactor - 1];

  return stringObj;


}

std::string PerfMonMTSvc::scaleMem(long memMeas){

  std::ostringstream ss;
  ss << std::fixed;
  ss << std::setprecision(2);

  double result = 0;

  std::string significance[4] = {"KB", "MB", "GB", "TB"};
  int scaleFactor = 0;

  if(memMeas > 1024*1024*1024){
    int teraCount = memMeas/(1024*1024*1024);
    memMeas = memMeas%(1024*1024*1024); 
    result += teraCount;
    scaleFactor++;
  }
  if(memMeas > 1024*1024){
    int gigaCount = memMeas/(1024*1024);
    memMeas = memMeas%(1024*1024);
    result += gigaCount*(1.0/1024);
    scaleFactor++;
  }
  if(memMeas > 1024){
    int megaCount = memMeas/(1024);
    memMeas = memMeas%(1024);
    result += megaCount*(1.0/(1024*1024));
    scaleFactor++;
  }
  if(memMeas >= 0){
    result += memMeas*(1.0/(1024*1024*1024));
    scaleFactor++;
  }

  result = result * std::pow(1024,(4 - scaleFactor));

  ss << result;
  std::string stringObj = ss.str() + " " + significance[scaleFactor - 1];

  return stringObj;
}

std::string PerfMonMTSvc::get_cpu_model_info() const  {

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

int PerfMonMTSvc::get_cpu_core_info() const  {

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

int PerfMonMTSvc::eventCounter(int eventID) {
  int size = m_eventIDsSeenSoFar.size();
  return size;
}

