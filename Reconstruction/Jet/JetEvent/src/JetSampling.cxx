/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetEvent/JetSampling.h"

JetSampling::JetSampling()
  : m_pr(0.,0.,0.,0.)     // reconstructed at EM scale 
    , m_pt(0.,0.,0.,0.)   // PIC jet
    , m_pn(0.,0.,0.,0.)   // NTJ jet
    , m_pd(0.,0.,0.,0.)   // Data driven jet kinematics
    , m_h1(0.,0.,0.,0.)   // calibrated H1 jet
    , m_pisa(0.,0.,0.,0.) // calibrated pisa jet
    , m_samp(0.,0.,0.,0.) // calibrated sampling jet
{
  // Initialize members to zero
  this->init();
}

JetSampling::~JetSampling()
{}

void
JetSampling::init()
{
  // Initialize all private members to zero

  // Distantce to NTJ jet
  m_dr1 = 0.;
  m_dr2 = 0.;

  // Sampling or Layer members
  m_ejsPreSamplerB = 0;
  m_ejsPreSamplerE = 0;
  m_ejsEMB1 = 0;
  m_ejsEME1 = 0;
  m_ejsEMB2 = 0;
  m_ejsEME2 = 0;
  m_ejsEMB3 = 0;
  m_ejsEME3 = 0;
  m_ejsTileBar0 = 0;
  m_ejsTileExt0 = 0;
  m_ejsTileBar1 = 0;
  m_ejsTileExt1 = 0;
  m_ejsTileBar2 = 0;
  m_ejsTileExt2 = 0;
  m_ejsHEC0 = 0;
  m_ejsHEC1 = 0;
  m_ejsHEC2 = 0;
  m_ejsHEC3 = 0;
  m_ejsTileGap1 = 0;
  m_ejsTileGap2 = 0;
  m_ejsTileGap3 = 0;
  m_ejsFCAL0 = 0;
  m_ejsFCAL1 = 0;
  m_ejsFCAL2 = 0;
  m_tot  = 0.;
  m_ctot = 0.;
  m_ehad = 0.;
  m_eem  = 0.;

  m_eCryo  = 0.;
  m_eGap   = 0.;
  m_eScint = 0.;
  m_eNull  = 0.;
  
  // JetECS: Loop over the 16 cell energy density bins
  for(int ie=0; ie<16; ++ie) {
    m_ePreSamBCell.at(ie)  = 0.;
    m_ePreSamECell.at(ie)  = 0.;
    //
    m_eEMB1Cell.at(ie)     = 0.;
    m_eEME1Cell.at(ie)     = 0.;
    //
    m_eEMB2Cell1.at(ie)    = 0.;
    m_eEMB2Cell2.at(ie)    = 0.;
    //
    m_eEMB3Cell1.at(ie)    = 0.;
    m_eEMB3Cell2.at(ie)    = 0.;
    //
    m_eEME2Cell1.at(ie)    = 0.;
    m_eEME2Cell2.at(ie)    = 0.;
    //
    m_eEME3Cell1.at(ie)    = 0.;
    m_eEME3Cell2.at(ie)    = 0.;
    //
    m_eTileBar0Cell.at(ie) = 0.;
    m_eTileBar1Cell.at(ie) = 0.;
    m_eTileBar2Cell.at(ie) = 0.;
    m_eTileExt0Cell.at(ie) = 0.;
    m_eTileExt1Cell.at(ie) = 0.;
    m_eTileExt2Cell.at(ie) = 0.;
    //
    m_eHec0Cell1.at(ie)    = 0.;
    m_eHec1Cell1.at(ie)    = 0.;
    m_eHec2Cell1.at(ie)    = 0.;
    m_eHec3Cell1.at(ie)    = 0.;
    //
    m_eHec0Cell2.at(ie)    = 0.;
    m_eHec1Cell2.at(ie)    = 0.;
    m_eHec2Cell2.at(ie)    = 0.;
    m_eHec3Cell2.at(ie)    = 0.;
    //
    m_eFCal0Cell.at(ie)    = 0.;
    m_eFCal1Cell.at(ie)    = 0.;
    m_eFCal2Cell.at(ie)    = 0.;
  }

  // Energy in cone Radii, loop over the 10 possible values
  for ( int i = 0; i < 10; ++i ) {
    m_erad_cells.at(i)  = 0.; // from cells
    m_erad_tracks.at(i) = 0.; // from tracks
  }
}

