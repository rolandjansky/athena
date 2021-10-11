/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigEgammaPrecisionTrackingHypoTool.h"


namespace TCU = TrigCompositeUtils;

TrigEgammaPrecisionTrackingHypoTool::TrigEgammaPrecisionTrackingHypoTool( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigEgammaPrecisionTrackingHypoTool::initialize()  
{
  ATH_MSG_DEBUG( "Initialization completed successfully"   );    
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}


bool TrigEgammaPrecisionTrackingHypoTool::decide() const {

  bool pass = false;
  
  if ( m_acceptAll ) 
  {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
  } else{
    ATH_MSG_DEBUG( "AcceptAll property not set: reprove all" );
  }

  return pass;
}

StatusCode TrigEgammaPrecisionTrackingHypoTool::decide( std::vector<ClusterInfo>& input )  const {
  for ( auto& i: input ) {
    if ( i.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) continue;
    TCU::addDecisionID( m_decisionId, i.decision );
  }
  return StatusCode::SUCCESS;
}

