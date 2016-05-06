/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowDatabase.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  23rd Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowDatabase.h"
#include "eflowRec/eflowCaloRegions.h"

//CLHEP Headers
#include "GaudiKernel/SystemOfUnits.h"

//C++ Headers
#include <cmath>

eflowDatabase::eflowDatabase() :
  m_cellEtaWidth(eflowCalo::nRegions),
  m_cellPhiWidth(eflowCalo::nRegions)
{

  m_etaUnit = 0.025;   // (2nd layer EM)
  m_phiUnit = M_PI / 128; // ~0.025 (2nd layer EM) 

  m_emX0PerUnitLength = 47.44680851/Gaudi::Units::meter; // calo tdr fig.2-ii
  m_hadX0PerUnitLength = 44.44444444/Gaudi::Units::meter; // atlas note tile-cal-no-127 (J.A. Budagov et.al.)
  
  m_cellEtaWidth[eflowCalo::EMB1] = 1.0 / 8.0;
  m_cellEtaWidth[eflowCalo::EMB2] = 1.0;
  m_cellEtaWidth[eflowCalo::EMB3] = 2.0;
  m_cellEtaWidth[eflowCalo::EME1] = 1.0 / 8.0;
  m_cellEtaWidth[eflowCalo::EME2] = 1.0;
  m_cellEtaWidth[eflowCalo::EME3] = 2.0;
  m_cellEtaWidth[eflowCalo::HEC1] = 4.0;
  m_cellEtaWidth[eflowCalo::HEC2] = 4.0;
  m_cellEtaWidth[eflowCalo::HEC3] = 4.0;
  m_cellEtaWidth[eflowCalo::HEC4] = 4.0;
  m_cellEtaWidth[eflowCalo::Tile1] = 4.0;
  m_cellEtaWidth[eflowCalo::Tile2] = 4.0;
  m_cellEtaWidth[eflowCalo::Tile3] = 8.0;

  m_cellPhiWidth[eflowCalo::EMB1] = 4.0;
  m_cellPhiWidth[eflowCalo::EMB2] = 1.0;
  m_cellPhiWidth[eflowCalo::EMB3] = 1.0;
  m_cellPhiWidth[eflowCalo::EME1] = 4.0;
  m_cellPhiWidth[eflowCalo::EME2] = 1.0;
  m_cellPhiWidth[eflowCalo::EME3] = 1.0;
  m_cellPhiWidth[eflowCalo::HEC1] = 4.0;
  m_cellPhiWidth[eflowCalo::HEC2] = 4.0;
  m_cellPhiWidth[eflowCalo::HEC3] = 4.0;
  m_cellPhiWidth[eflowCalo::HEC4] = 4.0;
  m_cellPhiWidth[eflowCalo::Tile1] = 4.0;
  m_cellPhiWidth[eflowCalo::Tile2] = 4.0;
  m_cellPhiWidth[eflowCalo::Tile3] = 4.0;
}
