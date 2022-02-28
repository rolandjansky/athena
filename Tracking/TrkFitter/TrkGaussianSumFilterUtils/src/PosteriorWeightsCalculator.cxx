/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   PosteriorWeightsCalculator.cxx
 * @date   Wednesday 22nd December 2004
 * @author Tom Atkinson Anthony Morley, Christos Anastopoulos
 *
 * Implementation of PosteriorWeightsCalculator.cxx
 */
#include "TrkGaussianSumFilterUtils/PosteriorWeightsCalculator.h"
#include "TrkGaussianSumFilterUtils/GsfConstants.h"
//
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ProjectionMatricesSet.h"
#include "TrkParameters/TrackParameters.h"
//
#include <array>
#include <stdexcept>
namespace {

using namespace Trk;
// expansion and reduction matrices set
const ProjectionMatricesSet reMatrices(5);

template<int DIM>
std::pair<double, double>
calculateWeight_T(
  const TrackParameters* componentTrackParameters,
  const AmgSymMatrix(5) * predictedCov,
  const AmgVector(DIM) & measPar,
  const AmgSymMatrix(DIM) & measCov,
  int paramKey)
{
  // Define the expansion matrix
  const AmgMatrix(DIM, 5) H =
    reMatrices.expansionMatrix(paramKey).block<DIM, 5>(0, 0);

  // Calculate the residual
  AmgVector(DIM) r = measPar - H * componentTrackParameters->parameters();
  // Residual covariance. Posterior weights is calculated used predicted state
  // and measurement. Therefore add covariances
  AmgSymMatrix(DIM) R(measCov + H * (*predictedCov) * H.transpose());
  // compute determinant of residual
  const double det = R.determinant();
  if (det == 0) {
    return {0, 0};
  }
  // Compute Chi2
  return std::pair<double, double>(
    det, (1. / (double)DIM) * ((r.transpose() * R.inverse() * r)(0, 0)));
}

std::pair<double, double>
calculateWeight_1D(
  const TrackParameters* componentTrackParameters,
  const AmgSymMatrix(5) * predictedCov,
  const double measPar,
  const double measCov,
  int paramKey)
{
  // use measuring coordinate (variable "mk") instead of reduction matrix
  int mk = 0;
  if (paramKey != 1) {
    for (int i = 0; i < 5; ++i) {
      if (paramKey & (1 << i)) {
        mk = i;
        break;
      }
    }
  }

  // Calculate the residual
  const double r = measPar - (componentTrackParameters->parameters())(mk);
  // Residual covariance. Posterior weights is calculated used predicted state
  // and measurement. Therefore add covariances
  const double R = measCov + (*predictedCov)(mk, mk);
  if (R == 0) {
    return {0, 0};
  }
  // Compute Chi2
  return {R, r * r / R};
}

std::pair<double, double>
calculateWeight_2D_3(
  const TrackParameters* componentTrackParameters,
  const AmgSymMatrix(5) * predictedCov,
  const AmgVector(2) & measPar,
  const AmgSymMatrix(2) & measCov)
{
  // Calculate the residual
  AmgVector(2) r =
    measPar - componentTrackParameters->parameters().block<2, 1>(0, 0);
  // Residual covariance. Posterior weights is calculated used predicted state
  // and measurement. Therefore add covariances
  AmgSymMatrix(2) R(measCov + predictedCov->block<2, 2>(0, 0));
  // compute determinant of residual
  const double det = R.determinant();
  if (det == 0) {
    return {0, 0};
  }
  // Compute Chi2
  return { det, 0.5 * ((r.transpose() * R.inverse() * r)(0, 0)) };
}

struct componentsCache
{
  struct element
  {
    double determinantR;
    double chi2;
  };
  std::array<element, GSFConstants::maxComponentsAfterConvolution> elements{};
  size_t numElements = 0;
};

} // end of anonymous namespace

