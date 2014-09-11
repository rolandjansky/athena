///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAlgorithm.h 
// Header file for class AthAlgorithm
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHALGORITHM_H
#define ATHENABASECOMPS_ATHALGORITHM_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaBaseComps/AthMemMacros.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/VarHandleProperty.h"
#include "StoreGate/RVar.h"
#include "StoreGate/RWVar.h"
#include "StoreGate/WVar.h"
#include "AthenaKernel/IUserDataSvc.h"

// Forward declaration

/** @class AthAlgorithm AthAlgorithm.h AthenaBaseComps/AthAlgorithm.h
 *
 *  Base class from which all concrete algorithm classes should 
 *  be derived. 
 *  In order for a concrete algorithm class to do anything
 *  useful the methods initialize(), execute() and finalize() 
 *  should be overridden.
 *  The base class provides utility methods for accessing 
 *  standard services (StoreGate service etc.); for declaring
 *  properties which may be configured by the job options 
 *  service; and for creating sub algorithms.
 *  The only base class functionality which may be used in the
 *  constructor of a concrete algorithm is the declaration of 
 *  member variables as properties. All other functionality, 
 *  i.e. the use of services and the creation of sub-algorithms,
 *  may be used only in initialise() and afterwards (see the 
 *  Gaudi and Athena user guides).
 *
 *  @author Sebastien Binet
 *  @date   2008
 */ 

class AthAlgorithm
  : public ::Algorithm,
    public ::AthMessaging
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // fwd compat w/ gaudi-21
  using AthMessaging::msg;

  // Copy constructor: 

  /// Constructor with parameters: 
  AthAlgorithm(const std::string& name, 
               ISvcLocator* pSvcLocator,
               const std::string& version=PACKAGE_VERSION);

  /// Destructor: 
  virtual ~AthAlgorithm(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief The standard @c StoreGateSvc
   * Returns (kind of) a pointer to the @c StoreGateSvc
   * @WARNING: deprecated. please use @c evtStore() instead
   */
  ServiceHandle<StoreGateSvc>& sgSvc() const;

  /** @brief The standard @c StoreGateSvc (event store)
   * Returns (kind of) a pointer to the @c StoreGateSvc
   */
  ServiceHandle<StoreGateSvc>& evtStore() const;

  /** @brief The standard @c StoreGateSvc/DetectorStore
   * Returns (kind of) a pointer to the @c StoreGateSvc
   */
  ServiceHandle<StoreGateSvc>& detStore() const;

  /** @brief The standard @c UserDataSvc 
   * Returns (kind of) a pointer to the @c UserDataSvc
   */
  ServiceHandle<IUserDataSvc>& userStore() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// callback for output level property 
  void msg_update_handler(Property& outputLevel);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  AthAlgorithm(); //> not implemented
  AthAlgorithm (const AthAlgorithm& ); //> not implemented
  AthAlgorithm& operator= (const AthAlgorithm&); //> not implemented

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Pointer to StoreGate (event store by default)
  mutable StoreGateSvc_t m_evtStore;

  /// Pointer to StoreGate (detector store by default)
  mutable StoreGateSvc_t m_detStore;

  typedef ServiceHandle<IUserDataSvc> UserDataSvc_t;
  /// Pointer to IUserDataSvc
  mutable UserDataSvc_t m_userStore;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline
ServiceHandle<StoreGateSvc>& AthAlgorithm::sgSvc() const 
{ return m_evtStore; }

inline
ServiceHandle<StoreGateSvc>& AthAlgorithm::evtStore() const 
{ return m_evtStore; }

inline
ServiceHandle<StoreGateSvc>& AthAlgorithm::detStore() const 
{ return m_detStore; }

inline
ServiceHandle<IUserDataSvc>& AthAlgorithm::userStore() const 
{ return m_userStore; }

#endif //> !ATHENABASECOMPS_ATHALGORITHM_H
