// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/TileMutableHitContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Helper for holding non-const raw data prior to recording in SG.
 */


#ifndef TILEEVENT_TILEMUTABLEHITCONTAINER_H
#define TILEEVENT_TILEMUTABLEHITCONTAINER_H


#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileMutableDataContainer.h"


typedef TileMutableDataContainer<TileHitContainer> TileMutableHitContainer;


#endif // not TILEEVENT_TILEMUTABLEHITCONTAINER_H
