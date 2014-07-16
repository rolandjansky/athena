/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileRawChannelContainer_H
#define TileRawChannelContainer_H

#include "TileEvent/TileRawDataContainer.h"
#include "TileEvent/TileRawChannelCollection.h"

class TileRawChannelContainer : 
  public TileRawDataContainer<TileRawChannelCollection> 
{
public:
  
  TileRawChannelContainer(bool createColl=false,
                          TYPE type=TileFragHash::Default,
                          UNIT unit=TileRawChannelUnit::ADCcounts,
                          SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS) 
    : TileRawDataContainer<TileRawChannelCollection> (createColl,type,unit,ownPolicy) { }

  TileRawChannelContainer(bool createColl,
                          int type,
                          int unit,
                          SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS) 
    : TileRawDataContainer<TileRawChannelCollection> (createColl,(TYPE)type,(UNIT)unit,ownPolicy) { }

  ~TileRawChannelContainer() { }
};

CLASS_DEF(TileRawChannelContainer,2927,0)

class TileRawChannelCollectionVec : 
  public TileRawDataCollectionVec<TileRawChannelCollection> 
{
public:

  TileRawChannelCollectionVec()
    : TileRawDataCollectionVec<TileRawChannelCollection> () { }
 
  ~TileRawChannelCollectionVec() { }
};

#endif

