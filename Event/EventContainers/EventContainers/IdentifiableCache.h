// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IdentifiableCache.h 791541 2017-01-09 10:43:53Z smh $
/**
 * @file IdentifiableCache.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief 
 */


#ifndef EVENTCONTAINERS_IDENTIFIABLECACHE_H
#define EVENTCONTAINERS_IDENTIFIABLECACHE_H


#include "EventContainers/IdentifiableCacheBase.h"


namespace EventContainers {


template <class T>
class IdentifiableCache
  : public IdentifiableCacheBase
{
public:
  

  class Maker
    : public IMaker
  {
  public:
    virtual std::unique_ptr<T> make (IdentifierHash hash) const = 0;
  private:
    virtual void_unique_ptr typelessMake (IdentifierHash hash) const
    { return void_unique_ptr (make (hash)); }
  };

  IdentifiableCache (IdentifierHash maxHash, const Maker* maker)
    : IdentifiableCacheBase (maxHash, maker)
  {
  }

  IdentifiableCache (IdentifierHash maxHash, const Maker* maker, size_t lockBucketSize)
    : IdentifiableCacheBase (maxHash, maker, lockBucketSize)
  {
  }

  ~IdentifiableCache()
  {
    IdentifiableCacheBase::cleanUp (void_unique_ptr::Deleter<T>::deleter);
  }

  // Return payload if there, null if not there.
  const T* find (IdentifierHash hash)
  {
    return reinterpret_cast<const T*> (IdentifiableCacheBase::find (hash));
  }

  const T* findWait (IdentifierHash hash)
  {
    return reinterpret_cast<const T*> (IdentifiableCacheBase::findWait (hash));
  }

  const T* get (IdentifierHash hash)
  {
    return reinterpret_cast<const T*> (IdentifiableCacheBase::get (hash));
  }

  std::pair<bool, const void*> add (IdentifierHash hash, const T* p)
  {
    return IdentifiableCacheBase::add (hash, p);
  }

  std::pair<bool, const void*> add (IdentifierHash hash, std::unique_ptr<T> p)
  {
    return IdentifiableCacheBase::add (hash, void_unique_ptr(std::move(p)));
  }

  void clearCache(){
    IdentifiableCacheBase::clear (void_unique_ptr::Deleter<T>::deleter);
  }
};


} // namespace EventContainers


#endif // not EVENTCONTAINERS_IDENTIFIABLECACHE_H
