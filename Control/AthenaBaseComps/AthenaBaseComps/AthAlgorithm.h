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

// Need to do this very early so parser for VarHandleKey picked up
#include <string>
#include "GaudiKernel/StatusCode.h"
namespace SG {
  class VarHandleKey;
  class VarHandleKeyArray;
  class VarHandleBase;
}
namespace Gaudi {
  namespace Parsers {
    StatusCode parse(SG::VarHandleKey& v, const std::string& s);
    StatusCode parse(SG::VarHandleKeyArray& v, const std::string& s);
    StatusCode parse(SG::VarHandleBase& v, const std::string& s);
  }
}


// Framework includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaBaseComps/AthMemMacros.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/HandleClassifier.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/VarHandleProperty.h"
#include "StoreGate/VarHandleKeyProperty.h"
#include "StoreGate/VarHandleKeyArrayProperty.h"
#include "StoreGate/VarHandleKey.h"
#include "StoreGate/VarHandleBase.h"
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
 *  may be used only in initialize() and afterwards (see the 
 *  Gaudi and Athena user guides).
 *
 *  @author Sebastien Binet
 *  @date   2008
 */ 

class AthAlgorithm
  : public ::Algorithm
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

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
   * @warning: deprecated. please use @c evtStore() instead
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


public:
  /////////////////////////////////////////////////////////////////
  //
  //// Enable use of Gaudi::Property<Foo> m_foo {this,"NAME",init,"doc"};
  //   style properties in AthAlgorithms
  //

  template <class T>
  Property& declareProperty(Gaudi::Property<T> &t) {
    typedef typename SG::HandleClassifier<T>::type htype;
    return AthAlgorithm::declareGaudiProperty(t, htype());
  }

private:
  /**
   * @brief specialization for handling Gaudi::Property<SG::VarHandleKey>
   *
   */
  template <class T>
  Property& declareGaudiProperty(Gaudi::Property<T> &hndl,
                                 const SG::VarHandleKeyType&)
  {
    return *AthAlgorithm::declareProperty(hndl.name(), hndl.value(), 
                                          hndl.documentation());

  }

  /**
   * @brief specialization for handling Gaudi::Property<SG::VarHandleKeyArray>
   *
   */
  template <class T>
  Property& declareGaudiProperty(Gaudi::Property<T> &hndl, 
                                 const SG::VarHandleKeyArrayType&)
  {
    return *AthAlgorithm::declareProperty(hndl.name(), hndl.value(), 
                                          hndl.documentation());

  }

  /**
   * @brief specialization for handling Gaudi::Property<SG::VarHandleBase>
   *
   */
  template <class T>
  Property& declareGaudiProperty(Gaudi::Property<T> &hndl, 
                                 const SG::VarHandleType&)
  {
    return *AthAlgorithm::declareProperty(hndl.name(), hndl.value(), 
                                          hndl.documentation());
  }


  /**
   * @brief specialization for handling everything that's not a
   * Gaudi::Property<SG::VarHandleKey> or a <SG::VarHandleKeyArray>
   *
   */
  template <class T>
  Property& declareGaudiProperty(Gaudi::Property<T> &t, const SG::NotHandleType&)
  {
    return Algorithm::declareProperty(t);
  }


  /////////////////////////////////////////////////////////////////
  //
  //// For automatic registration of Handle data products
  //

