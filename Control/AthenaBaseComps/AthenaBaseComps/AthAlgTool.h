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
#include <type_traits>

// FrameWork includes
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthMemMacros.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/VarHandleProperty.h"
#include "StoreGate/VarHandleKeyProperty.h"
#include "StoreGate/VarHandleKey.h"
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


#ifndef ATHENAHIVE
protected:
  virtual void declareInput(Gaudi::DataHandle* /*im*/) {}
  virtual void declareOutput(Gaudi::DataHandle* /*im*/) {}
#endif


  /////////////////////////////////////////////////////////////////
  //
  //// For automatic registration of Handle data products
  //

public:
  /**
   * @brief Declare a new Gaudi property.
   * @param name Name of the property.
   * @param property Object holding the property value.
   * @param doc Documenation string for the property.
   *
   * This is the version for types that derive from @c SG::VarHandleKey.
   * The property value object is put on the input and output lists as
   * appropriate; then we forward to the base class.
   */
  Property* declareProperty(const std::string& name,
                            SG::VarHandleKey& hndl,
                            const std::string& doc,
                            std::true_type) const
  {
    AthAlgTool* aa = const_cast<AthAlgTool*>(this);
    Gaudi::DataHandle::Mode mode = hndl.mode();
    if (mode & Gaudi::DataHandle::Reader)
      aa->declareInput(&hndl);
    if (mode & Gaudi::DataHandle::Writer)
      aa->declareOutput(&hndl);
#ifdef ATHENAHIVE
    hndl.setOwner(aa);
#endif

    return AlgTool::declareProperty(name,hndl,doc);
  }


  /**
   * @brief Declare a new Gaudi property.
   * @param name Name of the property.
   * @param property Object holding the property value.
   * @param doc Documenation string for the property.
   *
   * This is the generic version, for types that do not derive
   * from  @c SG::VarHandleKey.  It just forwards to the base class version
   * of @c declareProperty.
   */
  template <class T>
  Property* declareProperty(const std::string& name,
                            T& property,
                            const std::string& doc,
                            std::false_type) const
  {
    return AlgTool::declareProperty(name, property, doc);
  }


  /**
   * @brief Declare a new Gaudi property.
   * @param name Name of the property.
   * @param property Object holding the property value.
   * @param doc Documenation string for the property.
   *
   * This dispatches to either the generic @c declareProperty or the one
   * for VarHandle/Key, depending on whether or not @c property
   * derives from @c SG::VarHandleKey.
   */
  template <class T>
  Property* declareProperty(const std::string& name,
                            T& property,
                            const std::string& doc="none") const
  {
    return declareProperty (name, property, doc,
                            std::is_base_of<SG::VarHandleKey, T>());
  }

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