//////////////////////////////////////////////////
double JetSampling::eEMBCell(const int ireg) const {
  // Returns the jet energy in PreSamplerB and EMB1
  // ireg=0 PreSamplerB
  // ireg=1 EMB1
  const int dim=16;
  double energy = 0.;
  if (ireg == 0)      for (int i=0; i<dim; i++) energy += m_ePreSamBCell.at(i);
  else if (ireg == 1) for (int i=0; i<dim; i++) energy += m_eEMB1Cell.at(i);
  return energy;
}

//////////////////////////////////////////////////
double JetSampling::eEMBCell(const int ireg, const int ie) const {
  // Returns the jet energy in EMB2 and EMB3
  // ireg=0 EMB2+EMB3 eta<0.8
  // ireg=1 EMB2+EMB3 eta>=0.8
  double energy = 0.;
  if (ireg==0) {
    energy += m_eEMB2Cell1.at(ie);
    energy += m_eEMB3Cell1.at(ie);
  } else if (ireg==1) {
    energy += m_eEMB2Cell2.at(ie);
    energy += m_eEMB3Cell2.at(ie);
  }
  return energy;
}

//////////////////////////////////////////////////
double JetSampling::eEMECell(const int ireg) const {
  // Returns the jet energy in PreSamplerE and EME1
  // ireg=0 PreSamplerE
  // ireg=1 EME1
  const int dim=16;
  double energy=0.;
  if (ireg == 0)    for (int i=0; i<dim; i++) energy += m_ePreSamECell.at(i);
  else if (ireg==1) for (int i=0; i<dim; i++) energy += m_eEME1Cell.at(i);
  return energy;
}

//////////////////////////////////////////////////
double JetSampling::eEMECell(const int ireg, const int ie) const {
  // Returns the jet energy in EME2 and EME3
  // ireg=0 EME2+EME3 eta<2.5
  // ireg=1 EME2+EME3 eta>=2.5
  double energy = 0.;
  if (ireg==0) {
    energy += m_eEME2Cell1.at(ie);
    energy += m_eEME3Cell1.at(ie);
  } else if (ireg==1) {
    energy += m_eEME2Cell2.at(ie);
    energy += m_eEME3Cell2.at(ie);
  }
  return energy;
}

//////////////////////////////////////////////////
double JetSampling::eTileCell(const int ireg, const int ie) const {
  // Returns the jet energy in Tile
  // ireg=0  TileBar0+TileBar1+TileBar2
  // ireg=1  TileExt0+TileExt1+TileExt2
  double energy =0.;
  if (ireg==0){
    energy += m_eTileBar0Cell.at(ie);
    energy += m_eTileBar1Cell.at(ie);
    energy += m_eTileBar2Cell.at(ie);
  }else if (ireg==1) {
    energy += m_eTileExt0Cell.at(ie);
    energy += m_eTileExt1Cell.at(ie);
    energy += m_eTileExt2Cell.at(ie);
  }
  return energy;
}

//////////////////////////////////////////////////
double JetSampling::eHecCell(const int ireg, const int ie) const {
  // Returns the jet energy in HEC
  // ireg=0 HEC0 + HEC1 + HEC2 + HEC3  eta<2.5
  // ireg=1 HEC0 + HEC1 + HEC2 + HEC3  eta>=2.5
  double energy = 0.;
  if (ireg==0){
    energy += m_eHec0Cell1.at(ie);
    energy += m_eHec1Cell1.at(ie);
    energy += m_eHec2Cell1.at(ie);
    energy += m_eHec3Cell1.at(ie);
  }else if (ireg==1){
    energy += m_eHec0Cell2.at(ie);
    energy += m_eHec1Cell2.at(ie);
    energy += m_eHec2Cell2.at(ie);
    energy += m_eHec3Cell2.at(ie);
  }
  return energy;
}

//////////////////////////////////////////////////
double JetSampling::eFCalCell(const int ireg, const int ie) const {
  // Returns the jet energy in FCAL
  // ireg = 0 FCAL0
  // ireg = 1 FCAL1 + FCAL2
  if (ireg==0)      return m_eFCal0Cell.at(ie);
  else if (ireg==1) return (m_eFCal1Cell.at(ie) + m_eFCal2Cell.at(ie));
  return 0.;
}