public:
  /**
   * @brief Declare a new Gaudi property.
   * @param name Name of the property.
   * @param property Object holding the property value.
   * @param doc Documentation string for the property.
   *
   * This is the version for types that derive from @c SG::VarHandleKey.
   * The property value object is put on the input and output lists as
   * appropriate; then we forward to the base class.
   */
  Property* declareProperty(const std::string& name,
                            SG::VarHandleKey& hndl,
                            const std::string& doc,
                            const SG::VarHandleKeyType&)
  {
    this->declare(hndl);
    hndl.setOwner(this);

    return Algorithm::declareProperty(name,hndl,doc);
  }


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  /**
   * @brief Declare a new Gaudi property.
   * @param name Name of the property.
   * @param property Object holding the property value.
   * @param doc Documentation string for the property.
   *
   * This is the version for types that derive from @c SG::VarHandleBase.
   * The property value object is put on the input and output lists as
   * appropriate; then we forward to the base class.
   */
  Property* declareProperty(const std::string& name,
                            SG::VarHandleBase& hndl,
                            const std::string& doc,
                            const SG::VarHandleType&)
  {
    this->declare(hndl.vhKey());
    hndl.vhKey().setOwner(this);

    return Algorithm::declareProperty(name,hndl,doc);
  }


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  Property* declareProperty(const std::string& name,
                            SG::VarHandleKeyArray& hndArr,
                            const std::string& doc,
                            const SG::VarHandleKeyArrayType&)
  {

    // std::ostringstream ost;
    // ost << Algorithm::name() << " VHKA declareProp: " << name 
    //     << " size: " << hndArr.keys().size() 
    //     << " mode: " << hndArr.mode() 
    //     << "  vhka size: " << m_vhka.size()
    //     << "\n";
    // debug() << ost.str() << endmsg;

    m_vhka.push_back(&hndArr);

    Property* p =  Algorithm::declareProperty(name, hndArr, doc);
    if (p != 0) {
      p->declareUpdateHandler(&AthAlgorithm::updateVHKA, this);
    } else {
      ATH_MSG_ERROR("unable to call declareProperty on VarHandleKeyArray " 
                    << name);
    }

    return p;

  }


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  // Since the contents of the VarHandleKeyArrays have not been read 
  // in from the configurables by the time that declareProperty is
  // executed, we must cache them and loop through them later to
  // register the data dependencies.
  //
  // However, we cannot actually call declare() on the key instances
  // until we know that the vector cannot change size anymore --- otherwise,
  // the pointers given to declare() may become invalid.  That basically means
  // that we can't call declare() until the derived class's initialize()
  // completes.  So instead of doing it here (which would be too early),
  // we override sysInitialize() and do it at the end of that.  But,
  // Algorithm::sysInitialize() wants to have the handle lists after initialize()
  // completes in order to do dependency analysis.  It gets these lists
  // solely by calling inputHandles() and outputHandles(), so we can get this
  // to work by overriding those methods and adding in the current contents
  // of the arrays.

  void updateVHKA(Property& /*p*/) {
    // debug() << "updateVHKA for property " << p.name() << " " << p.toString() 
    //         << "  size: " << m_vhka.size() << endmsg;
    for (auto &a : m_vhka) {
      std::vector<SG::VarHandleKey*> keys = a->keys();
      for (auto k : keys) {
        k->setOwner(this);
      }
    }
  }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


  /**
   * @brief Declare a new Gaudi property.
   * @param name Name of the property.
   * @param property Object holding the property value.
   * @param doc Documentation string for the property.
   *
   * This is the generic version, for types that do not derive
   * from  @c SG::VarHandleKey.  It just forwards to the base class version
   * of @c declareProperty.
   */
  template <class T>
  Property* declareProperty(const std::string& name,
                            T& property,
                            const std::string& doc,
                            const SG::NotHandleType&)
  {
    return Algorithm::declareProperty(name, property, doc);
  }


  /**
   * @brief Declare a new Gaudi property.
   * @param name Name of the property.
   * @param property Object holding the property value.
   * @param doc Documentation string for the property.
   *
   * This dispatches to either the generic @c declareProperty or the one
   * for VarHandle/Key/KeyArray.
   */
  template <class T>
  Property* declareProperty(const std::string& name,
                            T& property,
                            const std::string& doc="none") 
  {
    typedef typename SG::HandleClassifier<T>::type htype;
    return declareProperty (name, property, doc, htype());
  }


  /**
   * @brief Perform system initialization for an algorithm.
   *
   * We override this to declare all the elements of handle key arrays
   * at the end of initialization.
   * See comments on updateVHKA.
   */
  virtual StatusCode sysInitialize() override;


  /**
   * @brief Return this algorithm's input handles.
   *
   * We override this to include handle instances from key arrays
   * if they have not yet been declared.
   * See comments on updateVHKA.
   */
  virtual std::vector<Gaudi::DataHandle*> inputHandles() const override;


  /**
   * @brief Return this algorithm's output handles.
   *
   * We override this to include handle instances from key arrays
   * if they have not yet been declared.
   * See comments on updateVHKA.
   */
  virtual std::vector<Gaudi::DataHandle*> outputHandles() const override;

  
  /**
   * @brief Return the list of extra output dependencies.
   *
   * This list is extended to include symlinks implied by inheritance
   * relations.
   */
  virtual const DataObjIDColl& extraOutputDeps() const override;


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
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// callback for output level property 
  void msg_update_handler(Property& outputLevel);
  /// callback to add storeName to ExtraInputs/Outputs data deps
  void extraDeps_update_handler(Property&);

  /// remove all handles from I/O resolution
  void renounceArray( SG::VarHandleKeyArray& handlesArray );

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

  /// Extra output dependency collection, extended by AthAlgorithmDHUpdate
  /// to add symlinks.  Empty if no symlinks were found.
  DataObjIDColl m_extendedExtraObjects;

  bool m_varHandleArraysDeclared;
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
