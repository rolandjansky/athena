/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EFMuonMonMT.h"

#include "xAODTrigMuon/TrigMuonDefs.h"
#include "MuonMatchingTool.h"
#include "StoreGate/ReadDecorHandle.h"

EFMuonMonMT :: EFMuonMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode EFMuonMonMT :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();
  ATH_CHECK( m_EFSAMuonContainerKey.initialize() );
  ATH_CHECK( m_EFCBMuonContainerKey.initialize() );
  ATH_CHECK( m_MStrackContainerKey.initialize() );
  ATH_CHECK( m_CBtrackContainerKey.initialize() );
  ATH_CHECK( m_muonIso30Key.initialize() );

  for( std::string chain : m_monitored_chains ){
    m_doEFSA[chain] = true;

    if( chain.find("msonly") != std::string::npos ) m_doEFCB[chain] = false;
    else m_doEFCB[chain] = true;

    if( chain.find("ivar") != std::string::npos ) m_doEFIso[chain] = true;
    else m_doEFIso[chain] = false;
  }

  return sc;
}


StatusCode EFMuonMonMT :: fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  const float ZERO_LIMIT = 0.00001;

  // EFSA
  if( m_doEFSA.at(chain) ){
    std::vector< TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> > featureContSA = getTrigDecisionTool()->features<xAOD::MuonContainer>( chain, TrigDefs::includeFailedDecisions, "HLT_Muons_.*");
  
    for (const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer>& muSALinkInfo : featureContSA) {
      ATH_CHECK( muSALinkInfo.isValid() );
      const ElementLink<xAOD::MuonContainer> muSAEL = muSALinkInfo.link;
      if ( (*muSAEL)->muonType() != xAOD::Muon::MuonType::MuonStandAlone ) continue;
  
      auto EFSAPt = Monitored::Scalar<float>(chain+"_EFSA_Pt", -999.);
      auto EFSAEta = Monitored::Scalar<float>(chain+"_EFSA_Eta", -999.);
      auto EFSAPhi = Monitored::Scalar<float>(chain+"_EFSA_Phi", -999.);
  
      EFSAPt = (*muSAEL)->pt()/1e3 * (*muSAEL)->charge();
      EFSAEta = (*muSAEL)->eta();
      EFSAPhi = (*muSAEL)->phi(); 
  
      fill(m_group+"_"+chain, EFSAPt, EFSAEta, EFSAPhi);
    }
  }


  // EFCB
  if( m_doEFCB.at(chain) ){
    std::vector< TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> > featureContCB = getTrigDecisionTool()->features<xAOD::MuonContainer>( chain, TrigDefs::includeFailedDecisions, "HLT_MuonsCB.*");
  
    for (const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer>& muCBLinkInfo : featureContCB) {
      ATH_CHECK( muCBLinkInfo.isValid() );
      const ElementLink<xAOD::MuonContainer> muCBEL = muCBLinkInfo.link;
      if ( (*muCBEL)->muonType() != xAOD::Muon::MuonType::Combined ) continue;
  
      auto EFCBPt = Monitored::Scalar<float>(chain+"_EFCB_Pt", -999.);
      auto EFCBEta = Monitored::Scalar<float>(chain+"_EFCB_Eta", -999.);
      auto EFCBPhi = Monitored::Scalar<float>(chain+"_EFCB_Phi", -999.);
  
      EFCBPt = (*muCBEL)->pt()/1e3 * (*muCBEL)->charge();
      EFCBEta = (*muCBEL)->eta();
      EFCBPhi = (*muCBEL)->phi(); 
  
      fill(m_group+"_"+chain, EFCBPt, EFCBEta, EFCBPhi);
    }
  }


  // EFIso
  if( m_doEFIso.at(chain) ){
    std::vector< TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> > featureContIso = getTrigDecisionTool()->features<xAOD::MuonContainer>( chain, TrigDefs::includeFailedDecisions, "HLT_MuonsIso");
    for (const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer>& muIsoLinkInfo : featureContIso) {
      ATH_CHECK( muIsoLinkInfo.isValid() );
      const ElementLink<xAOD::MuonContainer> muIsoEL = muIsoLinkInfo.link;
      if ( (*muIsoEL)->muonType() != xAOD::Muon::MuonType::Combined ) continue;
  
      // basic variables used decision
      auto PtCone03 = Monitored::Scalar<float>(chain+"_PtCone03",-999.);
      auto PtCone03overMuonPt = Monitored::Scalar<float>(chain+"_PtCone03overMuonPt",-999.);
  
      SG::ReadDecorHandle<xAOD::MuonContainer, double> muonIso30 ( m_muonIso30Key, ctx );
      float ptcone30 = muonIso30(*(*muIsoEL));
  
      if (ptcone30 > ZERO_LIMIT ){
        PtCone03 = ptcone30/1e3;
        PtCone03overMuonPt = ptcone30 / (*muIsoEL)->pt();
      }
  
      fill(m_group+"_"+chain, PtCone03, PtCone03overMuonPt);
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode EFMuonMonMT :: fillVariablesPerOfflineMuonPerChain(const EventContext &ctx, const xAOD::Muon* mu, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  const float ZERO_LIMIT = 0.00001;

  const xAOD::TrackParticle* OfflineSATrack = mu->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
  const xAOD::TrackParticle* OfflineCBTrack = mu->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);


  // OfflineSA
  if( m_doEFSA.at(chain) && OfflineSATrack ){
    auto OfflineSAPt = Monitored::Scalar<float>(chain+"_OfflineSA_Pt",-999.);
    auto OfflineSAEta = Monitored::Scalar<float>(chain+"_OfflineSA_Eta",-999.);
    auto OfflineSAPhi = Monitored::Scalar<float>(chain+"_OfflineSA_Phi",-999.);

    auto matchedEFSA = Monitored::Scalar<bool>(chain+"_matchedEFSA",false);
    auto matchedL2SA = Monitored::Scalar<bool>(chain+"_matchedL2SA",false);

    auto OfflineSAmatchedL2SAPt = Monitored::Scalar<float>(chain+"_OfflineSAmatchedL2SA_Pt",-999.);
    auto OfflineSAmatchedL2SAEta = Monitored::Scalar<float>(chain+"_OfflineSAmatchedL2SA_Eta",-999.);
    auto OfflineSAmatchedL2SAPhi = Monitored::Scalar<float>(chain+"_OfflineSAmatchedL2SA_Phi",-999.);
 
    auto MatchedEFSAPt = Monitored::Scalar<float>(chain+"_MatchedEFSA_Pt",-999.);
    auto MatchedEFSAEta = Monitored::Scalar<float>(chain+"_MatchedEFSA_Eta",-999.);
    auto MatchedEFSAPhi = Monitored::Scalar<float>(chain+"_MatchedEFSA_Phi",-999.);

    auto SAdR = Monitored::Scalar<float>(chain+"_SAdR",1000.);
    auto SAdPt = Monitored::Scalar<float>(chain+"_SAdPt",-999.);
    auto SAdEta = Monitored::Scalar<float>(chain+"_SAdEta",-999.);
    auto SAdPhi = Monitored::Scalar<float>(chain+"_SAdPhi",-999.);

    // basic EDM variables
    OfflineSAPt = OfflineSATrack->pt()/1e3 * OfflineSATrack->charge(); // convert to GeV
    OfflineSAEta = OfflineSATrack->eta();
    OfflineSAPhi = OfflineSATrack->phi();

    // correlation histograms EFSA vs. OfflineSA
    // get the EFSA muon matched to offlineSA muon
    const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> EFSAMuonLinkInfo = m_matchTool->matchEFSALinkInfo(mu, chain); 
    if( EFSAMuonLinkInfo.isValid() ){
      const xAOD::TrackParticle* EFSATrack = m_matchTool->SearchEFTrack(ctx, EFSAMuonLinkInfo, m_MStrackContainerKey);
      if ( EFSATrack ){
        matchedEFSA = true;
  
        MatchedEFSAPt = EFSATrack->pt()/1e3 * EFSATrack->charge(); // convert to GeV
        MatchedEFSAEta = EFSATrack->eta();
        MatchedEFSAPhi = EFSATrack->phi();
  
        SAdR = xAOD::P4Helpers::deltaR(OfflineSATrack, EFSATrack, false); 
        SAdPt = std::abs(OfflineSAPt) - std::abs(MatchedEFSAPt);
        SAdEta = OfflineSAEta - MatchedEFSAEta;
        SAdPhi = OfflineSAPhi - MatchedEFSAPhi;
  
  
        // correlation histograms offlineSA (matched to EFSA) vs. offlineSA (matched to L2SA)
        // get L2SA feature
        const TrigCompositeUtils::Decision* EFSAMuonDecision = EFSAMuonLinkInfo.source;
        const TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> L2SALinkInfo = TrigCompositeUtils::findLink<xAOD::L2StandAloneMuonContainer>(EFSAMuonDecision, "feature");
        ATH_CHECK( L2SALinkInfo.isValid() );
        const ElementLink<xAOD::L2StandAloneMuonContainer> L2SAEL = L2SALinkInfo.link;
  
        // get offline muon matched to L2SA
        const xAOD::Muon *OfflineSAmatchedL2SA = m_matchTool->matchL2SAtoOff(ctx, (*L2SAEL));
        if (OfflineSAmatchedL2SA){
          const xAOD::TrackParticle* OfflineSATrackmatchedL2SA = OfflineSAmatchedL2SA->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
          if ( OfflineSATrackmatchedL2SA ){
            matchedL2SA = true;
            OfflineSAmatchedL2SAPt = OfflineSATrackmatchedL2SA->pt()/1e3 * OfflineSATrackmatchedL2SA->charge(); // convert to GeV
            OfflineSAmatchedL2SAEta = OfflineSATrackmatchedL2SA->eta();
            OfflineSAmatchedL2SAPhi = OfflineSATrackmatchedL2SA->phi();
          }
        }
      }
    }
    fill(m_group+"_"+chain, OfflineSAPt, OfflineSAEta, OfflineSAPhi, OfflineSAmatchedL2SAPt, OfflineSAmatchedL2SAEta, OfflineSAmatchedL2SAPhi,
         MatchedEFSAPt, MatchedEFSAEta, MatchedEFSAPhi, SAdPt, SAdEta, SAdPhi, SAdR, matchedEFSA, matchedL2SA);
  }


  // OfflineCB
  if( m_doEFCB.at(chain) && OfflineCBTrack ){
    auto OfflineCBPt = Monitored::Scalar<float>(chain+"_OfflineCB_Pt",-999.);
    auto OfflineCBEta = Monitored::Scalar<float>(chain+"_OfflineCB_Eta",-999.);
    auto OfflineCBPhi = Monitored::Scalar<float>(chain+"_OfflineCB_Phi",-999.);

    auto matchedEFCB = Monitored::Scalar<bool>(chain+"_matchedEFCB",false);
    auto matchedL2CB = Monitored::Scalar<bool>(chain+"_matchedL2CB",false);

    auto OfflineCBmatchedL2CBPt = Monitored::Scalar<float>(chain+"_OfflineCBmatchedL2CB_Pt",-999.);
    auto OfflineCBmatchedL2CBEta = Monitored::Scalar<float>(chain+"_OfflineCBmatchedL2CB_Eta",-999.);
    auto OfflineCBmatchedL2CBPhi = Monitored::Scalar<float>(chain+"_OfflineCBmatchedL2CB_Phi",-999.);
 
    auto MatchedEFCBPt = Monitored::Scalar<float>(chain+"_MatchedEFCB_Pt",-999.);
    auto MatchedEFCBEta = Monitored::Scalar<float>(chain+"_MatchedEFCB_Eta",-999.);
    auto MatchedEFCBPhi = Monitored::Scalar<float>(chain+"_MatchedEFCB_Phi",-999.);
  
    auto CBdR = Monitored::Scalar<float>(chain+"_CBdR",1000.);
    auto CBdPt = Monitored::Scalar<float>(chain+"_CBdPt",-999.);
    auto CBdEta = Monitored::Scalar<float>(chain+"_CBdEta",-999.);
    auto CBdPhi = Monitored::Scalar<float>(chain+"_CBdPhi",-999.);

    // basic EDM variables
    OfflineCBPt = OfflineCBTrack->pt()/1e3 * OfflineCBTrack->charge(); // convert to GeV
    OfflineCBEta = OfflineCBTrack->eta();
    OfflineCBPhi = OfflineCBTrack->phi();

    // correlation histograms EFCB vs. OfflineCB
    // get the closest EFCB muon
    const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> EFCBMuonLinkInfo = m_matchTool->matchEFCBLinkInfo(mu, chain);
    if( EFCBMuonLinkInfo.isValid() ){
      const xAOD::TrackParticle* EFCBTrack = m_matchTool->SearchEFTrack(ctx, EFCBMuonLinkInfo, m_CBtrackContainerKey);
      if ( EFCBTrack ){
        matchedEFCB = true;
  
        MatchedEFCBPt = EFCBTrack->pt()/1e3 * EFCBTrack->charge(); // convert to GeV
        MatchedEFCBEta = EFCBTrack->eta();
        MatchedEFCBPhi = EFCBTrack->phi();
  
        CBdR = xAOD::P4Helpers::deltaR(OfflineCBTrack, EFCBTrack, false); 
        CBdPt = std::abs(OfflineCBPt) - std::abs(MatchedEFCBPt);
        CBdEta = OfflineCBEta - MatchedEFCBEta;
        CBdPhi = OfflineCBPhi - MatchedEFCBPhi;
  
  
        // correlation histograms offlineCB (matched to EFCB) vs. offlineCB (matched to  L2CB)
        // get L2CB feature
        const TrigCompositeUtils::Decision* EFCBMuonDecision = EFCBMuonLinkInfo.source;
        const TrigCompositeUtils::LinkInfo<xAOD::L2CombinedMuonContainer> L2CBLinkInfo = TrigCompositeUtils::findLink<xAOD::L2CombinedMuonContainer>(EFCBMuonDecision, "feature");
        ATH_CHECK( L2CBLinkInfo.isValid() );
        const ElementLink<xAOD::L2CombinedMuonContainer> L2CBEL = L2CBLinkInfo.link;
  
        // get offline muon matched to L2CB
        const xAOD::Muon *OfflineCBmatchedL2CB = m_matchTool->matchL2CBtoOff(ctx, (*L2CBEL));
        if (OfflineCBmatchedL2CB){
          const xAOD::TrackParticle* OfflineCBTrackmatchedL2CB = OfflineCBmatchedL2CB->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);
          if ( OfflineCBTrackmatchedL2CB ){
            matchedL2CB = true;
            OfflineCBmatchedL2CBPt = OfflineCBTrackmatchedL2CB->pt()/1e3 * OfflineCBTrackmatchedL2CB->charge(); // convert to GeV
            OfflineCBmatchedL2CBEta = OfflineCBTrackmatchedL2CB->eta();
            OfflineCBmatchedL2CBPhi = OfflineCBTrackmatchedL2CB->phi();
          }
        }
      }
    }
    fill(m_group+"_"+chain, OfflineCBPt, OfflineCBEta, OfflineCBPhi, OfflineCBmatchedL2CBPt, OfflineCBmatchedL2CBEta, OfflineCBmatchedL2CBPhi,
         MatchedEFCBPt, MatchedEFCBEta, MatchedEFCBPhi, CBdPt, CBdEta, CBdPhi, CBdR, matchedEFCB, matchedL2CB);
  }


  // offoine isolation variable
  float OfflineIsoptcone30=-1.;
  mu->isolation(OfflineIsoptcone30, xAOD::Iso::IsolationType::ptvarcone30);
  if ( m_doEFIso.at(chain) && OfflineIsoptcone30 > ZERO_LIMIT ){

    auto OfflineIsoPtCone03 = Monitored::Scalar<float>(chain+"_OfflineIsoPtCone03",-999.);
    auto OfflineIsoPtCone03overMuonPt = Monitored::Scalar<float>(chain+"_OfflineIsoPtCone03overMuonPt",-999.);
    auto MatchedEFIsoPtCone03 = Monitored::Scalar<float>(chain+"_MatchedEFPIsotCone03",-999.);
    auto MatchedEFIsoPtCone03overMuonPt = Monitored::Scalar<float>(chain+"_MatchedEFIsoPtCone03overMuonPt",-999.);

    OfflineIsoPtCone03 = OfflineIsoptcone30/1e3;
    OfflineIsoPtCone03overMuonPt = OfflineIsoptcone30 / mu->pt();

    // get the closest EFIso muon
    const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> EFIsoMuonLinkInfo = m_matchTool->matchEFIsoLinkInfo(mu, chain);
    if( EFIsoMuonLinkInfo.isValid() ){
      const ElementLink<xAOD::MuonContainer> EFIsoMuon = EFIsoMuonLinkInfo.link;

      SG::ReadDecorHandle<xAOD::MuonContainer, double> muonIso30 ( m_muonIso30Key, ctx );
      float EFIsoptcone30 = muonIso30(*(*EFIsoMuon));

      if ( EFIsoptcone30 > ZERO_LIMIT ){
        MatchedEFIsoPtCone03 = EFIsoptcone30/1e3;
        MatchedEFIsoPtCone03overMuonPt = EFIsoptcone30 / (*EFIsoMuon)->pt();
      }
    }
    fill(m_group+"_"+chain, OfflineIsoPtCone03, OfflineIsoPtCone03overMuonPt, MatchedEFIsoPtCone03, MatchedEFIsoPtCone03overMuonPt);
  }

  return StatusCode::SUCCESS;
}


