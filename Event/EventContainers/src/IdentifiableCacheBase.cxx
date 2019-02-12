/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IdentifiableCacheBase.cxx 791541 2017-01-09 10:43:53Z smh $
/**
 * @file IdentifiableCacheBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief 
 */

// null --- default state.
// ABORTED -- conversion failed or returned null
// ptr > null && ptr < ABORTED --- Have payload
// INVALID --- Conversion in progress or intention to add soon.

#include "EventContainers/IdentifiableCacheBase.h"


namespace EventContainers {

static const void* INVALID = reinterpret_cast<const void*>(IdentifiableCacheBase::INVALIDflag);
static const void* ABORTED = reinterpret_cast<const void*>(IdentifiableCacheBase::ABORTEDflag);

IdentifiableCacheBase::IdentifiableCacheBase (IdentifierHash maxHash,
                                              const IMaker* maker)
  : IdentifiableCacheBase(maxHash, maker, s_defaultBucketSize)
{
}

IdentifiableCacheBase::IdentifiableCacheBase (IdentifierHash maxHash,
                                              const IMaker* maker, size_t lockBucketSize)
  : m_vec(maxHash),
    m_maker (maker)
{
   m_NMutexes = lockBucketSize;
   if(m_NMutexes>0) m_HoldingMutexes = std::make_unique<mutexPair[]>(m_NMutexes);
   for(auto &h : m_vec) h.store(nullptr, std::memory_order_relaxed); //Ensure initialized to null - I'm not sure if this is implicit
   m_currentHashes.store(0, std::memory_order_relaxed);
}


IdentifiableCacheBase::~IdentifiableCacheBase()
{

}

int IdentifiableCacheBase::tryLock(IdentifierHash hash, IDC_WriteHandleBase &lock, std::vector<IdentifierHash> &wait){
   assert(m_NMutexes > 0);
   const void *ptr1 =nullptr;

   if(m_vec[hash].compare_exchange_strong(ptr1, INVALID)){//atomic swap (replaces ptr1 with value)
      //First call
      size_t slot = hash % m_NMutexes;
      auto &mutexpair = m_HoldingMutexes[slot];
      lock.LockOn(&m_vec[hash], &mutexpair);
      return 0;
   }

   if(ptr1 == INVALID){
      //Second call while not finished
#ifndef NDEBUG
      size_t slot = hash % m_NMutexes;
      m_HoldingMutexes[slot].counter++;
#endif
      wait.emplace_back(hash);
      return 1;
   }
   if(ptr1 == ABORTED) return 3;
   return 2;   //Already completed
}


void IdentifiableCacheBase::clear (deleter_f* deleter)
{
  size_t s = m_vec.size();
  for (size_t i=0; i<s ;i++) {
    const void* ptr = m_vec[i].exchange(nullptr);
    if (ptr && ptr < ABORTED){
      deleter (ptr);
    }
  }
  m_currentHashes.store(0, std::memory_order_relaxed);
#ifndef NDEBUG
  for(size_t i =0; i<m_NMutexes; i++){
     if(m_HoldingMutexes[i].counter!=0) std::cout << " counter is " << m_HoldingMutexes[i].counter << std::endl;
     assert(m_HoldingMutexes[i].counter==0);
  }
#endif
}


//Does not lock or clear atomics to allow faster destruction
void IdentifiableCacheBase::cleanUp (deleter_f* deleter)
{
  if(0 != m_currentHashes.load(std::memory_order_relaxed)){ //Reduce overhead if cache was unused
    size_t s = m_vec.size();
    for (size_t i=0; i<s ;i++) {
      const void* p = m_vec[i];
      if(p && p < ABORTED) deleter (p);
    }
  }
#ifndef NDEBUG
  for(size_t i =0; i<m_NMutexes; i++){
     if(m_HoldingMutexes[i].counter!=0) std::cout << "IdentifiableCacheBase counter is " << m_HoldingMutexes[i].counter << std::endl;
     assert(m_HoldingMutexes[i].counter==0);
  }
#endif
}

int IdentifiableCacheBase::itemAborted (IdentifierHash hash){
   const void* p = m_vec[hash].load();
   return (p == ABORTED);
}


int IdentifiableCacheBase::itemInProgress (IdentifierHash hash){
   const void* p = m_vec[hash].load();
   return (p == INVALID);
}


const void* IdentifiableCacheBase::find (IdentifierHash hash)
{
  if (hash >= m_vec.size()) return nullptr;
  const void* p = m_vec[hash].load();
  if (p >= ABORTED)
    return nullptr;
  return p;
}

const void* IdentifiableCacheBase::waitFor(IdentifierHash hash)
{
   const void* item = m_vec[hash].load();
   if(m_NMutexes ==0) return item;
   size_t slot = hash % m_NMutexes;
   if(item == INVALID){
      mutexPair &mutpair = m_HoldingMutexes[slot];
      uniqueLock lk(mutpair.mutex);
      while( (item =m_vec[hash].load()) ==  INVALID){
        mutpair.condition.wait(lk);
      }
   }
   return item;
}
#ifndef NDEBUG
void IdentifiableCacheBase::cancelWait(IdentifierHash hash){
   assert(m_NMutexes > 0);
   size_t slot = hash % m_NMutexes;
   m_HoldingMutexes[slot].counter--;
}
#endif
const void* IdentifiableCacheBase::findWait (IdentifierHash hash)
{
  if (hash >= m_vec.size()) return nullptr;
  const void* p = waitFor(hash);
  if(p>=ABORTED) return nullptr;
  return p;
}

void IdentifiableCacheBase::notifyHash(IdentifierHash hash)
{
    size_t slot = hash % m_NMutexes;
    mutexPair &mutpair = m_HoldingMutexes[slot];
    lock_t lk(mutpair.mutex);
    mutpair.condition.notify_all();
}

const void* IdentifiableCacheBase::get (IdentifierHash hash)
{
  // If it's there already, return directly without locking.
  const void* ptr = nullptr;
  if (hash >= m_vec.size()) return ptr;

  if(m_vec[hash].compare_exchange_strong(ptr, INVALID) ) {//Exchanges ptr with current value!!
     // Make the payload.
     if(m_maker == nullptr){
        m_vec[hash].store( ABORTED );
        return nullptr;
     }
     uniqueLock lock(m_mutex, std::defer_lock);
     if(!m_maker->m_IsReEntrant) lock.lock();//Allow reentrant or non reentrant makers

     try {
       ptr = m_maker->typelessMake (hash).release();
     }
     catch (...) {
       // FIXME: Can this be done with RAII?
       if(m_NMutexes >0) notifyHash(hash);
       throw;
     }
     assert(m_vec[hash] == INVALID);
     if(ptr){
        m_vec[hash].store( ptr );
        m_currentHashes++;
     }else{
        m_vec[hash].store( ABORTED );
     }
     if(m_NMutexes >0) notifyHash(hash);
  }
  else if(ptr == INVALID){
     ptr= waitFor(hash);
  }
  if(ptr == ABORTED) return nullptr;
  assert(ptr < ABORTED);
  return ptr;
}

void IdentifiableCacheBase::createSet (const std::vector<IdentifierHash>& hashes, std::vector<bool> &mask){
   assert(mask.size() == fullSize());
   for(IdentifierHash hash : hashes){
      const void* ptr = get(hash);
      if(ptr !=nullptr) mask[hash] = true;
   }
}

#ifdef IdentifiableCacheBaseRemove
bool IdentifiableCacheBase::remove (IdentifierHash hash)
{
   if (hash >= m_vec.size()) return false;
   if(m_vec[hash]){
      m_vec[hash] = nullptr;
      m_currentHashes--;
      return true;
   }
   return false;
}
#endif
size_t IdentifiableCacheBase::numberOfHashes()
{
  return m_currentHashes.load();
}

std::vector<IdentifierHash> IdentifiableCacheBase::ids()
{
  std::vector<IdentifierHash> ret;
  ret.reserve (m_currentHashes);
  size_t s = m_vec.size();
  for (size_t i =0; i<s; i++) {
    const void* p = m_vec[i].load();
    if (p && p < ABORTED)
      ret.push_back (i);
  }
  return ret;
}


bool IdentifiableCacheBase::add (IdentifierHash hash, const void* p)
{
  if (hash >= m_vec.size()) return false;
  if(p==nullptr) return false;
  const void* nul=nullptr;
  if(m_vec[hash].compare_exchange_strong(nul, p)){
     m_currentHashes++;
     return true;
  }
  const void* invalid = INVALID;
  if(m_vec[hash].compare_exchange_strong(invalid, p)){
     m_currentHashes++;
     return true;
  }
  return false;
}


bool IdentifiableCacheBase::add (IdentifierHash hash,
                                 void_unique_ptr p)
{
  bool b = add(hash, p.get());
  if(b) p.release();
  return b;
}



} // namespace EventContainers


