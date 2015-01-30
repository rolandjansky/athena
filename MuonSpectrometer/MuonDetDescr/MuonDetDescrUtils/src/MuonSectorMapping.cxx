/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDetDescrUtils/MuonSectorMapping.h"

namespace Muon {

  double MuonSectorMapping::m_oneEightsOfPi = M_PI/8.;
  double MuonSectorMapping::m_inverseOneEightsOfPi = 1./m_oneEightsOfPi;
  double MuonSectorMapping::m_sectorSize[2] = { 0.4*m_oneEightsOfPi,0.6*m_oneEightsOfPi }; // sector side 
  double MuonSectorMapping::m_sectorOverlap = 0.1*m_oneEightsOfPi;
  bool MuonSectorMapping::m_debug = false;
  
}
