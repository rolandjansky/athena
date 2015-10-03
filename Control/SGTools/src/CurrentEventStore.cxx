/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file SGTools/src/CurrentEventStore.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2015
 * @brief Hold a pointer to the current event store.
 */


#include "SGTools/CurrentEventStore.h"


namespace SG {


#ifdef ATHENAHIVE
thread_local IProxyDictWithPool* CurrentEventStore::m_curStore = nullptr;
#else
IProxyDictWithPool* CurrentEventStore::m_curStore = nullptr;
#endif


IProxyDictWithPool* CurrentEventStore::store()
{
  // XXX should really be inline.  Out-of-line temporarily for easier
  // profiling.
  return m_curStore;
}


/**
 * @brief Set the current store.
 * Returns the previous store.
 */
IProxyDictWithPool* CurrentEventStore::setStore (IProxyDictWithPool* store)
{
  IProxyDictWithPool* oldstore = m_curStore;
  m_curStore = store;
  return oldstore;
}


} // namespace SG
