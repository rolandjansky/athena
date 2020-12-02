/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     JetSums.cxx

AUTHORS:  Frank Paige
CREATED:  May 2002

June 2004: Modified from CombinedJetSums to use Jet and new Navigation.

********************************************************************/

#include "JetEvent/JetECS.h"
#include "JetEvent/Jet.h"


JetECS::JetECS(double e, double eta, double phi, double m)
{
  m_jetRawE = e;
  m_jetEta = eta;
  m_jetPhi = phi;
  m_jetM = m;
  m_jetCalE = 0;

  for(int ie=0; ie<m_dim; ++ie) 
    {
      m_ePreSamBCell[ie] = 0;
      m_ePreSamECell[ie] = 0;
      m_eEMB1Cell[ie] = 0;
      m_eEMB2Cell[ie] = 0;
      m_eEMB3Cell[ie] = 0;
      m_eEME1Cell[ie] = 0;
      m_eEME2Cell[ie] = 0;
      m_eEME3Cell[ie] = 0;
      m_eTileBar0Cell[ie] = 0;
      m_eTileBar1Cell[ie] = 0;
      m_eTileBar2Cell[ie] = 0;
      m_eTileExt0Cell[ie] = 0;
      m_eTileExt1Cell[ie] = 0;
      m_eTileExt2Cell[ie] = 0;
      m_eHec0Cell[ie] = 0;
      m_eHec1Cell[ie] = 0;
      m_eHec2Cell[ie] = 0;
      m_eHec3Cell[ie] = 0;
      m_eFCal0Cell[ie] = 0;      
      m_eFCal1Cell[ie] = 0; 
      m_eFCal2Cell[ie] = 0;
    }
  
  m_eCryo = 0;
  m_eGap = 0;
  m_eNull = 0;
  
}

JetECS::~JetECS()
{ }

