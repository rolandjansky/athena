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
  public ::Service
{ 
 protected:
  friend class SvcFactory<AthService>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

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

  // forward to CommonMessaging
  inline MsgStream& msg() const {
    return msgStream();
  }
  inline MsgStream& msg(const MSG::Level lvl) const {
    return msgStream(lvl);
  }
  inline bool msgLvl(const MSG::Level lvl) const {
    return msgLevel(lvl);
  }

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

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  AthService();
  AthService (const AthService&);
  AthService& operator= (const AthService&);

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHENABASECOMPS_ATHSERVICE_H
