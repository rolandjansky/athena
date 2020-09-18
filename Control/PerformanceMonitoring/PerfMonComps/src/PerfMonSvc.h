///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonSvc.h 
// Header file for class PerfMonSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONCOMPS_PERFMONSVC_H 
#define PERFMONCOMPS_PERFMONSVC_H 1

// Local includes
#include "LinFitSglPass.h"

// C includes
#include <stdio.h>

// STL includes
#include <string>
#include <iosfwd>
#include <unordered_map>

// FrameWork includes
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/StatEntity.h"
#include "GaudiKernel/IIoComponent.h"
#include "AthenaBaseComps/AthService.h"

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonSvc.h"

// PerfMonEvent includes
#include "PerfMonEvent/DataModel.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;
class IChronoStatSvc;
class StoreGateSvc;
struct _object; typedef _object PyObject;

namespace PerfMon {
  struct Tuple
  {
    typedef std::unordered_map<std::string,  PerfMon::Component> CompStore_t;
    typedef std::unordered_map<std::string,PerfMon::IoContainer> IoCompStore_t;
  
    typedef std::unordered_map<std::string,  CompStore_t> CompTuple_t;
    typedef std::unordered_map<std::string,IoCompStore_t> IoCompTuple_t;

    CompTuple_t comp;
    IoCompTuple_t iocomp;
  };
} //> namespace PerfMon

namespace PMonSD { class SemiDetHelper; }

