///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthReentrantAlgorithm.h 
// Header file for class AthReentrantAlgorithm
// Author: Charles Leggett
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHREENTRANTALGORITHM_H
#define ATHENABASECOMPS_ATHREENTRANTALGORITHM_H 1


// STL includes
#include <string>
#include <type_traits>

#include "AthenaBaseComps/AthCommonDataStore.h"
#include "AthenaBaseComps/AthCommonMsg.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaBaseComps/AthMemMacros.h"


// Framework includes
#ifndef REENTRANT_GAUDI
 #define ReEntAlgorithm Algorithm
 #include "GaudiKernel/Algorithm.h"
#else
 #include "GaudiKernel/ReEntAlgorithm.h"
#endif


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
  : public AthCommonDataStore<AthCommonMsg<ReEntAlgorithm>>
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AthReentrantAlgorithm(const std::string& name, 
                        ISvcLocator* pSvcLocator,
                        const std::string& version=PACKAGE_VERSION);

  /// Destructor: 
  virtual ~AthReentrantAlgorithm() override; 


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


  /** Specify if the algorithm is clonable
   *
   * Reentrant algorithms are clonable.
   */
  virtual bool isClonable() const override;


  /** Cardinality (Maximum number of clones that can exist)
   *  special value 0 means that algorithm is reentrant
   *
   * Override this to return 0 for reentrant algorithms.   */
  virtual unsigned int cardinality() const override;


  /**
   * @brief Return the current event context.
   *
   * Override this because the base class version won't work correctly
   * for reentrant algorithms.  (We shouldn't really be using this
   * for reentrant algorithms, but just in case.).
   */
  virtual const EventContext& getContext() const override;


  
  /**
   * @brief Return the list of extra output dependencies.
   *
   * This list is extended to include symlinks implied by inheritance
   * relations.
   */
  virtual const DataObjIDColl& extraOutputDeps() const override;


 private: 

  /// Default constructor: 
  AthReentrantAlgorithm(); //> not implemented
  AthReentrantAlgorithm (const AthReentrantAlgorithm& ); //> not implemented
  AthReentrantAlgorithm& operator= (const AthReentrantAlgorithm&); //> not implemented

  /// Extra output dependency collection, extended by AthAlgorithmDHUpdate
  /// to add symlinks.  Empty if no symlinks were found.
  DataObjIDColl m_extendedExtraObjects;

}; 

#endif //> !ATHENABASECOMPS_ATHREENTRANTALGORITHM_H
