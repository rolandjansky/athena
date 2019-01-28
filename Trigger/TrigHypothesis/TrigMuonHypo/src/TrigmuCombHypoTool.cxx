/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <algorithm>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/Monitored.h"

#include "DecisionHandling/Combinators.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/versions/L2CombinedMuonContainer_v1.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigMuonHypo/TrigmuCombHypoTool.h"

using namespace TrigCompositeUtils;
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigmuCombHypoTool::TrigmuCombHypoTool(const std::string& type, 
				       const std::string & name,
				       const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
}

TrigmuCombHypoTool::~TrigmuCombHypoTool(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoTool::initialize()
{
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
 
   ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );
 
   if (m_acceptAll) {
      ATH_MSG_DEBUG("AcceptAll = True");
      ATH_MSG_INFO("Accepting all the events with not cut!");
   } else {
      ATH_MSG_DEBUG("AcceptAll = False");
      m_bins.resize (m_ptBins.size());
      for ( size_t j=0; j<m_ptBins.size(); ++j) {
         m_bins[j] = m_ptBins[j].size() - 1;
         if (m_bins[j] != m_ptThresholds[j].size()) {
            ATH_MSG_INFO("bad thresholds setup .... exiting!");
            return StatusCode::FAILURE;
         }
 
         for (std::vector<float>::size_type i = 0; i < m_bins[j]; ++i) {
 
            ATH_MSG_INFO("bin[" << j << "] " << m_ptBins[j][i] << " - " <<  m_ptBins[j][i + 1]
               	  << " with Pt Threshold of " << (m_ptThresholds[j][i]) / CLHEP::GeV << " GeV");
         }
      }  
   }
 
   if ( not m_monTool.name().empty() ) {
      ATH_CHECK( m_monTool.retrieve() );
      ATH_MSG_DEBUG("MonTool name: " << m_monTool);
   }
 
   ATH_MSG_INFO("Initializing" << name() << "successfully");
 
   return StatusCode::SUCCESS;
}

