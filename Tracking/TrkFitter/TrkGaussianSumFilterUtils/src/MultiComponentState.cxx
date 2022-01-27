/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  MultiComponentState.cxx
 * @date Sunday 8th May 2005
 * @author atkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Implementation code for MultiComponentState class
 */

#include "TrkGaussianSumFilterUtils/MultiComponentState.h"
//
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

Trk::MultiComponentState
Trk::MultiComponentStateHelpers::clone(const Trk::MultiComponentState& in)
{
  auto clonedState = Trk::MultiComponentState();
  clonedState.reserve(in.size());
  for (const ComponentParameters& component : in) {
    clonedState.emplace_back(component.first->clone(), component.second);
  }
  return clonedState;
}

Trk::MultiComponentState
Trk::MultiComponentStateHelpers::WithScaledError(
  Trk::MultiComponentState&& in,
  double errorScaleLocX,
  double errorScaleLocY,
  double errorScalePhi,
  double errorScaleTheta,
  double errorScaleQoverP)
{
  AmgSymMatrix(5) coefficients;
  coefficients(0, 0) = (errorScaleLocX * errorScaleLocX);
  coefficients(1, 1) = (errorScaleLocY * errorScaleLocY);
  coefficients(2, 2) = (errorScalePhi * errorScalePhi);
  coefficients(3, 3) = (errorScaleTheta * errorScaleTheta);
  coefficients(4, 4) = (errorScaleQoverP * errorScaleQoverP);
  coefficients.fillSymmetric(0, 1, (errorScaleLocX * errorScaleLocY));
  coefficients.fillSymmetric(0, 2, (errorScaleLocX * errorScalePhi));
  coefficients.fillSymmetric(0, 3, (errorScaleLocX * errorScaleTheta));
  coefficients.fillSymmetric(0, 4, (errorScaleLocX * errorScaleQoverP));
  coefficients.fillSymmetric(1, 2, (errorScaleLocY * errorScalePhi));
  coefficients.fillSymmetric(1, 3, (errorScaleLocY * errorScaleTheta));
  coefficients.fillSymmetric(1, 4, (errorScaleLocY * errorScaleQoverP));
  coefficients.fillSymmetric(2, 3, (errorScalePhi * errorScaleTheta));
  coefficients.fillSymmetric(2, 4, (errorScalePhi * errorScaleQoverP));
  coefficients.fillSymmetric(3, 4, (errorScaleTheta * errorScaleQoverP));

  for (ComponentParameters& component : in) {
    Trk::TrackParameters* trackParameters = component.first.get();
    const AmgSymMatrix(5)* originalMatrix = trackParameters->covariance();
    // If no covariance skip
    if (!originalMatrix) {
      continue;
    }
    AmgSymMatrix(5) newCovarianceMatrix =
      (*originalMatrix).cwiseProduct(coefficients);
    trackParameters->updateParameters(trackParameters->parameters(),
                                      newCovarianceMatrix);
  }
  return {std::move(in)};
}

bool
Trk::MultiComponentStateHelpers::isMeasured(const Trk::MultiComponentState& in)
{
  bool isMeasured = true;
  for (const ComponentParameters& component : in) {
    const AmgSymMatrix(5)* originalMatrix = component.first->covariance();
    if (!originalMatrix) {
      isMeasured = false;
      break;
    }
  }
  return isMeasured;
}

void
Trk::MultiComponentStateHelpers::renormaliseState(Trk::MultiComponentState& in,
                                                  double norm)
{
  // Determine total weighting of state
  double sumWeights = 0.;
  for (const ComponentParameters& component : in) {
    sumWeights += component.second;
  }
  if (sumWeights == 0) {
    return;
  }
  double normalise = norm / sumWeights;
  for (ComponentParameters& component : in) {
    component.second = component.second * normalise;
  }
}
