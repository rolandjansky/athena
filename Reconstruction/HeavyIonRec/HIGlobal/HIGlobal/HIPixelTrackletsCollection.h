/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSCOLLECTION_H
#define HIPIXELTRACKLETSCOLLECTION_H

#include "DataModel/DataVector.h"
#include "HIGlobal/HIPixelTracklets.h"
#include "CLIDSvc/CLASS_DEF.h"

//typedef DataVector<HIPixelTracklets> HIPixelTrackletsCollection;
class HIPixelTrackletsCollection : public DataVector<HIPixelTracklets>
{
 public:
  HIPixelTrackletsCollection(SG::OwnershipPolicy own = SG::OWN_ELEMENTS): DataVector<HIPixelTracklets>(own) {}
    
    virtual ~HIPixelTrackletsCollection() {}
    
    
};

CLASS_DEF(HIPixelTrackletsCollection,1088327300,1)

//Tell StoreGate that HIPixelTrackletsCollection derive from DV<HI..>
SG_BASE(HIPixelTrackletsCollection, DataVector<HIPixelTracklets>);

#endif
  
