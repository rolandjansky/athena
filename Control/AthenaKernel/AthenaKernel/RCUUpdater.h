// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaKernel/RCUUpdater.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief Implementation of Updater for RCUSvc.
 */


#ifndef ATHENAKERNEL_RCUUPDATER_H
#define ATHENAKERNEL_RCUUPDATER_H


#include "AthenaKernel/RCUObject.h"
#include "AthenaKernel/IRCUSvc.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <utility>
#include <memory>


namespace Athena {


/**
 * @brief Implementation of Updater for RCUSvc.
 *
 * This is an implementation of the Updater interface defined in
 * CxxUtils/ConcurrentRangeMap.h using RCUSvc.
 *
 * This uses RCUObject to manage an instance of type T.
 */
template <class T>
class RCUUpdater
{
public:
  /// Execution context type.
  typedef EventContext Context_t;


  /**
   * @brief Constructor.
   * @param rcusvc RCU service instance.
   * @param args Additional arguments to pass to the T constructor.
   *
   * Creates a new instance of T.
   */
  template <typename... Args>
  RCUUpdater (IRCUSvc& rcusvc, Args&&... args);


  /**
   * @brief Install a new object.
   * @param p The new object to install.
   * @param ctx Current execution context.
   *
   * The existing object should not be deleted until it can no longer
   * be referenced by any thread.
   */
  void update (std::unique_ptr<T> p, const Context_t& ctx);


  /**
   * @brief Return a reference to the current object.
   */
  const T& get() const;


  /**
   * @brief Mark that an event slot is not referencing this object.
   */
  void quiescent (const Context_t& ctx);


  /**
   * @brief Queue an object for later deletion.
   * @param p The object to delete.
   *
   * The object @c p will be queued for deletion once a grace period
   * has passed for all slots.  In contrast to using @c update,
   * this does not change the current object.  It also does not mark
   * the current slot as having completed the grace period (so this can
   * be called by a thread running outside of a slot context).
   */
  void discard (std::unique_ptr<T> p);


  /**
   * @brief Return the current event context.
   */
  static const Context_t& defaultContext();

  
private:
  /// The managed object.
  RCUObject<T> m_obj;
};


} // namespace Athena


#include "AthenaKernel/RCUUpdater.icc"


#endif // not ATHENAKERNEL_RCUUPDATER_H
