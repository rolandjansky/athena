/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Boost include(s):
#include "boost/regex.hpp"

// Local include(s):
#include "TrigObjectMatching/TrigMatchToolCore.h"

TrigMatchToolCore::TrigMatchToolCore()
  : m_chainNameIndex (this),
    m_trigDecisionToolCore(0)
{
}

TrigMatchToolCore::~TrigMatchToolCore()
{
}

std::string
TrigMatchToolCore::propagateChainNames( const std::string &chainName ) const
{
   return m_chainNameIndex.propagateChainNames (chainName);
}


void TrigMatchToolCore::endEvent()
{
   m_slotCache->clear();
  
   return;
}


// Clear saved chain name -> index mapping and rebuild from current
// configuration.
void TrigMatchToolCore::clearChainIndex()
{
  m_chainNameIndex.clear();
}


TrigMatchToolCore::TrigFeatureCacheBase*&
TrigMatchToolCore::getCache1 (const std::type_info* tid, int type_key,
                              SlotCache& slotCache,
                              const SlotCache::lock_t& /*lock*/) const
{
  if (type_key < 0) {
    m_typeMap.key (tid);
  }

  if (static_cast<int> (slotCache.m_caches->size()) <= type_key)
    slotCache.m_caches->resize (type_key + 1);
  return (*slotCache.m_caches)[type_key];
}


size_t
TrigMatchToolCore::chainNameToIndex (const std::string& chainName) const
{
  return m_chainNameIndex.chainNameToIndex (chainName);
}


const Trig::FeatureContainer&
TrigMatchToolCore::getCachedFeatureContainer (size_t chainIndex,
                                              SlotCache& slotCache,
                                              const SlotCache::lock_t& /*lock*/) const
{
  if (chainIndex >= slotCache.m_featureContainers.size()) {
    if (chainIndex >= slotCache.m_nFeatureContainers)
      slotCache.m_nFeatureContainers = chainIndex + 1;
    slotCache.m_featureContainers.resize (slotCache.m_nFeatureContainers);
    slotCache.m_featureContainersValid.resize (slotCache.m_nFeatureContainers);
  }

  if (!slotCache.m_featureContainersValid[chainIndex]) {
#if 0
    // Use this once FeatureContainer::swap is available to avoid copies.
    Trig::FeatureContainer fc = 
      this->getFeatureContainer (m_chainNameIndex.chainName(chainIndex),
                                 TrigDefs::alsoDeactivateTEs);
    slotCache.m_featureContainers[chainIndex].swap (fc);
#else
    slotCache.m_featureContainers[chainIndex] = 
      this->getFeatureContainer (m_chainNameIndex.chainName(chainIndex),
                                 TrigDefs::alsoDeactivateTEs);
#endif
    slotCache.m_featureContainersValid[chainIndex] = true;
  }

  return slotCache.m_featureContainers[chainIndex];
}


//***************************************************************************


TrigMatchToolCore::ChainNameIndex::ChainNameIndex (TrigMatchToolCore* core)
  : m_core (core)
{
}


void
TrigMatchToolCore::ChainNameIndex::assertConfiguredChainNames()
{
  if (m_chainNames.empty()) {
    m_chainNames = m_core->getConfiguredChainNames();
    m_nConfiguredChainNames = m_chainNames.size();
    m_chainIndexMap.clear();
    for (size_t i = 0; i < m_chainNames.size(); i++) {
      const std::string& chainName = m_chainNames[i];
      m_chainIndexMap[chainName] = i;

      // Build the L1L2 map.  Note that we ignore regex support
      // at this stage - we'll implement regex support by matching
      // to the keys in the cache later on.  Also note that the
      // cached version does not have parentheses around it (so
      // they can easily be combined)
      if (chainName.find("L2_") != std::string::npos) {
        const std::string l1 = m_core->lowerChainName( chainName );

        if( m_l1l2Map.find( l1 ) == m_l1l2Map.end() ) {
          m_l1l2Map[ l1 ] = chainName;
        } else {
          m_l1l2Map[ l1 ] = m_l1l2Map[ l1 ] + "|" + chainName;
        }
      }
    }
  }
}


size_t
TrigMatchToolCore::ChainNameIndex::chainNameToIndex (const std::string& chainName)
{
  lock_t lock (m_mutex);
  assertConfiguredChainNames();
  chainIndexMap_t::iterator it = m_chainIndexMap.find (chainName);
  if (it == m_chainIndexMap.end()) {
    size_t chainIndex = m_chainNames.size();
    m_chainIndexMap[chainName] = chainIndex;
    m_chainNames.push_back (chainName);
    return chainIndex;
  }
  else
    return it->second;
}


std::vector<std::string>
TrigMatchToolCore::ChainNameIndex::configuredChainNames()
{
  lock_t lock (m_mutex);
  assertConfiguredChainNames();
  return std::vector<std::string> (m_chainNames.begin(),
                                   m_chainNames.begin() + m_nConfiguredChainNames);
}


std::string
TrigMatchToolCore::ChainNameIndex::chainName (size_t index)
{
  lock_t lock (m_mutex);
  assertConfiguredChainNames();
  return m_chainNames.at (index);
}


void
TrigMatchToolCore::ChainNameIndex::clear()
{
  lock_t lock (m_mutex);
  m_chainNames.clear();
  m_chainIndexMap.clear();
  m_nConfiguredChainNames = 0;
  m_l1l2Map.clear();
  assertConfiguredChainNames();
}


std::string
TrigMatchToolCore::ChainNameIndex::propagateChainNames( const std::string &chainName )
{
   // only applicable for L1 chains
   if( chainName.find( "L1_" ) == std::string::npos )
      return chainName;

  lock_t lock (m_mutex);

  // add it to the cache if necessary
   if( m_l1l2Map.find( chainName ) == m_l1l2Map.end() ) {

      // if its not in the cache, we need to check
      // if we can build it from the cache.
      // regex support
      boost::regex compiled( chainName );
      boost::cmatch what;
      std::string chains;

      std::map< std::string, std::string >::const_iterator iter =
         m_l1l2Map.begin();
      std::map< std::string, std::string >::const_iterator end =
         m_l1l2Map.end();
      for( ; iter != end; ++iter ) {
         // check if the l1 chain matches the regex supplied by chainName
         if( boost::regex_match( iter->first.c_str(), what, compiled ) ) {
            if( chains.empty() ) {
               chains += iter->second;
            } else {
               chains += "|" + iter->second;
            }
         }
      }

      // add it to the cache
      m_l1l2Map[ chainName ] = chains;
    }

   // if it wasn't in the cache before, it will be now, so pull it out
   std::string output = m_l1l2Map[ chainName ];
   if( output.find( "|" ) != std::string::npos ) {
      output = "(" + output + ")";
   }

   return output;
}


int TrigMatchToolCore::TypeMap::key (const std::type_info* tid)
{
  lock_t lock (m_mutex);
  typeMap_t::const_iterator it = m_typeMap.find (tid);
  if (it != m_typeMap.end()) {
    return it->second;
  }

  int type_key = m_typeMap.size();
  m_typeMap[tid] = type_key;
  return type_key;
}
