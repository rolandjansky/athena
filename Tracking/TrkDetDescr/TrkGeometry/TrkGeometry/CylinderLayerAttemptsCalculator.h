/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderLayerAttemptsCalculator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETY_CYLINDERLAYERATTEMPTSCALCULATOR_H
#define TRKGEOMETY_CYLINDERLAYERATTEMPTSCALCULATOR_H

#include "TrkGeometry/LayerAttemptsCalculator.h"
#include "TrkVolumes/BoundarySurfaceFace.h"

namespace Trk {

/** @class CylinderLayerAttemptsCalculator

    Simple helper class for CylinderLayer static array

    @author Andreas.Salzburger@cern.ch
  */

class CylinderLayerAttemptsCalculator final: public LayerAttemptsCalculator {
 public:
  /** Constructor */
  CylinderLayerAttemptsCalculator(unsigned int faceAttempts,
                                  unsigned int maxAttempts)
      : m_faceAttempts(faceAttempts), m_maxLayerAttempts(maxAttempts) {}

  /** one single interface method */
  virtual unsigned int layerAttempts(
      BoundarySurfaceFace bsFace = Trk::undefinedFace) const override final;

  /** The max attempts */
  virtual unsigned int maxLayerAttempts() const override;

 private:
  unsigned int m_faceAttempts;      //!< number of face attampts
  unsigned int m_maxLayerAttempts;  //!< the max attempt number
};

inline unsigned int CylinderLayerAttemptsCalculator::layerAttempts(
    BoundarySurfaceFace bsFace) const {
  // if you go through the Inner/Outer cover only one attempt is allowed
  if (bsFace == tubeInnerCover || bsFace == tubeOuterCover)
    return m_faceAttempts;
  // allow the maximum
  return m_maxLayerAttempts;
}

inline unsigned int CylinderLayerAttemptsCalculator::maxLayerAttempts() const {
  return m_maxLayerAttempts;
}
}  // namespace Trk

#endif
