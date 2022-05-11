/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTOfflineTrack.h"
#include <iostream>

ClassImp(HTTOfflineTrack)

HTTOfflineTrack::HTTOfflineTrack() :
  m_qoverpt(0.), m_eta(0.), m_phi(0.), m_d0(0.), m_z0(0.),
  m_barcode(-1), m_barcode_frac(0.)
{}

std::ostream& operator<<(std::ostream& s, const HTTOfflineTrack& offline_t) {

  s << "pt: " << offline_t.getPt() << ", "
    << "eta: " << offline_t.getEta() << ", "
    << "phi: " << offline_t.getPhi() << ", "
    << "d0: " << offline_t.getD0() << ", "
    << "z0: " << offline_t.getZ0() << ", "
    << "qoverpt: " << offline_t.getQOverPt() << ", "
    << "barcode: " << offline_t.getBarcode() << ", "
    << "barcode fraction: " << offline_t.getBarcodeFrac() << std::endl;

  std::vector<HTTOfflineHit> hits = offline_t.getOfflineHits();
  for (int j = 0; j < offline_t.nHits(); j++) {
    s << " " << j << "  " << hits[j] << "\n";
  }
  s << std::endl;

  return s;
}
