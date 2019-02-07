/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <algorithm>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/Monitored.h"

#include "DecisionHandling/Combinators.h"

#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigMuisoHypoTool.h"

using namespace TrigCompositeUtils;
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuisoHypoTool::TrigMuisoHypoTool(const std::string& type, 
				     const std::string & name,
				     const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
}

TrigMuisoHypoTool::~TrigMuisoHypoTool(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuisoHypoTool::initialize()
{

  ATH_MSG_DEBUG("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  if (m_acceptAll) {
    ATH_MSG_DEBUG( "Accepting all the events with not cut!" );
  }
 
  ATH_MSG_DEBUG("Initialization completed successfully");
  return StatusCode::SUCCESS;
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigMuisoHypoTool::decideOnSingleObject( TrigMuisoHypoTool::MuisoInfo& input,
                                              size_t cutIndex ) const
{

  auto cutCounter = Monitored::Scalar( "CutCounter", 0 );
  auto sumPtCone  = Monitored::Scalar( "SumPtCone", 0 );

  auto monitorIt  = Monitored::Group( m_monTool, cutCounter, sumPtCone ); 

  ATH_MSG_VERBOSE( "Cut index " << cutIndex );

  // frag to pass or not
  bool result = false;

  if ( m_acceptAll ) {
    result = true;
    ATH_MSG_DEBUG( "Accept property is set: taking all the events" );
    return result;
  } else {
    result = false;
    ATH_MSG_DEBUG( "Accept property not set: applying selection!" );
  }

  const xAOD::L2IsoMuon* pMuonIso = input.muIso;

  // Requirements
  float ptsum  = 0.0;
  if (m_IDConeSize == 1)   ptsum  = pMuonIso->sumPt01();
  else if (m_IDConeSize == 2)   ptsum  = pMuonIso->sumPt02();
  else if (m_IDConeSize == 3)   ptsum  = pMuonIso->sumPt03();
  else if (m_IDConeSize == 4)   ptsum  = pMuonIso->sumPt04();
  else                          ptsum  = pMuonIso->sumPt02();

  float etamu  = pMuonIso->eta();
  float ptmu   = fabs(pMuonIso->pt());

  sumPtCone = ptsum / CLHEP::GeV;

  float isoID = 0.0;
  if (ptmu > 0) isoID = ptsum/ptmu;

  if (isoID < 0) {
     ATH_MSG_DEBUG( " isoID < 0 --> should never happen, set to zero " );
     isoID = 0.0;
  }

  bool isIsolatedID   = true;
  if (fabs(etamu) <= m_EtaRegion_1) {
     if (isoID > m_MaxIDIso_1.value())       isIsolatedID = false;
     if (isIsolatedID) cutCounter = 1;
  } else if (fabs(etamu) > m_EtaRegion_1 && fabs(etamu) <= m_EtaRegion_2) {
     if (isoID > m_MaxIDIso_2.value())       isIsolatedID = false;
     if (isIsolatedID) cutCounter = 2;
  } else if (fabs(etamu) > m_EtaRegion_2 && fabs(etamu) <= m_EtaRegion_3) {
     if (isoID > m_MaxIDIso_3.value())       isIsolatedID = false;
     if (isIsolatedID) cutCounter = 3;
  }

  result = isIsolatedID;

  ATH_MSG_DEBUG( " REGTEST pt / eta / SumPtCone / isoID : "
                 << " / " << ptmu / CLHEP::GeV
                 << " / " << etamu
                 << " / " << sumPtCone
                 << " / " << isoID
                 << " / Muon Isolation Hypotesis is " << (result ? "true" : "false") );

  return result;
}



// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuisoHypoTool::decide( std::vector<MuisoInfo>& toolInput ) const 
{
  size_t numMuon = toolInput.size();
  ATH_MSG_DEBUG("Retrieved from TrigMuisoHypoAlg and Running TrigMuisoHypoTool for selections.");
  ATH_MSG_DEBUG("Number of muon = " << numMuon );

  for( auto &i: toolInput ) {
     // If muon event has defference DecisionID, it shouldn't apply.   
     if( TrigCompositeUtils::passed( m_decisionId.numeric(), i.previousDecisionIDs )) {
       if( decideOnSingleObject( i, 0 ) == true ) {
         ATH_MSG_DEBUG("Pass through selection " << m_decisionId );
         TrigCompositeUtils::addDecisionID(m_decisionId, i.decision);
       } else {
         ATH_MSG_DEBUG("Not pass through selection " << m_decisionId );
       }
     } else {
       ATH_MSG_DEBUG("Not match DecisionID:" << m_decisionId );
     }
  }

  return StatusCode::SUCCESS;
}


