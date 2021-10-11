/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINGPERFORMANCEFUNCTIONS_CXX
#define TRACKINGPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "TH2D.h"

namespace UpgradePerformance {
  // Forward declarations of these very long lookup tables
  // double ITKPercentageresol_YourFavoriteLayout(double ptMeV, double eta);
}


float UpgradePerformanceFunctions::getTrackPtResolution(float /*ptMeV*/, float /*eta*/) {
  using namespace UpgradePerformance;

  switch (m_layout ) {
    //case yourFavoriteLayout :
    //return ptMeV * ITKPercentageresol_YourFavoriteLayout(ptMeV, std::abs(eta));
    //break;
  default :
    std::cout << "ERROR getTrackPtResolution: No recent ITk layout supported yet. Please get in touch with Upgrade Tracking group to get updated numbers (atlas-cp-tracking-upgrade@cern.ch)" << std::endl;
    break;
  }
  return 0.;
}

float UpgradePerformanceFunctions::getTrackInvPtResolution(float /*invptMeV*/, float /*eta*/) {
  using namespace UpgradePerformance;
  switch (m_layout ) {
    //case yourFavoriteLayout :
    //return invptMeV * ITKPercentageresol_YourFavoriteLayout(1./invptMeV, std::abs(eta));
    //break;
  default :
    std::cout << "ERROR getTrackInvPtResolution: No recent ITk layout supported yet. Please get in touch with Upgrade Tracking group to get updated numbers (atlas-cp-tracking-upgrade@cern.ch)" << std::endl;
    break;
  }
  return 0.;
}

float UpgradePerformanceFunctions::getTrackPtRelativeResolution(float /*ptMeV*/, float /*eta*/) {
  using namespace UpgradePerformance;
  switch (m_layout ) {
    //case yourFavoriteLayout :
    //return ITKPercentageresol_YourFavoriteLayout(ptMeV, std::abs(eta));
    //break;
  default :
    std::cout << "ERROR getTrackPtRelativeResolution: No recent ITk layout supported yet. Please get in touch with Upgrade Tracking group to get updated numbers (atlas-cp-tracking-upgrade@cern.ch)" << std::endl;
    break;
  }
  return 0.;
}


#endif
