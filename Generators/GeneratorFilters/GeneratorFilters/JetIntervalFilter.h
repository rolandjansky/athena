/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_JETINTERVALFILTER_H
#define GENERATORFILTERS_JETINTERVALFILTER_H

#include "GeneratorModules/GenFilter.h"
class TRandom3;

/// Filter events by interval between jets
/// @author Graham Jones
class JetIntervalFilter : public GenFilter {
public:

  JetIntervalFilter(const std::string & name, ISvcLocator * pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  std::string m_jetContainer;
  int m_jetNumber;
  bool m_jetOppositeSide;

  /// Jet Energy Cuts (give choice of et or pt cutting)
  bool m_jetCutByPt;
  double m_jet1MinEt;
  double m_jet1MaxEt;
  double m_jet2MinEt;
  double m_jet2MaxEt;

  double m_jet1MinPt;
  double m_jet1MaxPt;
  double m_jet2MinPt;
  double m_jet2MaxPt;

  /// Jet Position Cuts (absolute)
  double m_jet1MaxEta;
  double m_jet1MinEta;
  double m_jet2MaxEta;
  double m_jet2MinEta;

  /// Jet delta eta cuts
  double m_minDeltaEta;
  double m_maxDeltaEta;

  /// Filter with weight!
  bool m_weightingEvents;
  bool m_flatWeightBelowMean;
  double m_gausMean;
  double m_gausSigma;
  double m_absDEtaCutOff; //Above which keep all
  TRandom3* myRandGen;

  /// Important functions
  double m_calculateProbToKeep(double absoluteDeltaEta);

};

#endif
