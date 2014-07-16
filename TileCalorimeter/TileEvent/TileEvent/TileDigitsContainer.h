/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileDigitsContainer_H
#define TileDigitsContainer_H

#include "TileEvent/TileRawDataContainer.h"
#include "TileEvent/TileDigitsCollection.h"

class TileDigitsContainer : 
  public TileRawDataContainer<TileDigitsCollection> 
{
public:

  TileDigitsContainer(bool createColl=false, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS) 
    : TileRawDataContainer<TileDigitsCollection> (createColl, TileFragHash::Digitizer, 
                                                  TileRawChannelUnit::ADCcounts, ownPolicy) { }

  ~TileDigitsContainer() { }
};

CLASS_DEF(TileDigitsContainer,2925,0)

class TileDigitsCollectionVec : 
  public TileRawDataCollectionVec<TileDigitsCollection> 
{
public:

  TileDigitsCollectionVec()
    : TileRawDataCollectionVec<TileDigitsCollection> () { }
  
  ~TileDigitsCollectionVec() { }
};

#endif

