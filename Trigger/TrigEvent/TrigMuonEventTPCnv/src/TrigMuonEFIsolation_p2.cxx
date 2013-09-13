/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p2.h"

/**
 * Default constructor.
 * Initialize everything to default values.
 */
TrigMuonEFIsolation_p2::TrigMuonEFIsolation_p2() : m_trkPos(-1), m_muonEFLink() {

  allTheFloats[0] = -1.0;
  allTheFloats[1] = -1.0;
  allTheFloats[2] = -1.0;
  allTheFloats[3] = -1.0;
  allTheFloats[4] = -1.0;
  allTheFloats[5] = -1.0;

}
