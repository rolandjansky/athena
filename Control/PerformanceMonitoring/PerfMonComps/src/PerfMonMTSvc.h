/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch 
 */

#ifndef PERFMONCOMPS_PERFMONMTSVC_H
#define PERFMONCOMPS_PERFMONMTSVC_H

// Framework includes
#include "AthenaBaseComps/AthService.h"

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"

// PerfMonComps includes
#include "PerfMonMTUtils.h"

// Containers
#include <set>

// Input/Output includes
#include <fstream>
#include <iomanip>
#include "boost/format.hpp"
#include <nlohmann/json.hpp>

// Other Libraries
#include <algorithm>
#include <functional>
#include <cmath>



/*
 * In the snapshot level monitoring, currently we monitor 3 steps as a whole:
 * Initialize, Event Loop and Finalize
 */ 
#define SNAPSHOT_NUM 3

class PerfMonMTSvc : virtual public IPerfMonMTSvc,
                     public AthService
{

  public:

    /// Standard Gaudi Service constructor
    PerfMonMTSvc( const std::string& name, ISvcLocator* pSvcLocator );

    // Destructor
    ~PerfMonMTSvc();

    /// Function declaring the interface(s) implemented by the service
    virtual StatusCode queryInterface( const InterfaceID& riid,
                                       void** ppvInterface ) override;

    /// Standard Gaudi Service initialization
    virtual StatusCode initialize() override;

    /// Standard Gaudi Service finalization
    virtual StatusCode finalize() override;

    /// Start Auditing
    virtual void startAud( const std::string& stepName,
                           const std::string& compName ) override;

    /// Stop Auditing
    virtual void stopAud ( const std::string& stepName,
                           const std::string& compName ) override;

    /// Count the number of processed events
    void incrementEventCounter();

    // Do event level monitoring
    virtual void eventLevelMon() override; 
    
    /// Snapshot Auditing: Take snapshots at the beginning and at the end of each step
    void startSnapshotAud ( const std::string& stepName,
                            const std::string& compName );

    void stopSnapshotAud ( const std::string& stepName,
                           const std::string& compName );

    /// Component Level Auditing in Serial Steps
    void startCompAud_serial ( const std::string& stepName,
                               const std::string& compName );

    void stopCompAud_serial ( const std::string& stepName,
                              const std::string& compName );

    void startCompAud_MT(const std::string& stepName,
                                       const std::string& compName);
    void stopCompAud_MT(const std::string& stepName,
                                      const std::string& compName);


    // Report the results
    void report();

    // Report to stdout    
    void report2Log();
    void report2Log_Description() const;
    void report2Log_Time_Serial();
    void report2Log_Mem_Serial();
    void report2Log_EventLevel();
    void report2Log_CompLevel_Time_Parallel();
    void report2Log_Summary();  // make it const
    void report2Log_CpuInfo() const;

    // Report to the JSON File
    void report2JsonFile();
    void report2JsonFile_Summary(nlohmann::json& j) const;
    void report2JsonFile_Time_Serial(nlohmann::json& j) const;
    void report2JsonFile_EventLevel_Time_Parallel(nlohmann::json& j) const;
    void report2JsonFile_CompLevel_Time_Parallel(nlohmann::json& j) const;
    void report2JsonFile_Mem_Serial(nlohmann::json& j) const;
    void report2JsonFile_EventLevel_Mem_Parallel(nlohmann::json& j);

    EventIDBase::event_number_t getEventID() const;
    
    bool isPower(int input, int base); // check if input is power of base or not
    bool isLoop() const; // Returns true if the execution is at the event loop, false o/w.

    void divideData2Steps_serial();
    void divideData2Steps_parallel();     

    void parallelDataAggregator();

    std::string scaleTime(double timeMeas);
    std::string scaleMem(long memMeas);

    bool isCheckPoint();
 
    std::string get_cpu_model_info() const;
    int get_cpu_core_info() const;

    PMonMT::StepComp generate_serial_state( const std::string& stepName,
                                            const std::string& compName) const;

    PMonMT::StepCompEvent generate_parallel_state( const std::string& stepName,
                                                                 const std::string& compName,
                                                                 const uint64_t& eventNumber) const;


    
  private:

    PMonMT::Measurement m_peaks;

    /// Measurement to capture the CPU time
    PMonMT::Measurement m_measurement;

    /// Do event loop monitoring
    BooleanProperty m_doEventLoopMonitoring;

    /// Print detailed tables
    BooleanProperty m_printDetailedTables;


    Gaudi::Property<int> m_nThreads {this, "nThreads", 0, "Number of threads which is given as argument"};

    Gaudi::Property< std::string > m_checkPointType { this, "checkPointType", "Arithmetic", "Type of the check point sequence: Arithmetic(0, k, 2k...) or Geometric(0,k,k^2...)" };
    Gaudi::Property<int> m_checkPointFactor {this, "checkPointFactor", 10, "Common difference if check point sequence is arithmetic, Common ratio if it is Geometric"};


    // An array to store snapshot measurements: Init - EvtLoop - Fin
    PMonMT::MeasurementData m_snapshotData[SNAPSHOT_NUM];

    // TODO: It gives error when defining this variable as a class member. Fix it.
    //const static std::string m_snapshotStepNames[3];
    std::vector< std::string > m_snapshotStepNames;
  
    // Store event level measurements
    PMonMT::MeasurementData m_eventLevelData;

    // Lock for capturing event loop measurements
    std::mutex m_mutex_capture; 

    // Count the number of events processed 
    std::atomic<unsigned long long> m_eventCounter;

    /* Data structure  to store component level measurements
     * We use pointer to the MeasurementData, because we use new keyword while creating them. Clear!
     */
    std::map < PMonMT::StepComp , PMonMT::MeasurementData* > m_compLevelDataMap;

    // m_compLevelDataMap is divided into following maps and these are stored in the m_stdoutVec_serial.
    // There should be a more clever way!
    std::map < PMonMT::StepComp , PMonMT::MeasurementData* > m_compLevelDataMap_ini;
    std::map < PMonMT::StepComp , PMonMT::MeasurementData* > m_compLevelDataMap_start;
    std::map < PMonMT::StepComp , PMonMT::MeasurementData* > m_compLevelDataMap_stop;
    std::map < PMonMT::StepComp , PMonMT::MeasurementData* > m_compLevelDataMap_fin;
    std::map < PMonMT::StepComp , PMonMT::MeasurementData* > m_compLevelDataMap_plp; // preLoadProxy
    std::map < PMonMT::StepComp , PMonMT::MeasurementData* > m_compLevelDataMap_cbk; // callback
    
    std::vector<std::map < PMonMT::StepComp , PMonMT::MeasurementData* > > m_stdoutVec_serial;  

    //
    // Comp level measurements inside event loop
    PMonMT::MeasurementData m_parallelCompLevelData;

    std::map< PMonMT::StepComp, PMonMT::Measurement > m_aggParallelCompLevelDataMap;
    
    // m_aggParallelCompLevelDataMap is divided into following maps and these are stored in the m_stdoutVec_parallel.
    // There should be a more clever way!
    std::map< PMonMT::StepComp, PMonMT::Measurement > m_aggParallelCompLevelDataMap_evt;
    std::map< PMonMT::StepComp, PMonMT::Measurement > m_aggParallelCompLevelDataMap_stop;
    std::map< PMonMT::StepComp, PMonMT::Measurement > m_aggParallelCompLevelDataMap_plp;
    std::map< PMonMT::StepComp, PMonMT::Measurement > m_aggParallelCompLevelDataMap_cbk;

    std::vector<std::map < PMonMT::StepComp , PMonMT::Measurement> > m_stdoutVec_parallel;



}; // class PerfMonMTSvc

#endif // PERFMONCOMPS_PERFMONMTSVC_H
