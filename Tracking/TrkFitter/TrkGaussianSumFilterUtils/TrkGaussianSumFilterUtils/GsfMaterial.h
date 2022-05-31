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

/** Helper struct for energy loss effects, multicomponent description*/
struct EnergyLoss
{
  struct element
  {
    double weight = 0;
    double deltaP = 0;
    double deltaQOvePCov = 0;
  };
  std::array<element, GSFConstants::maxNumberofMatComponents> elements = {};
  int numElements = 0;
};

/** Helper struct for multiple scattering effects single component description*/
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

/** Helper struct for combined material effects, multicomponent  description*/
struct Combined
{
  std::array<double, GSFConstants::maxNumberofMatComponents> weights = {};
  std::array<double, GSFConstants::maxNumberofMatComponents> deltaPs = {};
  alignas(GSFConstants::alignment) std::array<
    AmgVector(5),
    GSFConstants::maxNumberofMatComponents> deltaParameters = {};
  alignas(GSFConstants::alignment) std::array<
    AmgSymMatrix(5),
    GSFConstants::maxNumberofMatComponents> deltaCovariances = {};

  size_t numEntries = 0;

};

} // end namespace GSFMaterial

#endif

