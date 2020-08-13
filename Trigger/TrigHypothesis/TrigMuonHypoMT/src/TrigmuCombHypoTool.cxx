/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigCompositeUtils/Combinators.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigmuCombHypoTool.h"
#include "CxxUtils/phihelper.h"

#include <cmath>

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
   ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );
 
   if (m_acceptAll) {
      ATH_MSG_INFO("Accepting all the events!");
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
               	  << " with Pt Threshold of " << (m_ptThresholds[j][i]) / Gaudi::Units::GeV << " GeV");
         }
      }  
   }
 
   if ( not m_monTool.name().empty() ) {
      ATH_CHECK( m_monTool.retrieve() );
      ATH_MSG_DEBUG("MonTool name: " << m_monTool);
   }

   // Overlap Removal
   if( m_applyOR ) {
     ATH_MSG_DEBUG( "--- overlap removal as: ---"     );
     if( m_requireDR ) {
       ATH_MSG_DEBUG( "+ dR cut:" );
       if( (m_etaBins.size()-1) != m_dRThres.size() ) {
	 ATH_MSG_ERROR( "bad thresholds setup .... exiting!" );
	 return StatusCode::FAILURE;
       }
       for(unsigned int i=0; i<m_dRThres.size(); i++) {
	 ATH_MSG_DEBUG( "     EtaBin " << m_etaBins[i] << " - " << m_etaBins[i+1]
			<< " : dR < " << m_dRThres[i] );
       }
     }
     if( m_requireMufastDR ) {
       ATH_MSG_DEBUG( "+ dr(by mF) cut:" );
       if( (m_etaBins.size()-1) != m_mufastDRThres.size() ) {
	 ATH_MSG_ERROR( "bad thresholds setup .... exiting!" );
	 return StatusCode::FAILURE;
       }
       for(unsigned int i=0; i<m_mufastDRThres.size(); i++) {
	 ATH_MSG_DEBUG( "     EtaBin " << m_etaBins[i] << " - " << m_etaBins[i+1]
			<< " : dR(mF) < " << m_mufastDRThres[i] );
       }
     }
     if( m_requireMass ) {
       ATH_MSG_DEBUG( "+ Mass cut:" );
       if( (m_etaBins.size()-1) != m_massThres.size() ) {
	 ATH_MSG_ERROR( "bad thresholds setup .... exiting!" );
	 return StatusCode::FAILURE;
       }
       for(unsigned int i=0; i<m_massThres.size(); i++) {
	 ATH_MSG_DEBUG( "     EtaBin " << m_etaBins[i] << " - " << m_etaBins[i+1]
			<< " : Mass < " << m_massThres[i] );
       }
     }
     if( m_requireSameSign ) ATH_MSG_DEBUG( "+ Same charge sign" );
   }

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
 
   auto ptValue = pMuon->pt() * pMuon->charge() / Gaudi::Units::GeV;
 
   fexPt    = ptValue;
   ptFL      = ptValue;
   idEta    = pMuon->eta();
   idPhi    = pMuon->phi();
   int usealgo      = pMuon->strategy();
   float ptresComb = pMuon->sigmaPt() / Gaudi::Units::GeV;
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
   float threshold = (idEta != -9999) ? 0 : 999999;
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
   if (std::abs(fexPt) <= (threshold / Gaudi::Units::GeV)) stdCut = false;
   ATH_MSG_DEBUG("REGTEST muon pt is " << fexPt
              << " GeV and threshold cut is " << threshold / Gaudi::Units::GeV
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
     ATH_MSG_DEBUG("Number of muon event = " << numMuon );
     ATH_MSG_DEBUG("Applying selection of multiplicity << " << m_decisionId );

     if(m_applyOR)
       ATH_CHECK(applyOverlapRemoval(toolInput));

     return multiplicitySelection(toolInput);
   }
 
   return StatusCode::SUCCESS;
}