StatusCode EFMuonMonMT :: fillVariables(const EventContext &ctx) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  ATH_CHECK( fillVariableEtaPhi<xAOD::Muon>(ctx, m_EFSAMuonContainerKey, "EFSA", &MuonMatchingTool::trigPosForMatchSATrack));
  ATH_CHECK( fillVariableEtaPhi<xAOD::Muon>(ctx, m_EFCBMuonContainerKey, "EFCB", &MuonMatchingTool::trigPosForMatchCBTrack));

  return StatusCode::SUCCESS;

}


StatusCode EFMuonMonMT :: fillVariablesPerOfflineMuon(const EventContext &ctx, const xAOD::Muon* mu) const {

  ATH_CHECK( fillVariablesRatioPlots<xAOD::Muon>(ctx, mu, "EFSA", xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle,
                                                 [this](const EventContext &ctx, const xAOD::Muon *mu){ return m_matchTool->matchEFSAReadHandle(ctx,mu); }
                                                 )); 

  ATH_CHECK( fillVariablesRatioPlots<xAOD::Muon>(ctx, mu, "EFCB", xAOD::Muon::TrackParticleType::CombinedTrackParticle,
                                                 [this](const EventContext &ctx, const xAOD::Muon *mu){ return m_matchTool->matchEFCBReadHandle(ctx,mu); }
                                                 )); 

  return StatusCode::SUCCESS;

}
