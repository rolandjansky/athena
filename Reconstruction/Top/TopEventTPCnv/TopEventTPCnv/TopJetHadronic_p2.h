/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETHADRONIC_P2_H
#define TOPJETHADRONIC_P2_H

#include "TopEventTPCnv/TopJet_p1.h"
#include "DataModelAthenaPool/ElementLink_p3.h"

struct TopJetHadronic_p2 : public TopJet_p1 {
  float m_split12;
  float m_split23;
  float m_split34;
  float m_scalefactor;
  int m_nsubjet;
  float m_zcut12;
  float m_zcut23;
  float m_zcut34;
  float m_qw;
  bool m_use_zcuts;
};

#endif/*TOPJETHADRONIC_P2_H*/
