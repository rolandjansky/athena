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


#include "Gaudi/Algorithm.h"


/**
 * @brief An algorithm that can be simultaneously executed in multiple threads.
 *
 * The default behavior of AthenaMT when an algorithm is to be run on different
 * events in multiple threads is to clone the algorithm object and use
 * a different instance for each thread.  If, however, your algorithm
 * derives from @c AthReentrantAlgorithm, then the same algorithm object may
 * be used in multiple threads without cloning.
 *
 * Rather than @c execute, a reentrant algorithm will call @c execute,
 * which has two differences.  First, @c execute is @c const.  If the
 * same object is being used in multiple threads, it should not have any
 * state which is being changed.  Any attempt to get around this with
 * @c mutable or @c const_cast should be viewed with great suspicion:
 * that may not be thread-safe.
 *
 * Second, the @c execute method takes an explicit event context argument.
 * This may be used to find the proper store for the event being processed
 * by the current thread.
 *
 * The typical usage will be to declare a key object as a property of the
 * algorithm and then construct a transient handle instance during @c execute
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
 *  StatusCode MyAlg::execute (const EventContext& ctx) const
 *  {
 *    SG::ReadHandle<MyObj> myobj (m_rhandle, ctx);
 *    const MyObj& p = *myobj;
 *    ...
 *  }
 @endcode
 */


class AthReentrantAlgorithm
  : public AthCommonDataStore<AthCommonMsg<Gaudi::Algorithm>>
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

  /** @brief Override sysInitialize
   *
   * Loop through all output handles, and if they're WriteCondHandles,
   * automatically register them and this Algorithm with the CondSvc
   */
  virtual StatusCode sysInitialize() override;
  

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
   * @brief Execute an algorithm.
   *
   * We override this in order to work around an issue with the Algorithm
   * base class storing the event context in a member variable that can
   * cause crashes in MT jobs.
   */
  virtual StatusCode sysExecute (const EventContext& ctx) override;

  
  /**
   * @brief Return the list of extra output dependencies.
   *
   * This list is extended to include symlinks implied by inheritance
   * relations.
   */
  virtual const DataObjIDColl& extraOutputDeps() const override;

  virtual bool filterPassed(const EventContext& ctx) const {
    return execState( ctx ).filterPassed();
  }

  virtual void setFilterPassed( bool state, const EventContext& ctx ) const {
    execState( ctx ).setFilterPassed( state );
  }


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