bool TrigmuCombHypoTool::decideOnSingleObject(TrigmuCombHypoTool::CombinedMuonInfo& input, size_t cutIndex) const
{
 
   // defined Monitoring variables
   auto fexPt	= Monitored::Scalar("Pt", -9999.);
   auto ptFL	= Monitored::Scalar("PtFL", -9999.);
   auto Strategy	= Monitored::Scalar("StrategyFlag", 0);
   auto idEta 	= Monitored::Scalar("Eta", -9999.);
   auto idPhi 	= Monitored::Scalar("Phi", -9999.);
   auto idZ0 	= Monitored::Scalar("Z0", -9999.);
   auto idA0 	= Monitored::Scalar("A0", -9999.);
 
   auto monitorIt = Monitored::Group(m_monTool, fexPt, ptFL, Strategy, 
 					   idEta, idPhi, idZ0, idA0);

   ATH_MSG_VERBOSE( "Cut index " << cutIndex );

   // flag to pass or not
   bool result = false;
 
   if(m_acceptAll) {
      result = true;
      ATH_MSG_DEBUG("Accept property is set: taking all the events");
      return result;
   } else {
      result = false;
      ATH_MSG_DEBUG("Accept property not set: applying selection!");
   }
 
   //Retrieve combined muon
   //xAOD::L2CombinedMuon
   auto pMuon = input.muComb;
   if (!pMuon) {
      result = false;
      ATH_MSG_ERROR("Retrieval of xAOD::L2CombinedMuon from vector failed");
      return result;
   }
 
   auto ptValue = pMuon->pt() * pMuon->charge() / CLHEP::GeV;
 
   fexPt    = ptValue;
   ptFL      = ptValue;
   idEta    = pMuon->eta();
   idPhi    = pMuon->phi();
   int usealgo      = pMuon->strategy();
   float ptresComb = pMuon->sigmaPt() / CLHEP::GeV;
   Strategy  = usealgo;
   ATH_MSG_DEBUG("combined muon pt (GeV)/ sigma_pt (GeV)/ eta / phi / usedalgo: " 
               << fexPt << " (GeV) / " << ptresComb << " (GeV) / " << idEta << " / " << idPhi 
               << " / " << usealgo);
 
   if (pMuon->pt() == 0) {
      ATH_MSG_DEBUG("L2CombinedMuon pt == 0, empty container -> rejected");
      result = false;
      return result;
   }
 
   // check the pointers to the L2StandAloneMuon
   if (!pMuon->muSATrack()) {
      ATH_MSG_DEBUG("L2CombinedMuon has no valid xaOD::L2StandaloneMuon -> rejected");
      result = false;
      return result;
   }
 
   // check the pointer to the ID track
   if (!pMuon->idTrack()) {
      ATH_MSG_DEBUG("L2CombinedMuon has no valid xAOD:TrackParticle IDtrack -> rejected");
      result = false;
      return result;
   }
 
   idA0     = pMuon->idTrack()->d0();
   idZ0     = pMuon->idTrack()->z0();
 
   //Get the Pt cut for that eta bin
   float threshold = (idEta != -9999) ? 0 : 99999999;
   float absEta = fabs(idEta);
   for (std::vector<float>::size_type i = 0; i < m_bins[cutIndex]; ++i) {
      if (absEta > m_ptBins[cutIndex][i] && absEta < m_ptBins[cutIndex][i + 1]) threshold = m_ptThresholds[cutIndex][i];
   }
 
   // Check pt threshold for hypothesis and pi/k rejection cuts,
   // convert units since Muonfeature is in GeV
 
   //Kpi rejection
   bool pikCut = true;
   if (m_pikCuts && (std::abs(fexPt) < m_maxPtToApplyPik)) {
      if (pMuon->idTrack()->chiSquared() > m_chi2MaxID) pikCut = false;
   }
 
   //Std Pt cut
   bool stdCut = true;
   if (std::abs(fexPt) <= (threshold / CLHEP::GeV)) stdCut = false;
   ATH_MSG_DEBUG("REGTEST muon pt is " << fexPt
              << " GeV and threshold cut is " << threshold / CLHEP::GeV
              << " GeV and pik_cut is " << (pikCut ? "true" : "false"));
 
   //Strategy dependent Pt cuts
   bool sdpCut = true;
   if (m_strategydependent && usealgo > 0) {
      if (usealgo >= 1 && usealgo <= 4) {
         double tmpcut = m_strategyDependentPtCuts.value()[usealgo - 1];
         if (std::abs(fexPt) <= std::abs(tmpcut)) sdpCut = false;
         if (tmpcut < 0) stdCut = true; //Do not apply std Pt cut
         ATH_MSG_DEBUG("REGTEST muon pt is " << fexPt << " GeV"
                    << " and threshold for strategy dependent cut is " << tmpcut
                    << " GeV and strategy dependent / std cuts are " << (sdpCut ? "true" : "false") << " / " << (stdCut ? "true" : "false"));
      } else {
         ATH_MSG_DEBUG("usealgo out of range, is: " << usealgo << " while should be in [1, 4]");
      }
   }
 
   result = stdCut && pikCut && sdpCut;
 
   if (result) ptFL = -9999.;
 
   ATH_MSG_DEBUG("REGTEST: Muon passed pt threshold: " << (stdCut ? "true" : "false")
              << " and pik_cut is " << (pikCut ? "true" : "false")
              << " and strategy dependent cuts is " << (sdpCut ? "true" : "false")
              << " so hypothesis is " << (result ? "true" : "false"));
 
   return result;
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

// from TrigmuCombHypoAlg 
StatusCode TrigmuCombHypoTool::decide(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& toolInput) const
{
   size_t numTrigger = m_ptBins.size();
   size_t numMuon = toolInput.size();
   ATH_MSG_DEBUG("Retrieved from TrigmuCombHypoAlg and Running TrigmuCombHypoTool for selections.");
  
   if ( numTrigger == 1 ) {	// in case of HLT_mu4, HLT_mu6 and so on.
      ATH_MSG_DEBUG("Number of muon event = " << numMuon );
      ATH_MSG_DEBUG("Applying selection of single << " << m_decisionId );
      return inclusiveSelection(toolInput); 
   } else {			// in case of HLT_2mu6 and so on.
      if ( numMuon == 1 ) {	// If RoIs have only 1 muon, multipul selection wouldn't run.
         ATH_MSG_DEBUG("Number of muon event = " << numMuon );
         ATH_MSG_DEBUG("Not applying selection " << m_decisionId << " because of " << numMuon << " muon" );
      } else {			// IF RoIs have some muon, multipul selection would run.
         ATH_MSG_DEBUG("Number of muon event = " << numMuon );
         ATH_MSG_DEBUG("Applying selection of multiplicity << " << m_decisionId );
         return multiplicitySelection(toolInput);
      }
   }
 
   return StatusCode::SUCCESS;
}


StatusCode TrigmuCombHypoTool::inclusiveSelection(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& input) const
{
   for ( auto& i: input) {
     // If muon event has defference DecisionID, it shouldn't apply.   
     if (TrigCompositeUtils::passed(m_decisionId.numeric(), i.previousDecisionIDs)) {
       if ( decideOnSingleObject(i, 0)==true ) {
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


StatusCode TrigmuCombHypoTool::multiplicitySelection(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& input) const
{
   HLT::Index2DVec passingSelection( input.size() );

   for ( size_t cutIndex=0; cutIndex < m_ptBins.size(); ++cutIndex ) {
      size_t elementIndex{ 0 };      
      for ( auto& i: input ) {
         // If muon event has defference DecisionID, it shouldn't apply.   
         if ( TrigCompositeUtils::passed( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
            if ( decideOnSingleObject( i, cutIndex ) == true ) {
               ATH_MSG_DEBUG("Pass through selection " << m_decisionId << " : Event[" << elementIndex << "]" );
               passingSelection[cutIndex].push_back( elementIndex );
            } else {
               ATH_MSG_DEBUG("Not pass through selection " << m_decisionId << " : Event[" << elementIndex << "]" );
            } 
         } else {
            ATH_MSG_DEBUG("Not match DecisionID " << m_decisionId );
         }
         elementIndex++;
      }

      // If no object passes the selection, multipul selection should stop.
      if ( passingSelection[cutIndex].empty() ) {
         ATH_MSG_DEBUG( "No object passed selection " << cutIndex << " rejecting" );
         return StatusCode::SUCCESS;
      }
   }

   std::set<size_t> passingIndices;
   if ( m_decisionPerCluster==true ) {
      auto notFromSameRoI = [&]( const HLT::Index1DVec& comb ) {
         std::set<const xAOD::L2CombinedMuon*> setOfClusters;
         for ( auto index: comb ) {
            setOfClusters.insert( input[index].muComb );
         }
         return setOfClusters.size() == comb.size();
      };

      HLT::elementsInUniqueCombinations( passingSelection, passingIndices, notFromSameRoI );    

   } else {
      HLT::elementsInUniqueCombinations( passingSelection, passingIndices );
   }

   if ( passingIndices.empty() ) {
      ATH_MSG_DEBUG("No muon event passed through selection " << m_decisionId );
      return StatusCode::SUCCESS;
   }

   for ( auto idx: passingIndices ) {
      ATH_MSG_DEBUG("Muon event[" << idx << "] passes through Chain/ID " << m_decisionId 
                    << " with pT = " << input[idx].muComb->pt() << "GeV" );
      TrigCompositeUtils::addDecisionID( m_decisionId.numeric(), input[idx].decision );
   }

   return StatusCode::SUCCESS;
}

