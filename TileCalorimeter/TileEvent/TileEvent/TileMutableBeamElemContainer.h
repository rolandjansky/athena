// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/TileMutableBeamElemContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Helper for holding non-const raw data prior to recording in SG.
 */


#ifndef TILEEVENT_TILEMUTABLEBEAMELEMCONTAINER_H
#define TILEEVENT_TILEMUTABLEBEAMELEMCONTAINER_H


#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileMutableDataContainer.h"


class TileMutableBeamElemContainer
  : public TileMutableDataContainer<TileBeamElemContainer>
{
public:
  typedef TileMutableDataContainer<TileBeamElemContainer> BASE;

  TileMutableBeamElemContainer(bool createColl=false,
                               SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS) 
    : BASE (createColl,
            TileFragHash::Beam,
            TileRawChannelUnit::ADCcounts,
            ownPolicy)
  {
  }

  
  TileMutableBeamElemContainer(bool createColl,
                               TYPE type,
                               UNIT unit=TileRawChannelUnit::ADCcounts,
                               SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS) 
    : BASE (createColl, type, 
            unit, ownPolicy)
  {
  }


  /**
   * @brief Copy constructor.
   * @param other Container to copy.
   *
   * This is a deep copy; all contained collections and channels will be copied.
   * Call status() after this to check for errors.
   */
  TileMutableBeamElemContainer (const BASE::BASE& other)
    : BASE (other)
  {
  }
};


#endif // not TILEEVENT_TILEMUTABLEBEAMELEMCONTAINER_H
