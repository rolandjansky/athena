// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file SGTools/CurrentEventStore.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2015
 * @brief Hold a pointer to the current event store.
 */


#ifndef SGTOOLS_CURRENTEVENTSTORE_H
#define SGTOOLS_CURRENTEVENTSTORE_H

class IProxyDict;


namespace SG {



/**
 * @brief Hold a pointer to the current event store.
 *
 * Athena has a notion of a `current' event store.  This is used, for example,
 * when @c ElementLinks are initialized without explicitly specifying the store.
 *
 * Be aware that the current event store is thread-specific and thus
 * fetching it is relatively expensive.  It would be better to move
 * calls to store() outside of loops.
 */
class CurrentEventStore
{
public:
  /// Fetch the current store.
  static IProxyDict* store();


  /// Fetch the current store (out-of-line version).
  static IProxyDict* storeOol();


  /// Set the current store.
  /// Returns the previous store.
  static IProxyDict* setStore (IProxyDict* store);


  /**
   * @brief Temporarily change the current store.
   *
   * The previous store will be restored when this object is destroyed.
   */
  class Push
  {
  public:
    Push (IProxyDict* store);
    ~Push();

  private:
    IProxyDict* m_oldStore;
  };


private:
  /// The current event store.
  static thread_local IProxyDict* m_curStore;
};


} // namespace SG


#include "SGTools/CurrentEventStore.icc"


#endif // not SGTOOLS_CURRENTEVENTSTORE_H
