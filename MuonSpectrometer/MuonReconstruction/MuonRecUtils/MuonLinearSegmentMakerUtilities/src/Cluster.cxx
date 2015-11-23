/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
////// Cluster.cxx (c) ATLAS Detector software
////// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
///////////////////////////////////////////////////////////////////////////
#include "MuonLinearSegmentMakerUtilities/ClusterNtuple.h"
#include <iostream>

namespace ClusterSeg { 

  Cluster::Cluster( double x_, double y_, double z_, bool isPhi_, Muon::MuonStationIndex::TechnologyIndex tIndex_, Muon::MuonStationIndex::PhiIndex pIndex_, bool isMatch_, int barcode_) : m_x(x_),m_y(y_),m_z(z_),m_isPhi(isPhi_),m_tIndex(tIndex_),m_pIndex(pIndex_),m_isMatch(isMatch_),m_barcode(barcode_),m_chamberId("") {}
  Cluster::Cluster( double x_, double y_, double z_, bool isPhi_, int tIndex_, int pIndex_, bool isMatch_, int barcode_) : m_x(x_),m_y(y_),m_z(z_),m_isPhi(isPhi_),m_tIndex((Muon::MuonStationIndex::TechnologyIndex)tIndex_),m_pIndex((Muon::MuonStationIndex::PhiIndex)pIndex_),m_isMatch(isMatch_),m_barcode(barcode_) {}

  SpacePoint::SpacePoint( double eta_, double phi_, double z_, Muon::MuonStationIndex::TechnologyIndex tIndex_, Muon::MuonStationIndex::PhiIndex pIndex_, bool isMatch_, int barcode_, int eit_, int pit_) : m_eta(eta_),m_phi(phi_),m_z(z_),m_tIndex(tIndex_),m_pIndex(pIndex_),m_isMatch(isMatch_),m_barcode(barcode_),m_eit(eit_),m_pit(pit_) {}

}
