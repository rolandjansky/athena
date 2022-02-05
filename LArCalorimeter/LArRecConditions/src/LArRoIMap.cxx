/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArRecConditions/src/LArRoIMap.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2021
 * @brief Mapping between calorimeter trigger id to offline/online Identifier.
 *
 * Based on the old LArRoI_Map tool:
 * @author Hong Ma
 * @version 00-00-01 , 26/08/2002 
 */


#include "LArRecConditions/LArRoIMap.h"


/**
 * @brief Constructor.
 * @param layer_hash_max Maximum hash value.
 */
LArRoIMap::LArRoIMap (TT_ID layer_hash_max)
  : m_layer_hash_max (layer_hash_max)
{
}


/**
 * @brief Translate offline channel identifier to trigger tower ID.
 */
LArRoIMap::TT_ID LArRoIMap::trigTowerID (const Identifier& channel_id) const
{
  auto it = m_offmap.find (channel_id);
  if (it == m_offmap.end()) {
    return m_layer_hash_max + 1;
  }
  return it->second;
}


/**
 * @brief Translate online channel identifier to trigger tower ID.
 */
LArRoIMap::TT_ID LArRoIMap::trigTowerID (const HWIdentifier& id) const
{
  auto it = m_onlmap.find (id);
  if (it == m_onlmap.end()) {
    return m_layer_hash_max + 1;
  }
  return it->second;
}


/**
 * @brief Get list of robIds for a given trigger tower ID.
 */
const std::vector<LArRoIMap::COLL_ID>&
LArRoIMap::collectionID (const TT_ID& RoIid) const 
{
  auto it = m_roimap.find(RoIid); 
  if(it !=m_roimap.end()) {
    return it->second; 
  }

  // It is expected to have request that may not be in the map. 
  // Not all TTs have all samplings.  For example, at eta>1.8, there is no 
  // presampler. 

  static const std::vector<COLL_ID> v ; 
  return v; 
}


/**
 * @brief Check validity of a trigger tower ID.
 */
bool LArRoIMap::validID (const TT_ID& RoIid ) const 
{
  return m_roimap.find (RoIid) != m_roimap.end();
}


/**
 * @brief Add a new entry to the map.
 * @param id Offline channel ID.
 * @param sigId Online channel ID.
 * @param rob ID.
 * @param hashid_sam Trigger tower ID.
 */
void LArRoIMap::addEntry (const Identifier& id,
                          const HWIdentifier& sigId,
                          const HWIdentifier& robId,
                          TT_ID hashid_sam)
{
  m_offmap[id] = hashid_sam;
  m_onlmap[sigId] = hashid_sam;
  std::vector<COLL_ID>& v = m_roimap[hashid_sam]; 
  if (std::find (v.begin(), v.end(), robId) == v.end()) {
    v.push_back (robId);
  }
}
