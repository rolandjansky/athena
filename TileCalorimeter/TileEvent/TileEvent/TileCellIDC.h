/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileCellIDC_H
#define TileCellIDC_H

#include "TileEvent/TileRawDataContainer.h"
#include "TileEvent/TileCellCollection.h"

class TileCellIDC : 
  public TileRawDataContainer<TileCellCollection> 
{
public:
  
  // ATTENTION !!!
  // TileCellIDC is not owner of TileCells by default !!!
  TileCellIDC(bool createColl=false, SG::OwnershipPolicy ownPolicy=SG::VIEW_ELEMENTS) 
    : TileRawDataContainer<TileCellCollection> (createColl,ownPolicy) { }

  ~TileCellIDC() { }
};

CLASS_DEF(TileCellIDC,2929,0)

#endif

