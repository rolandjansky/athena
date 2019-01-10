/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/src/TileBeamElemCollection.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Out-of-line implementations for TileBeamElemCollection.
 */


#include "TileEvent/TileBeamElemCollection.h"


/**
 * @brief Copy constructor.
 * @param other Collection to copy.
 * Does a deep copy of the contents.
 */
TileBeamElemCollection::TileBeamElemCollection
  (const TileBeamElemCollection& other)
    : TileRawDataCollection<TileBeamElem> (other.identify())
{
  // Copy flags from the base class.
  setLvl1Id          (other.getLvl1Id());
  setLvl1Type        (other.getLvl1Type());
  setDetEvType       (other.getDetEvType());
  setRODBCID         (other.getRODBCID());

  // Copy the collection contents.
  reserve (other.size());
  for (const TileBeamElem* chan : other) {
    push_back (std::make_unique<TileBeamElem> (*chan));
  }
}
