/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Jean-Raphael Lessard, 2009

#ifndef TTBARLJDITOPSFITTED_P1_H
#define TTBARLJDITOPSFITTED_P1_H

#include "TopEventTPCnv/TTbarLJDiTops_p1.h"
#include <vector>

struct TTbarLJDiTopsFitted_p1 {
  
  TTbarLJDiTops_p1 m_initialTTbar;
  
  double m_rescalingHadB_E;
  double m_rescalingHadB_P;
  double m_hadB_deltaEta;
  double m_hadB_deltaPhi;
  
  double m_rescalingLepB_E;
  double m_rescalingLepB_P;
  double m_lepB_deltaEta;
  double m_lepB_deltaPhi;
  
  std::vector<double> m_rescalingLightJets_E;
  std::vector<double> m_rescalingLightJets_P;
  std::vector<double> m_lightJets_deltaEta;
  std::vector<double> m_lightJets_deltaPhi;
  
  double m_rescalingLepton_E;
  double m_rescalingLepton_P;
  double m_lepton_deltaEta;
  double m_lepton_deltaPhi;
  
  double m_rescalingEtMiss;
  double m_rescalingEtMissX;
  double m_rescalingEtMissY;
  double m_neutrinoPz;
  
  double m_permutationProbability;
  double m_permutationDiscriminant;
};

#endif //TTBARLJDITOPSFITTED_P1_H 
