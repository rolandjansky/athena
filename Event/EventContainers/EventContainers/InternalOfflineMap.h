/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_InternalOfflineMap_H
#define EVENTCONTAINERS_InternalOfflineMap_H
#include "EventContainers/I_InternalIDC.h"
#include "CxxUtils/checker_macros.h"
#include <atomic>
#include <mutex>
#include <unordered_map>

namespace EventContainers{
/*
This class implements the IdentifiableContainer code for the offline case.
This class balances speed against memory usage by using an unordered_map

Moderately fast random access, fast iteration, moderate memory usage.
*/
class InternalOfflineMap final : public I_InternalIDC {
public:
    InternalOfflineMap(size_t max);
    virtual ~InternalOfflineMap()=default;
    virtual InternalConstItr cbegin() const override;
    virtual InternalConstItr cend() const override;
    virtual InternalConstItr indexFind( IdentifierHash hashId ) const override;
    virtual const std::vector < hashPair >& getAllHashPtrPair() const override;
    virtual bool tryAddFromCache(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock) override;
    virtual bool tryAddFromCache(IdentifierHash hashId) override;
    virtual void wait() const override;
    virtual std::vector<IdentifierHash> getAllCurrentHashes() const override;
    virtual size_t numberOfCollections() const override;
    virtual void cleanUp(deleter_f* deleter) noexcept override;
    virtual size_t fullSize() const noexcept override {return m_maxsize;}
    virtual StatusCode fetchOrCreate(IdentifierHash hashId) override;
    virtual StatusCode fetchOrCreate(const std::vector<IdentifierHash> &hashIds) override;
    virtual bool insert(IdentifierHash hashId, const void* ptr) override;
    virtual const void* findIndexPtr(IdentifierHash hashId) const noexcept override;
    virtual StatusCode addLock(IdentifierHash hashId, const void* ptr) override;
    virtual void* removeCollection( IdentifierHash hashId ) override;
    virtual void destructor(deleter_f*) noexcept override;
private:
    mutable std::vector<I_InternalIDC::hashPair> m_map ATLAS_THREAD_SAFE;
    std::unordered_map<IdentifierHash::value_type, const void*> m_fullMap;
    mutable std::mutex m_waitMutex ATLAS_THREAD_SAFE;
    mutable std::atomic<bool> m_needsupdate ATLAS_THREAD_SAFE; //These mutables are carefully thought out, do not change
    const size_t m_maxsize;
};

}
#endif
