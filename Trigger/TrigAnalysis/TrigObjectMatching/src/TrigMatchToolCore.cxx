/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Boost include(s):
#include "boost/regex.hpp"
#include "boost/foreach.hpp"

// Local include(s):
#include "TrigObjectMatching/TrigMatchToolCore.h"

TrigMatchToolCore::TrigMatchToolCore()
  : m_nConfiguredChainNames(0),
    m_trigDecisionToolCore(0),
    m_nFeatureContainers (100)
{

   m_l1l2Map.clear();
   m_featureLabel = "";
   m_caches = &m_cacheMap[""];
}

TrigMatchToolCore::~TrigMatchToolCore()
{
  BOOST_FOREACH (const cacheMap_t::value_type& p, m_cacheMap) {
    BOOST_FOREACH (TrigFeatureCacheBase* cache, p.second) {
      delete cache;
    }
  }
}

std::string
TrigMatchToolCore::propagateChainNames( const std::string &chainName ) {

   // only applicable for L1 chains
   if( chainName.find( "L1_" ) == std::string::npos )
      return chainName;

   // check that the cache is built
   this->buildL1L2Map();

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

// Build the L1L2 map.  Note that we ignore regex support
// at this stage - we'll implement regex support by matching
// to the keys in the cache later on.  Also note that the
// cached version does not have parentheses around it (so
// they can easily be combined)
void TrigMatchToolCore::buildL1L2Map() {

   if( m_l1l2Map.size() ) return;

   // loop through all the L2 chains and check if they are seeded by the L1 item
   this->assertConfiguredChainNames();

   for (size_t i = 0; i < m_nConfiguredChainNames; i++) {
      const std::string& chainName = m_chainNames[i];

      if( chainName.find("L2_") == std::string::npos ) {
         continue;
      }

      const std::string l1 = this->lowerChainName( chainName );

      if( m_l1l2Map.find( l1 ) == m_l1l2Map.end() ) {
         m_l1l2Map[ l1 ] = chainName;
      } else {
         m_l1l2Map[ l1 ] = m_l1l2Map[ l1 ] + "|" + chainName;
      }
   }

   return;
}

void TrigMatchToolCore::endEvent() {

   BOOST_FOREACH (const cacheMap_t::value_type& p, m_cacheMap) {
     BOOST_FOREACH (TrigFeatureCacheBase* cache, p.second) {
       cache->clear();
     }
   }

   std::vector<Trig::FeatureContainer>().swap (m_featureContainers);
   std::vector<bool>().swap (m_featureContainersValid);
   
   return;
}


TrigMatchToolCore::TrigFeatureCacheBase*&
TrigMatchToolCore::getCache1 (const std::type_info* tid, int& type_key)
{
  if (type_key < 0) {
    typeMap_t::const_iterator it = m_typeMap.find (tid);
    if (it != m_typeMap.end())
      type_key = it->second;
    else {
      type_key = m_typeMap.size();
      m_typeMap[tid] = type_key;
    }
  }

  if (static_cast<int> (m_caches->size()) <= type_key)
    m_caches->resize (type_key + 1);
  return (*m_caches)[type_key];
}


void TrigMatchToolCore::buildChainIndexMap()
{
  m_chainIndexMap.clear();
  for (size_t i = 0; i < m_chainNames.size(); i++)
    m_chainIndexMap[m_chainNames[i]] = i;
}


size_t
TrigMatchToolCore::chainNameToIndex (const std::string& chainName)
{
  this->assertConfiguredChainNames();
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


const Trig::FeatureContainer&
TrigMatchToolCore::getCachedFeatureContainer (size_t chainIndex)
{
  if (chainIndex >= m_featureContainers.size()) {
    if (chainIndex >= m_nFeatureContainers)
      m_nFeatureContainers = chainIndex + 1;
    m_featureContainers.resize (m_nFeatureContainers);
    m_featureContainersValid.resize (m_nFeatureContainers);
  }

  if (!m_featureContainersValid[chainIndex]) {
#if 0
    // Use this once FeatureContainer::swap is available to avoid copies.
    Trig::FeatureContainer fc = 
      this->getFeatureContainer (m_chainNames[chainIndex],
                                 TrigDefs::alsoDeactivateTEs);
    m_featureContainers[chainIndex].swap (fc);
#else
    m_featureContainers[chainIndex] = 
      this->getFeatureContainer (m_chainNames[chainIndex],
                                 TrigDefs::alsoDeactivateTEs);
#endif
    m_featureContainersValid[chainIndex] = true;
  }

  return m_featureContainers[chainIndex];
}
