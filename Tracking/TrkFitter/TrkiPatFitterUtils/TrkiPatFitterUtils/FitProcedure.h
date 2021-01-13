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

#ifndef TRKIPATFITTERUTILS_FITPROCEDURE_H
#define TRKIPATFITTERUTILS_FITPROCEDURE_H

#include "AthContainers/DataVector.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkiPatFitterUtils/FitMatrices.h"
#include "TrkiPatFitterUtils/FitProcedureQuality.h"
#include <vector>

class MsgStream;

namespace Trk {
class FitMeasurement;
class FitParameters;
class IIntersector;
class IPropagator;
class Track;
class TrackInfo;
class TrackStateOnSurface;
class Volume;

class FitProcedure
{
public:
  // Cache to be passed by the caller
  struct Cache
  {
    Cache(bool constrainedAlignmentEffects)
      : chRatio1(0.)
      , chRatio2(0.)
      , chiSq(0.)
      , chiSqCut(20.)
      , chiSqMin(0.)
      , chiSqOld(0.)
      , chiSqWorst(0.)
      , convergence(false)
      , cutStep(false)
      , cutTaken(0)
      , debug(false)
      , driftSum(0.)
      , driftSumLast(0.)
      , fitMatrices(std::make_unique<FitMatrices>(constrainedAlignmentEffects))
      , fitProbability(0.)
      , fitQuality(nullptr)
      , iteration(0)
      , log(nullptr)
      , nearConvergence(false)
      , nCuts(0)
      , numberDoF(0)
      , numberParameters(0)
      , verbose(false)
      , worstMeasurement(0)
    {}
    ~Cache() = default;
    // delete the rest as should not be needed.
    Cache(const Cache&) = delete;
    Cache& operator=(const Cache&) = delete;
    Cache(Cache&&) = delete;
    Cache& operator=(Cache&&) = delete;

    double chRatio1 = 0;
    double chRatio2 = 0;
    double chiSq = 0;
    double chiSqCut = 20;
    double chiSqMin = 0;
    double chiSqOld = 0;
    double chiSqWorst = 0;
    bool convergence = false;
    bool cutStep = 0;
    int cutTaken = false;
    bool debug = false;
    double driftSum = 0;
    double driftSumLast = 0;
    std::unique_ptr<FitMatrices> fitMatrices = nullptr;
    double fitProbability = 0;
    std::unique_ptr<FitProcedureQuality> fitQuality = nullptr;
    int iteration = 0;
    MsgStream* log = nullptr;
    bool nearConvergence = false;
    int nCuts = 0;
    int numberDoF = 0;
    int numberParameters = 0;
    bool verbose = false;
    int worstMeasurement = 0;
  };

  FitProcedure(bool constrainedAlignmentEffects,
               bool extendedDebug,
               bool lineFit,
               ToolHandle<IIntersector>& rungeKuttaIntersector,
               ToolHandle<IIntersector>& solenoidalIntersector,
               ToolHandle<IIntersector>& straightLineIntersector,
               const ToolHandle<IPropagator>& stepPropagator,
               const Volume* indetVolume = 0,
               int maxIterations = 25,
               int useStepPropagator = 0);

  ~FitProcedure(void); // destructor
  // forbidden copy constructor
  // forbidden assignment operator

  // clean up memory (after fit completion)
  void clear(FitProcedure::Cache& cache);

  // retrieve result
  Track* constructTrack(
    FitProcedure::Cache& cache,
    const std::vector<FitMeasurement*>& measurements,
    FitParameters& parameters,
    const TrackInfo& trackInfo,
    const DataVector<const TrackStateOnSurface>* leadingTSOS = nullptr) const;

  // perform fit procedure
  const FitProcedureQuality& execute(FitProcedure::Cache& cache,
                                     bool asymmetricCaloEnergy,
                                     MsgStream& log,
                                     std::vector<FitMeasurement*>& measurements,
                                     FitParameters*& parameters,
                                     const FitQuality* perigeeQuality = 0,
                                     bool for_iPatTrack = false) const;

  // for IGlobalTrackFit interface
  Amg::MatrixX* fullCovariance() const;

  // set minimum number of iterations to perform (IGlobalTrackFit friends)
  void setMinIterations(int minIter);

  bool constrainedAlignmentEffects() const
  {
    return m_constrainedAlignmentEffects;
  }

private:
  // copy, assignment: no semantics, no implementation
  FitProcedure(const FitProcedure&) = delete;
  FitProcedure& operator=(const FitProcedure&) = delete;
  FitProcedure(FitProcedure&&) = delete;
  FitProcedure& operator=(FitProcedure&&) = delete;

  void calculateChiSq(FitProcedure::Cache& cache,
                      std::vector<FitMeasurement*>& measurements) const;

  ToolHandle<IIntersector>& chooseIntersector(
    std::vector<FitMeasurement*>& measurements,
    const FitParameters& parameters) const;

  void reportQuality(FitProcedure::Cache& cache,
                     const std::vector<FitMeasurement*>& measurements,
                     const FitParameters& parameters) const;

  bool m_constrainedAlignmentEffects;
  bool m_extendedDebug;
  double m_extremeOneOverP;
  const Trk::Volume* m_indetVolume;
  double m_largeRadius;
  bool m_lineFit;
  int m_maxIter;
  int m_minIter;
  double m_minPt;
  ToolHandle<IIntersector>& m_rungeKuttaIntersector;
  double m_scatteringConstant;
  double m_scatteringLogCoeff;
  ToolHandle<IIntersector>& m_solenoidalIntersector;
  ToolHandle<IIntersector>& m_straightLineIntersector;
  const ToolHandle<IPropagator>& m_stepPropagator;
  int m_useStepPropagator;
};
} // end of namespace

#endif // TRKIPATFITTERUTILS_FITPROCEDURE_H
