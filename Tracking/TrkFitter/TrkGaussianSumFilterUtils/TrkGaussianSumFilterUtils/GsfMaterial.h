/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

/**
 * @author Anthony Morley, Christos Anastopoulos
 * @brief  Collect Structs we use for material effects
 * in GSF and their meaning in one place
 */
#ifndef GSFMATERIAL_H
#define GSFMATERIAL_H

#include "TrkGaussianSumFilterUtils/GsfConstants.h"
//
#include "EventPrimitives/EventPrimitives.h"
//
#include <array>

namespace GsfMaterial {

/* Helper struct for energy loss effects */
struct EnergyLoss
{
  struct element
  {
    double weight = 0;
    double deltaP = 0;
    double deltaQOvePCov = 0;
  };
  std::array<element, GSFConstants::maxNumberofBHComponents> elements = {};
  int numElements = 0;
};
/** Helper struct for multiple scattering effects*/
struct Scattering
{
  double deltaThetaCov = 0;
  double deltaPhiCov = 0;
  void reset()
  {
    deltaThetaCov = 0;
    deltaPhiCov = 0;
  }
};

/** Helper struct for combined material effects*/
struct Combined
{
  std::array<double, GSFConstants::maxNumberofBHComponents> weights = {};
  std::array<double, GSFConstants::maxNumberofBHComponents> deltaPs = {};
  alignas(GSFConstants::alignment) std::array<
    AmgVector(5),
    GSFConstants::maxNumberofBHComponents> deltaParameters = {};
  alignas(GSFConstants::alignment) std::array<
    AmgSymMatrix(5),
    GSFConstants::maxNumberofBHComponents> deltaCovariances = {};

  size_t numWeights = 0;
  size_t numDeltaPs = 0;
  size_t numDeltaParameters = 0;
  size_t numDeltaCovariance = 0;
  void reset()
  {
    numWeights = 0;
    numDeltaPs = 0;
    numDeltaParameters = 0;
    numDeltaCovariance = 0;
  }

  void resetAndAddDummyValues()
  {
    weights[0] = 1;
    deltaPs[0] = 0;
    deltaParameters[0] = AmgVector(5)::Zero();
    deltaCovariances[0] = AmgSymMatrix(5)::Zero();
    numWeights = 1;
    numDeltaPs = 1;
    numDeltaParameters = 1;
    numDeltaCovariance = 1;
  }
};

} // end namespace GSFMaterial

#endif

