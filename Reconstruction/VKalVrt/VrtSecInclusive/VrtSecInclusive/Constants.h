/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _VrtSecInclusive_Constants_H
#define _VrtSecInclusive_Constants_H

#include <cmath>

namespace VKalVrtAthena {
  
  namespace PhysConsts {
    constexpr double mass_chargedPion = 139.57018;
    constexpr double mass_electron    = 0.511;
    constexpr double mass_proton      = 938.27205;
  }
  
  namespace AlgConsts {
    constexpr double infinitesimal            =  1.e-9;
    constexpr double maxValue                 =  1.e10;
    constexpr double chi2PerTrackInitValue    =  1.e5;
    constexpr double minVertexChi2Probability =  1.e-3;
    constexpr double invalidFloat             = -9999.;
    constexpr int    invalidInt               = -9999;
    constexpr unsigned invalidUnsigned        =  9999;
  }
}


#endif
