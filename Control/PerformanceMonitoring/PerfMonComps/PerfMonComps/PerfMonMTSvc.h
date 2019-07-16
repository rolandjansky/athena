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
#include "PerfMonComps/PerfMonMTUtils.h"

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

    
    /// Snapshot Auditing: Take snapshots at the beginning and at the end of each step
    void startSnapshotAud ( const std::string& stepName,
                            const std::string& compName );

    void stopSnapshotAud ( const std::string& stepName,
                           const std::string& compName );

    /// Component Level Auditing ( just for serial parts for now) 
    void startCompAud_serial ( const std::string& stepName,
                             const std::string& compName );

    void stopCompAud_serial ( const std::string& stepName,
                            const std::string& compName );

    /// Component Level Auditing ( just for serial parts for now) 
    void startCompAud_MT ( const std::string& stepName,
                             const std::string& compName );

    void stopCompAud_MT ( const std::string& stepName,
                            const std::string& compName );

    int getEventNumber();
   
    // Returns true if the execution is at the event loop, false o/w.
    bool isLoop();

    void parallelDataAggregator();

    // Report the results
    void report();

    void report2Stdout();
    void report2JsonFile();

    
  private:

    /// Measurement to capture the CPU time
    PMonMT::Measurement m_measurement;

    /* Data structure  to store component level measurements
     * We use pointer to the MeasurementData, because we use new keyword while creating them. Clear!
     */
    std::map < PMonMT::StepComp , PMonMT::MeasurementData* > m_compLevelDataMap;
    
    // An array to store snapshot measurements: Init - EvtLoop - Fin
    PMonMT::MeasurementData m_snapshotData[SNAPSHOT_NUM];

    // TODO: It gives error when defining this variable as a class member. Fix it.
    //const static std::string m_snapshotStepNames[3];
    std::vector< std::string > m_snapshotStepNames;
  

    // Comp level measurements inside event loop
    PMonMT::MeasurementData m_parallelCompLevelData;

    // get rid of pair
    std::map< PMonMT::StepComp, std::pair<double, double> > m_aggParallelCompLevelDataMap;

    std::mutex m_mutex;

}; // class PerfMonMTSvc

#endif // PERFMONCOMPS_PERFMONMTSVC_H
