/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_boosttocenterofmass_header
#define jetsubstructureutils_boosttocenterofmass_header

#include <vector>
#include "fastjet/PseudoJet.hh"
namespace JetSubStructureUtils {
  std::vector<fastjet::PseudoJet> boostToCenterOfMass(fastjet::PseudoJet jet,
                                                 std::vector<fastjet::PseudoJet> constituents);
}

#endif
