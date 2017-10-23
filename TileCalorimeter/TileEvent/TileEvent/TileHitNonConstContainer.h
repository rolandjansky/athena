// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file TileEvent/TileHitNonConstContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @brief Helper class for building a TileHitContainer.
 */


#ifndef TILEEVENT_TILEHITNONCONSTCONTAINER_H
#define TILEEVENT_TILEHITNONCONSTCONTAINER_H


#include "TileEvent/TileRawDataNonConstContainer.h"
#include "TileEvent/TileHitCollection.h"


/**
 * @brief This is a minimal version of a @c TileHitContainer in which
 *        the saved collections remain non-const.
 *
 * This can be used as a temporary while building the container.
 * Once everything's built, the collections can then be moved to the
 * final @c TileHitContainer, after which they'll be const.
 */
class TileHitNonConstContainer
  : public TileRawDataNonConstContainer<TileHitCollection>
{
public:
  using TileRawDataNonConstContainer::TileRawDataNonConstContainer;
};


#endif // not TILEEVENT_TILEHITNONCONSTCONTAINER_H
