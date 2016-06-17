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
#include "StoreGate/VarHandleKeyArray.h"
#include "StoreGate/VarHandleKeyArrayProperty.h"
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

private:
  // to keep track of VarHandleKeyArrays for data dep registration
  mutable std::vector<SG::VarHandleKeyArray*> m_vhka;

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
                            std::true_type,
                            std::false_type) const
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  Property* declareProperty(const std::string& name,
                            SG::VarHandleKeyArray& hndArr,
                            const std::string& doc,
                            std::false_type,
                            std::true_type) const
  {

    AthAlgTool* aa = const_cast<AthAlgTool*>(this);

    m_vhka.push_back(&hndArr);

    Property* p =  AlgTool::declareProperty(name, hndArr, doc);
    if (p != 0) {
      p->declareUpdateHandler(&AthAlgTool::updateVHKA, aa);
    } else {
      ATH_MSG_ERROR("unable to call declareProperty on VarHandleKeyArray " 
                    << name);
    }

    return p;

  }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  // since the contents of the VarHandleKeyArrays have not been read 
  // in from the configurables by the time that declareProperty is
  // executed, we must cache them and loop through them later to
  // register the data dependencies

  void updateVHKA(Property& /*p*/) {
    // debug() << "updateVHKA for property " << p.name() << " " << p.toString() 
    //         << "  size: " << m_vhka.size() << endmsg;
    for (auto &a : m_vhka) {
      Gaudi::DataHandle::Mode mode = a->mode();
      std::vector<SG::VarHandleKey*> keys = a->keys();
      for (auto k : keys) {
        if (mode & Gaudi::DataHandle::Reader)
          this->declareInput(k);
        if (mode & Gaudi::DataHandle::Writer)
          this->declareOutput(k);
        k->setOwner(this);
      }
    }
  }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
                            std::false_type,
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
   * derives from @c SG::VarHandleKey or SG::VarHandleKeyArray.
   */
  template <class T>
  Property* declareProperty(const std::string& name,
                            T& property,
                            const std::string& doc="none") const
  {
    return declareProperty (name, property, doc,
                            std::is_base_of<SG::VarHandleKey, T>(),
                            std::is_base_of<SG::VarHandleKeyArray,T>()
                            );
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
