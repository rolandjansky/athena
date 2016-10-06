/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTUtils.h"
#include "ViewAlgs/HypoDecision.h"



bool HypoDecision::containsDecision( const xAOD::TrigComposite& src ) {
  return src.hasDetail<std::vector<unsigned>>("Passed");
}

HypoDecision::HypoDecision( const xAOD::TrigComposite& src ) {
  src.getDetail( "Passed", m_passed );
  src.getDetail( "Failed", m_failed ); // this may be missing
}

void HypoDecision::store( xAOD::TrigComposite& destination ) const {

  std::vector<unsigned> temp_passed( m_passed.begin(), m_passed.end() );
  destination.setDetail( "Passed", temp_passed );

  std::vector<unsigned> temp_failed( m_failed.begin(), m_failed.end() );
  destination.setDetail( "Failed", temp_failed );
}


void HypoDecision::addDecision( const std::string& hypo_name, bool decision ) {
  unsigned hypo_id = TrigConf::HLTUtils::string2hash(hypo_name, "TE");
  addDecision(hypo_id, decision);
}

void HypoDecision::addDecision( unsigned hypo_id, bool decision ) {
  if ( decision ) 
    m_passed.push_back(hypo_id);
  else
    m_failed.push_back(hypo_id);
}

