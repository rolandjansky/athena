// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/TileMutableRawChannelContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Helper for holding non-const raw data prior to recording in SG.
 */


#ifndef TILEEVENT_TILEMUTABLERAWCHANNELCONTAINER_H
#define TILEEVENT_TILEMUTABLERAWCHANNELCONTAINER_H


#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileMutableDataContainer.h"


typedef TileMutableDataContainer<TileRawChannelContainer> TileMutableRawChannelContainer;


#endif // not TILEEVENT_TILEMUTABLERAWCHANNELCONTAINER_H
