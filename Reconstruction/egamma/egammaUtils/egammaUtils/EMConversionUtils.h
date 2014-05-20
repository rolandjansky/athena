/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAUTILS_EMCONVERSIONUTILS_H
#define EGAMMAUTILS_EMCONVERSIONUTILS_H

#include "VxVertex/ExtendedVxCandidate.h"
#include "TrkNeutralParameters/NeutralParameters.h"

namespace EMConversionUtils {

  /** @brief Get the position and momentum of the conversion candidate  */
  void getConversionPositionAndPt(const Trk::VxCandidate*, Amg::Vector3D& p, double& pt);

  /** @brief Get the momentum of the conversion candidate  */
  double getConversionP(const Trk::VxCandidate*);

  /** @brief Get the position of the conversion candidate  */
  Amg::Vector3D getConversionPosition(const Trk::VxCandidate*);
  
  /** @brief Create neutral track parameters for the reconstructed converted photon.  */
  const Trk::NeutralPerigee* createPhotonParameters(const Trk::VxCandidate * myCandidate);
}

#endif
