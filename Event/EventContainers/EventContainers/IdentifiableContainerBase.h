/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EVENTCONTAINERS_IDENTIFIABLECONTAINERBASE_H
#define EVENTCONTAINERS_IDENTIFIABLECONTAINERBASE_H
#include "EventContainers/I_InternalIDC.h"
#include "CxxUtils/checker_macros.h"


namespace EventContainers{ class IdentifiableCacheBase; 
enum class Mode { OfflineLowMemory, OfflineFast, OfflineMap };
class IdentifiableContainerBase{
public:
#include "EventContainers/deleter.h"
    
    typedef std::vector<IdentifierHash> Hash_Container;
    IdentifiableContainerBase(EventContainers::IdentifiableCacheBase *cache);
    IdentifiableContainerBase(size_t maxsize);
    IdentifiableContainerBase(size_t maxsize, Mode mode);
    ~IdentifiableContainerBase() = default;
protected:
    bool m_OnlineMode;
    typedef EventContainers::IdentifiableCacheBase IdentifiableCacheBase;
    std::unique_ptr<I_InternalIDC> m_link;
    std::vector<IdentifierHash> GetAllCurrentHashes() const { return m_link->getAllCurrentHashes(); }
    bool insert(IdentifierHash hashId, const void* ptr) { return m_link->insert(hashId, ptr); }
    void Wait() const { m_link->wait(); }
    bool tryAddFromCache(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock) { 
        return m_link->tryAddFromCache(hashId, lock); }
    bool tryAddFromCache(IdentifierHash hashId){ return m_link->tryAddFromCache(hashId); }
    void cleanup(deleter_f* deleter);
    size_t numberOfCollections() const { return m_link->numberOfCollections(); }
    void ResetMask();
    StatusCode fetchOrCreate(IdentifierHash hashId){ return m_link->fetchOrCreate(hashId); }
    StatusCode fetchOrCreate(const std::vector<IdentifierHash> &hashIds) { return m_link->fetchOrCreate(hashIds); }
    const void* indexFindPtr( IdentifierHash hashId ) const { return m_link->findIndexPtr(hashId); }
};

}


#endif
