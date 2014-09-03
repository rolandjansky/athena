///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IVALGRINDSVC_H 
#define ATHENAKERNEL_IVALGRINDSVC_H 

// STL includes
#include <iosfwd>

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IService.h"

// Forward declaration

/**
 * @class IValgrindSvc
 * @brief Abstract interface for ValgrindSvc. FIXME
 * @author Sebastien Binet
 */
class IValgrindSvc : virtual public IService
{ 
 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IValgrindSvc();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /// Start callgrind instrumentation
  virtual void callgrindStartInstrumentation() = 0;

  /// Stop callgrind instrumentation
  virtual void callgrindStopInstrumentation() = 0;

  /// Dump callgrind profiling stats
  virtual void callgrindDumpStats( std::ostream& out ) = 0;

  /// Toggle callgrind event collection
  virtual void callgrindToggleCollect() = 0;

  /// Do a leak check now
  virtual void valgrindDoLeakCheck() = 0;

  /// Number of created callgrind profiles
  virtual unsigned int profileCount() = 0;
  
  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& IValgrindSvc::interfaceID() 
{ 
  static const InterfaceID IID_IValgrindSvc("IValgrindSvc", 2, 0);
  return IID_IValgrindSvc; 
}

#endif //> ATHENAKERNEL_IVALGRINDSVC_H
