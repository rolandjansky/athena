/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EVENTCONTAINERS_IDENTIFIABLETEMP_H
#define EVENTCONTAINERS_IDENTIFIABLETEMP_H
//This is a dummy version of the identifiable container.
//Do not use with storegate
//Do not use multithreadedly

#include "EventContainers/IIdentifiableCont.h"
#include "EventContainers/IdentifiableContainerMT.h"
#include <vector>
namespace EventContainers {
template < class T>
class IdentifiableContTemp : public IIdentifiableCont<T>{

    std::vector< std::pair<IdentifierHash, const T* > > m_usedhashes;
    std::vector< const T* > m_randomcont;

    IdentifiableContainerMT< T > *m_extIDC;
    bool m_hasExternalCache;
    IdentifiableContTemp(size_t size, IdentifiableContainerMT< T > *extcache) : m_randomcont(size, nullptr), m_extIDC(extcache) { 
       m_hasExternalCache = m_extIDC && m_extIDC->hasExternalCache();
    }
public:
    IdentifiableContTemp(size_t size) : IdentifiableContTemp(size, nullptr) { } 

    IdentifiableContTemp(IdentifiableContainerMT< T > *extcache = nullptr) : IdentifiableContTemp(extcache->fullSize(), extcache)  {     }
    IdentifiableContTemp() = delete;
    IdentifiableContTemp(const IdentifiableContTemp& that) = delete;
    IdentifiableContTemp& operator= ( const IdentifiableContTemp& ) = delete;

    virtual ~IdentifiableContTemp(){
        for(auto &x : m_usedhashes){
            delete x.second;
        }
    }

    virtual  const T* indexFindPtr( IdentifierHash hashId ) const override{
        if(hashId >=  m_randomcont.size()) return nullptr;
        return m_randomcont[hashId];
    }
    
    virtual std::vector<IdentifierHash> GetAllCurrentHashes() const override{
        std::vector<IdentifierHash> out;
        out.reserve(m_usedhashes.size());
        for(auto x : m_usedhashes) out.push_back(x.first);
        return out;
    }
    
    virtual StatusCode addCollection(const T* coll, IdentifierHash hashId) override{
        if(hashId >=  m_randomcont.size()) return StatusCode::FAILURE;
        m_usedhashes.emplace_back(hashId, coll);
        m_randomcont[hashId] = coll;
        return  StatusCode::SUCCESS;
    }


    virtual StatusCode addOrDelete(std::unique_ptr<T> ptr, IdentifierHash hashId) override{
        if(hashId >=  m_randomcont.size()) return StatusCode::FAILURE;
        if(m_randomcont[hashId] == nullptr){
            return addCollection(ptr.release(), hashId);
        }
        ptr.reset();
        return StatusCode::SUCCESS;
    }
    
    virtual StatusCode addOrDelete(std::unique_ptr<const T> ptr, IdentifierHash hashId) override{
        if(hashId >=  m_randomcont.size()) return StatusCode::FAILURE;
        if(m_randomcont[hashId] == nullptr){
            return addCollection(ptr.release(), hashId);
        }
        ptr.reset();
        return StatusCode::SUCCESS;
    }
    
    virtual size_t fullSize() const override{
        return m_randomcont.size();
    }
    
    virtual size_t numberOfCollections() const override{
        return m_usedhashes.size();
    }
    
    virtual bool  tryAddFromCache(IdentifierHash hashId) override{
      if(m_extIDC){
          return m_extIDC->tryAddFromCache(hashId);
      }
      return false;
    }
  
    virtual bool hasExternalCache() const override{
        return m_hasExternalCache;
    }
    
    virtual StatusCode naughtyRetrieve ATLAS_NOT_THREAD_SAFE (IdentifierHash hashId, T* &collToRetrieve) const override{
        if(hashId >=  m_randomcont.size()) return StatusCode::FAILURE;
        collToRetrieve = const_cast<T*>( m_randomcont[hashId]);
        return StatusCode::SUCCESS; 
    }
    
    StatusCode MergeToRealContainer(IIdentifiableCont<T> *real){
        for(auto &x : m_usedhashes){
          auto ptr = std::unique_ptr<const T>(x.second);
          auto sc = real->addOrDelete(std::move(ptr), x.first);
          if(sc.isFailure()) { return StatusCode::FAILURE; }
          m_randomcont[x.first] = nullptr;
        }
        m_usedhashes.clear();
        return StatusCode::SUCCESS;
    }
    
    virtual void cleanup() override{
        for(auto &x : m_usedhashes){
            delete x.second;
        }
        m_usedhashes.clear();
        m_randomcont.assign(m_randomcont.size(), nullptr);
    }

};

}
#endif

