/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// class FitProcedure
//  Basic iterative least square fit procedure producing a fitted Track
//
//  execute and constructTrack methods allow use from standard ITrackFitter
//  and iPatRec specific Algtools.
//  This class is kept transient rather than a tool as there are many state
//  variables.
//
//  for clarity (i.e. to avoid an overly long class) the inner loop over
//  measurements is performed by MeasurementProcessor
//
//////////////////////////////////////////////////////////////////////////////

#include "TrkiPatFitterUtils/FitProcedure.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/MeasurementProcessor.h"
#include <cmath>
#include <iomanip>

namespace Trk {

// constructor
FitProcedure::FitProcedure(bool constrainedAlignmentEffects,
                           bool extendedDebug,
                           bool lineFit,
                           ToolHandle<IIntersector>& rungeKuttaIntersector,
                           ToolHandle<IIntersector>& solenoidalIntersector,
                           ToolHandle<IIntersector>& straightLineIntersector,
                           const ToolHandle<IPropagator>& stepPropagator,
                           const Volume* indetVolume,
                           int maxIterations,
                           int useStepPropagator)
  : m_constrainedAlignmentEffects(constrainedAlignmentEffects)
  , m_extendedDebug(extendedDebug)
  , m_extremeOneOverP(1. / (10. * Gaudi::Units::TeV))
  , m_indetVolume(indetVolume)
  , m_largeRadius(1000. * Gaudi::Units::mm)
  , m_lineFit(lineFit)
  , m_maxIter(maxIterations)
  , m_minIter(0)
  , m_minPt(0.05 * Gaudi::Units::GeV)
  , m_rungeKuttaIntersector(rungeKuttaIntersector)
  , m_solenoidalIntersector(solenoidalIntersector)
  , m_straightLineIntersector(straightLineIntersector)
  , m_stepPropagator(stepPropagator)
  , m_useStepPropagator(useStepPropagator)
{}

// destructor
FitProcedure::~FitProcedure() {}

void
FitProcedure::clear(FitProcedure::Cache& cache)
{
  cache.fitMatrices->releaseMemory();
}

Track*
FitProcedure::constructTrack(
  FitProcedure::Cache& cache,
  const std::vector<FitMeasurement*>& measurements,
  FitParameters& parameters,
  const TrackInfo& trackInfo,
  const DataVector<const TrackStateOnSurface>* leadingTSOS) const
{
  // debug
  if (cache.debug) {
    reportQuality(cache, measurements, parameters);
  }

  // NB keep first and last measurements distinct i.e. separate TSOS (no
  // scatterers etc) NB trackParameters outwards from TSOS i.e. always last
  // FitMeas on surface

  // create vector of TSOS - reserve upper limit for size (+1 as starts with
  // perigee)
  DataVector<const TrackStateOnSurface>* trackStateOnSurfaces =
    new DataVector<const TrackStateOnSurface>;
  unsigned size = measurements.size() + 1;
  if (leadingTSOS)
    size += leadingTSOS->size();
  trackStateOnSurfaces->reserve(size);
  const AlignmentEffectsOnTrack* alignmentEffects = nullptr;
  const FitMeasurement* fitMeasurement = measurements.front();
  const FitQualityOnSurface* fitQoS = nullptr;
  const MaterialEffectsBase* materialEffects = nullptr;
  const MeasurementBase* measurementBase = nullptr;
  const Surface* surface = nullptr;
  const TrackParameters* trackParameters = nullptr;
  std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>
    defaultPattern;
  std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>
    typePattern = defaultPattern;

  // start with (measured) perigee
  unsigned scatter = 0;
  unsigned tsos = 0;
  const Perigee* perigee = parameters.perigee();
  typePattern.set(TrackStateOnSurface::Perigee);
  trackStateOnSurfaces->push_back(new TrackStateOnSurface(measurementBase,
                                                          perigee,
                                                          fitQoS,
                                                          materialEffects,
                                                          typePattern,
                                                          alignmentEffects));
  ++tsos;

  // append leading TSOS to perigee
  if (leadingTSOS) {
    for (DataVector<const TrackStateOnSurface>::const_iterator t =
           leadingTSOS->begin();
         t != leadingTSOS->end();
         ++t) {
      if (!(**t).type(Trk::TrackStateOnSurface::Perigee)) {
        trackStateOnSurfaces->push_back((**t).clone());
        ++tsos;
      }
    }
  }

  // then append the fitted TSOS
  for (auto* m : measurements) {
    if (m->isMaterialDelimiter())
      continue;

    // push back previous TSOS when fresh surface reached
    if (m->surface() != surface || alignmentEffects || m->alignmentEffects()) {
      if (surface) {
        if (typePattern == defaultPattern) {
          if (cache.debug) {
            *cache.log << MSG::DEBUG << " skip empty TSOS# " << tsos + 1;
            if (m->materialEffects())
              *cache.log << " with material";
            m->print(*cache.log);
            *cache.log << endmsg;
          }
        } else {
          // get the MeasuredParameters (with covariance)
          bool withCovariance = true;
          trackParameters = parameters.trackParameters(
            *cache.log, *fitMeasurement, withCovariance);

          if (!trackParameters) {
            *cache.log
              << MSG::WARNING
              << " fail track with incomplete return TSOS: no trackParameters"
              << endmsg;
            delete trackStateOnSurfaces;
            return nullptr;
          }
          typePattern.set(TrackStateOnSurface::Parameter);
          trackStateOnSurfaces->push_back(
            new TrackStateOnSurface(measurementBase,
                                    trackParameters,
                                    fitQoS,
                                    materialEffects,
                                    typePattern,
                                    alignmentEffects));
          ++tsos;
        }
      }
      fitMeasurement = m;
      surface = m->surface();
      measurementBase = nullptr;
      fitQoS = nullptr;
      materialEffects = nullptr;
      typePattern = defaultPattern;
      alignmentEffects = nullptr;
    } else {
      fitMeasurement = m;
      if (cache.verbose)
        *cache.log << MSG::VERBOSE << " tsos# " << tsos << " shared surface"
                   << endmsg;
    }

    // it's a measurement
    if (m->measurementBase()) {
      // create an extra TSOS if there is already a measurement on this surface
      // (dirty fix for pseudoMeasurements)
      if (measurementBase) {
        // get the MeasuredParameters (with covariance)
        bool withCovariance = true;
        trackParameters = parameters.trackParameters(
          *cache.log, *fitMeasurement, withCovariance);
        if (!trackParameters) {
          *cache.log
            << MSG::WARNING
            << " fail track with incomplete return TSOS: no trackParameters"
            << endmsg;
          delete trackStateOnSurfaces;
          return nullptr;
        }
        typePattern.set(TrackStateOnSurface::Parameter);
        trackStateOnSurfaces->push_back(
          new TrackStateOnSurface(measurementBase,
                                  trackParameters,
                                  fitQoS,
                                  materialEffects,
                                  typePattern,
                                  alignmentEffects));
        ++tsos;
        fitMeasurement = m;
        fitQoS = nullptr;
        materialEffects = nullptr;
        typePattern = defaultPattern;
        alignmentEffects = nullptr;
      }

      measurementBase = m->measurementBase()->clone();
      typePattern.set(TrackStateOnSurface::Measurement);
      if (m->isOutlier())
        typePattern.set(TrackStateOnSurface::Outlier);
    }

    // it's a CaloDeposit or Scatterer (scatterers may be fitted or not fitted)
    if (m->materialEffects()) {
      // update momentum to account for energy loss
      delete materialEffects;

      if (m->isEnergyDeposit()) {
        materialEffects = m->materialEffects()->clone();
        typePattern.set(TrackStateOnSurface::CaloDeposit);
      } else if (m->isScatterer()) {
        // set materialPattern as the scattering parameters are fitted
        std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>
          typeMaterial;
        typeMaterial.set(MaterialEffectsBase::FittedMaterialEffects);
        const MaterialEffectsOnTrack* meot =
          dynamic_cast<const MaterialEffectsOnTrack*>(m->materialEffects());
        if (meot && meot->energyLoss()) // standard scatterer
        {
          const EnergyLoss* energyLoss = meot->energyLoss()->clone();
          typeMaterial.set(Trk::MaterialEffectsBase::EnergyLossEffects);
          if (m->numberDoF()) // fitted scatterer
          {
            materialEffects = new MaterialEffectsOnTrack(
              m->materialEffects()->thicknessInX0(),
              parameters.scatteringAngles(*m, scatter),
              energyLoss,
              m->materialEffects()->associatedSurface(),
              typeMaterial);
            ++scatter;
          } else // unfitted (leading material)
          {
            materialEffects = new MaterialEffectsOnTrack(
              m->materialEffects()->thicknessInX0(),
              parameters.scatteringAngles(*m),
              energyLoss,
              m->materialEffects()->associatedSurface(),
              typeMaterial);
          }
        } else // no meot for special calo scattering centres
        {
          if (m->numberDoF()) // fitted scatterer
          {
            materialEffects = new MaterialEffectsOnTrack(
              m->materialEffects()->thicknessInX0(),
              parameters.scatteringAngles(*m, scatter),
              m->materialEffects()->associatedSurface(),
              typeMaterial);
            ++scatter;
          } else // unfitted (leading material)
          {
            materialEffects = new MaterialEffectsOnTrack(
              m->materialEffects()->thicknessInX0(),
              parameters.scatteringAngles(*m),
              m->materialEffects()->associatedSurface(),
              typeMaterial);
          }
        }

        typePattern.set(TrackStateOnSurface::Scatterer);
      } else {
        *cache.log << MSG::WARNING
                   << " deprecated TrackStateOnSurface::InertMaterial"
                   << endmsg;
        materialEffects = m->materialEffects()->clone();
        typePattern.set(TrackStateOnSurface::InertMaterial);
      }
    }

    // additional perigee (e.g. at MS entrance)
    if (m->isPerigee()) {
      typePattern.set(TrackStateOnSurface::Perigee);
    }

    // or alignment effects
    else if (m->alignmentEffects()) {
      const AlignmentEffectsOnTrack& AEOT = *m->alignmentEffects();
      unsigned align = m->alignmentParameter() - 1;

      *cache.log << MSG::VERBOSE << " Fitprocedure AEOT input deltaTranslation "
                 << AEOT.deltaTranslation() << " deltaAngle "
                 << AEOT.deltaAngle() << " output Trans "
                 << parameters.alignmentOffset(align) << " deltaAngle "
                 << parameters.alignmentAngle(align) << endmsg;
      alignmentEffects =
        new Trk::AlignmentEffectsOnTrack(parameters.alignmentOffset(align),
                                         AEOT.sigmaDeltaTranslation(),
                                         parameters.alignmentAngle(align),
                                         AEOT.sigmaDeltaAngle(),
                                         AEOT.vectorOfAffectedTSOS(),
                                         m->surface());
      typePattern.set(TrackStateOnSurface::Alignment);
    }

    // passive types: hole for now
    else if (m->isPassive()) {
      if (m->type() == hole)
        typePattern.set(TrackStateOnSurface::Hole);
    }
  }

  // remember the final TSOS !
  bool withCovariance = true;
  trackParameters =
    parameters.trackParameters(*cache.log, *fitMeasurement, withCovariance);
  if (!trackParameters) {
    *cache.log << MSG::WARNING
               << " fail track with incomplete return TSOS: no trackParameters"
               << endmsg;
    delete trackStateOnSurfaces;
    return nullptr;
  }
  typePattern.set(TrackStateOnSurface::Parameter);
  trackStateOnSurfaces->push_back(new TrackStateOnSurface(measurementBase,
                                                          trackParameters,
                                                          fitQoS,
                                                          materialEffects,
                                                          typePattern,
                                                          alignmentEffects));
  ++tsos;

  // construct track
  double chiSquared = cache.chiSq * static_cast<double>(cache.numberDoF);
  Track* track = new Track(trackInfo,
                           trackStateOnSurfaces,
                           new FitQuality(chiSquared, cache.numberDoF));

  if (cache.verbose)
    *cache.log << MSG::VERBOSE << " track with " << tsos << " TSOS " << endmsg;
  return track;
}

const FitProcedureQuality&
FitProcedure::execute(FitProcedure::Cache& cache,
                      bool asymmetricCaloEnergy,
                      MsgStream& log,
                      std::vector<FitMeasurement*>& measurements,
                      FitParameters*& parameters,
                      const FitQuality* perigeeQuality,
                      bool for_iPatTrack) const
{
  // report start value
  cache.log = &log;
  if (cache.log->level() > MSG::DEBUG) {
    cache.debug = false;
    cache.verbose = false;
  } else {
    cache.debug = true;
    if (cache.log->level() < MSG::DEBUG)
      cache.verbose = true;
    *cache.log << MSG::DEBUG << "parameter start values:  ";
    parameters->print(*cache.log);
    *cache.log << endmsg;
  }

  // choose appropriate intersector
  ToolHandle<IIntersector>& intersector =
    chooseIntersector(measurements, *parameters);

  // resize matrices
  int fitCode = cache.fitMatrices->setDimensions(measurements, parameters);
  if (fitCode) {
    cache.fitQuality = std::make_unique<FitProcedureQuality>(
      fitCode, cache.fitMatrices->numberDoF());
    if (cache.debug)
      reportQuality(cache, measurements, *parameters);
    return *cache.fitQuality;
  }

  // remaining initialization
  cache.chiSq = 0.;
  cache.chiSqWorst = 0.;
  cache.driftSum = 0.;
  cache.driftSumLast = 0.;
  cache.fitProbability = 0.;
  cache.iteration = -1;
  cache.numberDoF = cache.fitMatrices->numberDoF();
  cache.numberParameters = parameters->numberParameters();
  cache.worstMeasurement = 0;
  MeasurementProcessor measurementProcessor(
    asymmetricCaloEnergy,
    *cache.fitMatrices->derivativeMatrix(),
    intersector,
    measurements,
    parameters,
    m_rungeKuttaIntersector,
    m_stepPropagator,
    m_useStepPropagator);

  // perigee or vertex used as measurements in fit
  if (measurements.front()->isPerigee()) {
    cache.fitMatrices->usePerigee(*measurements.front());
  }

  // set requested options and initial values
  double ptInvCut = 1. / m_minPt; // protection against trapped particles
  cache.cutStep = true;
  cache.convergence = false;
  cache.nearConvergence = false;

  // keep best (original if not reasonable quality) results
  double bestChiSq = cache.chiSqCut;
  FitParameters* bestParameters = nullptr;

  // iteration loop to fit track parameters
  while (!fitCode && !cache.convergence) {
    bool forceIteration = false;
    if (cache.iteration > m_maxIter && bestParameters && !for_iPatTrack) {
      parameters->reset(*bestParameters);
      cache.convergence = true;
      cache.cutStep = false;
      if (cache.verbose)
        *cache.log << MSG::VERBOSE
                   << " convergence problem: accept after max iter " << endmsg;
    } else if (!cache.cutStep) {
      //  solve equations and update parameters
      if (!cache.iteration) {
        cache.fitMatrices->refinePointers();
        if (m_extendedDebug)
          cache.fitMatrices->checkPointers(*cache.log);
        if (cache.verbose)
          cache.fitMatrices->printDerivativeMatrix();
      }

      if (!cache.fitMatrices->solveEquations()) {
        fitCode = 11; // fails matrix inversion
      } else if (parameters->fitEnergyDeposit() &&
                 !parameters->extremeMomentum() &&
                 std::abs(parameters->qOverP()) < m_extremeOneOverP) {
        if (cache.debug)
          *cache.log << MSG::DEBUG << " extremeMomentum " << endmsg;
        parameters->extremeMomentum(true);
        delete bestParameters;
        fitCode = cache.fitMatrices->setDimensions(measurements, parameters);
        bestChiSq = cache.chiSqCut;
        bestParameters = nullptr;
        forceIteration = true;
        cache.chiSq = 0.;
        cache.chiSqWorst = 0.;
        cache.driftSum = 0.;
        cache.driftSumLast = 0.;
        cache.numberParameters = parameters->numberParameters();
      }
      if (cache.verbose && !cache.iteration)
        cache.fitMatrices->printWeightMatrix();
    }
    ++cache.iteration;

    // report parameters
    if (cache.verbose) {
      *cache.log << MSG::VERBOSE << " ===== start iteration "
                 << cache.iteration;
      if (cache.iteration) {
        if (cache.cutStep)
          *cache.log << " ====== cutStep";
      } else {
        if (for_iPatTrack)
          *cache.log << "  ====== for_iPatTrack ";
      }
      parameters->printVerbose(*cache.log);
    }

    //  check for some error conditions (if none found yet)
    if (fitCode) {
      // e.g. fitCode == 11 (singular matrix)
    } else if (std::abs(parameters->ptInv0()) > ptInvCut) {
      fitCode = 8; //  fail with pt below cutoff
    } else if (measurements.front()->isVertex() && m_indetVolume &&
               !m_indetVolume->inside(parameters->position())) {
      fitCode = 9; // fail if vertex outside indet
    } else if (cache.iteration &&
               (std::abs(parameters->difference(3)) > 1.0 ||
                std::abs(parameters->difference(0)) > m_largeRadius) &&
               !measurements.front()->isVertex()) {
      if (std::abs(parameters->difference(3)) > 1.0) {
        fitCode = 10; // fail with ill-defined cot_theta
      } else {
        fitCode = 9; // fail crazy impact parameter
      }
    } else if (!fitCode) {
      //  extrapolate to each measurement and calculate derivatives
      if (!measurementProcessor.calculateFittedTrajectory(cache.iteration) ||
          !measurementProcessor.calculateDerivatives()) {
        fitCode = 5; //  fail as trapped
        delete bestParameters;
        cache.fitQuality =
          std::make_unique<FitProcedureQuality>(cache.chiSq,
                                                cache.chiSqWorst,
                                                cache.fitProbability,
                                                fitCode,
                                                cache.iteration,
                                                parameters->numberAlignments(),
                                                cache.fitMatrices->numberDoF(),
                                                parameters->numberScatterers(),
                                                cache.worstMeasurement);

        if (cache.debug) {
          if (cache.verbose)
            *cache.log << endmsg;
          reportQuality(cache, measurements, *parameters);
        }
        return *cache.fitQuality;
      }

      //  have extrapolation and derivatives, calculate residual
      measurementProcessor.calculateResiduals();

      // check for remaining error conditions. If OK then compute chisquared.
      if (cache.iteration > m_maxIter && !cache.cutStep && for_iPatTrack) {
        fitCode = 6; //  fail with no convergence
      } else if (cache.iteration == 4 && cache.chiSq > 1000. && for_iPatTrack) {
        fitCode = 7; //  fail with too high chisquared
      } else if (!fitCode) {
        calculateChiSq(cache, measurements);

        // check for cutstep conditions if no significant chi2 improvement
        if (!forceIteration && !cache.convergence && cache.chRatio1 > 0.9) {
          double cutStep = 0.;
          if (cache.iteration > 4 &&
              cache.driftSum * cache.driftSumLast < -1.) {
            cache.cutStep = true;
            cutStep = std::abs(cache.driftSumLast /
                               (cache.driftSum - cache.driftSumLast));
            if (cutStep < 0.001)
              cutStep = 0.001;
            if (cache.verbose)
              *cache.log
                << MSG::VERBOSE
                << " take cutStep following chi2 increase on iteration "
                << cache.iteration << "  chi2Ratio " << cache.chRatio1
                << "   driftSum " << cache.driftSum << " prev "
                << cache.driftSumLast << "   " << cutStep << endmsg;
          } else if (parameters->numberOscillations() > 2) {
            cache.cutStep = true;
            cutStep = 0.5;
            if (cache.verbose)
              *cache.log << MSG::VERBOSE
                         << " take cutStep as oscillating, iteration "
                         << cache.iteration << ", numberOscillations "
                         << parameters->numberOscillations() << endmsg;
          }

          // perform cutstep
          if (cache.cutStep) {
            cache.convergence = false;
            parameters->performCutStep(cutStep);
            if (cache.verbose)
              parameters->printVerbose(*cache.log);
            if (measurementProcessor.calculateFittedTrajectory(
                  cache.iteration)) {
              // note: derivatives should not be recalculated for cutstep
              measurementProcessor.calculateResiduals();
              calculateChiSq(cache, measurements);
              if (cache.verbose)
                *cache.log << "   after cutStep: "
                           << "  chi2Ratio " << cache.chRatio1 << "   driftSum "
                           << cache.driftSum << endmsg;
            }
          }
        }

        // keep current best parameters
        if (!bestParameters || cache.chiSq < bestChiSq) {
          bestChiSq = cache.chiSq;
          delete bestParameters;
          bestParameters = new FitParameters(*parameters);
          parameters->resetOscillations();
        }

        if (bestParameters &&
            ((cache.convergence && cache.chiSq > bestChiSq + 0.5) ||
             (parameters->phiInstability() && cache.iteration == m_maxIter))) {
          parameters->reset(*bestParameters);
          if (cache.verbose) {
            *cache.log << MSG::VERBOSE << " revert to bestParameters ";
            parameters->printVerbose(*cache.log);
          }
          if (measurementProcessor.calculateFittedTrajectory(cache.iteration)) {
            measurementProcessor.calculateDerivatives();
            measurementProcessor.calculateResiduals();
            calculateChiSq(cache, measurements);
            cache.convergence = true;
          }
        }

        if (forceIteration)
          cache.convergence = false;
      }
    } // if (std::abs(ptInv0) > ptInvCut)
    if (cache.verbose)
      *cache.log << endmsg;

    // try to rescue phi instability failures
    if (fitCode && cache.iteration && bestParameters &&
        !parameters->phiInstability() &&
        (**(measurements.rbegin())).position().perp() > m_largeRadius) {
      if (cache.verbose)
        *cache.log << MSG::VERBOSE << " phi instability " << endmsg;
      parameters->reset(*bestParameters);
      parameters->setPhiInstability();
      cache.cutStep = true;
      cache.convergence = false;
      fitCode = 0;
      cache.iteration = 0;
    }
  } // while

  // store successful fit :
  if (!fitCode) {
    // set covariance in parameters class after inclusion of uncertainty from
    // field integral
    const Amg::MatrixX* fullCovariance = cache.fitMatrices->fullCovariance();
    if (fullCovariance) {
      Amg::MatrixX* finalCovariance = cache.fitMatrices->finalCovariance();
      if (!for_iPatTrack) {
        if (!m_lineFit)
          measurementProcessor.fieldIntegralUncertainty(*cache.log,
                                                        *finalCovariance);
        measurementProcessor.propagationDerivatives();
      }
      parameters->covariance(finalCovariance, fullCovariance);

      // fit quality
      if (perigeeQuality) {
        // take care when mixing normalized with unnormalized
        cache.chiSq = perigeeQuality->chiSquared() +
                      cache.chiSq * static_cast<double>(cache.numberDoF);
        cache.numberDoF += perigeeQuality->numberDoF();
        cache.chiSq /= static_cast<double>(cache.numberDoF);
      }

      // probability of chisquared
      cache.fitProbability = 1.;
      if (cache.numberDoF > 0 && cache.chiSq > 0.) {
        if (cache.chiSq < 100.) {
          double chiSquared =
            cache.chiSq * static_cast<double>(cache.numberDoF);
          cache.fitProbability -=
            Genfun::CumulativeChiSquare(cache.numberDoF)(chiSquared);
        } else {
          cache.fitProbability = 0.;
        }
      }

      if (cache.verbose) {
        *cache.log << MSG::VERBOSE << " fit converged";
        if (cache.chiSqWorst > 6.25)
          *cache.log << " with possible outlier #" << cache.worstMeasurement
                     << " (residual " << std::sqrt(cache.chiSqWorst) << ")";
        *cache.log << endmsg;
      }
    } else {
      fitCode = 11; //  singular weight matrix
    }
  }

  delete bestParameters;
  cache.fitQuality =
    std::make_unique<FitProcedureQuality>(cache.chiSq,
                                          cache.chiSqWorst,
                                          cache.fitProbability,
                                          fitCode,
                                          cache.iteration,
                                          parameters->numberAlignments(),
                                          cache.numberDoF,
                                          parameters->numberScatterers(),
                                          cache.worstMeasurement);
  if (cache.debug && (for_iPatTrack || fitCode))
    reportQuality(cache, measurements, *parameters);

  return *cache.fitQuality;
}

Amg::MatrixX*
FitProcedure::fullCovariance() const
{
  // note const_cast - ughhh
  // return const_cast<Amg::MatrixX*>(cache.fitMatrices->fullCovariance());
  return nullptr; // NOT mig5
}

void
FitProcedure::setMinIterations(int minIter)
{
  m_minIter = minIter;
  if (m_minIter > m_maxIter)
    m_maxIter = m_minIter;
}

void
FitProcedure::calculateChiSq(FitProcedure::Cache& cache,
                             std::vector<FitMeasurement*>& measurements) const
{
  // convergence criterion
  const double dChisqConv = 0.025;

  // compute total chisquared and sum of hit differences
  // flag hit with highest chisquared contribution (on entry if RoadFit)
  cache.chiSq = 0.;
  double driftResidual = 0.;
  double DSqMax = 0.;
  for (auto* m : measurements) {
    if (!m->numberDoF())
      continue;
    // if (m->isPerigee())
    // {
    //     cache.chiSq += cache.fitMatrices->perigeeChiSquared();
    //     continue;
    // }

    double residual = m->residual();
    double DiffSq = residual * residual;
    cache.chiSq += DiffSq;
    if (m->isPositionMeasurement()) {
      if (m->isDrift())
        driftResidual += residual;
      if (DiffSq > DSqMax) {
        DSqMax = DiffSq;
        cache.worstMeasurement = m->hitIndex() + 1;
        cache.chiSqWorst = std::min(999., DSqMax);
      }
    }
    if (m->is2Dimensional()) {
      residual = m->residual2();
      DiffSq = residual * residual;
      cache.chiSq += DiffSq;
      if (m->isPositionMeasurement()) {
        if (DiffSq > DSqMax) {
          DSqMax = DiffSq;
          cache.worstMeasurement = m->hitIndex() + 1;
          cache.chiSqWorst = std::min(999., DSqMax);
        }
      }
    }
  }

  // assess chi squared per degree of freedom (and its stability)
  if (cache.fitMatrices->numberDoF() > 0)
    cache.chiSq /= static_cast<double>(cache.fitMatrices->numberDoF());
  if (cache.iteration == 0) {
    cache.cutTaken = 0;
    cache.chRatio1 = 0.;
    cache.chRatio2 = 0.;
    cache.chiSqMin = cache.chiSq;
  }

  cache.chiSqOld = cache.chiSqMin;
  double DChiSq = cache.chiSqOld - cache.chiSq;
  if (DChiSq > -dChisqConv) {
    cache.chiSqMin = cache.chiSq;
    cache.nCuts = 0;
  }
  if (cache.iteration > 0) {
    cache.chRatio2 = cache.chRatio1;
    cache.chRatio1 = cache.chiSq / cache.chiSqOld;
  }
  if (cache.fitMatrices->numberDriftCircles()) {
    cache.driftSumLast = cache.driftSum;
    cache.driftSum = driftResidual / static_cast<double>(
                                       cache.fitMatrices->numberDriftCircles());
  }

  //
  // debugging info
  if (cache.verbose) {
    *cache.log << "----------------------------------" << std::endl
               << std::setiosflags(std::ios::fixed)
               << " Debugging Info in ChiSquare method" << std::endl
               << " # of track-fit iterations " << std::setw(3)
               << cache.iteration << std::endl
               << " fit ChiSquared (per degree of freedom) " << std::setw(13)
               << std::setprecision(3) << cache.chiSq
               << "   # of degrees of freedom "
               << cache.fitMatrices->numberDoF() << std::endl
               << " ChSq Ratio1/2 " << std::setw(9) << std::setprecision(3)
               << cache.chRatio1 << std::setw(10) << std::setprecision(3)
               << cache.chRatio2 << std::endl
               << " driftResidual " << std::setw(9) << std::setprecision(3)
               << cache.driftSum << " #driftCircles "
               << cache.fitMatrices->numberDriftCircles() << std::endl
               << " CutTaken " << cache.cutTaken << std::endl
               << "----------------------------------" << std::endl
               << "   ";

    (**measurements.begin()).printHeading(*cache.log);
    int n = 0;
    for (auto* m : measurements) {
      *cache.log << std::setiosflags(std::ios::fixed) << std::setw(3) << ++n;
      if (m->isPerigee()) {
        *cache.log << " perigee     ";
        *cache.log << std::endl;
      } else {
        m->print(*cache.log);
      }
    }
  }

  //
  // check for possible convergence (nearConvergence forces extra iteration)
  if (!cache.cutStep && !cache.nCuts &&
      (cache.chiSq < 0.1 ||
       (cache.chRatio2 < 1.1 &&
        (std::abs(DChiSq) < dChisqConv ||
         std::abs((cache.chiSq - cache.chiSqOld) / cache.chiSqOld) < 0.01)))) {
    if ((cache.chiSq < 2.0 || cache.nearConvergence || cache.iteration == 1) &&
        cache.iteration >= m_minIter) {
      cache.convergence = true;
    } else {
      cache.nearConvergence = true;
      if (cache.verbose)
        *cache.log << MSG::VERBOSE << " near convergence " << endmsg;
    }
  } else {
    cache.nearConvergence = false;
  }

  // else take cutstep if divergent or oscillating
  cache.cutStep = false;
}

ToolHandle<IIntersector>&
FitProcedure::chooseIntersector(std::vector<FitMeasurement*>& measurements,
                                const FitParameters& parameters) const
{
  if (m_lineFit) {
    return m_straightLineIntersector;
  }

  // decide which intersector to use for curved tracks (default RungeKutta)
  // ToolHandle<IIntersector>& intersector = m_rungeKuttaIntersector;

  // solenoidal intersector must start close to origin with last measurement
  // inside valid region
  for (std::vector<FitMeasurement*>::reverse_iterator m = measurements.rbegin();
       m != measurements.rend();
       ++m) {
    if (!(**m).isPositionMeasurement())
      continue;
    if (!m_solenoidalIntersector->isValid(parameters.position(),
                                          (**m).position()))
      break;
    return m_solenoidalIntersector;
  }

  return m_rungeKuttaIntersector;
}

void
FitProcedure::reportQuality(FitProcedure::Cache& cache,
                            const std::vector<FitMeasurement*>& measurements,
                            const FitParameters& parameters) const
{
  if (!cache.fitQuality)
    return;

  int fitCode = cache.fitQuality->fitCode();
  if (fitCode) {
    *cache.log << MSG::DEBUG << "failure: fitCode " << fitCode;
    std::string msg = "";
    switch (fitCode) {
      case 1:
        *cache.log << "  missing Trk::Surface ";
        break;
      case 2:
        *cache.log << "  too many measurements for fit matrix size: "
                   << measurements.size();
        break;
      case 3:
        *cache.log << "  too many parameters for fit matrix size: "
                   << parameters.numberParameters();
        break;
      case 4:
        *cache.log << "  unconstrained fit: negative numberDoF "
                   << cache.fitQuality->numberDoF();
        break;
      case 5:
        *cache.log << "  trapped in magnetic field: pT = "
                   << 1. / (parameters.ptInv0() * Gaudi::Units::GeV)
                   << "  at iteration# " << cache.fitQuality->iterations();
        break;
      case 6:
        *cache.log << "  no convergence: chiSq = " << cache.fitQuality->chiSq()
                   << "  at iteration# " << cache.fitQuality->iterations();
        break;
      case 7:
        *cache.log << "  enormous chi squared: chiSq = "
                   << cache.fitQuality->chiSq() << "  at iteration# "
                   << cache.fitQuality->iterations();
        break;
      case 8:
        *cache.log << "  below pT cutoff. pT = "
                   << 1. / (parameters.ptInv0() * Gaudi::Units::GeV)
                   << "  at iteration# " << cache.fitQuality->iterations();
        break;
      case 9:
        *cache.log << "  ill-defined impact parameter " << parameters.d0()
                   << "  with difference " << parameters.difference(0)
                   << "  at iteration# " << cache.fitQuality->iterations();
        break;
      case 10:
        *cache.log << "  ill-defined cotTheta " << parameters.cotTheta()
                   << "  with difference " << parameters.difference(3)
                   << "  at iteration# " << cache.fitQuality->iterations();
        break;
      case 11:
        *cache.log << "  singular matrix fails inversion:"
                   << "  at iteration# " << cache.fitQuality->iterations();
        break;
      case 12:
        *cache.log << "  maximum of one calorimeter permitted";
        break;
      case 13:
        *cache.log << "  NO derivativeMatrix available";
        break;
      default:
        break;
    };
    *cache.log << std::endl << endmsg;
  } else {
    *cache.log << MSG::DEBUG << "fitted parameter values: ";
    parameters.print(*cache.log);
    *cache.log << endmsg;
    *cache.log << MSG::DEBUG;
    cache.fitQuality->print(*cache.log);
    parameters.printCovariance(*cache.log);
    *cache.log << endmsg;
  }
}

} // end of namespace
