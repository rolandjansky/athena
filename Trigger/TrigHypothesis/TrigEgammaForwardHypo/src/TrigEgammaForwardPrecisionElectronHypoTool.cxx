/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODEgamma/Electron.h"
#include "TrigEgammaForwardPrecisionElectronHypoTool.h"

namespace TCU = TrigCompositeUtils;

TrigEgammaForwardPrecisionElectronHypoTool::TrigEgammaForwardPrecisionElectronHypoTool( const std::string& type, 
    const std::string& name, 
    const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {
}


StatusCode TrigEgammaForwardPrecisionElectronHypoTool::initialize()  
{
  ATH_MSG_DEBUG( "Initialization completed successfully"   );    
 
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}


bool TrigEgammaForwardPrecisionElectronHypoTool::decide( const ITrigEgammaForwardPrecisionElectronHypoTool::ElectronInfo& /*input*/) const {

  bool pass = true;

  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;
}



int TrigEgammaForwardPrecisionElectronHypoTool::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}


StatusCode TrigEgammaForwardPrecisionElectronHypoTool::decide( std::vector<ElectronInfo>& input)  const {
  for ( auto& i: input ) {
    if ( TCU::passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
        TCU::addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}
