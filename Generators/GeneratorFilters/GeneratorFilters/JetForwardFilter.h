/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_JETFORWARDFILTER_H
#define GENERATORFILTERS_JETFORWARDFILTER_H

#include "GeneratorModules/GenFilter.h"
class JetCollection;
class Jet;
class TRandom3;

/// Filter suppresses events in the low pt/deltaY region.
/// @author Graham Jones
class JetForwardFilter : public GenFilter {
public:

  JetForwardFilter(const std::string & name, ISvcLocator * pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  std::string m_jetContainer;

  /// Jet kinematic cuts
  double m_minDeltaY;//absolute deltaY
  double m_maxDeltaY;
  double m_minPt1;//GeV
  double m_minPt2;
  double m_maxPt1;
  double m_maxPt2;

  /// Gaussian distributions describing the event probability density in the deltaY-pt plane!
  std::vector<double> m_muXs;
  std::vector<double> m_muYs;
  std::vector<double> m_sigmaXs;
  std::vector<double> m_sigmaYs;
  std::vector<double> m_rhos;
  std::vector<double> m_weights;
  double m_suppressionFactor;

  /// Random number generator
  TRandom3* myRandGen;

  /// Important functions
  double m_evaluatePDF(double x, double y, int gausIndex);
  double m_calculateProbToKeep(double absoluteDeltaY, double leadJetPt);

};

#endif
