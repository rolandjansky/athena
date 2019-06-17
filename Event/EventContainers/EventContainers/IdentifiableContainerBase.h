/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EVENTCONTAINERS_IDENTIFIABLECONTAINERBASE_H
#define EVENTCONTAINERS_IDENTIFIABLECONTAINERBASE_H

#include "EventContainers/IdentifiableCacheBase.h"
#include "CxxUtils/checker_macros.h"

class IdentifiableContainerBase{

public:
    typedef std::vector<IdentifierHash> Hash_Container;
    IdentifiableContainerBase(EventContainers::IdentifiableCacheBase *cache, bool online);
    static Hash_Container::const_iterator findHash(Hash_Container*, IdentifierHash);
    ~IdentifiableContainerBase();
protected:
    EventContainers::IdentifiableCacheBase *m_cacheLink;
    bool m_OnlineMode;
    mutable std::atomic<bool> m_waitNeeded ATLAS_THREAD_SAFE; //These mutables are carefully thought out, do not change
    typedef EventContainers::IdentifiableCacheBase IdentifiableCacheBase;
    mutable std::vector< IdentifierHash > m_waitlist ATLAS_THREAD_SAFE;
    mutable std::mutex m_waitMutex ATLAS_THREAD_SAFE;
    mutable std::vector<bool> m_mask ATLAS_THREAD_SAFE;
    std::vector<IdentifierHash> GetAllCurrentHashes() const;

    void Wait() const;
    bool tryAddFromCache(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock);
    bool tryAddFromCache(IdentifierHash hashId);
    void cleanup();
    size_t numberOfCollections() const;
    void ResetMask();
    StatusCode fetchOrCreate(IdentifierHash hashId);
    StatusCode fetchOrCreate(const std::vector<IdentifierHash> &hashIds);
};

#endif
