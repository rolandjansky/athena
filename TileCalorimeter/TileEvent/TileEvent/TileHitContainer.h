/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileHitContainer_H
#define TileHitContainer_H

#include "TileEvent/TileRawDataContainer.h"
#include "TileEvent/TileHitCollection.h"

class TileHitContainer : 
  public TileRawDataContainer<TileHitCollection> 
{
public:
  
  TileHitContainer(bool createColl=false, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS) 
    : TileRawDataContainer<TileHitCollection> (createColl, ownPolicy) { }

  ~TileHitContainer() { }
};

CLASS_DEF(TileHitContainer,2923,0)

class TileHitCollectionVec : 
  public TileRawDataCollectionVec<TileHitCollection> 
{
public:

  TileHitCollectionVec()
    : TileRawDataCollectionVec<TileHitCollection> () { }

  ~TileHitCollectionVec() { }
};

#endif

