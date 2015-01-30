/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSMICEVENTTPCNV_TRIGCOSMICEVENTTPCNVDICT_H
#define TRIGCOSMICEVENTTPCNV_TRIGCOSMICEVENTTPCNVDICT_H

#include "TrigCosmicEventTPCnv/CosmicMuon_p1.h"
#include "TrigCosmicEventTPCnv/CosmicMuonCollection_p1.h"
#include "TrigCosmicEventTPCnv/CosmicMuonCollection_tlp1.h"

#include "TrigCosmicEventTPCnv/MdtTrackSegment_p1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegment_p2.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_p1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_p2.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_tlp1.h"

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigCosmicEventTPCnvDict
{

  std::vector<CosmicMuon_p1>                  m_dummyCosmicMuon_p1;
  std::vector<MdtTrackSegment_p1>             m_dummyMdtTrackSegment_p1;
  std::vector<MdtTrackSegment_p2>             m_dummyMdtTrackSegment_p2;

};

#endif
