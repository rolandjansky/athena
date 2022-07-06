/**
 **     @file    TagNProbe.cxx
 **
 **     @author  mark sutton
 **     @date    Sat Apr  9 12:55:17 CEST 2022
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#include "TrigInDetAnalysisUtils/TagNProbe.h"


TagNProbe::TagNProbe( const std::string& refName, double massMin, double massMax, bool unique_flag ) :
  m_particleType(refName),
  m_mass(0),
  m_massMin(massMin),
  m_massMax(massMax),
  m_unique(unique_flag)
{
  const double muonMass     = 0.10565;  // GeV
  const double electronMass = 0.000510; // GeV
  const double tauMass      = 1.77686;  // GeV

  if      ( m_particleType.find("Muon")!=std::string::npos )      m_mass = muonMass;
  else if ( m_particleType.find("Electron")!=std::string::npos )  m_mass = electronMass;
  else if ( m_particleType.find("Tau")!=std::string::npos )       m_mass = tauMass;
}



double TagNProbe::mass_obj( const TIDA::Track* t1, const TIDA::Track* t2, TrigObjectMatcher* tom ) const {

  if ( tom!=0 && tom->status() ) { 
    return mass( tom->object(t1->id()), tom->object(t2->id()) );
  }
  else {
    return mass( t1, t2 );
  }
  
}


TIDA::Chain* TagNProbe::findChain( const std::string& chainname, std::vector<TIDA::Chain>& chains ) const {
  for ( size_t i=chains.size() ; i-- ; ) {
    if ( chains[i].name() == chainname ) return &chains[i];
  }
  return 0;
}


