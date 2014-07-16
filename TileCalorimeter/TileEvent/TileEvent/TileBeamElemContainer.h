/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileBeamElemContainer_H
#define TileBeamElemContainer_H

#include "TileEvent/TileRawDataContainer.h"
#include "TileEvent/TileBeamElemCollection.h"

class TileBeamElemContainer : 
  public TileRawDataContainer<TileBeamElemCollection> 
{
public:

  TileBeamElemContainer(bool createColl=false, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS) 
    : TileRawDataContainer<TileBeamElemCollection> (createColl, TileFragHash::Beam,
                                                    TileRawChannelUnit::ADCcounts, ownPolicy) { }

  ~TileBeamElemContainer() { }
};

CLASS_DEF(TileBeamElemContainer,2935,0)

class TileBeamElemCollectionVec : 
  public TileRawDataCollectionVec<TileBeamElemCollection> 
{
public:

  TileBeamElemCollectionVec() 
    : TileRawDataCollectionVec<TileBeamElemCollection> () { }  

  ~TileBeamElemCollectionVec() { }
};

#endif

