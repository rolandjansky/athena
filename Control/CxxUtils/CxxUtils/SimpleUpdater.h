// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/SimpleUpdater.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2020
 * @brief Simple (non-deleting) Updater implementation.
 */


#ifndef CXXUTILS_SIMPLEUPDATER_H
#define CXXUTILS_SIMPLEUPDATER_H


#include <atomic>
#include <vector>
#include <memory>


namespace CxxUtils {


/**
 * @brief Simple (non-deleting) Updater implementation.
 *
 * This is a simple implementation of the Updater interface as described
 * in @c ConcurrentRangeMap that does not delete any memory until
 * the @c SimpleUpdater itself is deleted.  This can be useful for,
 * say, a @c ConcurrentStrMap if you have a good initial guess
 * for the size of the table.
 */
template <class T>
class SimpleUpdater
{
public:
  /// Context object.  Required by the interface, though we don't
  /// really use it.
  struct Context_t {};

  /// We need a default constructor.
  SimpleUpdater() = default;


  /**
   * @brief Move constructor.
   */
  SimpleUpdater (SimpleUpdater&& other);


  /**
   * @brief Return a reference to the current object.
   */
  const T& get() const;


  /**
   * @brief Install a new object.
   * @param p The new object to install.
   * @param ctx Current execution context.
   *
   * The existing object should not be deleted until it can no longer
   * be referenced by any thread.
   */
  void update (std::unique_ptr<T> p, const Context_t&);


  /**
   * @brief Queue an object for later deletion.
   * @param p The object to delete.
   *
   * The object @c p will be queued for deletion once a grace period
   * has passed for all slots.  In contrast to using @c update,
   * this does not change the current object.
   */
  void discard (std::unique_ptr<T> p);


  /**
   * @brief Mark that an event slot is not referencing this object.
   *
   * A no-op for @c SimpleUpdater.
   */
  void quiescent (const Context_t&);


  /**
   * @brief Return the current event context.
   */
  static const Context_t defaultContext();


private:
  /// Pointer to the current object.
  std::atomic<const T*> m_obj = 0;

  /// List of all allocated objects.
  std::vector<std::unique_ptr<T> > m_objs;
};


} // namespace CxxUtils


#include "CxxUtils/SimpleUpdater.icc"


#endif // not CXXUTILS_SIMPLEUPDATER_H
