/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagEmulationTool/TrigBtagEmulationJet.h"

using namespace Trig;

//**********************************************************************

bool Trig::sortEmulationJetByPt(Trig::TrigBtagEmulationJet* a,Trig::TrigBtagEmulationJet* b) { return a->pt() > b->pt(); }

//**********************************************************************

TrigBtagEmulationJet::TrigBtagEmulationJet()
  : m_pt( 0 ),
    m_eta( 0 ),
    m_phi( 0 ) {}
TrigBtagEmulationJet::TrigBtagEmulationJet(const xAOD::Jet *jet)
  : m_jet( new xAOD::Jet( *jet ) ),
    m_pt( jet->p4().Et() ),
    m_eta( jet->eta() ),
    m_phi( jet->phi() ) {}
TrigBtagEmulationJet::TrigBtagEmulationJet(const xAOD::JetRoI *jetRoI,bool isJJ)
  : m_jetRoI( new xAOD::JetRoI( *jetRoI ) ),
    m_pt( isJJ ? jetRoI->et4x4() * 0.001 : jetRoI->et8x8() * 0.001 ),
    m_eta( jetRoI->eta() ),
    m_phi( jetRoI->phi() ) {}
TrigBtagEmulationJet::TrigBtagEmulationJet(const TrigBtagEmulationJet& other)
  : m_jet( other.m_jet ? new xAOD::Jet( *other.m_jet ) : nullptr ),
    m_jetRoI( other.m_jetRoI ? new xAOD::JetRoI( *other.m_jetRoI ) : nullptr ),
    m_pt( other.m_pt ),
    m_eta( other.m_eta ),
    m_phi( other.m_phi ),
    m_weights( other.m_weights.begin(),other.m_weights.end() ) {}
TrigBtagEmulationJet::~TrigBtagEmulationJet() {}

double TrigBtagEmulationJet::pt() const { return m_pt; }
double TrigBtagEmulationJet::eta() const { return m_eta; }
double TrigBtagEmulationJet::phi() const { return m_phi; }

double TrigBtagEmulationJet::weights(std::string tagger) const { 
  if ( m_jetRoI ) return -1000;
  if ( !m_jet ) return -1000;
  if ( m_weights.find( tagger.c_str() ) == m_weights.end() ) return -1000;
  return m_weights.at(  tagger.c_str() );
}

void TrigBtagEmulationJet::weights(std::string tagger,double value) {
  if ( m_jetRoI ) return;
  if ( !m_jet ) return;
  if ( m_weights.find( tagger.c_str() ) != m_weights.end() ) m_weights.at( tagger.c_str() ) = value;
  else m_weights.insert( std::make_pair( tagger.c_str(),value ) );
}

std::map< std::string,double > TrigBtagEmulationJet::weights() const {
  std::map< std::string,double > output;
  if ( m_jetRoI ) return output;

  std::map< std::string,double >::const_iterator it = m_weights.begin();
  for ( ; it!=m_weights.end(); it++ ) 
    output.insert( std::make_pair( it->first.c_str(),it->second ) );

  return output;
}

xAOD::Jet* TrigBtagEmulationJet::jet() const {
  if (!m_jet) return nullptr;
  return new xAOD::Jet( *m_jet );
}

//**********************************************************************

