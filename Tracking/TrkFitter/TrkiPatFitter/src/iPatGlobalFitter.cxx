/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

//////////////////////////////////////////////////////////////////
// iPatGlobalFitter.cxx
//   access full derivative and covariance matrices
//
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/FitProcedure.h"
#include "TrkiPatFitter/iPatGlobalFitter.h"

namespace Trk
{
  iPatGlobalFitter::iPatGlobalFitter (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
    :   iPatFitter(type, name, parent, true),
    m_allParameters(false) {
    declareInterface<IGlobalTrackFitter>(this);
    declareProperty("AllParameters", m_allParameters);
  }

  iPatGlobalFitter::~iPatGlobalFitter (void) {
  }

  Track* iPatGlobalFitter::alignmentFit ( AlignmentCache& alignCache,
  											const Track& trk,
  											const RunOutlierRemoval  runOutlier,
  											const ParticleHypothesis matEffects) const
  {
    //  @TODO ensure the number of iterations is passed through to the fitter
  	//setMinIterations (alignCache.m_minIterations);
    if(alignCache.m_derivMatrix != nullptr) {
  	  delete alignCache.m_derivMatrix;
    }
  	alignCache.m_derivMatrix = nullptr;
  
  	if(alignCache.m_fullCovarianceMatrix != nullptr) {
  	  delete alignCache.m_fullCovarianceMatrix;
    }
  	alignCache.m_fullCovarianceMatrix =  nullptr;
    alignCache.m_iterationsOfLastFit = 0;

    auto [refittedTrack, fitState] =
      fitWithState(Gaudi::Hive::currentContext(), trk, runOutlier, matEffects);

    if(refittedTrack){
  		alignCache.m_derivMatrix = derivMatrix(*fitState).release();
  	  alignCache.m_fullCovarianceMatrix = fullCovarianceMatrix(*fitState).release();
  	  alignCache.m_iterationsOfLastFit = iterationsOfLastFit(*fitState);
  	}
  	return refittedTrack.release();
  }


  std::unique_ptr<Amg::MatrixX>
  iPatGlobalFitter::derivMatrix(const FitState& fitState) const {
    // copy derivatives to a new HepMatrix
    if (!fitState.hasMeasurements() || !fitState.parameters) { return nullptr; }

    int numberParameters = 5;
    if (m_allParameters) { numberParameters = fitState.parameters->numberParameters(); }
    int rows = 0;

    for (const FitMeasurement* m : fitState.getMeasurements()) {
      if (!m->isPositionMeasurement()) { continue; }
      rows += m->numberDoF();
    }

    if (!numberParameters || !rows) { return nullptr; }

    ATH_MSG_VERBOSE(" DerivMatrix : " << fitState.getMeasurements().size() << " measurement objects giving "
                                      << rows << " rows and " << numberParameters << " columns (parameters)");

    auto derivativeMatrix = std::make_unique<Amg::MatrixX>(rows, numberParameters);
    int row = 0;
    for (const FitMeasurement* m : fitState.getMeasurements()) {
      if (!m->numberDoF() || !m->isPositionMeasurement()) { continue; }
      double norm = 0.;
      if (m->weight() > 0.) { norm = 1. / m->weight(); }

      for (int col = 0; col < numberParameters; ++col) {
        (*derivativeMatrix)(row, col) = norm * m->derivative(col);
      }

      // take care of units for momentum derivs
      (*derivativeMatrix)(row, 4) *= Gaudi::Units::TeV;
      if (fitState.parameters->fitEnergyDeposit()) { (*derivativeMatrix)(row, 5) *= Gaudi::Units::TeV; }
      ++row;
      if (m->numberDoF() < 2) { continue; }

      // pixel measurements
      norm = 0.;
      if (m->weight2() > 0.) norm = 1. / m->weight2();
      for (int col = 0; col < numberParameters; ++col) {
        (*derivativeMatrix)(row, col) = norm * m->derivative2(col);
      }
      (*derivativeMatrix)(row, 4) *= Gaudi::Units::TeV;
      if (fitState.parameters->fitEnergyDeposit()) { (*derivativeMatrix)(row, 5) *= Gaudi::Units::TeV; }
      ++row;
    }

    if (row != rows) { ATH_MSG_WARNING("iPatGlobalFitter: inconsistent #rows in deriv matrix "); }

    return derivativeMatrix;
  }

  std::unique_ptr<Amg::MatrixX>
  iPatGlobalFitter::fullCovarianceMatrix(const FitState& fitState) const {
    int numberParameters = 5;

    if (m_allParameters) { numberParameters = fitState.parameters->numberParameters(); }
    ATH_MSG_VERBOSE(" FullCovarianceMatrix for " << numberParameters << " parameters");

    return std::make_unique<Amg::MatrixX>(m_fitProcedure->fullCovariance()->block(0, 0, numberParameters, numberParameters));
  }

  int
  iPatGlobalFitter::iterationsOfLastFit(const FitState& fitState) const
  {return fitState.iterations;}


  void
  iPatGlobalFitter::setMinIterations(int minIterations) {
    m_fitProcedure->setMinIterations(minIterations);
  }
} // end of namespace
