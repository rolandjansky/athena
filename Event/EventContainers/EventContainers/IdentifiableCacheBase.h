// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IdentifiableCacheBase.h 791541 2017-01-09 10:43:53Z smh $



#ifndef EVENTCONTAINERS_IDENTIFIABLECACHEBASE_H
#define EVENTCONTAINERS_IDENTIFIABLECACHEBASE_H


#include "Identifier/IdentifierHash.h"
#include <mutex>
#include <atomic>
#include "EventContainers/IDC_WriteHandleBase.h"
//abarton
//Enabling the ability to remove collections to help compatability with old code.
//This may be removed to improved thread-safety.
#define IdentifiableCacheBaseRemove

namespace EventContainers {



class IdentifiableCacheBase
{

public:
//here for access from other classes
static constexpr uintptr_t INVALIDflag = UINTPTR_MAX;
static constexpr uintptr_t ABORTEDflag = UINTPTR_MAX-1;
static constexpr size_t s_defaultBucketSize =2;

typedef std::true_type thread_safe;
typedef std::set<IdentifierHash> idset_t;

#include "EventContainers/deleter.h"

  struct IMaker
  {
    bool m_IsReEntrant = false;
    virtual ~IMaker() {}
    virtual void_unique_ptr typelessMake (IdentifierHash hash) const = 0;  // unique_ptr<T>??
  };
  
  /// Return payload if there, null if not there.
  const void* find (IdentifierHash hash) noexcept;
  /// Retrieve ptr, will wait if there is something in progress
  const void* findWait (IdentifierHash hash);

  /// Try to make payload if not there.
  const void* get (IdentifierHash hash);

  ///In a threaded situation this collection will be valid but will not container hashes later added
  std::vector<IdentifierHash> ids();
  
  std::pair<bool, const void*> add (IdentifierHash hash, const void* p) noexcept;

  // addLock is same as method above except we check for invalid state first,
  // more optimal for calling using writehandle lock method
  std::pair<bool, const void*> addLock (IdentifierHash hash, const void* p) noexcept;
  std::pair<bool, const void*> addLock (IdentifierHash hash, void_unique_ptr p) noexcept;
  std::pair<bool, const void*> add (IdentifierHash hash, void_unique_ptr p) noexcept;

  bool IMakerPresent() const { return m_maker!=nullptr; }

  /// Checks if the item is completed if it is not started it extablishes lock (returns 0),
  /// If it is started but not completed it adds to wait list (returns 1)
  /// If the item is already completed it returns 2
  /// If the item is aborted it does nothing and returns 3
  int tryLock(IdentifierHash, IDC_WriteHandleBase &, std::vector<IdentifierHash>&);

  ///Returns 1 is the item has been aborted otherwise 0
  int itemAborted(IdentifierHash);

  ///Returns 1 is the item is inprogress otherwise 0
  int itemInProgress(IdentifierHash);

  ///Halts the thread until the require hash is completed or aborted
  const void* waitFor(IdentifierHash);

  ///Create a set of hashes, updates an IDC mask as appropriate
  void createSet (const std::vector<IdentifierHash>& hashes, std::vector<bool> &mask);

#ifdef IdentifiableCacheBaseRemove
  ///Use to remove a collection from the cache, this is for backwards compatability 
  /// and should not be used in a concurrent environment
  bool remove (IdentifierHash hash);
#endif
  size_t fullSize() const { return m_vec.size(); }
  ///In a concurrent situation this number isn't necessarily perfectly synchronised with ids().size()
  size_t numberOfHashes();

protected:
  IdentifiableCacheBase (IdentifierHash maxHash, const IMaker* maker, size_t lockBucketSize);
  IdentifiableCacheBase (IdentifierHash maxHash, const IMaker* maker);
  ~IdentifiableCacheBase();
  void clear (deleter_f* deleter);
  void cleanUp(deleter_f* deleter);//Call once before destruction
  void notifyHash(IdentifierHash hash);
private:
  std::vector<std::atomic<const void*> > m_vec;
  friend class InternalOnline;
  const IMaker* m_maker;

  typedef std::mutex mutex_t;
  typedef std::scoped_lock<mutex_t> lock_t;
  typedef std::unique_lock<mutex_t> uniqueLock;
  mutex_t m_mutex;
  ///Pool of mutexes used for waiting on completion if in a concurrent environment
  std::unique_ptr<mutexPair[]> m_HoldingMutexes;
  size_t m_NMutexes;
  ///Holds the number of valid hashes in container, in concurrent use it is not guaranteed to be up to date.
  std::atomic<size_t> m_currentHashes;
};


} // namespace EventContainers


#endif // not EVENTCONTAINERS_IDENTIFIABLECACHEBASE_H
