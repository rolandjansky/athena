/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrigEgammaForwardFastCaloHypoTool.h"


using namespace TrigCompositeUtils;


TrigEgammaForwardFastCaloHypoTool::TrigEgammaForwardFastCaloHypoTool( const std::string& type, 
    const std::string& name, 
    const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) ,
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
{
  declareProperty("LumiBlockMuTool", m_lumiBlockMuTool, "Luminosity Tool" );
}


TrigEgammaForwardFastCaloHypoTool::~TrigEgammaForwardFastCaloHypoTool(){}



StatusCode TrigEgammaForwardFastCaloHypoTool::initialize()  
{

  ATH_MSG_DEBUG( "AcceptAll      = " << ( m_acceptAll==true ? "True" : "False" ) );
  
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );
  
  if (m_lumiBlockMuTool.retrieve().isFailure())
    return StatusCode::FAILURE;
  
  ATH_MSG_DEBUG( "Initialization completed successfully"   );   

  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaForwardFastCaloHypoTool::decide( std::vector<FastClusterInfo>& input )  const {
  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
	      addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}


bool TrigEgammaForwardFastCaloHypoTool::decide( const ITrigEgammaForwardFastCaloHypoTool::FastClusterInfo& /*input*/ ) const {
  return true;
}

