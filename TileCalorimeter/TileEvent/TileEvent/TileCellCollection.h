/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileCellCollection_H       
#define TileCellCollection_H       
                                         
#include "TileEvent/TileCell.h" 
#include "TileEvent/TileRawDataCollection.h"
                                         
class TileCellCollection : public TileRawDataCollection<TileCell>
{
public:

  typedef TileRawDataCollection<TileCell> MyBase;
  typedef MyBase::ID ID;
  typedef TileCell _TElement;

  // ATTENTION !!!
  // TileCollection is not owner of TileCells by default !!!
  TileCellCollection ( ID& id, SG::OwnershipPolicy ownPolicy=SG::VIEW_ELEMENTS )
    : MyBase(id,ownPolicy) { }
  TileCellCollection ( SG::OwnershipPolicy ownPolicy=SG::VIEW_ELEMENTS )
    : MyBase(ownPolicy) { }

  ~TileCellCollection ( ) { }
};

CLASS_DEF(TileCellCollection,2930,0)
           
#endif

