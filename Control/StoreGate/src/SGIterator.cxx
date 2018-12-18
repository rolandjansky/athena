/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file StoreGate/src/SGIterator.cxx
 * @brief A standard conforming forward iterator over items in StoreGate.
 */


#include "StoreGate/SGIterator.h"


/**
 * @brief Default constructor.
 */
SG::detail::IteratorBase::IteratorBase()
{
}


/**
 * @brief Constructor from proxy iterator pair.
 * @param itr Starting proxy iterator.
 * @param itrEnd Ending proxy iterator.
 * @param isConst If false, take only non-const proxies.
 *
 * Will skip ahead to the first valid proxy.
 */
SG::detail::IteratorBase::IteratorBase (SG::ConstProxyIterator itr,
                                        SG::ConstProxyIterator itrEnd,
                                        bool isConst)
{
  // Save all valid proxies in reverse order.  Also take out a refcount on each.
  while (itr != itrEnd) {
    --itrEnd;
    DataProxy* dp = itrEnd->second;
    if (dp->isValid() && (isConst || !dp->isConst())) {
      m_proxies.push_back (dp);
      dp->addRef();
    }
  }
}


/**
 * @brief Copy constructor.
 */
SG::detail::IteratorBase::IteratorBase (const IteratorBase& other)
  : m_proxies (other.m_proxies)
{
  // Maintain shared ownership of elements in our range.
  this->addRef();
}


/**
 * @brief Destructor.
 */
SG::detail::IteratorBase::~IteratorBase()
{
  // Release ownership of everything in our range.
  this->release();
}


/**
 * @brief Assignment.
 */
SG::detail::IteratorBase&
SG::detail::IteratorBase::operator= (const IteratorBase& other)
{
  if (this != &other) {
    // Assign, maintaining refcounts.
    this->release();
    m_proxies = other.m_proxies;
    this->addRef();
  }
  return *this;
}


/**
 * @brief Reset state of the iterator.
 * @param itr Starting proxy iterator.
 * @param itrEnd Ending proxy iterator.
 * @param isConst Is this for a const iterator?
 *
 * Will skip ahead to the first valid proxy.
 * Returns FAILURE if the range is empty.
 */
StatusCode
SG::detail::IteratorBase::setState (SG::ConstProxyIterator itr,
                                    SG::ConstProxyIterator itrEnd,
                                    bool isConst)
{
  this->release();
  m_proxies.clear();

  while (itr != itrEnd) {
    --itrEnd;
    DataProxy* dp = itrEnd->second;
    if (dp->isValid() && (isConst || !dp->isConst())) {
      m_proxies.push_back (dp);
      dp->addRef();
    }
  }

  if (m_proxies.empty()) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Move to the next valid proxy.
 *
 * Used by the boost iterator adapter.
 */
void SG::detail::IteratorBase::increment()
{
  if (!m_proxies.empty()) {
    m_proxies.back()->release();
    m_proxies.pop_back();
  }
}


/**
 * @brief Const check: throw an exception if we're pointing at a const proxy.
 *
 * Called when dereferencing a non-const iterator.
 */
void SG::detail::IteratorBase::const_check() const
{
  // This should usually not be needed, since we check for constness
  // when initializing the iterator.  However, it is possible for a proxy
  // to be marked as const after the iterator has been formed, so keep the check.
  const SG::DataProxy* dp = this->proxy();
  if (dp->isConst()) {
    const IProxyDict* store = dp->store();
    throw SG::ExcConstObject (dp->clID(),
                              dp->name(),
                              store ? store->name() : "(unknown)");
  }
}


/**
 * @brief Get the key string with which the current object was stored.
 */
const std::string& SG::detail::IteratorBase::key() const
{
  return m_proxies.back()->name();
}
