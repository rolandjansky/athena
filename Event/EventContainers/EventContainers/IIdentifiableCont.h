/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EVENTCONTAINERS_IIDENTIFIABLECONT_H
#define EVENTCONTAINERS_IIDENTIFIABLECONT_H
#include "Identifier/IdentifierHash.h"
#include <memory>

namespace EventContainers {
template <class T>
class IIdentifiableCont{

public:
    virtual  const T* indexFindPtr( IdentifierHash hashId ) const = 0;

    virtual std::vector<IdentifierHash> GetAllCurrentHashes() const =0;

    virtual StatusCode addOrDelete(std::unique_ptr<T> ptr, IdentifierHash hashId) =0;

    virtual size_t fullSize() const =0;

    virtual size_t numberOfCollections() const =0;

    virtual bool  tryAddFromCache(IdentifierHash hashId) = 0;

    virtual bool hasExternalCache() const =0;

    virtual StatusCode naughtyRetrieve(IdentifierHash hashId, T* &collToRetrieve)const =0;

    virtual StatusCode addCollection(const T* coll, IdentifierHash hashId) =0;

    virtual void cleanup() = 0;
};
}
#endif

