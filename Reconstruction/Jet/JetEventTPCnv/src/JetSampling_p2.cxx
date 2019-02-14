///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetSampling_p2.cxx 
// Implementation file for class JetSampling_p2
// Belen.Salvachua@cern.ch
// Based on JetSampling_p1.cxx from S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// JetEventTPCnv includes
#include "JetEventTPCnv/JetSampling_p2.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

JetSampling_p2::JetSampling_p2()
  : m_dr1(0)
  , m_dr2(0)
  , m_ejsPreSamplerB(0)
  , m_ejsPreSamplerE(0)
  , m_ejsEMB1(0)
  , m_ejsEME1(0)  
  , m_ejsEMB2(0)
  , m_ejsEME2(0)
  , m_ejsEMB3(0)
  , m_ejsEME3(0)
  , m_ejsTileBar0(0)
  , m_ejsTileExt0(0)
  , m_ejsTileBar1(0)
  , m_ejsTileExt1(0)
  , m_ejsTileBar2(0)
  , m_ejsTileExt2(0)  
  , m_ejsHEC0(0)
  , m_ejsHEC1(0)
  , m_ejsHEC2(0)
  , m_ejsHEC3(0)
  , m_ejsTileGap1(0)
  , m_ejsTileGap2(0)
  , m_ejsTileGap3(0)
  , m_ejsFCAL0(0)
  , m_ejsFCAL1(0)
  , m_ejsFCAL2(0)
  , m_tot(0)
  , m_ctot(0)
  , m_ehad(0)
  , m_eem(0) 
  , m_eCryo(0)
  , m_eGap(0)
  , m_eScint(0)
  , m_eNull(0)
{
  m_ePreSamBCell.assign(0);
  m_ePreSamECell.assign(0);
  m_eEMB1Cell.assign(0);
  m_eEMB2Cell1.assign(0);
  m_eEMB2Cell2.assign(0);
  m_eEMB3Cell1.assign(0);
  m_eEMB3Cell2.assign(0);
  m_eEME1Cell.assign(0);
  m_eEME2Cell1.assign(0);
  m_eEME2Cell2.assign(0);
  m_eEME3Cell1.assign(0);
  m_eEME3Cell2.assign(0);
  m_eTileBar0Cell.assign(0);
  m_eTileBar1Cell.assign(0);
  m_eTileBar2Cell.assign(0);
  m_eTileExt0Cell.assign(0);
  m_eTileExt1Cell.assign(0);
  m_eTileExt2Cell.assign(0);
  m_eHec0Cell1.assign(0);
  m_eHec0Cell2.assign(0);
  m_eHec1Cell1.assign(0);
  m_eHec1Cell2.assign(0);
  m_eHec2Cell1.assign(0);
  m_eHec2Cell2.assign(0);
  m_eHec3Cell1.assign(0);
  m_eHec3Cell2.assign(0);
  m_eFCal0Cell.assign(0);
  m_eFCal1Cell.assign(0);
  m_eFCal2Cell.assign(0);
  m_erad_cells.assign(0);
  m_erad_tracks.assign(0);
}

