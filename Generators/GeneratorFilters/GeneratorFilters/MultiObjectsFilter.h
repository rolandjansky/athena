/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_MULTIOBJECTSFILTER_H
#define GENERATORFILTERS_MULTIOBJECTSFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Multi Objects Filter
/// @author Michiru Kaneda, Feb 2008
class MultiObjectsFilter : public GenFilter {
public:

  MultiObjectsFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  double m_PtCut;
  double m_EtaCut;
  double m_jetPtCut;
  double m_jetEtaCut;
  std::vector<double> m_PtCutEach;
  double m_sumPtCut;
  bool m_useEle;
  bool m_useMuo;
  bool m_usePho;
  bool m_useJet;
  bool m_useSumPt;

  std::string m_TruthJetContainerName;

};

#endif
