/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSCOLLECTION_TLP1_H
#define HIPIXELTRACKLETSCOLLECTION_TLP1_H
#include "HIRecTPCnv/HIPixelTrackletsCollection_p1.h"
#include "HIRecTPCnv/HIPixelTracklets_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3DCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p1.h"
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

#include <ios>

class HIPixelTrackletsCollection_tlp1 {
 public:
  HIPixelTrackletsCollection_tlp1(){}
  //
  TPCnvTokenList_p1 m_tokenList;
  
  std::vector<HIPixelTrackletsCollection_p1> m_trackletsCollections;
  std::vector<HIPixelTracklets_p1> m_tracklets;
  std::vector<HepPoint3DCnv_p1> m_hepPoint3Ds;
  std::vector<PixelClusterCnv_p1> m_pixelClusters;
  
};

#endif
