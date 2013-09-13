/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURE_TLP1_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURE_TLP1_H

#include <stdint.h>
#include <string>
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p1.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p2.h"
#include "TrigMuonEventTPCnv/TileMuFeature_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p1.h"


class TileTrackMuFeature_tlp1 {

 public:
  TileTrackMuFeature_tlp1() {}
 

  std::vector<TileTrackMuFeature_p1>       m_TileTrackMu;
  std::vector<TileTrackMuFeature_p2>       m_TileTrackMu_p2;
  std::vector<TileMuFeature_p1>		   m_TileMu;
  std::vector<TrigInDetTrack_p1>           m_Track;
  std::vector<TrigInDetTrackFitPar_p1>     m_TrackFitPar;
};

#endif
