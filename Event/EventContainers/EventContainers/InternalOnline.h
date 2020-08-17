/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_INTERNALONLINE_H
#define EVENTCONTAINERS_INTERNALONLINE_H

#include "EventContainers/I_InternalIDC.h"
#include "CxxUtils/checker_macros.h"
#include <atomic>
#include <mutex>

namespace EventContainers{
class IdentifiableCacheBase;
/*
The online implementation keeps a pointer to an external cache which owns the collections
and can be filled concurrently. The container itself maintains a bit mask to identify which
collections can be accessed from this container. It also maintains a wait list which records
uncompleted objects that were added to the container. When a complete collection is
requested the wait-list is processed, locking the thread until all elements are completed.
The intention of this is to delay thread locking until the last possible moment and thus
hopefully avoid it altogether.
Random access is fast since it takes a direct index to the external cache element.
Ordered iteration is also fast after the lazily initialised collection is constructed.
*/
class InternalOnline final : public I_InternalIDC {
public:
    InternalOnline(EventContainers::IdentifiableCacheBase *cache);
    virtual InternalConstItr cbegin() const override;
    virtual InternalConstItr cend() const override;
    virtual InternalConstItr indexFind( IdentifierHash hashId ) const override;
    virtual const std::vector < hashPair >& getAllHashPtrPair() const override;
    virtual ~InternalOnline()=default;
    virtual void wait() const override;
    virtual bool tryAddFromCache(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock) override;
    virtual bool tryAddFromCache(IdentifierHash hashId) override;
    virtual std::vector<IdentifierHash> getAllCurrentHashes() const override;
    virtual size_t numberOfCollections() const override;
    void resetMask();
    virtual size_t fullSize() const noexcept override { return m_mask.size(); }
    virtual StatusCode fetchOrCreate(IdentifierHash hashId) override;
    virtual StatusCode fetchOrCreate(const std::vector<IdentifierHash> &hashIds) override;
    virtual bool insert(IdentifierHash hashId, const void* ptr) override;
    virtual const void* findIndexPtr(IdentifierHash hashId) const noexcept override;
    virtual StatusCode addLock(IdentifierHash hashId, const void* ptr) override;
    virtual void* removeCollection( IdentifierHash hashId ) override;
    virtual void destructor(deleter_f*) noexcept override;
    virtual void cleanUp(deleter_f*) noexcept override;
private:
    EventContainers::IdentifiableCacheBase *m_cacheLink;
    mutable std::vector< IdentifierHash > m_waitlist ATLAS_THREAD_SAFE;
    mutable std::vector<I_InternalIDC::hashPair> m_map ATLAS_THREAD_SAFE;
    mutable std::mutex m_waitMutex ATLAS_THREAD_SAFE;
    mutable std::vector<bool> m_mask ATLAS_THREAD_SAFE;
    mutable std::atomic<bool> m_waitNeeded ATLAS_THREAD_SAFE; //These mutables are carefully thought out, do not change
};

}
#endif
