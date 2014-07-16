/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileBeamElemCollection_H       
#define TileBeamElemCollection_H       
                                         
#include "TileEvent/TileBeamElem.h" 
#include "TileEvent/TileRawDataCollection.h"
                                         
class TileBeamElemCollection : public TileRawDataCollection<TileBeamElem>
{
  friend class TileBeamElemContainerCnv;
public:

  typedef TileRawDataCollection<TileBeamElem> MyBase;
  typedef MyBase::ID ID;
  typedef TileBeamElem _TElement;

  TileBeamElemCollection (  ID& id, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS  ) 
    : MyBase(id,ownPolicy) { }
  TileBeamElemCollection ( SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS )
    : MyBase(ownPolicy) { }

  ~TileBeamElemCollection() { }
};

CLASS_DEF(TileBeamElemCollection,2936,0)
           
#endif

