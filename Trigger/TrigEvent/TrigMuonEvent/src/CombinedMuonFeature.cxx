// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// STL include(s):
#include <sstream>
#include <cmath>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigMuonEvent/CombinedMuonFeature.h"

// "Distance" used by the comparison operator(s):
static const double DELTA = 0.001;

CombinedMuonFeature::CombinedMuonFeature() : P4PtEtaPhiMBase(), NavigableTerminalNode(), 
   m_pt(0.), m_sigma_pt(0.), m_charge(0.0), m_flag(0), m_muFastTrack(), m_IDTrack()
{}

CombinedMuonFeature::CombinedMuonFeature(float ptsign, float sigma_pt,
                                         const ElementLink< MuonFeatureContainer>& muFastOutput,
                                         const ElementLink< TrigInDetTrackCollection>& IdscanOutput): 
   P4PtEtaPhiMBase(),
   NavigableTerminalNode(),
   m_pt(fabs(ptsign)),
   m_sigma_pt(sigma_pt),
   m_flag(0),
   m_muFastTrack(muFastOutput),
   m_IDTrack(IdscanOutput)
{
  if      (ptsign == 0) m_charge =  0.0;
  else if (ptsign > 0)  m_charge =  1.0;
  else                  m_charge = -1.0;
}

CombinedMuonFeature::CombinedMuonFeature(float ptsign, float sigma_pt, int fs, int fe, int fm,
                                         const ElementLink< MuonFeatureContainer>& muFastOutput,
                                         const ElementLink< TrigInDetTrackCollection>& IdscanOutput): 
   P4PtEtaPhiMBase(),
   NavigableTerminalNode(),
   m_pt(fabs(ptsign)),
   m_sigma_pt(sigma_pt),
   m_flag(fs*10000+fe*100+fm),
   m_muFastTrack(muFastOutput),
   m_IDTrack(IdscanOutput)
{
  if      (ptsign == 0) m_charge =  0.0;
  else if (ptsign > 0)  m_charge =  1.0;
  else                  m_charge = -1.0;
}

CombinedMuonFeature::CombinedMuonFeature(float pt, float sigma_pt, float q, int fs, int fe, int fm,
                                         const ElementLink< MuonFeatureContainer>& muFastOutput,
                                         const ElementLink< TrigInDetTrackCollection>& IdscanOutput):
   P4PtEtaPhiMBase(),
   NavigableTerminalNode(),
   m_pt(pt),
   m_sigma_pt(sigma_pt),
   m_charge(q),
   m_flag(fs*10000+fe*100+fm),
   m_muFastTrack(muFastOutput),
   m_IDTrack(IdscanOutput)
{}

CombinedMuonFeature::CombinedMuonFeature(float pt, float sigma_pt, float q,
                                         int flag,
                                         const ElementLink< MuonFeatureContainer>& muFastOutput,
                                         const ElementLink< TrigInDetTrackCollection>& IdscanOutput):
   P4PtEtaPhiMBase(),
   NavigableTerminalNode(),
   m_pt(pt),
   m_sigma_pt(sigma_pt),
   m_charge(q),
   m_flag(flag),
   m_muFastTrack(muFastOutput),
   m_IDTrack(IdscanOutput)
{}

double CombinedMuonFeature::sigma_pt(void) const {
  if (m_charge == 0) {//for backward compatibility
    if (m_sigma_pt < 0) return 0.;
    else {
      double ftmp{0};
      double stmp = modf(m_sigma_pt/10000., &ftmp)*1000000.;
      return static_cast<float>(stmp);
    }
  }
  else 
   return m_sigma_pt;
}

int CombinedMuonFeature::comb_strategy(void)            const {
  if (m_charge == 0) {//for backward compatibility
     if (m_sigma_pt < 0) return -1;
     else                return static_cast<int>(floor(m_sigma_pt/10000.));
  }
  else {
   int a = static_cast<int>(m_flag/10000);
   return a;
  }
}

int CombinedMuonFeature::comb_errorFlag(void)           const {
   int a = static_cast<int>(m_flag/10000);
   int b = static_cast<int>((m_flag-a*10000)/100);
   return b;
}

int CombinedMuonFeature::comb_matchFlag(void)           const {
   int a = static_cast<int>(m_flag/10000);
   int b = static_cast<int>((m_flag-a*10000)/100);
   int c = static_cast<int>(m_flag-a*10000-b*100);
   return c;
}

