// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaBaseComps/AthReentrantAlgorithm.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief An algorithm that can be simultaneously executed in multiple threads.
 */


#ifndef ATHENABASECOMPS_ATHREENTRANTALGORITHM_H
#define ATHENABASECOMPS_ATHREENTRANTALGORITHM_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/EventContext.h"


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
 *  StatusCode MyAlg::execute_r (const EventContext& ctx)
 *  {
 *    SG::ReadHandle<MyObj> myobj (m_rhandle, ctx);
 *    const MyObj& p = *myobj;
 *    ...
 *  }
 @endcode
 */
class AthReentrantAlgorithm
  : public AthAlgorithm
{
public:
  /// Use constructor from base class.
  using AthAlgorithm::AthAlgorithm;


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
};


#endif // not ATHENABASECOMPS_ATHREENTRANTALGORITHM_H
