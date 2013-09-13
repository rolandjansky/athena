/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_TLP1_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_TLP1_H

#include <stdint.h>
#include <string>
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p2.h"
#include "TrigMuonEventTPCnv/MuonFeature_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p1.h"


class CombinedMuonFeature_tlp1 {

 public:
  CombinedMuonFeature_tlp1() {}
 

  std::vector<CombinedMuonFeature_p1>      m_combMu;
  std::vector<CombinedMuonFeature_p2>      m_combMu_p2;
  std::vector<MuonFeature_p1>              m_muFastTrack;
  std::vector<TrigInDetTrack_p1>           m_inDetTracks;
  std::vector<TrigInDetTrackFitPar_p1>     m_inDetTrackFitPar;

};

#endif
