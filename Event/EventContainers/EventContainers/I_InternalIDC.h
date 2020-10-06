/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_I_INTERNALIDC_H
#define EVENTCONTAINERS_I_INTERNALIDC_H

#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>

namespace EventContainers{
class IDC_WriteHandleBase;
/*
The intention of the IdentifiableContainer is to provide a key-value map
for collection pointers. To increase memory and cpu efficiency the online trigger 
system different ''views'' can share collection that are created concurrently.
To efficiently support these different uses while not imposing overhead on the
offline case and to maintain a consistent interface the internals of the class are
virtualised.

A standard iterator is provided for fast iteration in a sequential order.
*/
template<typename T>
struct hashPair{
   IdentifierHash::value_type first;
   const T*                second;
   constexpr hashPair(IdentifierHash::value_type f, const T* s) : first(f), second(s) {  }
   bool operator <(const hashPair &b) const noexcept{
      return first < b.first;
   }
};

class I_InternalIDC{
public:
    typedef EventContainers::hashPair<void> hashPair;
    typedef std::vector < hashPair >::const_iterator InternalConstItr;
    #include "EventContainers/deleter.h"
    virtual InternalConstItr cbegin() const=0;
    virtual InternalConstItr cend() const=0;
    virtual InternalConstItr indexFind( IdentifierHash hashId ) const =0;
    virtual ~I_InternalIDC() = default;
    virtual void wait() const = 0;
    virtual bool tryAddFromCache(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock) = 0;
    virtual bool tryAddFromCache(IdentifierHash hashId) = 0;
    virtual std::vector<IdentifierHash> getAllCurrentHashes() const =0;
    virtual const std::vector < hashPair >& getAllHashPtrPair() const = 0;
    virtual size_t numberOfCollections() const = 0;
    virtual size_t fullSize() const noexcept =0;
    virtual StatusCode fetchOrCreate(IdentifierHash hashId) =0;
    virtual StatusCode fetchOrCreate(const std::vector<IdentifierHash> &hashIds) =0;
    virtual bool insert(IdentifierHash hashId, const void* ptr) =0;
    virtual const void* findIndexPtr(IdentifierHash hashId) const noexcept = 0;
    virtual StatusCode addLock(IdentifierHash hashId, const void* ptr) = 0;
    virtual void* removeCollection( IdentifierHash hashId ) =0;
    virtual void destructor(deleter_f*) noexcept =0;
    virtual void cleanUp(deleter_f*) noexcept =0;
};

}
#endif