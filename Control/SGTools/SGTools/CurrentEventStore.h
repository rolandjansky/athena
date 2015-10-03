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


class IProxyDictWithPool;


namespace SG {



/**
 * @brief Hold a pointer to the current event store.
 *
 * Athena has a notion of a `current' event store.  This is used, for example,
 * when @c ElementLinks are initialized without explictly specifying the store.
 *
 * Be aware that the current event store is thread-specific and thus
 * fetching it is relatively expensive.  It would be better to move
 * calls to store() outside of loops.
 *
 * XXX Temporarily, we don't use TSS, and the fetch function is out-of-line.
 * XXX To be cleaned up later.
 */
class CurrentEventStore
{
public:
  /// Fetch the current store.
  static IProxyDictWithPool* store();


  /// Set the current store.
  /// Returns the previous store.
  static IProxyDictWithPool* setStore (IProxyDictWithPool* store);


  /**
   * @brief Temporarily change the current store.
   *
   * The previous store will be restored when this object is destroyed.
   */
  class Push
  {
  public:
    Push (IProxyDictWithPool* store);
    ~Push();

  private:
    IProxyDictWithPool* m_oldStore;
  };


private:
  /// The current event store.
  // FIXME: Get rid of this conditional once all the changes to reduce the
  // number of default store lookups are in.
#ifdef ATHENAHIVE
  static thread_local IProxyDictWithPool* m_curStore;
#else
  static IProxyDictWithPool* m_curStore;
#endif
};


} // namespace SG


#include "SGTools/CurrentEventStore.icc"


#endif // not SGTOOLS_CURRENTEVENTSTORE_H
