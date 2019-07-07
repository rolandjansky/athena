/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERFMONCOMPS_PERFMONMTSVC_H
#define PERFMONCOMPS_PERFMONMTSVC_H

// Framework includes
#include "AthenaBaseComps/AthService.h"

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"

// PerfMonComps includes
#include "PerfMonComps/PerfMonMTUtils.h"

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
    void startCompLevelAud ( const std::string& stepName,
                             const std::string& compName );

    void stopCompLevelAud ( const std::string& stepName,
                            const std::string& compName );

    // Report the results
    void report();

    void report2Stdout();
    void report2JsonFile();

    // Get the IncidentSvc
    //virtual void handle ( const Incident& incident  ) override;
    
  private:

    /// Measurement to capture the CPU time
    PMonMT::Measurement m_measurement;

    /// Data to hold the measurement
    /// We use pointer to the MeasurementData, because we use new keyword while creating them. Clear!
    //std::unordered_map < PMonMT::StepCompPair , PMonMT::MeasurementData* > m_compLevelDataMap;
    std::map < PMonMT::StepCompPair , PMonMT::MeasurementData* > m_compLevelDataMap;
    
    // Clear!
    PMonMT::MeasurementData m_data[3];

}; // class PerfMonMTSvc

#endif // PERFMONCOMPS_PERFMONMTSVC_H
