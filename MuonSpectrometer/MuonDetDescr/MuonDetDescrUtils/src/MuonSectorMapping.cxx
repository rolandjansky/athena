/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDetDescrUtils/MuonSectorMapping.h"

namespace Muon {

  const double MuonSectorMapping::s_oneEighthsOfPi = M_PI/8.;
  const double MuonSectorMapping::s_inverseOneEighthsOfPi = 1./s_oneEighthsOfPi;
  const double MuonSectorMapping::s_sectorSize[2] = { 0.4*s_oneEighthsOfPi,0.6*s_oneEighthsOfPi }; // sector side 
  const double MuonSectorMapping::s_sectorOverlap = 0.1*s_oneEighthsOfPi;
  
}
