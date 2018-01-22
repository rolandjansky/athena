/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWCOLLECTIONHEAD_H
#define AFP_RAWCOLLECTIONHEAD_H

#include <stdint.h>

/// @brief Class represnting header record in bytestream
///
/// Header record in bytestream starts a new collection that is why this
/// class is meant to be base for the data collections.
class AFP_RawCollectionHead
{
public:
  /// @copydoc AFP_RawCollectionHead::m_lvl1Id
  uint16_t lvl1Id () const {return m_lvl1Id;}
  void setLvl1Id (const uint16_t lvl1Id) {m_lvl1Id = lvl1Id;}

  /// @copydoc AFP_RawCollectionHead::m_link
  uint16_t link () const {return m_link;}
  void setLink (const uint16_t link) {m_link = link;}

  /// @copydoc AFP_RawCollectionHead::m_frontendFlag
  uint32_t frontendFlag ()  const {return m_frontendFlag;};
  void setFrontendFlag (const uint32_t flag) {m_frontendFlag = flag;}

  /// @copydoc AFP_RawCollectionHead::m_bcId
  uint16_t bcId ()  const {return m_bcId;};
  void setBcId (const uint16_t bcId) {m_bcId = bcId;}

  /// @copydoc AFP_RawCollectionHead::m_robId
  uint32_t robId () const {return m_robId;}
  void setRobId (const uint32_t robId) {m_robId = robId;}
  
private:
  /// @brief Value of 11-15 least significant bits
  ///
  /// In the following record: `xxxx xxxx xxxx xxxx xLLL LLxx xxxx xxxx`
  /// it means bits marked with `L`.
  uint16_t m_lvl1Id;

  /// @brief Value of 5-8 most significant bits
  ///
  /// In the following record: `xxxx LLLL xxxx xxxx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `L`. Contains information about outlink.
  uint16_t m_link;
  
  /// @brief Value of the 16 least significant bit
  ///
  /// In the following record: `xxxx xxxx xxxx xxxx Fxxx xxxx xxxx xxxx`
  /// it means bits marked with `F`.
  uint32_t m_frontendFlag;
  
  /// @brief Value of 10 least significant bits
  ///
  /// In the following record: `xxxx xxxx xxxx xxxx xxxx xxBB BBBB BBBB`
  /// it means bits marked with `B`. Contains information about bunch crossing ID.
  uint16_t m_bcId;

  /// ROB in from which the collection was read
  uint32_t m_robId;
};

#endif 

