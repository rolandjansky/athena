///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ValgrindSvc.h 
// Header file for class ValgrindSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_VALGRINDSVC_H 
#define ATHENASERVICES_VALGRINDSVC_H 1

// STL includes
#include <string>
#include <iosfwd>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

// AthenaKernel includes
#include "AthenaKernel/IValgrindSvc.h"

// Forward declaration
class ISvcLocator;
class IAuditorSvc;

template <class TYPE> class SvcFactory;

class ValgrindSvc : virtual public IValgrindSvc,
                    virtual public IIncidentListener,
                    public AthService
{ 

protected:
    
  friend class SvcFactory<ValgrindSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ValgrindSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ValgrindSvc(); 

  /// Gaudi Service Implementation
  //@{
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /** incident service handle for Begin/EndEvent
   */
  void handle( const Incident& incident );

  /// Start callgrind instrumentation
  virtual void callgrindStartInstrumentation();

  /// Stop callgrind instrumentation
  virtual void callgrindStopInstrumentation();

  /// Dump callgrind profiling stats
  virtual void callgrindDumpStats( std::ostream& out );

  /// Toggle callgrind event collection
  virtual void callgrindToggleCollect();
  
  /// Do a leak check now
  virtual void valgrindDoLeakCheck();
  
  /// Number of created callgrind profiles
  virtual unsigned int profileCount() { return m_profileCounter; }
  
  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /** helper method to create auditors
   */
  StatusCode makeAuditor (const std::string& audName, IAuditorSvc* audSvc);

    
  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  ValgrindSvc();

  /// List of algorithms to profile
  /// If list is empty, profile between begin/end event
  std::vector<std::string> m_algs;

  /// List of auditor intervals to profile
  /// Syntax: "MessageSvc.initialize:MessageSvc.finalize"
  std::vector<std::string> m_intervals;
  
  /// Dump separate profile after each event
  bool m_dumpAfterEachEvent;

  /// Dump separate profile after each interval
  bool m_dumpAfterEachInterval;

  /// Don't profile on the first N events
  unsigned int m_ignoreFirstNEvents;

  /// List of incidents on which to create a profile dump
  std::vector<std::string> m_dumpAfterIncident;
  
  /// Internal event counter for BeginEvent incident
  unsigned int m_eventCounter;

  /// Counter of created profiles
  unsigned int m_profileCounter;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& ValgrindSvc::interfaceID() 
{ 
   return IValgrindSvc::interfaceID(); 
}

#endif //> ATHENASERVICES_VALGRINDSVC_H
