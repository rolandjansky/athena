/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_I_INTERNALIDC_H
#define EVENTCONTAINERS_I_INTERNALIDC_H

#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <utility>

namespace EventContainers{
class IDC_WriteHandleBase;
/*
The intention of the IdentifiableContainer is to provide a key-value map
for collection pointers. To increase memory and cpu efficiency the online trigger 
system different “views” can share collection that are created concurrently.
To efficiently support these different uses while not imposing overhead on the
offline case and to maintain a consistent interface the internals of the class are
virtualised.

A standard iterator is provided for fast iteration in a sequential order.
*/
class I_InternalIDC{
public:
    typedef std::pair < IdentifierHash::value_type, const void* > hashPair;
    typedef std::vector < hashPair >::const_iterator InternalConstItr;
    #include "EventContainers/deleter.h"
    virtual InternalConstItr cbegin() const=0;
    virtual InternalConstItr cend() const=0;
    virtual InternalConstItr indexFind( IdentifierHash hashId ) const =0;
    virtual ~I_InternalIDC() = default;
    virtual void Wait() const = 0;
    virtual bool tryAddFromCache(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock) = 0;
    virtual bool tryAddFromCache(IdentifierHash hashId) = 0;
    virtual std::vector<IdentifierHash> GetAllCurrentHashes() const =0;
    virtual const std::vector < hashPair >& GetAllHashPtrPair() const = 0;
    virtual size_t numberOfCollections() const = 0;
    virtual size_t fullSize() const noexcept =0;
    virtual StatusCode fetchOrCreate(IdentifierHash hashId) =0;
    virtual StatusCode fetchOrCreate(const std::vector<IdentifierHash> &hashIds) =0;
    virtual bool insert(IdentifierHash hashId, const void* ptr) =0;
    virtual const void* FindIndexPtr(IdentifierHash hashId) const noexcept = 0;
    virtual StatusCode addLock(IdentifierHash hashId, const void* ptr) = 0;
    virtual void* removeCollection( IdentifierHash hashId ) =0;
};

}
#endif