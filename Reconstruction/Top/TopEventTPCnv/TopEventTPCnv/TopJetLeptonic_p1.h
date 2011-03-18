/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETLEPTONIC_P1_H
#define TOPJETLEPTONIC_P1_H

#include "TopEventTPCnv/TopJet_p1.h"

#include <vector>

#include "DataModelAthenaPool/ElementLink_p3.h"

struct TopJetLeptonic_p1 : public TopJet_p1 {
  ElementLinkInt_p3 m_lepton;
  float m_x_lepton;
  float m_y_lepton;
  float m_yprime_lepton;
};

#endif/*TOPJETLEPTONIC_P1_H*/
