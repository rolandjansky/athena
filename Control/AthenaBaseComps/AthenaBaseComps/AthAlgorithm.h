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
#include <type_traits>

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
#include "StoreGate/VarHandleKeyProperty.h"
#include "StoreGate/VarHandleKeyArrayProperty.h"
#include "StoreGate/VarHandleKey.h"
#include "StoreGate/VarHandleKeyArray.h"
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
    AthAlgorithm* aa = const_cast<AthAlgorithm*>(this);
    Gaudi::DataHandle::Mode mode = hndl.mode();
    if (mode & Gaudi::DataHandle::Reader)
      aa->declareInput(&hndl);
    if (mode & Gaudi::DataHandle::Writer)
      aa->declareOutput(&hndl);
    hndl.setOwner(aa);

    return Algorithm::declareProperty(name,hndl,doc);
  }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  Property* declareProperty(const std::string& name,
                            SG::VarHandleKeyArray& hndArr,
                            const std::string& doc,
                            std::false_type,
                            std::true_type) const
  {

    // std::ostringstream ost;
    // ost << Algorithm::name() << " VHKA declareProp: " << name 
    //     << " size: " << hndArr.keys().size() 
    //     << " mode: " << hndArr.mode() 
    //     << "  vhka size: " << m_vhka.size()
    //     << "\n";
    // debug() << ost.str() << endmsg;

    AthAlgorithm* aa = const_cast<AthAlgorithm*>(this);

    m_vhka.push_back(&hndArr);

    Property* p =  Algorithm::declareProperty(name, hndArr, doc);
    if (p != 0) {
      p->declareUpdateHandler(&AthAlgorithm::updateVHKA, aa);
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
                            std::false_type
                            ) const
  {
    return Algorithm::declareProperty(name, property, doc);
  }


  /**
   * @brief Declare a new Gaudi property.
   * @param name Name of the property.
   * @param property Object holding the property value.
   * @param doc Documenation string for the property.
   *
   * This dispatches to either the generic @c declareProperty or the one
   * for VarHandle/Key, depending on whether or not @c property
   * derives from @c SG::VarHandleKey or @c SG::VarHandleKeyArray.
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