CombinedMuonFeature::CombinedMuonFeature(const CombinedMuonFeature*
                                         muon_feature) :
     I4Momentum( *muon_feature ),
     INavigable( *muon_feature ),
     INavigable4Momentum( *muon_feature ),
     P4PtEtaPhiMBase ( *muon_feature ),
     NavigableTerminalNode ( ),
     m_pt (muon_feature->m_pt), 
     m_sigma_pt (muon_feature->m_sigma_pt),
     m_charge (muon_feature->m_charge),
     m_flag (muon_feature->m_flag),
     m_muFastTrack (muon_feature->m_muFastTrack),
     m_IDTrack (muon_feature->m_IDTrack)
{}


CombinedMuonFeature::CombinedMuonFeature(const CombinedMuonFeature& 
                                         muon_feature)
   : I4Momentum(muon_feature),INavigable(muon_feature),IAthenaBarCode(muon_feature),INavigable4Momentum(muon_feature),
     P4PtEtaPhiMBase(muon_feature),NavigableTerminalNode( ),
     m_pt(muon_feature.m_pt),
     m_sigma_pt(muon_feature.m_sigma_pt),
     m_charge(muon_feature.m_charge),
     m_flag (muon_feature.m_flag),
     m_muFastTrack (muon_feature.m_muFastTrack),
     m_IDTrack (muon_feature.m_IDTrack)
{}



CombinedMuonFeature&
CombinedMuonFeature::operator=(const CombinedMuonFeature& muon_feature) {
   m_pt = muon_feature.m_pt;
   m_sigma_pt = muon_feature.m_sigma_pt;
   m_charge = muon_feature.m_charge;
   m_flag = muon_feature.m_flag,
   m_muFastTrack = muon_feature.m_muFastTrack;
   m_IDTrack = muon_feature.m_IDTrack;

   return *this;    
}

double CombinedMuonFeature::eta(void) const {
   return m_IDTrack.isValid() ? (*m_IDTrack)->param()->eta() : 0.0;
}

double CombinedMuonFeature::phi(void) const {
   return m_IDTrack.isValid() ? (*m_IDTrack)->param()->phi0() : 0.0;
}

//////////////////////////////////////////////////////////////////
// helper operators

std::string str ( const CombinedMuonFeature& d ) {
   std::stringstream ss;
   ss << "Pt: " << d.pt()
      << "; sigmaPt: " << d.sigma_pt()
      << "; charge: " << d.charge()
      << "; flag: " << d.getFlag()
      << "; Eta: " << d.eta()
      << "; Phi: " << d.phi();

   return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const CombinedMuonFeature& d ) {

   return ( m << str( d ) );
}

bool operator== ( const CombinedMuonFeature& a, const CombinedMuonFeature& b ) {

   if( std::abs( a.pt() - b.pt() ) > DELTA ) return false;
   if( std::abs( a.charge() - b.charge() ) > DELTA ) return false;
   if( std::abs( a.sigma_pt() - b.sigma_pt() ) > DELTA ) return false;
   if( std::abs( a.getFlag() - b.getFlag() ) > DELTA ) return false;
   if( std::abs( a.eta() - b.eta() ) > DELTA ) return false;
   if( std::abs( a.phi() - b.phi() ) > DELTA ) return false;

   return true;
}

/**
 * Function compares two objects and returns "semi verbose" output 
 * in the form of map where there are varaible names and relative differences
 * between two obejcts
 *
 * @param variableChange Map to record the differences
 * In case of collections (or objects when the size may be different) that
 * information can also be returned in varaibleChange
 */
void diff( const CombinedMuonFeature& a, const CombinedMuonFeature& b,
           std::map< std::string, double >& variableChange ) {

   if( std::abs( a.pt() - b.pt() ) > DELTA ) {
      variableChange[ "Pt" ] = a.pt() - b.pt();
   }
   if( std::abs( a.charge() - b.charge() ) > DELTA ) {
      variableChange[ "Charge" ] = a.charge() - b.charge();
   }
   if( std::abs( a.sigma_pt() - b.sigma_pt() ) > DELTA ) {
      variableChange[ "sigmaPt" ] = a.sigma_pt() - b.sigma_pt();
   }
   if( std::abs( a.getFlag() - b.getFlag() ) > DELTA ) {
      variableChange[ "flag" ] = a.getFlag() - b.getFlag();
   }
   if( std::abs( a.eta() - b.eta() ) > DELTA ) {
      variableChange[ "Eta" ] = a.eta() - b.eta();
   }
   if( std::abs( a.phi() - b.phi() ) > DELTA ) {
      variableChange[ "Phi" ] = a.phi() - b.phi();
   }

   return;
}

