/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscLayerAttemptsCalculator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETY_DISCLAYERATTEMPTSCALCULATOR_H
#define TRKGEOMETY_DISCLAYERATTEMPTSCALCULATOR_H

#include "TrkGeometry/LayerAttemptsCalculator.h"
#include "TrkVolumes/BoundarySurfaceFace.h"

namespace Trk {

/** @class DiscLayerAttemptsCalculator

    Simple helper class for DiscLAyer static array

    @author Andreas.Salzburger@cern.ch
  */
class DiscLayerAttemptsCalculator final : public LayerAttemptsCalculator {
 public:
  /** Constructor */
  DiscLayerAttemptsCalculator(unsigned int faceAttempts,
                              unsigned int maxAttempts)
      : m_faceAttempts(faceAttempts), m_maxLayerAttempts(maxAttempts) {}

  /** one single interface method */
  virtual unsigned int layerAttempts(
      BoundarySurfaceFace bsFace = Trk::undefinedFace) const override final;

  /** The max attempts */
  virtual unsigned int maxLayerAttempts() const override  final;

 private:
  unsigned int m_faceAttempts;      //!< number of face attampts
  unsigned int m_maxLayerAttempts;  //!< the max attempt number
};

inline unsigned int DiscLayerAttemptsCalculator::layerAttempts(
    BoundarySurfaceFace bsFace) const {
  // if you go through the Inner/Outer cover only one attempt is allowed
  if (bsFace == negativeFaceXY || bsFace == positiveFaceXY)
    return m_faceAttempts;
  // allow the maximum
  return m_maxLayerAttempts;
}

inline unsigned int DiscLayerAttemptsCalculator::maxLayerAttempts() const {
  return m_maxLayerAttempts;
}
}  // namespace Trk

#endif