MultiComponentState
Trk::PosteriorWeightsCalculator::weights(
  MultiComponentState&& predictedState,
  const MeasurementBase& measurement)
{

  const size_t predictedStateSize = predictedState.size();
  if (predictedStateSize == 0) {
    return {};
  }
  if (predictedStateSize > GSFConstants::maxComponentsAfterConvolution) {
    throw std::runtime_error(
      "PosteriorWeightsCalculator :Invalid predictedState size");
  }
  const Trk::LocalParameters& measurementLocalParameters =
    measurement.localParameters();
  int nLocCoord = measurement.localParameters().dimension();
  if (nLocCoord < 1 || nLocCoord > 5) {
    return {};
  }

  // Move  to output and update
  MultiComponentState returnMultiComponentState = std::move(predictedState);

  // Calculate chi2 and determinant of each component.
  componentsCache determinantRandChi2{};
  double minimumChi2(10.e10); // Initalise high
  // Loop over all components
  for (const auto& component : returnMultiComponentState) {

    const Trk::TrackParameters* componentTrackParameters =
      component.first.get();
    if (!componentTrackParameters) {
      continue;
    }
    const AmgSymMatrix(5)* predictedCov =
      componentTrackParameters->covariance();
    if (!predictedCov) {
      continue;
    }

    std::pair<double, double> result(0, 0);
    switch (nLocCoord) {
      case 1: {
        result = calculateWeight_1D(
          componentTrackParameters,
          predictedCov,
          measurementLocalParameters(0),
          measurement.localCovariance()(0, 0),
          measurementLocalParameters.parameterKey());
      } break;
      case 2: {
        if (measurementLocalParameters.parameterKey() == 3) {
          result = calculateWeight_2D_3(
            componentTrackParameters,
            predictedCov,
            measurementLocalParameters.block<2, 1>(0, 0),
            measurement.localCovariance().block<2, 2>(0, 0));
        } else {
          result = calculateWeight_T<2>(
            componentTrackParameters,
            predictedCov,
            measurementLocalParameters.block<2, 1>(0, 0),
            measurement.localCovariance().block<2, 2>(0, 0),
            measurementLocalParameters.parameterKey());
        }
      } break;
      case 3: {
        result = calculateWeight_T<3>(
          componentTrackParameters,
          predictedCov,
          measurementLocalParameters.block<3, 1>(0, 0),
          measurement.localCovariance().block<3, 3>(0, 0),
          measurementLocalParameters.parameterKey());
      } break;
      case 4: {
        result = calculateWeight_T<4>(
          componentTrackParameters,
          predictedCov,
          measurementLocalParameters.block<4, 1>(0, 0),
          measurement.localCovariance().block<4, 4>(0, 0),
          measurementLocalParameters.parameterKey());
      } break;
      case 5: {
        result = calculateWeight_T<5>(
          componentTrackParameters,
          predictedCov,
          measurementLocalParameters.block<5, 1>(0, 0),
          measurement.localCovariance().block<5, 5>(0, 0),
          measurementLocalParameters.parameterKey());
      } break;
      default: {
      }
    }

    if (result.first == 0) {
      continue;
    }
    // Cache R and Chi2
    determinantRandChi2.elements[determinantRandChi2.numElements] = {
      result.first, result.second
    };
    ++determinantRandChi2.numElements;
    if (result.second < minimumChi2) {
      minimumChi2 = result.second;
    }
  } // end loop over components

  // If something went wrong in the loop return empty
  if (determinantRandChi2.numElements != predictedStateSize) {
    return {};
  }

  // Calculate posterior weights.
  size_t index(0);
  double sumWeights(0.);
  std::array<double, GSFConstants::maxComponentsAfterConvolution>
    fallBackWeights{};
  auto componentItr = returnMultiComponentState.begin();
  for (; componentItr != returnMultiComponentState.end();
       ++componentItr, ++index) {
    // Extract common factor to avoid numerical problems during exponentiation
    double chi2 = determinantRandChi2.elements[index].chi2 - minimumChi2;
    const double priorWeight = componentItr->second;
    fallBackWeights[index] = priorWeight;
    double updatedWeight(0.);
    // Determinant can not be below 1e-19. Rather ugly but protect
    // against 0 determinants Normally occur when the component is a poor fit
    if (determinantRandChi2.elements[index].determinantR > 1e-20) {
      updatedWeight =
        priorWeight *
        sqrt(1. / determinantRandChi2.elements[index].determinantR) *
        exp(-0.5 * chi2);
    } else {
      updatedWeight = 1e-10;
    }
    componentItr->second = updatedWeight;
    sumWeights += updatedWeight;
  }
  if (sumWeights > 0.) {
    double invertSumWeights = 1. / sumWeights;
    // Renormalise the state to total weight = 1
    for (auto& returnComponent : returnMultiComponentState) {
      returnComponent.second *= invertSumWeights;
    }
  } else {
    // If the sum weights is less than 0 revert them back
    size_t fallbackIndex(0);
    for (auto& returnComponent : returnMultiComponentState) {
      returnComponent.second = fallBackWeights[fallbackIndex];
      ++fallbackIndex;
    }
  }
  return returnMultiComponentState;
}
