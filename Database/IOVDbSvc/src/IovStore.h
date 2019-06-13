/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IOVDbSvc_IovStore_h
#define IOVDbSvc_IovStore_h
/**
 * @file IovStore.h
 * @brief helper class for IOVDbFolder managing cached iov since/until pairs
 * @author Shaun Roe March 2019
 **/
#include "CoolKernel/ValidityKey.h"
#include <vector>
#include <utility> //pair

namespace IOVDbNamespace{
  //@brief Manages the caching of IOV since/until pairs but encapsulating the functionality
  //in a single class
  class IovStore{
  public:
    typedef std::pair<cool::ValidityKey, cool::ValidityKey> Iov_t;
    ///Default constructor, cache bounds set to 0,0
    IovStore();
    ///Constructor, cache bounds set to supplied parameter
    IovStore(const Iov_t & cacheBounds);
    ///Add an iov to the store and update the span variables
    void addIov(const Iov_t & iov);
    void addIov(const cool::ValidityKey & since, const cool::ValidityKey & until);
    ///Report the current cache bounds
    Iov_t getCacheBounds() const;
    ///Set the cache bound variables
    void setCacheBounds(const Iov_t & cacheBounds);
    ///Get the minimum straddling span (max 'since' to min 'until')
    Iov_t getMinimumStraddlingSpan() const;
    ///Set span
    void setIovSpan(const Iov_t & span);
    ///Return the internal vector store
    std::vector<Iov_t> & vectorStore();
    ///Return the current count of added Iovs which straddled cache boundaries
    std::pair<unsigned int, unsigned int> numberOfIovsOnBoundaries() const;
    ///is the store empty?
    bool empty() const { return m_iovs.empty();}
    ///length of store
    unsigned int size() const { return m_iovs.size();}
    ///clear the store
    void clear();
    ///'at' to read iov at specific index
    Iov_t at(const unsigned int idx) const { return m_iovs.at(idx);}
    ///Extend a specific iov at the index to a new 'until' time
    void extendIov(const unsigned int idx, const cool::ValidityKey & newUntilTime);
    ///Extend lower cache bound  to the maximum 'since' time; return whether this changes its value
    bool extendCacheLo();
    ///Extend upper cache bound to the minimum 'until' time; return whether this changes its value
    bool extendCacheHi();
  private:
    std::pair<bool,bool> straddling(const Iov_t & iov, const Iov_t & bounds) const;
    ///main vector of iovs
    std::vector<Iov_t> m_iovs{};
    ///cache bounds, 'since', 'until', corresponds to m_cachestart, m_cachestop in original
    Iov_t m_cacheBounds{0,0};
    /** max 'since' of the iovs which straddle the cache boundary, corresponds to 'm_boundmin'
     *  in the original code 
     **/
    cool::ValidityKey m_maxStraddlingSince{cool::ValidityKeyMin};
    /** min 'until' of the iovs which straddle the boundary
     *  corresponds to 'm_boundmax' in the original code 
     **/
    cool::ValidityKey m_minStraddlingUntil{cool::ValidityKeyMax};
    ///count of iovs straddling the since or until boundaries
    std::pair<unsigned int, unsigned int> m_countStraddling{};
  };
 
}

#endif
