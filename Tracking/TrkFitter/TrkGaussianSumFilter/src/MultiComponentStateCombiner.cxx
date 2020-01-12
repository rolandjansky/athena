/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      MultiComponentStateCombiner.cxx  -  description
      -----------------------------------------------
begin                : Monday 20th December 2004
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Implementation code for MultiComponentStateCombiner class
*********************************************************************************/

#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/MultiComponentStateModeCalculator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

Trk::MultiComponentStateCombiner::MultiComponentStateCombiner(const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_useMode(false)
  , m_useModeD0(true)
  , m_useModeZ0(true)
  , m_useModePhi(true)
  , m_useModeTheta(true)
  , m_useModeqOverP(true)
  , m_fractionPDFused(1.0)
{

  declareInterface<IMultiComponentStateCombiner>(this);
  declareProperty("UseMode", m_useMode, "Calculate mode for all mergers  (not recommended)");
  declareProperty("UseModeqOverP", m_useModeqOverP);
  declareProperty("UseModeD0", m_useModeD0);
  declareProperty("UseModeZ0", m_useModeZ0);
  declareProperty("UseModePhi", m_useModePhi);
  declareProperty("UseModeTheta", m_useModeTheta);
  declareProperty("FractionPDFused", m_fractionPDFused);
}

StatusCode
Trk::MultiComponentStateCombiner::initialize()
{

  if (m_fractionPDFused < 0.1) {
    ATH_MSG_INFO("Fraction of PDF is set too low begin reset to 1");
    m_fractionPDFused = 1;
  }

  if (m_fractionPDFused > 1) {
    ATH_MSG_INFO("Fraction of PDF is set high low begin reset to 1");
    m_fractionPDFused = 1;
  }

  ATH_MSG_VERBOSE("Initialisation of " << name() << " was successful");

  return StatusCode::SUCCESS;
}

StatusCode
Trk::MultiComponentStateCombiner::finalize()
{

  ATH_MSG_INFO("-----------------------------------------------");
  ATH_MSG_INFO("         GSF MCS Combiner  Statistics          ");
  ATH_MSG_INFO("-----------------------------------------------");
  ATH_MSG_INFO("Finalisation of " << name() << " was successful");

  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::TrackParameters>
Trk::MultiComponentStateCombiner::combine(const Trk::MultiComponentState& uncombinedState, bool useModeTemp) const
{
  std::unique_ptr<Trk::ComponentParameters> combinedComponent = compute(&uncombinedState, useModeTemp);
  return std::move(combinedComponent->first);
}

std::unique_ptr<Trk::ComponentParameters>
Trk::MultiComponentStateCombiner::combineWithWeight(const Trk::MultiComponentState& uncombinedState,
                                                    bool useModeTemp) const
{
  return compute(&uncombinedState, useModeTemp);
}

void
Trk::MultiComponentStateCombiner::combineWithWeight(Trk::ComponentParameters& mergeTo,
                                                    const Trk::ComponentParameters& addThis) const
{

  const Trk::TrackParameters* firstParameters = mergeTo.first.get();
  // Check to see if first track parameters are measured or not
  const AmgSymMatrix(5)* firstMeasuredCov = firstParameters->covariance();
  double firstWeight = mergeTo.second;

  const Trk::TrackParameters* secondParameters = addThis.first.get();
  // Check to see if first track parameters are measured or not
  const AmgSymMatrix(5)* secondMeasuredCov = secondParameters->covariance();
  double secondWeight = addThis.second;
  double totalWeight = firstWeight + secondWeight;

  AmgVector(5) mean;
  mean.setZero();

  AmgVector(5) parameters = secondParameters->parameters();

  // Ensure that we don't have any problems with the cyclical nature of phi
  // Use first state as reference poin
  double deltaPhi = firstParameters->parameters()[2] - parameters[2];

  if (deltaPhi > M_PI) {
    parameters[2] += 2 * M_PI;
  } else if (deltaPhi < -M_PI) {
    parameters[2] -= 2 * M_PI;
  }

  mean = firstWeight * firstParameters->parameters() + secondWeight * parameters;
  mean /= totalWeight;

  // Ensure that phi is between -pi and pi
  //
  if (mean[2] > M_PI) {
    mean[2] -= 2 * M_PI;
  } else if (mean[2] < -M_PI) {
    mean[2] += 2 * M_PI;
  }

  // Extract local error matrix: Must make sure track parameters are measured, ie have an associated error matrix.
  if (firstMeasuredCov && secondMeasuredCov) {
    AmgSymMatrix(5)* covariance = new AmgSymMatrix(5);
    AmgSymMatrix(5) covariancePart1;
    covariancePart1.setZero();
    AmgSymMatrix(5) covariancePart2;
    covariancePart2.setZero();

    covariancePart1 = firstWeight * (*firstMeasuredCov) + secondWeight * (*secondMeasuredCov);
    AmgVector(5) parameterDifference = firstParameters->parameters() - parameters;

    if (parameterDifference[2] > M_PI) {
      parameterDifference[2] -= 2 * M_PI;
    } else if (parameterDifference[2] < -M_PI) {
      parameterDifference[2] += 2 * M_PI;
    }
    AmgSymMatrix(5) unity;
    for (int i(0); i < 5; ++i) {
      for (int j(0); j < 5; ++j) {
        unity(i, j) = parameterDifference(i) * parameterDifference(j);
      }
    }
    covariancePart2 = firstWeight * secondWeight * unity;
    (*covariance) = covariancePart1 / totalWeight + covariancePart2 / (totalWeight * totalWeight);

    mergeTo.first->updateParameters(mean, covariance);
    mergeTo.second = totalWeight;
  } else {
    mergeTo.first->updateParameters(mean, 0);
    mergeTo.second = totalWeight;
  }
}

std::unique_ptr<Trk::ComponentParameters>
Trk::MultiComponentStateCombiner::compute(const Trk::MultiComponentState* uncombinedState, bool useModeTemp) const
{
  if (uncombinedState->empty()) {
    ATH_MSG_WARNING("Trying to collapse state with zero components");
    return nullptr;
  }

  const Trk::TrackParameters* firstParameters = uncombinedState->front().first.get();

  // Check to see if first track parameters are measured or not
  const AmgSymMatrix(5)* firstMeasuredCov = firstParameters->covariance();

  if (uncombinedState->size() == 1)
    return std::make_unique<Trk::ComponentParameters>(uncombinedState->front().first->clone(), 
                                                      uncombinedState->front().second);

  double sumW(0.);
  const int dimension = (uncombinedState->front()).first->parameters().rows();
  if (dimension != 5) {
    ATH_MSG_FATAL("More than 5 track parameters");
  }

  AmgVector(5) mean;
  mean.setZero();
  AmgSymMatrix(5)* covariance = new AmgSymMatrix(5);
  AmgSymMatrix(5) covariancePart1;
  covariancePart1.setZero();
  AmgSymMatrix(5) covariancePart2;
  covariancePart2.setZero();

  /* =============================
     Loop over all components
     ============================= */

  Trk::MultiComponentState::const_iterator component = uncombinedState->begin();
  double totalWeight(0.);
  for (; component != uncombinedState->end(); ++component) {
    double weight = (*component).second;
    totalWeight += weight;
  }

  component = uncombinedState->begin();

  for (; component != uncombinedState->end(); ++component) {

    const TrackParameters* trackParameters = (*component).first.get();
    double weight = (*component).second;

    AmgVector(5) parameters = trackParameters->parameters();

    // Ensure that we don't have any problems with the cyclical nature of phi
    // Use first state as reference poin
    double deltaPhi = (*uncombinedState->begin()).first->parameters()[2] - parameters[2];

    if (deltaPhi > M_PI) {
      parameters[2] += 2 * M_PI;
    } else if (deltaPhi < -M_PI) {
      parameters[2] -= 2 * M_PI;
    }

    sumW += weight;
    mean += weight * parameters;

    // Extract local error matrix: Must make sure track parameters are measured, ie have an associated error matrix.
    const AmgSymMatrix(5)* measuredCov = trackParameters->covariance();

    if (measuredCov) {
      // Changed from errorMatrixInMeasurementFrame

      covariancePart1 += weight * (*measuredCov);

      /* ============================================================================
         Loop over all remaining components to find the second part of the covariance
         ============================================================================ */

      Trk::MultiComponentState::const_iterator remainingComponentIterator = component;

      for (; remainingComponentIterator != uncombinedState->end(); ++remainingComponentIterator) {

        if (remainingComponentIterator == component)
          continue;

        AmgVector(5) parameterDifference = parameters - ((*remainingComponentIterator).first)->parameters();

        double remainingComponentIteratorWeight = (*remainingComponentIterator).second;

        AmgSymMatrix(5) unity;
        for (int i(0); i < 5; ++i) {
          for (int j(0); j < 5; ++j) {
            unity(i, j) = parameterDifference(i) * parameterDifference(j);
          }
        }
        covariancePart2 += weight * remainingComponentIteratorWeight * unity;

      } // end loop over remaining components

    } // end clause if errors are involved
    if (weight / totalWeight > m_fractionPDFused)
      break;
  } // end loop over all components

  mean /= sumW;

  // Ensure that phi is between -pi and pi
  //
  if (mean[2] > M_PI) {
    mean[2] -= 2 * M_PI;
  } else if (mean[2] < -M_PI) {
    mean[2] += 2 * M_PI;
  }

  (*covariance) = covariancePart1 / sumW + covariancePart2 / (sumW * sumW);

  if (m_useMode || useModeTemp) {

    if (dimension == 5) {

      // Calculate the mode of the q/p distribution
      Amg::VectorX modes(10);
      modes.setZero();
      modes = Trk::MultiComponentStateModeCalculator::calculateMode(*uncombinedState, msgStream());

      if (msgLvl(MSG::VERBOSE) && modes[4])
        ATH_MSG_VERBOSE("Calculated mode q/p is: " << modes[4]);

      //  Replace mean with mode if qOverP mode is not 0
      if (modes[4] != 0) {
        if (m_useModeD0) {
          mean[0] = modes[0];
          if (modes[5 + 0] > 0) {
            double currentErr = sqrt((*covariance)(0, 0));
            currentErr = modes[5 + 0] / currentErr;
            (*covariance)(0, 0) = modes[5 + 0] * modes[5 + 0];
            covariance->fillSymmetric(1, 0, (*covariance)(1, 0) * currentErr);
            covariance->fillSymmetric(2, 0, (*covariance)(2, 0) * currentErr);
            covariance->fillSymmetric(3, 0, (*covariance)(3, 0) * currentErr);
            covariance->fillSymmetric(4, 0, (*covariance)(4, 0) * currentErr);
          }
        }
        if (m_useModeZ0) {
          mean[1] = modes[1];
          if (modes[5 + 1] > 0) {
            double currentErr = sqrt((*covariance)(1, 1));
            currentErr = modes[5 + 1] / currentErr;
            covariance->fillSymmetric(1, 0, (*covariance)(1, 0) * currentErr);
            (*covariance)(1, 1) = modes[5 + 1] * modes[5 + 1];
            covariance->fillSymmetric(2, 1, (*covariance)(2, 1) * currentErr);
            covariance->fillSymmetric(3, 1, (*covariance)(3, 1) * currentErr);
            covariance->fillSymmetric(4, 1, (*covariance)(4, 1) * currentErr);
          }
        }
        if (m_useModePhi) {
          mean[2] = modes[2];
          if (modes[5 + 2] > 0) {
            double currentErr = sqrt((*covariance)(2, 2));
            currentErr = modes[5 + 2] / currentErr;
            covariance->fillSymmetric(2, 0, (*covariance)(2, 0) * currentErr);
            covariance->fillSymmetric(2, 1, (*covariance)(2, 1) * currentErr);
            (*covariance)(2, 2) = modes[5 + 2] * modes[5 + 2];
            covariance->fillSymmetric(3, 2, (*covariance)(3, 2) * currentErr);
            covariance->fillSymmetric(4, 2, (*covariance)(4, 2) * currentErr);
          }
        }
        if (m_useModeTheta) {
          mean[3] = modes[3];
          if (modes[5 + 3] > 0) {
            double currentErr = sqrt((*covariance)(3, 3));
            currentErr = modes[5 + 3] / currentErr;
            covariance->fillSymmetric(3, 0, (*covariance)(3, 0) * currentErr);
            covariance->fillSymmetric(3, 1, (*covariance)(3, 1) * currentErr);
            covariance->fillSymmetric(3, 2, (*covariance)(3, 2) * currentErr);
            (*covariance)(3, 3) = modes[5 + 3] * modes[5 + 3];
            covariance->fillSymmetric(4, 3, (*covariance)(4, 3) * currentErr);
          }
        }
        if (m_useModeqOverP) {
          mean[4] = modes[4];
          if (modes[5 + 4] > 0) {
            double currentErr = sqrt((*covariance)(4, 4));
            currentErr = modes[5 + 4] / currentErr;
            covariance->fillSymmetric(4, 0, (*covariance)(4, 0) * currentErr);
            covariance->fillSymmetric(4, 1, (*covariance)(4, 1) * currentErr);
            covariance->fillSymmetric(4, 2, (*covariance)(4, 2) * currentErr);
            covariance->fillSymmetric(4, 3, (*covariance)(4, 3) * currentErr);
            (*covariance)(4, 4) = modes[5 + 4] * modes[5 + 4];
          }
        }
      }
    } else {

      ATH_MSG_DEBUG(" Dimension != 5 not updating q/p to mode q/p");
    }
  }

  std::unique_ptr<Trk::TrackParameters> combinedTrackParameters = nullptr;

  double loc1 = mean[Trk::loc1];
  double loc2 = mean[Trk::loc2];
  double phi = mean[Trk::phi];
  double theta = mean[Trk::theta];
  double qoverp = mean[Trk::qOverP];

  if (firstMeasuredCov)
    // combinedTrackParameters =
    // firstParameters->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],par[Trk::phi],par[Trk::theta],par[Trk::qOverP],
    // covariance );
    combinedTrackParameters.reset(
      firstParameters->associatedSurface().createTrackParameters(loc1, loc2, phi, theta, qoverp, covariance));
  else {
    // combinedTrackParameters =
    // firstParameters->associatedSurface().createTrackParameters(par[Trk::loc1],par[Trk::loc2],par[Trk::phi],par[Trk::theta],par[Trk::qOverP],
    // 0 );
    combinedTrackParameters.reset(
      firstParameters->associatedSurface().createTrackParameters(loc1, loc2, phi, theta, qoverp, nullptr));
    delete covariance;
  }

    return std::make_unique<ComponentParameters>(std::move(combinedTrackParameters), totalWeight);

}
