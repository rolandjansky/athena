///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthService.h 
// Header file for class AthService
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHSERVICE_H 
#define ATHENABASECOMPS_ATHSERVICE_H 1

// STL includes
#include <string>

// Framework includes
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/UpdateHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/VarHandleProperty.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;

class AthService : 
  public ::Service, 
  public ::AthMessaging
{ 
 protected:
  friend class SvcFactory<AthService>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // fwd compat w/ gaudi-21
  using ::AthMessaging::msg;

  // Copy constructor: 

  /// Constructor with parameters: 
  AthService( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AthService(); 

  /// Initialize @c AthService
  virtual StatusCode sysInitialize();
  virtual StatusCode initialize();

  /// Reinitialize @c AthService
  virtual StatusCode sysReinitialize();
  virtual StatusCode reinitialize();

  /// Finalize @c AthService
  virtual StatusCode sysFinalize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

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
  AthService();
  AthService (const AthService&);
  AthService& operator= (const AthService&);

  /// need to cache output level during initialize
  int m_ol;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHENABASECOMPS_ATHSERVICE_H