StatusCode TrigmuCombHypoTool::inclusiveSelection(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& input) const
{
   for ( auto& i: input) {
     // If muon event has difference DecisionID, it shouldn't apply.
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
   HLT::Index2DVec passingSelection( m_ptBins.size() );

   for ( size_t cutIndex=0; cutIndex < m_ptBins.size(); ++cutIndex ) {
      size_t elementIndex{ 0 };      
      for ( auto& i: input ) {

	if(!m_acceptAll && m_applyOR && !i.passOR) {
	  ATH_MSG_DEBUG("skip due to overap, DecisionID " << m_decisionId );
	  continue;
	}

         // If muon event has difference DecisionID, it shouldn't apply.
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


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoTool::applyOverlapRemoval(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& toolInput) const {

  ATH_MSG_DEBUG("Running Overlap Removal for muComb");

  std::vector<TrigmuCombHypoTool::CombinedMuonInfo*> input;

  for ( auto& i: toolInput ) {
    if ( TrigCompositeUtils::passed( m_decisionId.numeric(), i.previousDecisionIDs) ){
      input.emplace_back(&i);
    }
  }

  size_t numMuon = input.size();

  auto mucombNrAllEVs     = Monitored::Scalar("NrAllEVs", -9999.);
  auto mucombNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
  auto monitorIt          = Monitored::Group(m_monTool, mucombNrAllEVs, mucombNrActiveEVs);
  if ( numMuon == 0) {
    ATH_MSG_DEBUG( "No positive previous hypo decision. Not need overlap removal." );
    mucombNrActiveEVs = numMuon;
    mucombNrAllEVs = numMuon;
    return StatusCode::SUCCESS;
  }
  else if ( numMuon == 1 ) {
    ATH_MSG_DEBUG("Number of muon event = " << numMuon );
    ATH_MSG_DEBUG("no overlap Removal necessary. exitting with all EventViews active." );
    mucombNrActiveEVs = numMuon;
    mucombNrAllEVs = numMuon;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Number of muon event = " << numMuon );
    mucombNrAllEVs = numMuon;
    ATH_CHECK(checkOverlap(input));
    return StatusCode::SUCCESS;
  }

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoTool::checkOverlap(std::vector<TrigmuCombHypoTool::CombinedMuonInfo*>& input) const {

  size_t numMuon = input.size();
  unsigned int i,j;
  std::vector<unsigned int> mucombResult;

  bool errorWhenIdentifyingOverlap = false;

  for(i=0; i<numMuon; i++) {mucombResult.emplace_back(i); }
  for(i=0; i<numMuon-1; i++){
    for(j=i+1; j<numMuon; j++){
      ATH_MSG_DEBUG("++ i=" << i << " vs j=" << j);
      bool overlapped = isOverlap((*input[i]).muComb, (*input[j]).muComb);
      if( ! overlapped ){ // judged as different
	ATH_MSG_DEBUG("   judged as: differenr objects");
	if( mucombResult[i] == mucombResult[j] ) { // but marked as same by someone
	  ATH_MSG_DEBUG( "inconsistentency in muComb overlap removal for more than two objects" );
	  ATH_MSG_DEBUG( "two objects are judged as different but both were already marked as identical by someone else as: " );
	  ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mucombResult[i] << " / "  << mucombResult[j] );
	  auto mucombError  = Monitored::Scalar("MucombError", -9999.);
	  auto monitorIt    = Monitored::Group(m_monTool, mucombError);
	  mucombError = TrigmuCombHypoToolConsts::errorCode_inconsistent_overlap1;
	  errorWhenIdentifyingOverlap = true;
	}
      }
      else{ // judged as overlap
	if( (mucombResult[j] != j && mucombResult[i] != mucombResult[j]) || (mucombResult[j] == j && mucombResult[i] != i) ){
	  ATH_MSG_DEBUG( "inconsistentency in muComb based overlap removal for more than two objects" );
	  ATH_MSG_DEBUG( "two objects are judged as overlap but only either was already marked as overlap to someone else: " );
	  ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mucombResult[i] << " / "  << mucombResult[j] );
	  auto mucombError  = Monitored::Scalar("MucombError", -9999.);
	  auto monitorIt    = Monitored::Group(m_monTool, mucombError);
	  mucombError = TrigmuCombHypoToolConsts::errorCode_inconsistent_overlap2;
	  errorWhenIdentifyingOverlap = true;
	}
	ATH_MSG_DEBUG("   judged as: overlapped objects");
	if( mucombResult[i] == i ) {
	  ATH_MSG_DEBUG( "   i is not yet marked as overlap. so, it is a newly found overlap" );
	  ATH_MSG_DEBUG( "   -> marking mucombResult[j] as i..." );
	  mucombResult[j] = i;
	} else {
	  ATH_MSG_DEBUG( "   both i/j already marked as overlap by: mucombResult[i]=" << mucombResult[i] );
	  ATH_MSG_DEBUG( "   -> do nothing..." );
	}
      }
    }
  }

  if( errorWhenIdentifyingOverlap ) {
    ATH_MSG_WARNING( "error when resolving overlap. exitting with all EVs active..." );
    auto mucombNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
    auto monitorIt          = Monitored::Group(m_monTool, mucombNrActiveEVs);
    mucombNrActiveEVs = numMuon;
    // for(i=0; i<numMuon; i++) TrigCompositeUtils::addDecisionID( m_decisionId, toolInput[i].decision );
    return StatusCode::SUCCESS;
  }

  unsigned int n_uniqueMuon = 0;
  for(i=0; i<numMuon; i++) {
    ATH_MSG_DEBUG( "muComb based results: i=" << i << ": ");
    if( mucombResult[i] != i ) { ATH_MSG_DEBUG( "      overlap to j=" << mucombResult[i] ); }
    else {
      n_uniqueMuon++;
      ATH_MSG_DEBUG( "      unique" );
    }
  }

  ATH_MSG_DEBUG( "nr of unique Muons after muComb-based removal=" << n_uniqueMuon );

  if( numMuon != n_uniqueMuon ){
    ATH_CHECK(chooseBestMuon(input, mucombResult));
  } else {
    ATH_MSG_DEBUG( "no overlap identified. exitting with all EventViews active" );
    auto mucombNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
    auto monitorIt          = Monitored::Group(m_monTool, mucombNrActiveEVs);
    mucombNrActiveEVs = n_uniqueMuon;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigmuCombHypoTool::isOverlap(const xAOD::L2CombinedMuon *combMf1,
				   const xAOD::L2CombinedMuon *combMf2) const
{

  auto mucombDR             = Monitored::Scalar("DR", -9999.);
  auto mucombMass           = Monitored::Scalar("Mass", -9999.);
  auto mucombDRLog10        = Monitored::Scalar("DRLog10", -9999.);
  auto mucombMassLog10      = Monitored::Scalar("MassLog10", -9999.);

  auto monitorIt       = Monitored::Group(m_monTool, mucombDR, mucombMass, mucombDRLog10, mucombMassLog10);


  ATH_MSG_DEBUG( "   ...mF1: pt/eta/phi=" << combMf1->pt()/Gaudi::Units::GeV << " / " << combMf1->eta() << " / " << combMf1->phi() );
  ATH_MSG_DEBUG( "   ...mF2: pt/eta/phi=" << combMf2->pt()/Gaudi::Units::GeV << " / " << combMf2->eta() << " / " << combMf2->phi() );

  // if dR or invMass is necessary but (eta,phi) info is not avaiable
  // (i.e. eta,phi=0,0; rec failed)
  const double ZERO_LIMIT_FOR_ETAPHI = 1e-4;
  if( (fabs(combMf1->eta()) <ZERO_LIMIT_FOR_ETAPHI && fabs(combMf1->phi()) < ZERO_LIMIT_FOR_ETAPHI) ||
      (fabs(combMf2->eta()) <ZERO_LIMIT_FOR_ETAPHI && fabs(combMf2->phi()) < ZERO_LIMIT_FOR_ETAPHI) ) {
    ATH_MSG_DEBUG( "   ...-> (eta,phi) info not available (rec at (eta,phi)=(0,0))" );
    if( m_requireDR || m_requireMass ) {
      ATH_MSG_DEBUG( "   ...-> but dR of invMass check is required. cannot judge overlap -> return with false" );
      return false;
    }
  }

  // if charge or invMass is necessary but charge(=pT) info is not avaiable
  const double ZERO_LIMIT_FOR_PT = 1e-4;
  if( (fabs(combMf1->pt()) <ZERO_LIMIT_FOR_PT) || (fabs(combMf2->pt()) < ZERO_LIMIT_FOR_PT) ) {
    ATH_MSG_DEBUG( "   ...-> pT info not available (rec at pT=0)" );
    if( m_requireSameSign || m_requireMass ) {
      ATH_MSG_DEBUG( "   ...-> but same sign or invMass check is required. cannot judge overlap -> return with false" );
      return false;
    }
  }

  // determine etabin and thresholds
  double absEta = (fabs(combMf1->pt()) > fabs(combMf2->pt())) ? fabs(combMf1->eta()) : fabs(combMf2->eta());
  unsigned int iThres = 0;
  for(unsigned int i=0; i<(m_etaBins.size()-1); i++) {
    if ( m_etaBins[i] <= absEta && absEta < m_etaBins[i+1] ) iThres = i;
  }
  double dRThres     = m_dRThres[iThres];
  double dRbyMFThres = m_mufastDRThres[iThres];
  double massThres   = m_massThres[iThres];
  ATH_MSG_DEBUG( "   ...iThres=" << iThres );
  if(m_requireDR)        ATH_MSG_DEBUG( "   ...dR       threshold=" << dRThres     );
  if(m_requireMufastDR)  ATH_MSG_DEBUG( "   ...dR(byMF) threshold=" << dRbyMFThres );
  if(m_requireMass)      ATH_MSG_DEBUG( "   ...mass     threshold=" << massThres   );

  // same sign cut
  bool sameSign = false;
  if( m_requireSameSign ) {
    sameSign = ((combMf1->pt()*combMf2->pt()) > 0) ? true : false;
    ATH_MSG_DEBUG( "   ...-> sameSign=" << sameSign );
  }

  // dR cut
  bool dRisClose = false;
  double dr = dR(combMf1->eta(),combMf1->phi(),combMf2->eta(),combMf2->phi());

  mucombDR = dr;
  const double monitor_limit = 1e-4;
  double dr_mon = (dr>=monitor_limit) ? dr : monitor_limit;
  mucombDRLog10 = log10(dr_mon);

  if( m_requireDR ) {
    if( dr < dRThres ) dRisClose = true;
    ATH_MSG_DEBUG( "   ...-> dR=" << dr << " : dRisClose=" << dRisClose );
  }

  // dR(by MF) cut
  bool dRbyMFisClose = false;
  if( m_requireMufastDR ) {
    const xAOD::L2StandAloneMuon* mf1 = combMf1->muSATrack();
    const xAOD::L2StandAloneMuon* mf2 = combMf2->muSATrack();
    if( mf1 == 0 || mf2 == 0 ) {
      ATH_MSG_DEBUG( "mF link from combinedMF broken" );
      ATH_MSG_DEBUG( "   ...-> mF dR is required but mF link broken. cannot judge overlap -> return with false" );
      return false;
    }
    else {
      // here, we do not check (eta,phi) of mF is not (0,0)
      // (i.e. we apply muComb based cut even if muFast rec is failed)
      double dRByMF = dR(mf1->etaMS(),mf1->phiMS(),mf2->etaMS(),mf2->phiMS());
      if( dRByMF < dRbyMFThres ) dRbyMFisClose = true;
      ATH_MSG_DEBUG( "   ...-> dR(by MF)=" << dRByMF << " : dRbyMFisClose=" << dRbyMFisClose );
    }
  }

  // mass cut
  const double TRACK_MASS = 0;  // just assume zero mass
  bool massIsClose = false;
  double mass = invMass(TRACK_MASS,combMf1->pt()/Gaudi::Units::GeV,combMf1->eta(),combMf1->phi(),TRACK_MASS,combMf2->pt()/Gaudi::Units::GeV,combMf2->eta(),combMf2->phi());

  mucombMass = mass;
  double mass_mon = (mass>=monitor_limit) ? mass : monitor_limit;
  mucombMassLog10 = log10(mass_mon);

  if( m_requireMass ) {
    if( mass < massThres ) massIsClose = true;
    ATH_MSG_DEBUG( "   ...-> mass=" << mass << " : massIsClose=" << massIsClose );
  }

  // total judge
  bool overlap = false;
  if( ((m_requireSameSign &&   sameSign)     || (! m_requireSameSign)) &&
      ((m_requireDR       &&  dRisClose)     || (! m_requireDR))       &&
      ((m_requireMufastDR &&  dRbyMFisClose) || (! m_requireMufastDR)) &&
      ((m_requireMass     &&  massIsClose)   || (! m_requireMass)) ) {
    overlap = true;
  }

  ATH_MSG_DEBUG( "   ...=> isOverlap=" << overlap );

  return overlap;

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigmuCombHypoTool::dR(double eta1, double phi1, double eta2, double phi2) const
{
  const double deta = eta1 - eta2;
  const double dphi = CxxUtils::deltaPhi(phi1, phi2);
  return std::sqrt(deta*deta + dphi*dphi);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigmuCombHypoTool::invMass(double m1, double pt1, double eta1, double phi1,
				   double m2, double pt2, double eta2, double phi2) const
{
  const double ZERO_LIMIT = 1e-12;

  double theta1 = 2*atan2((double)exp(-eta1),1.);
  double theta2 = 2*atan2((double)exp(-eta2),1.);

  double fpt1   = fabs(pt1);
  double fpt2   = fabs(pt2);

  double px1    = fpt1*cos(phi1);
  double py1    = fpt1*sin(phi1);
  double pz1    = fpt1/tan(theta1);
  double  e1    = sqrt(px1*px1+py1*py1+pz1*pz1+m1*m1);

  double px2    = fpt2*cos(phi2);
  double py2    = fpt2*sin(phi2);
  double pz2    = fpt2/tan(theta2);
  double  e2    = sqrt(px2*px2+py2*py2+pz2*pz2+m2*m2);

  double pxsum  = px1 + px2;
  double pysum  = py1 + py2;
  double pzsum  = pz1 + pz2;
  double esum   =  e1 +  e2;

  double mass  = 0;
  double mass2 = esum*esum - pxsum*pxsum - pysum*pysum - pzsum*pzsum;
  if( mass2 > ZERO_LIMIT ) mass = sqrt(mass2);

  return mass;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoTool::chooseBestMuon(std::vector<TrigmuCombHypoTool::CombinedMuonInfo*>& input, std::vector<unsigned int> mucombResult) const
{
  size_t numMuon = input.size();
  unsigned int i,j,k;

  auto mucombNrActiveEVs    = Monitored::Scalar("NrActiveEVs", -9999.);
  auto mucombNrOverlapped   = Monitored::Scalar("NrOverlapped", 0);
  auto mucombOverlappedEta  = Monitored::Scalar("OverlappedEta", -9999.);
  auto mucombOverlappedPhi  = Monitored::Scalar("OverlappedPhi", -9999.);
  auto mucombOverlappedPt   = Monitored::Scalar("OverlappedPt", -9999.);

  auto monitorIt       = Monitored::Group(m_monTool, mucombNrActiveEVs, mucombNrOverlapped,
  					  mucombOverlappedPt, mucombOverlappedEta, mucombOverlappedPhi);

  ATH_MSG_DEBUG( "--- choose best among overlaps & disable EVs (muComb based) ---" );
  for(i=0; i<numMuon; i++) {
    ATH_MSG_DEBUG( "++ i=" << i << ": result=" << mucombResult[i] );
    if( mucombResult[i] != i ) {
      ATH_MSG_DEBUG( "   overlap to some one. skip." );

      (*input[i]).passOR = false;

      continue;
    }
    std::vector<unsigned int> others;
    for(j=0; j<numMuon; j++) {
      if( mucombResult[j] == mucombResult[i] ) others.emplace_back(j);
    }
    if( others.size() == 1 ) {
      ATH_MSG_DEBUG( "   unique object. keep it active." );
      continue;
    }
    else {// must choose one best
      ATH_MSG_DEBUG( "   overlapped objects among: " << others );
      unsigned int best_ev = 0;
      float maxPtCombMf  = 0;
      for(k=0; k<others.size(); k++) {
	j=others[k];

	float ptCombMf  = 0.;
	const xAOD::L2CombinedMuon* combMf = (*input[j]).muComb;
	ptCombMf  = fabs(combMf->pt()/Gaudi::Units::GeV);
	ATH_MSG_DEBUG("     j="<< j << " , ptCombMf=" << ptCombMf);
	if( ptCombMf > maxPtCombMf ) {
	  maxPtCombMf  = ptCombMf;
	  best_ev  = j;
	}
      }
      ATH_MSG_DEBUG( "      best is: best_ev/maxPtCombMf=" << best_ev << " / " <<  maxPtCombMf );

      for(k=0; k<others.size(); k++) {
	j=others[k];
	if( j != best_ev ) {
	  ATH_MSG_DEBUG( "      EventView( j=" << j << " ) is not active" );

	  (*input[j]).passOR = false;

	  // monitoring
	  const xAOD::L2CombinedMuon* CombMf = (*input[j]).muComb;
	  mucombNrOverlapped++;
	  mucombOverlappedPt = CombMf->pt()* CombMf->charge() /Gaudi::Units::GeV;
	  mucombOverlappedEta = CombMf->eta();
	  mucombOverlappedPhi = CombMf->phi();
	}
	if( j == best_ev ){
	  ATH_MSG_DEBUG( "      EventView( j=" << j << " ) is best one" );
	}
      }
    }
  }
  mucombNrActiveEVs = numMuon - mucombNrOverlapped;

  return StatusCode::SUCCESS;
}
