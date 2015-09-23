///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAlgTool.h 
// Header file for class AthAlgTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHALGTOOL_H 
#define ATHENABASECOMPS_ATHALGTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthMemMacros.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/UpdateHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/VarHandleProperty.h"
#include "AthenaKernel/IUserDataSvc.h"

class AthAlgTool : 
  public ::AlgTool,
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
  AthAlgTool( const std::string& type,
              const std::string& name, 
              const IInterface* parent );

  /// Destructor: 
  virtual ~AthAlgTool(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

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


#ifdef ATHENAHIVE
  /////////////////////////////////////////////////////////////////
  //
  //// For automatic registration of Handle data products
  //

public:

  using AlgTool::declareProperty;
  template <class T>
  Property* declareProperty(const std::string& name,
                            SG::ReadHandle<T>& hndl,
                            const std::string& doc="none") const {

    AthAlgTool* aa = const_cast<AthAlgTool*>(this);
    aa->declareInput(&hndl);
    hndl.setOwner(aa);

    return AlgTool::getPropertyMgr()->declareProperty(name,hndl,doc);
  }

  template <class T>
  Property* declareProperty(const std::string& name,
                            SG::WriteHandle<T>& hndl,
                            const std::string& doc="none") const {

    AthAlgTool* aa = const_cast<AthAlgTool*>(this);
    aa->declareOutput(&hndl);
    hndl.setOwner(aa);

    return AlgTool::getPropertyMgr()->declareProperty(name,hndl,doc);
  }

  template <class T>
  Property* declareProperty(const std::string& name,
                            SG::UpdateHandle<T>& hndl,
                            const std::string& doc="none") const {

    AthAlgTool* aa = const_cast<AthAlgTool*>(this);
    aa->declareInput(&hndl);
    aa->declareOutput(&hndl);
    hndl.setOwner(aa);

    // std::cout << "---> declareProperty<UpdateHandle> " << hndl.name()
    // 	         << " fullKey: " << hndl.fullKey() << std::endl;

    return AlgTool::getPropertyMgr()->declareProperty(name,hndl,doc);
  }

#endif

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
  AthAlgTool(); //> not implemented
  AthAlgTool (const AthAlgTool&); //> not implemented
  AthAlgTool& operator= (const AthAlgTool&); //> not implemented

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
ServiceHandle<StoreGateSvc>& AthAlgTool::evtStore() const 
{ return m_evtStore; }

inline
ServiceHandle<StoreGateSvc>& AthAlgTool::detStore() const 
{ return m_detStore; }

inline
ServiceHandle<IUserDataSvc>& AthAlgTool::userStore() const 
{ return m_userStore; }

#endif //> ATHENABASECOMPS_ATHALGTOOL_H
