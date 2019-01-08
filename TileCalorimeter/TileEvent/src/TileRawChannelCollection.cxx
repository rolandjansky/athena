/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/src/TileRawChannelCollection.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Out-of-line implementations for TileRawChannelCollection.
 */


#include "TileEvent/TileRawChannelCollection.h"


/**
 * @brief Copy constructor.
 * @param other Collection to copy.
 * Does a deep copy of the contents.
 */
TileRawChannelCollection::TileRawChannelCollection
  (const TileRawChannelCollection& other)
    : TileRawDataCollection<TileRawChannel> (other.identify()),
      m_FragGlobalCRC   (other.m_FragGlobalCRC),
      m_FragDSPBCID     (other.m_FragDSPBCID),
      m_FragBCID        (other.m_FragBCID),
      m_FragMemoryPar   (other.m_FragMemoryPar),
      m_FragSstrobe     (other.m_FragSstrobe),
      m_FragDstrobe     (other.m_FragDstrobe),
      m_FragHeaderBit   (other.m_FragHeaderBit),
      m_FragHeaderPar   (other.m_FragHeaderPar),
      m_FragSampleBit   (other.m_FragSampleBit),
      m_FragSamplePar   (other.m_FragSamplePar),
      m_FragFEChipMask  (other.m_FragFEChipMask),
      m_FragRODChipMask (other.m_FragRODChipMask)
{
  // Copy flags from the base class.
  setLvl1Id          (other.getLvl1Id());
  setLvl1Type        (other.getLvl1Type());
  setDetEvType       (other.getDetEvType());
  setRODBCID         (other.getRODBCID());

  // Copy the collection contents.
  reserve (other.size());
  for (const TileRawChannel* chan : other) {
    push_back (std::make_unique<TileRawChannel> (*chan));
  }
}


void TileRawChannelCollection::clear()
{
  m_FragGlobalCRC   = 0;
  m_FragBCID        = 0;
  m_FragMemoryPar   = 0;
  m_FragSstrobe     = 0;
  m_FragDstrobe     = 0;
  m_FragHeaderBit   = 0;
  m_FragHeaderPar   = 0;
  m_FragSampleBit   = 0;
  m_FragSamplePar   = 0;
  m_FragFEChipMask  = 0xFFFF;
  m_FragRODChipMask = 0xFFFF;
  m_FragDSPBCID     = 0xDEAD;

  TileRawChannelCollection::MyBase::clear();
}

