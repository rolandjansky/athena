/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSCOLLECTION_TLP1_H
#define HIPIXELTRACKLETSCOLLECTION_TLP1_H

#include "HIRecTPCnv/HIPixelTrackletsCollection_p1.h"
#include "HIRecTPCnv/HIPixelTracklets_p1.h"

class HIPixelTrackletsCollection_tlp1 {
 public:
  HIPixelTrackletsCollection_tlp1(){}
  
  std::vector<HIPixelTrackletsCollection_p1> m_trackletsCollections;
  std::vector<HIPixelTracklets_p1> m_tracklets;
};

#endif
