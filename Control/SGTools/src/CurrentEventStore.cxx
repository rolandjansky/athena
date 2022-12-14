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


thread_local IProxyDict* CurrentEventStore::m_curStore = nullptr;


/**
 * @brief Fetch the current store (out-of-line version).
 */
IProxyDict* CurrentEventStore::storeOol()
{
  return m_curStore;
}


/**
 * @brief Set the current store.
 * Returns the previous store.
 */
IProxyDict* CurrentEventStore::setStore (IProxyDict* store)
{
  IProxyDict* oldstore = m_curStore;
  m_curStore = store;
  return oldstore;
}


} // namespace SG
