/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EVENTCONTAINERS_IDENTIFIABLECONTAINERBASE_H
#define EVENTCONTAINERS_IDENTIFIABLECONTAINERBASE_H

#include "EventContainers/IdentifiableCacheBase.h"

class IdentifiableContainerBase{

public:
    typedef std::vector<IdentifierHash> Hash_Container;
    IdentifiableContainerBase(EventContainers::IdentifiableCacheBase *cache, bool online);
    static Hash_Container::const_iterator findHash(Hash_Container*, IdentifierHash);
    ~IdentifiableContainerBase();
protected:
    EventContainers::IdentifiableCacheBase *m_cacheLink;
    bool m_OnlineMode;
    mutable std::atomic<bool> m_waitNeeded;
    typedef EventContainers::IdentifiableCacheBase IdentifiableCacheBase;
    mutable std::vector< IdentifierHash > m_waitlist;
    mutable std::mutex m_waitMutex;
    mutable std::vector<bool> m_mask;
    std::vector<IdentifierHash> GetAllCurrentHashes() const;

    void Wait() const;
    bool tryFetch(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock);
    bool tryFetch(IdentifierHash hashId);
    void cleanup();
    size_t numberOfCollections() const;
    void ResetMask();
    StatusCode fetchOrCreate(IdentifierHash hashId);
    StatusCode fetchOrCreate(const std::vector<IdentifierHash> &hashIds);
};

#endif
