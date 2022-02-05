// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileEvent/TileMutableDigitsContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Helper for holding non-const raw data prior to recording in SG.
 */


#ifndef TILEEVENT_TILEMUTABLEDIGITSCONTAINER_H
#define TILEEVENT_TILEMUTABLEDIGITSCONTAINER_H


#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileMutableDataContainer.h"


typedef TileMutableDataContainer<TileDigitsContainer> TileMutableDigitsContainer;


#endif // not TILEEVENT_TILEMUTABLEDIGITSCONTAINER_H
