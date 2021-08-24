/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTRTHTHhypoTool.h"
#include "AthenaMonitoringKernel/Monitored.h"
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

  auto fHT_road         = Monitored::Scalar( "HTRatioRoad", -1. ); 
  auto fHT_wedge        = Monitored::Scalar( "HTRatioWedge", -1. );
  auto trththits_road   = Monitored::Scalar( "TRTHTHitsRoad"   , -1.0 );
  auto trththits_wedge  = Monitored::Scalar( "TRTHTHitsWedge", -1.0 );
  auto monitorIt        = Monitored::Group( m_monTool, fHT_road, fHT_wedge, trththits_road, trththits_wedge);
  
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

  fHT_road = vec.at(1);
  fHT_wedge = vec.at(3);
  trththits_road = vec.at(0);
  trththits_wedge = vec.at(2);

  ATH_MSG_DEBUG ( "trththits_road: " << trththits_road);
  ATH_MSG_DEBUG ("m_minTRTHTHitsRoad in hypotool: "<<m_minTRTHTHitsRoad);
  ATH_MSG_DEBUG ( "trththits_wedge: " << trththits_wedge);
  ATH_MSG_DEBUG ("m_minTRTHTHitsWedge in hypotool:: "<< m_minTRTHTHitsWedge);
  ATH_MSG_DEBUG ( "fHT_road: " << fHT_road);
  ATH_MSG_DEBUG ("m_minHTratioRoad in hypotool: "<<m_minHTratioRoad); 
  ATH_MSG_DEBUG ( "fHT_wedge: " << fHT_wedge);
  ATH_MSG_DEBUG ("m_minHTratioWedge in hypotool: "<<m_minHTratioWedge);

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

