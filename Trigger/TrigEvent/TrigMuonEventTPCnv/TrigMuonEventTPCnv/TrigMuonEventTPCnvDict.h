/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEVENTTPCNVDICT_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEVENTTPCNVDICT_H

#include "TrigMuonEventTPCnv/MuonFeature_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_tlp1.h"

#include "TrigMuonEventTPCnv/MuonFeature_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_tlp2.h"

#include "TrigMuonEventTPCnv/MuonFeature_p3.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_p3.h"

#include "TrigMuonEventTPCnv/MuonFeatureDetails_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_tlp1.h"

#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetails_p2.h"

#include "TrigMuonEventTPCnv/CombinedMuonFeature_p1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p2.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p3.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p4.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_tlp1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p4.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_tlp1.h"

#include "TrigMuonEventTPCnv/IsoMuonFeature_tlp1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p2.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p3.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_tlp1.h"

#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainer_tlp1.h"

#include "TrigMuonEventTPCnv/TileMuFeature_p1.h"
#include "TrigMuonEventTPCnv/TileMuFeature_p2.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainer_tlp1.h"

#include "TrigMuonEventTPCnv/TileTrackMuFeature_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p2.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p3.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_tlp1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_tlp1.h"

#include "TrigMuonEventTPCnv/TrigMuonEFContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p1.h"

#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p2.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_tlp1.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_tlp2.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p4.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p6.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p7.h"

#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p2.h"

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigMuonEventTPCnvDict
{

  std::vector<MuonFeature_p1>	              m_dummyMuonFeat_p1;
  std::vector<MuonFeature_p2>	              m_dummyMuonFeat_p2;
  std::vector<MuonFeature_p3>	              m_dummyMuonFeat_p3;
  std::vector<MuonFeatureDetails_p1>          m_dummyMuonFeatureDetails_p1;
  std::vector<MuonFeatureDetails_p2>          m_dummyMuonFeatureDetails_p2;
  std::vector<mdthit>                         m_dummymdthit;
  std::vector<tgchit>                         m_dummytgchit;
  std::vector<padhit>                         m_dummypadhit;
  std::vector<CombinedMuonFeature_p1>	      m_dummyCombMuonFeat_p1;
  std::vector<CombinedMuonFeature_p2>	      m_dummyCombMuonFeat_p2;
  std::vector<TileMuFeature_p1>               m_dummyTileMuFeature_p1;
  std::vector<TileTrackMuFeature_p1>          m_dummyTileTrackMuFeat_p1;
  std::vector<TileTrackMuFeature_p2>          m_dummyTileTrackMuFeat_p2;
  std::vector<IsoMuonFeature_p1>	      m_dummyIsoMuonFeat_p1;
  std::vector<IsoMuonFeature_p2>	      m_dummyIsoMuonFeat_p2;
  std::vector<IsoMuonFeature_p3>	      m_dummyIsoMuonFeat_p3;
  std::vector<TrigMuonClusterFeature_p1>      m_dummyTrigMuonClusterFeat_p1;
  std::vector<TrigMuonEF_p1>                  m_dummyMuonEF_p1;
  std::vector<TrigMuonEF_p2>                  m_dummyMuonEF_p2;
  std::vector<TrigMuonEFCbTrack_p1>           m_dummyMuonEFCbTrack_p1;
  std::vector<TrigMuonEFCbTrack_p2>           m_dummyMuonEFCbTrack_p2;
  std::vector<TrigMuonEFCbTrack_p3>           m_dummyMuonEFCbTrack_p3;
  std::vector<TrigMuonEFCbTrack_p4>           m_dummyMuonEFCbTrack_p4;
  std::vector<TrigMuonEFCbTrack_p6>           m_dummyMuonEFCbTrack_p6;
  std::vector<TrigMuonEFCbTrack_p7>           m_dummyMuonEFCbTrack_p7;
  std::vector<TrigMuonEFTrack_p1>             m_dummyMuonEFTrack_p1;
  std::vector<TrigMuonEFTrack_p2>             m_dummyMuonEFTrack_p2;
  std::vector<TrigMuonEFTrack_p3>             m_dummyMuonEFTrack_p3;
  std::vector<TrigMuonEFTrack_p4>             m_dummyMuonEFTrack_p4;
  std::vector<TrigMuonEFTrack_p5>             m_dummyMuonEFTrack_p5;
  std::vector<TrigMuonEFTrack_p6>             m_dummyMuonEFTrack_p6;
  std::vector<TrigMuonEFInfo_p1>              m_dummyMuonEFInfo_p1;
  std::vector<TrigMuonEFInfo_p2>              m_dummyMuonEFInfo_p2;
};

#endif