class PerfMonSvc : virtual public IPerfMonSvc,
                   virtual public IIncidentListener,
                   virtual public IIoComponent,
		           public AthService
{ 

protected:
    
  friend class SvcFactory<PerfMonSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  PerfMonSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~PerfMonSvc(); 

  /// Gaudi Service Implementation
  //@{
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /// @c IIoComponent interface
  //@{
  /** @brief callback method to reinitialize the internal state of
   *         the component for I/O purposes (e.g. upon @c fork(2))
   */
  virtual
  StatusCode io_reinit();
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// @brief return the list of components' names for a given step
  virtual
  std::vector<std::string>
  components(const std::string& stepName) const;

  /// @brief return the list of io-components' names
  virtual
  std::vector<std::string>
  io_components() const;

  /// retrieve the monitored value for the component `compName` and 
  /// for the step `stepName` (ini,evt,fin,cbk,...)
  virtual
  const PerfMon::Component*
  component(const std::string& stepName,
            const std::string& compName) const;

  /// retrieve the I/O monitored value for the I/O component `compName`
  virtual
  const PerfMon::IoContainer*
  io_component(const std::string& compName) const;

  /// retrieve the domain name for a given component `compName`
  virtual
  void
  domain(const std::string& compName, std::string& domain) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /** incident service handle for EndEvent
   */
  void handle( const Incident& incident );

  /** Declare monitoring information
      @param name Monitoring information name knwon to the external system
      @param var Monitoring Listener address
      @param desc Textual description
      @param owner Owner identifier of the monitoring information (needed to peform clean up
  */
  virtual void declareInfo( const std::string& name, 
                            const bool& var, 
                            const std::string& desc, 
                            const IInterface* owner );
  virtual void declareInfo( const std::string& name, 
                            const int& var, 
                            const std::string& desc,  
                            const IInterface* owner );
  virtual void declareInfo( const std::string& name, 
                            const long& var, 
                            const std::string& desc, 
                            const IInterface* owner );
  virtual void declareInfo( const std::string& name, 
                            const double& var, 
                            const std::string& desc, 
                            const IInterface* owner );
  virtual void declareInfo( const std::string& name, 
                            const std::string& var, 
                            const std::string& desc, 
                            const IInterface* owner );
  virtual void declareInfo( const std::string& name, 
                            const std::pair<double,double>& var, 
                            const std::string& desc, 
                            const IInterface* owner );
  virtual void declareInfo( const std::string& name, 
                            const AIDA::IBaseHistogram* var, 
                            const  std::string& desc, 
                            const IInterface* owner );
  virtual void declareInfo( const std::string& name, 
                            const std::string& format, 
                            const void * var, int size, 
                            const std::string& desc, 
                            const IInterface* owner );
  virtual void declareInfo(const std::string& name, 
                           const StatEntity& var, 
                           const std::string& desc, 
                           const IInterface* owner);

  /** Undeclare monitoring information
      @param name Monitoring information name knwon to the external system
      @param owner Owner identifier of the monitoring information
  */
  virtual void undeclareInfo( const std::string& name, 
                              const IInterface* owner );
  /** Undeclare monitoring information
      @param owner Owner identifier of the monitoring information
  */
  virtual void undeclareAll( const IInterface* owner );

  /** Get the names for all declared monitoring informations for a given
      owener. If the owner is NULL, then it returns for all owners
      informationUndeclare monitoring information
  */
  virtual std::set<std::string>* getInfos( const IInterface* owner );


  /** Start collecting monitoring data for a given component, for a given
   *  step of the Gaudi FSM
   */
  void startAud( const std::string& stepName, const std::string& compName );

  /** Stop collecting monitoring data for a given component, for a given
   *  step of the Gaudi FSM
   */
  void stopAud ( const std::string& stepName, const std::string& compName );

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// Default constructor: 
  PerfMonSvc();

  /// callback to synchronize the list of algorithms to be profiled
  void setupProfiledAlgList( Gaudi::Details::PropertyBase& profiledAlgNames );

  /// callback to synchronize the list of I/O containers to be monitored
  void setupIoContainerList( Gaudi::Details::PropertyBase& ioContainerNames );

  /// loop over user-declared variables (via the @c IMonitorSvc interface)
  /// and poll the data
  void poll();

  /** start collection monitoring cpu+mem data for a given component
   */
  void comp_startAud(const std::string& stepName, 
                     const std::string& compName);

  /** stop collection monitoring cpu+mem data for a given component
   */
  void comp_stopAud(const std::string& stepName, 
                    const std::string& compName);

  /** collect I/O monitoring data
   */
  void do_io_mon();

  void postFinalize();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// Pointer to the @c PyObject doing the real work
  PyObject* m_pySvc;

  /// handle to the chronostat service
  ServiceHandle<IChronoStatSvc> m_chrono;

  /// Maximum allowed variation of virtual memory (in bytes) between last 
  /// print-out and current vmem value, before triggering a new print-out
  DoubleProperty m_deltaVMemAlert;

  /// List of algorithms to be profiled. If empty list, all the algorithms
  /// known to the ApplicationMgr are taken.
  StringArrayProperty m_profiledAlgNames;

  /// List of containers for which one wants to gather I/O data. 
  /// If empty list, no I/O data will be collected (except the overall 
  /// 'commitOutput' timing)
  StringArrayProperty m_ioContainerNames;

  /// Name of the output file to hold performance data
  StringProperty m_outFileName;

  /// switch to enable or not the on-the-fly double-counting correction
  BooleanProperty m_enableDblCountingCorrection;

  /// level of granularity for monitoring
  int m_monLvl;

  /// Name of the shelve file containing jobo metadata
  StringProperty m_shelveFileName;

  /// Level of semi-detailed monitoring
  int m_semiDetMonLvl;

  /// (needed as a property to survive athena.py --drop-and-reload
  std::string m_jobStartJiffies;

  //Trigger tons of extra printouts, before and after each component
  bool m_extraPrintouts;

  /// tuple of data (step->component[name->perf-data])
  PerfMon::Tuple m_ntuple;

  /// first event vmem
  float m_firstVmem;

  /// 10th-event vmem
  float m_10evtVmem;

  /// last event vmem
  float m_lastVmem;

  PerfMon::LinFitSglPass m_vmemfit_evt2to20;
  PerfMon::LinFitSglPass m_vmemfit_evt21to100;
  PerfMon::LinFitSglPass m_vmemfit_evt101plus;

  /// pmon-stream data file descriptor
  int m_stream;

  /// Worker directory in athenaMP
  std::string m_workerDir;

  /// number of events processed during the monitored job
  long m_nevts;

  /// number of algorithms the monitored job ran
  std::size_t m_nalgs;

  struct Stats_t {
    StatEntity cpu;
    StatEntity cpu_user;
    StatEntity cpu_sys;
    StatEntity real;
    StatEntity vmem;
    StatEntity malloc;
    StatEntity nalloc;
    StatEntity rt;
  };
  typedef std::unordered_map<std::string, Stats_t>    StatStore_t;
  StatStore_t m_stats;

  // a struct which shadows PerfMon::Mem to correct for double counting
  // memory-related resources in possible deeply nested callgraphs 
  struct CgHandler {
    float vmem;
    float rss;
    float mall;
    float nmall;
    float nfree;

    CgHandler() :
      vmem(0.), rss(0.), mall(0.), nmall(0.), nfree(0.)
    {}
  };
  CgHandler m_corrector;

  //PMonSD:
  PMonSD::SemiDetHelper * m_pmonsd;

  bool m_pf;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& PerfMonSvc::interfaceID() 
{ 
  return IPerfMonSvc::interfaceID(); 
}

#endif //> PERFMONCOMPS_PERFMONSVC_H
