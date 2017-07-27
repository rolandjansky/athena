// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IdentifiableCacheBase.h 791541 2017-01-09 10:43:53Z smh $
/**
 * @file IdentifiableCacheBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief 
 */


#ifndef EVENTCONTAINERS_IDENTIFIABLECACHEBASE_H
#define EVENTCONTAINERS_IDENTIFIABLECACHEBASE_H


#include "Identifier/IdentifierHash.h"
#include <stdexcept>
#include <mutex>
#include <atomic>
#include <condition_variable>

//abarton
//Enabling the ability to remove collections to help compatability with old code.
//This may be removed to improved threadsafety.
#define IdentifiableCacheBaseRemove

namespace EventContainers {


class IdentifiableCacheBase
{
public:

typedef std::true_type thread_safe;
typedef std::set<IdentifierHash> idset_t;
#if 0
  struct deleter
  {
    void operator() (const void* p);
  };
#endif

  typedef void deleter_f (const void* p);

  class void_unique_ptr
    : public std::unique_ptr<const void, deleter_f*>
  {
  public:
    using std::unique_ptr<const void, deleter_f*>::unique_ptr;

    template <class T>
    struct Deleter
    {
      static void deleter (const void* p)
      {
        delete reinterpret_cast<const T*>(p);
      }
    };

    template <class T>
    void_unique_ptr(std::unique_ptr<T> p)
      : std::unique_ptr<const void, deleter_f*> (p.release(),
                                                 Deleter<T>::deleter)
    {
    }
  };
  
  struct IMaker
  {
    virtual ~IMaker() {}
    virtual void_unique_ptr typelessMake (IdentifierHash hash) const = 0;  // unique_ptr<T>??
  };
  
  // Return payload if there, null if not there.
  const void* find (IdentifierHash hash) const;

  // Try to make payload if not there.
  const void* get (IdentifierHash hash);

  std::vector<IdentifierHash> ids() const;
  
  bool add (IdentifierHash hash, const void* p, bool TakeOwnerShip);
  bool add (IdentifierHash hash, void_unique_ptr p);

#ifdef IdentifiableCacheBaseRemove
  bool remove (IdentifierHash hash);
#endif
  size_t fullSize() const { return m_owns.size(); }
  size_t numberOfHashes() const;
  
protected:
  IdentifiableCacheBase (IdentifierHash maxHash, const IMaker* maker);
  ~IdentifiableCacheBase();
  void clear (deleter_f* deleter);
  void cleanUp(deleter_f* deleter);//Call once before delection
  
private:
  std::vector<std::atomic<const void*> > m_vec;
  std::vector<bool> m_owns;
  
  const IMaker* m_maker;
  // Set of currently populated hashes.
  // If a hash has an entry in the set but not in the vector,
  // a conversion is in progress.

  idset_t m_ids;

  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutable mutex_t m_mutex;

  std::condition_variable m_cond;
};


} // namespace EventContainers


#endif // not EVENTCONTAINERS_IDENTIFIABLECACHEBASE_H
