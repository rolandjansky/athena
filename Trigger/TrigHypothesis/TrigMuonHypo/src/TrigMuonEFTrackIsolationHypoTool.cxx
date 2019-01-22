/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <algorithm>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/Monitored.h"

#include "DecisionHandling/Combinators.h"

#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigMuonHypo/TrigMuonEFTrackIsolationHypoTool.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

using namespace TrigCompositeUtils;
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFTrackIsolationHypoTool::TrigMuonEFTrackIsolationHypoTool(const std::string& type, 
                                                                   const std::string & name,
                                                                   const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
}

TrigMuonEFTrackIsolationHypoTool::~TrigMuonEFTrackIsolationHypoTool(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFTrackIsolationHypoTool::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  if ( m_acceptAll ) {
    ATH_MSG_DEBUG("AcceptAll = True");
    ATH_MSG_DEBUG("Accepting all the events with not cut!");
  }
  else {
    ATH_MSG_DEBUG("AcceptAll = False");
    if ( m_ptcone02_cut < 0.0 && m_ptcone03_cut < 0.0 ) {
      ATH_MSG_ERROR("Configured to apply cuts, but not cut was specified");
      return StatusCode::SUCCESS;
    }
    if ( m_ptcone02_cut > 0.0 ) {
      if ( m_abscut ) {
        ATH_MSG_DEBUG("Requiring sum pT in 0.2 cone < " << m_ptcone02_cut.value() << " MeV");
      } else {
        ATH_MSG_DEBUG("Requiring sum pT in 0.2 cone / muon pT < " << m_ptcone02_cut.value());
      }
    }
    if ( m_ptcone03_cut > 0.0 ) {
      if ( m_abscut ) {
        ATH_MSG_DEBUG("Requiring sum pT in 0.3 cone < " << m_ptcone03_cut.value() << " MeV");
      } else {
        ATH_MSG_DEBUG("Requiring sum pT in 0.3 cone / muon pT < " << m_ptcone03_cut.value());
      }
    }
  }

  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("MonTool name: " << m_monTool);
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigMuonEFTrackIsolationHypoTool::decideOnSingleObject(TrigMuonEFTrackIsolationHypoTool::EFIsolationMuonInfo& input, size_t cutIndex) const
{
   ATH_MSG_DEBUG("Decision ...");

   std::vector<float> ini_ptcone02(0);
   std::vector<float> ini_ptcone03(0);
   auto fex_ptcone02 = Monitored::Collection("PtCone02", ini_ptcone02);
   auto fex_ptcone03 = Monitored::Collection("PtCone03", ini_ptcone03);

   auto monitorIt    = Monitored::Group( m_monTool, fex_ptcone02, fex_ptcone03 );

   ATH_MSG_VERBOSE( "Cut index " << cutIndex );

   bool result = false;
 
   if( m_acceptAll ) {
     result = true;
     ATH_MSG_DEBUG("Accept property is set: taking all the events");	    
     return result;
   } else {
     result = false;
     ATH_MSG_DEBUG("Accept property not set: applying selection!");
   }
   
   auto pMuon = input.muEFIso;
   if ( !pMuon ) {
     result = false;
     ATH_MSG_ERROR("Retrieval of L2StandAloneMuon from vector failed");
     return result;
   }

   const xAOD::Muon::MuonType muontype = pMuon->muonType();

   if ( m_requireCombined && muontype != xAOD::Muon::MuonType::Combined ) {
     result = false;
     ATH_MSG_DEBUG("Require combined muon, but this muon is not combined. -> false");
     return result;
   } else if ( muontype != xAOD::Muon::MuonType::MuonStandAlone && muontype != xAOD::Muon::MuonType::Combined ) {
     result = false;
     ATH_MSG_DEBUG("This muon type is neither standalone nor  combined. -> false");
     return result;
   }

   float ptcone20(-1), ptcone30(-1);
   bool res = false; 
   if ( m_useVarIso ) {
     res = pMuon->isolation( ptcone20, xAOD::Iso::IsolationType::ptvarcone20 );
     if ( !res ) ATH_MSG_WARNING("Problem accessing ptvarcone20, " << ptcone20);
   } else {
     res = pMuon->isolation( ptcone20, xAOD::Iso::IsolationType::ptcone20 );
     if ( !res ) ATH_MSG_WARNING("Problem accessing ptcone20, " << ptcone20);
   }
   if( m_useVarIso ){
     res = pMuon->isolation( ptcone30, xAOD::Iso::IsolationType::ptvarcone30 );
     if ( !res ) ATH_MSG_WARNING("Problem accessing ptvarcone30, " << ptcone30);
   } else {
     res = pMuon->isolation( ptcone30, xAOD::Iso::IsolationType::ptcone30 );
     if ( !res ) ATH_MSG_WARNING("Problem accessing ptcone30, " << ptcone30);
   }
 
   // monitoring
   ini_ptcone02.push_back(ptcone20/1000.0);
   ini_ptcone03.push_back(ptcone30/1000.0);
   
   bool goodmu=true;
   
   if ( m_abscut ) { //absolute cut
     if ( m_ptcone02_cut > 0.0 ) {
       if ( ptcone20 >= m_ptcone02_cut ) goodmu=false;
     }
     if ( m_ptcone03_cut > 0.0 ) {
       if ( ptcone30 >= m_ptcone03_cut ) goodmu=false;
     }
     
      ATH_MSG_DEBUG("Muon with pT cone 0.2 = " << ptcone20 << ", pT cone 0.3 = " << ptcone30
      		    << " so result for this muon is " << (goodmu?"true":"false") );
   } else { //relative cut
     
     const double mupt = pMuon->pt();
     ATH_MSG_DEBUG("Muon with pT " << mupt);
      
     // now we can make the cut(s)
     if ( m_ptcone02_cut > 0.0 ) {
       if( ptcone20/mupt >= m_ptcone02_cut ) goodmu=false;
     }
     if ( m_ptcone03_cut > 0.0 ) {
       if ( ptcone30/mupt >= m_ptcone03_cut ) goodmu=false;
     }
     
     ATH_MSG_DEBUG("Muon with pT cone 0.2 / pt = " << ptcone20/mupt << ", pT cone 0.3 / pt = " << ptcone30/mupt
                   << " so result for this muon is " << (goodmu?"true":"false") );
   }
   
   if ( goodmu )  {
     result = true;
   }  

   ATH_MSG_DEBUG("Algo result = " << (result?"true":"false") );
 
   return result;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFTrackIsolationHypoTool::decide(std::vector<EFIsolationMuonInfo>& toolInput) const 
{
   ATH_MSG_DEBUG("Deciding ...");

   size_t numMuon = toolInput.size();
   ATH_MSG_DEBUG("Number of muon event = " << numMuon); 

   if ( numMuon==0 ) {
     ATH_MSG_DEBUG("The event has no muon, so No applying selection " << m_decisionId);
     return StatusCode::SUCCESS;
   } else {
     ATH_MSG_DEBUG("Applying selection of single" << m_decisionId);
     return inclusiveSelection(toolInput);
   }

   return StatusCode::SUCCESS;
}


StatusCode TrigMuonEFTrackIsolationHypoTool::inclusiveSelection(std::vector<TrigMuonEFTrackIsolationHypoTool::EFIsolationMuonInfo>& toolInput) const
{
   ATH_MSG_DEBUG("Inclusive ...");

   for ( auto&i : toolInput ) {
     // If muon event has defference DecisionID, it shouldn't apply.   
     if ( TrigCompositeUtils::passed( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
       if ( decideOnSingleObject( i, 0 )==true ) {
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

