/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************************************
//  Filename : TileMuFeature.cxx
//  Author   : Heuijin Lim
//  Created  : Sep. 2007
//
//  DESCRIPTION: 
//    Contain the muon candidates from TileCal and the combined tracks from 
//    Inner-Detector.		
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

// Gaudi/Athena includes:
#include "GaudiKernel/MsgStream.h"

// Trigger includes:
#include "TrigMuonEvent/TileMuFeature.h"

/** Distance used by the comparison operators */
static const double DELTA = 0.001;

TileMuFeature::TileMuFeature ()
{
  m_eta	= -999.9;
  m_phi	= -999.9;
  m_energy_deposited.clear();
  m_quality_factor = -999.9;
}

TileMuFeature::TileMuFeature ( float eta, float phi, const std::vector<float> & ener, float qual )
  : m_eta(eta)
  , m_phi(phi)
  , m_energy_deposited(ener)
  , m_quality_factor(qual)
{ 
}

TileMuFeature::TileMuFeature ( float eta, float phi, std::vector<float> && ener, float qual )
  : m_eta(eta)
  , m_phi(phi)
  , m_energy_deposited(std::move(ener))
  , m_quality_factor(qual)
{ 
}

TileMuFeature::TileMuFeature (const TileMuFeature* muon_feature) {
  m_eta	             = muon_feature->eta();
  m_phi	             = muon_feature->phi();
  m_energy_deposited = muon_feature->enedep();
  m_quality_factor   = muon_feature->quality();
}

TileMuFeature::TileMuFeature (const TileMuFeature& muon_feature) {
  m_eta              = muon_feature.eta();
  m_phi              = muon_feature.phi();
  m_energy_deposited = muon_feature.enedep();
  m_quality_factor   = muon_feature.quality();
}

TileMuFeature&
TileMuFeature::operator=(const TileMuFeature& muon_feature)
{
  if (this != &muon_feature) {
    m_eta              = muon_feature.eta();
    m_phi              = muon_feature.phi();  
    m_energy_deposited = muon_feature.enedep();  
    m_quality_factor   = muon_feature.quality();
  }

  return *this;
}

TileMuFeature&
TileMuFeature::operator=(TileMuFeature&& muon_feature)
{
  if (this != &muon_feature) {
    m_eta              = muon_feature.eta();
    m_phi              = muon_feature.phi();  
    m_energy_deposited = std::move(muon_feature.m_energy_deposited);
    m_quality_factor   = muon_feature.quality();
  }

  return *this;
}

/** Helper operators */

std::string str ( const TileMuFeature& d )
{
  std::stringstream ss;

  ss << "eta: " << d.eta()
     << "; phi: " << d.phi()
     << "; quality: " << d.quality();

  return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const TileMuFeature& d )
{
  return ( m << str( d ) );
}

bool operator== ( const TileMuFeature& a, const TileMuFeature& b )
{
  if( std::abs( a.eta() - b.eta() ) > DELTA ) return false;
  if( std::abs( a.phi() - b.phi() ) > DELTA ) return false;
  if( std::abs( a.quality() - b.quality() ) > DELTA ) return false;

  return true;
}

void diff( const TileMuFeature& a, const TileMuFeature& b, std::map< std::string, double >& variableChange )
{
  if( std::abs( a.eta() - b.eta() ) > DELTA ) {
    variableChange[ "eta" ] = a.eta() - b.eta();
  }
  if( std::abs( a.phi() - b.phi() ) > DELTA ) {
    variableChange[ "phi" ] = a.phi() - b.phi();
  }
  if( ( a.quality() - b.quality() ) > DELTA ) {
    variableChange[ "quality" ] = a.quality() - b.quality();
  }

  return;
}

/*
void TileMuFeature::Set( float eta, float phi, std::vector<float> & ener, float qual )
{
  m_eta = eta;
  m_phi = phi;
  m_energy_deposited = ener;
  m_quality_factor = qual;
}

void TileMuFeature::print() const
{
  std::cout << (std::string) (*this) << std::endl;
}

TileMuFeature::operator std::string() const
{
  const int MAX_BUF = 1024;
  char buf[MAX_BUF];
  std::ostringstream text(buf);

  //text << whoami();
  text << " eta=" << m_eta;
  text << " phi=" << m_phi;
  for (unsigned int i=0; i<m_energy_deposited.size(); ++i)
    text << " ene[" << i << "]=" << m_energy_deposited[i];
  text << " qual=" << m_quality_factor;

  return text.str();
}
*/

/*
void TileMuFeature::SetIDTrk(float Pt_IDTrk, float Eta_IDTrk, float EtaTR_IDTrk, float Phi_IDTrk, float PhiTR_IDTrk, float zPos_IDTrk, int Typ_IDTrk)
{
  m_Pt_IDTrk 	= Pt_IDTrk;
  m_Eta_IDTrk 	= Eta_IDTrk;
  m_EtaTR_IDTrk = EtaTR_IDTrk; 
  m_Phi_IDTrk	= Phi_IDTrk;
  m_PhiTR_IDTrk = PhiTR_IDTrk;
  m_zPos_IDTrk	= zPos_IDTrk;
  m_IDTrk	= Typ_IDTrk;
}
*/
