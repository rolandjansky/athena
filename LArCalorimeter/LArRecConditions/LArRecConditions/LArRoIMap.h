// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArRecConditions/LArRoIMap.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2021
 * @brief Mapping between calorimeter trigger id to offline/online Identifier.
 *
 * Based on the old LArRoI_Map tool:
 * @author Hong Ma
 * @version 00-00-01 , 26/08/2002 
 */


#ifndef LARRECCONDITIONS_LARROIMAP_H
#define LARRECCONDITIONS_LARROIMAP_H


#include "Identifier/Identifier.h" 
#include "Identifier/HWIdentifier.h"
#include <vector>
#include <unordered_map>


/**
 * @brief Mapping between calorimeter trigger id to offline/online Identifier.
 */
class LArRoIMap
{
public:
  // TT_ID is TT hashId + sam coded in unsigned int,  (sam+ hash<<2)
  typedef unsigned int TT_ID; 
  typedef HWIdentifier  COLL_ID ; 

  
  /**
   * @brief Constructor.
   * @param layer_hash_max Maximum hash value.
   */
  LArRoIMap (TT_ID layer_hash_max);


  /**
   * @brief Translate offline channel identifier to trigger tower ID.
   */
  TT_ID trigTowerID (const Identifier& channel_id  ) const;


  /**
   * @brief Translate online channel identifier to trigger tower ID.
   */
  TT_ID trigTowerID (const HWIdentifier& id  ) const;


  /**
   * @brief Get list of robIds for a given trigger tower ID.
   */
  const std::vector<COLL_ID>& collectionID(const TT_ID& id) const ; 


  /**
   * @brief Check validity of a trigger tower ID.
   */
  bool validID(const TT_ID& tt_id) const ;



  /**
   * @brief Add a new entry to the map.
   * @param id Offline channel ID.
   * @param sigId Online channel ID.
   * @param rob ID.
   * @param hashid_sam Trigger tower ID.
   */
  void addEntry (const Identifier& id,
                 const HWIdentifier& sigId,
                 const HWIdentifier& robId,
                 TT_ID hashid_sam);


private:
  /// Maximum hash ID.
  TT_ID m_layer_hash_max;

  /// Map from offline ID to trigger tower ID.
  std::unordered_map<Identifier, TT_ID> m_offmap;

  /// Map from online ID to trigger tower ID.
  std::unordered_map<Identifier, TT_ID> m_onlmap;

  /// Map from trigger tower ID to list of ROBs.
  std::unordered_map<TT_ID, std::vector<COLL_ID> > m_roimap;
};


#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArRoIMap, 13658032, 1)
CONDCONT_DEF( LArRoIMap, 125932578 );


#endif // not LARRECCONDITIONS_LARROIMAP_H
