/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTRTHTHhypoTool.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"

namespace TCU = TrigCompositeUtils;

TrigTRTHTHhypoTool::TrigTRTHTHhypoTool( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) )
{}

StatusCode TrigTRTHTHhypoTool::initialize()  {

 ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  return StatusCode::SUCCESS;
}

bool TrigTRTHTHhypoTool::decide( const ITrigTRTHTHhypoTool::RNNOutputInfo& input ) const {

  bool pass = false;
  
  if( m_acceptAll ){
    pass=true;
    ATH_MSG_DEBUG ( "Accepting all events in " << name());
    return pass;
  }
  
  auto rnn = input.rnnOutput;
  std::vector<float> vec = rnn->rnnDecision();

  if (vec.size()!=6){
    pass = true;
    return pass;
  }

  //Parse the vector to assign fHT and nHT appropriately

  float fHT_road = vec.at(1);
  float fHT_wedge = vec.at(3);
  float trththits_road = vec.at(0);
  float trththits_wedge = vec.at(2);

  ATH_MSG_DEBUG ( "trththits_road: " << trththits_road);
  ATH_MSG_DEBUG ( "trththits_wedge: " << trththits_wedge);
  ATH_MSG_DEBUG ( "fHT_road: " << fHT_road); 
  ATH_MSG_DEBUG ( "fHT_wedge: " << fHT_wedge);

  //Apply the cuts

  if (fHT_road > m_minHTratioRoad && trththits_road > m_minTRTHTHitsRoad && m_doRoad) pass = true;

  if (fHT_wedge > m_minHTratioWedge && trththits_wedge > m_minTRTHTHitsWedge && m_doWedge) pass = true;

  ATH_MSG_DEBUG ( name() << " hypo result " << pass);

  return pass;
 
}


StatusCode TrigTRTHTHhypoTool::decide( std::vector<RNNOutputInfo>& input )  const {
  for ( auto& i: input ) {
    if ( TCU::passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
        TCU::addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

