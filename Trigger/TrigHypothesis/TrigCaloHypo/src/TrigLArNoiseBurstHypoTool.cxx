
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigLArNoiseBurstHypoTool.h"


using namespace TrigCompositeUtils;

TrigLArNoiseBurstHypoTool::TrigLArNoiseBurstHypoTool( const std::string& type, 
            const std::string& name, 
            const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
}

StatusCode TrigLArNoiseBurstHypoTool::initialize()  {

  
  ATH_MSG_INFO("TrigLArNoiseBurstHypoTool initialization completed successfully.");
 
  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode TrigLArNoiseBurstHypoTool::decide( std::vector<FlagNoiseInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
    addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

bool TrigLArNoiseBurstHypoTool::decide( const ITrigLArNoiseBurstHypoTool::FlagNoiseInfo& input ) const {

  return input.flag;
}
