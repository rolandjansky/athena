/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PSUTILS_MEMPRINT_H
#define PSUTILS_MEMPRINT_H

#include <map>
#include <vector>
#include "getMemUsage.h"
#include "SiDigitization/HitIndexAndTime.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include <sstream>

namespace PSUtils{

  typedef std::map< IdentifierHash, SiChargedDiodeCollection* >    DiodeCache;
  typedef std::map< IdentifierHash, std::vector<HitIndexAndTime> > HitCache;

  struct HitCacheCounter
  {
    
  HitCacheCounter():m_count(0){}
    void operator()(const HitCache::value_type& entry)
    {
      m_count += (entry.second).size();
    }
    unsigned int m_count;
  };
  
  struct DiodeCacheCounter
  {
    
  DiodeCacheCounter():m_count(0){}
    void operator()(const DiodeCache::value_type&  entry)
    {
      m_count += ((entry.second)->chargedDiodes()).size();
    }
    unsigned int m_count;
  };
  
  
  std::string memPrint(const DiodeCache& cache){
    std::size_t nWafers = cache.size();
    std::size_t nDiodes = (std::for_each(cache.begin(), cache.end(), DiodeCacheCounter() ) ).m_count;
    std::ostringstream sstream;
    sstream<<getMemUsage()<<" n wafers "<< nWafers<<" n diodes "<<nDiodes<<'\n';
    return sstream.str();
  }
  
  std::string memPrint(const HitCache& cache){
    std::size_t nWafers = cache.size();
    std::size_t nDiodes = (std::for_each(cache.begin(), cache.end(), HitCacheCounter() ) ).m_count;
    std::ostringstream sstream;
    sstream<<getMemUsage()<<" n wafers "<< nWafers<<" n hits "<<nDiodes<<'\n';
    return sstream.str();    
  }
}
#endif
