///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAlgorithm.h 
// Header file for class AthReentrantAlgorithm
// Author: Charles Leggett
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHREENTRANTALGORITHM_H
#define ATHENABASECOMPS_ATHREENTRANTALGORITHM_H 1


// STL includes
#include <string>
#include <type_traits>

// FrameWork includes
#ifndef REENTRANT_GAUDI
 #define ReEntAlgorithm Algorithm
 #include "GaudiKernel/Algorithm.h"
#else
 #include "GaudiKernel/ReEntAlgorithm.h"
#endif

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

/**
 * @brief An algorithm that can be simultaneously executed in multiple threads.
 *
 * The default behavior of AthenaMT when an algorithm is to be run on different
 * events in multiple threads is to clone the algorithm object and use
 * a different instance for each thread.  If, however, your algorithm
 * derives from @c AthReentrantAlgorithm, then the same algorithm object may
 * be used in multiple threads without cloning.
 *
 * Rather than @c execute, a reentrant algorithm will call @c execute_r,
 * which has two differences.  First, @c execute_r is @c const.  If the
 * same object is being used in multiple threads, it should not have any
 * state which is being changed.  Any attempt to get around this with
 * @c mutable or @c const_cast should be viewed with great suspicion:
 * that may not be thread-safe.
 *
 * Second, the @c execute_r method takes an explicit event context argument.
 * This may be used to find the proper store for the event being processed
 * by the current thread.
 *
 * The typical usage will be to declare a key object as a property of the
 * algorithm and then construct a transient handle instance during @c execute_r
 * from the key and the event context.  For example:
 *
 *@code
 *  class MyAlg : public AthReentrantAlgorithm
 *  {
 *  ...
 *    SG::ReadHandleKey<MyObj> m_rhandle;
 *  };
 *
 *  MyAlg::MyAlg (const std::string& name, ISvcLocator* svcLoc)
 *    : AthReentrantAlgorithm (name, svcLoc)
 *  {
 *    declareProperty ("rhandle", m_rhandle);
 *  }
 *
 *  StatusCode MyAlg::initialize()
 *  {
 *    ATH_CHECK( m_rhandle.initialize() );
 *    return StatusCode::SUCCESS;
 *  }
 *
 *  StatusCode MyAlg::execute_r (const EventContext& ctx) const
 *  {
 *    SG::ReadHandle<MyObj> myobj (m_rhandle, ctx);
 *    const MyObj& p = *myobj;
 *    ...
 *  }
 @endcode
 */


class AthReentrantAlgorithm
  : public ::ReEntAlgorithm,
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
  AthReentrantAlgorithm(const std::string& name, 
               ISvcLocator* pSvcLocator,
               const std::string& version=PACKAGE_VERSION);

  /// Destructor: 
  virtual ~AthReentrantAlgorithm() override; 

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

#ifndef REENTRANT_GAUDI
  /**
   * @brief Standard Gaudi execute method.
   *
   * This cannot be overridden; you should override the reentrant method
   * @c execute_r instead.
   */
  virtual StatusCode execute() override final;


  /**
   * @brief Reentrant execute method.
   * @param ctx The current event context.
   *
   * Override this for your algorithm.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const = 0;
#endif


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
                            std::false_type)
  {
    this->declare(hndl);
    hndl.setOwner(this);

    return Algorithm::declareProperty(name,hndl,doc);
  }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  Property* declareProperty(const std::string& name,
                            SG::VarHandleKeyArray& hndArr,
                            const std::string& doc,
                            std::false_type,
                            std::true_type)
  {

    m_vhka.push_back(&hndArr);

    Property* p =  Algorithm::declareProperty(name, hndArr, doc);
    if (p != 0) {
      p->declareUpdateHandler(&AthReentrantAlgorithm::updateVHKA, this);
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
      std::vector<SG::VarHandleKey*> keys = a->keys();
      for (auto k : keys) {
        this->declare(*k);
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
                            ) 
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
                            const std::string& doc="none")
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
  AthReentrantAlgorithm(); //> not implemented
  AthReentrantAlgorithm (const AthReentrantAlgorithm& ); //> not implemented
  AthReentrantAlgorithm& operator= (const AthReentrantAlgorithm&); //> not implemented

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
ServiceHandle<StoreGateSvc>& AthReentrantAlgorithm::sgSvc() const 
{ return m_evtStore; }

inline
ServiceHandle<StoreGateSvc>& AthReentrantAlgorithm::evtStore() const 
{ return m_evtStore; }

inline
ServiceHandle<StoreGateSvc>& AthReentrantAlgorithm::detStore() const 
{ return m_detStore; }

inline
ServiceHandle<IUserDataSvc>& AthReentrantAlgorithm::userStore() const 
{ return m_userStore; }

#endif //> !ATHENABASECOMPS_ATHREENTRANTALGORITHM_H
