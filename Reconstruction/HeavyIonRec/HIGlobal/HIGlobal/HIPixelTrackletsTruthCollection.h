/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSTRUTHCOLLECTION_H
#define HIPIXELTRACKLETSTRUTHCOLLECTION_H

#include "HIGlobal/HIPixelTrackletsTruthKey.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "HIGlobal/HIPixelTrackletsCollection.h"
#include "DataModel/DataLink.h"
#include "CLIDSvc/CLASS_DEF.h"

class HIPixelTrackletsTruthCollection: public std::map<HIPixelTrackletsTruthKey,HepMcParticleLink>
{
  DataLink<HIPixelTrackletsCollection> m_pixelTrackletsCollection;
  
 public:
  HIPixelTrackletsTruthCollection(const DataLink<HIPixelTrackletsCollection> tracklets): m_pixelTrackletsCollection(tracklets) {}
  DataLink<HIPixelTrackletsCollection> pixelTrackletsCollectionLink() const {return m_pixelTrackletsCollection; }
  HIPixelTrackletsTruthCollection() {}
};

CLASS_DEF(HIPixelTrackletsTruthCollection,1330033446,1)
#endif
