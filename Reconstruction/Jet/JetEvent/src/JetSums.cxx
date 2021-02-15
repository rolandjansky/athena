/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     JetSums.cxx

AUTHORS:  Frank Paige
CREATED:  May 2002

June 2004: Modified from CombinedJetSums to use Jet and new Navigation.

********************************************************************/

#include "JetEvent/JetSums.h"
#include "JetEvent/Jet.h"


JetSums::JetSums(double e, double et, double eta, double phi, double m)
{
  m_jetE = e;
  m_jetEt = et;
  m_jetEta = eta;
  m_jetPhi = phi;
  m_jetM = m;

  for(int ireg=0; ireg<2; ++ireg) {
    m_eEMB0Cell[ireg] = 0;
    m_eEME0Cell[ireg] = 0;
  }

  for(int ireg=0; ireg<2; ++ireg) {
    for(int ie=0; ie<m_dim; ++ie) {
      m_eEMB1Cell[ireg][ie] = 0;
      m_eEME1Cell[ireg][ie] = 0;
      m_eTileCell[ireg][ie] = 0;
      m_eHecCell[ireg][ie] = 0;
      m_eFCalCell[ireg][ie] = 0;
    }
  }
  m_eCryo = 0;
  m_eGap = 0;
  m_eNull = 0;

}

JetSums::~JetSums()
{ }

