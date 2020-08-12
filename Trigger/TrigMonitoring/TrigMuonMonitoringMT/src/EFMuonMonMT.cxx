/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EFMuonMonMT.h"

#include "xAODTrigMuon/TrigMuonDefs.h"
#include "MuonMatchingTool.h"

EFMuonMonMT :: EFMuonMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}

StatusCode EFMuonMonMT :: fillVariablesPerChain(const EventContext& , const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  // EFSA
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> > featureContSA = getTrigDecisionTool()->features<xAOD::MuonContainer>( chain, TrigDefs::includeFailedDecisions, "HLT_Muons_");
  for (const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer>& muSALinkInfo : featureContSA) {
    ATH_CHECK( muSALinkInfo.isValid() );
    const ElementLink<xAOD::MuonContainer> muSAEL = muSALinkInfo.link;
    if ( ! (*muSAEL)->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle) ) continue;

    // basic EDM variables
    auto EFSAPt = Monitored::Scalar<float>(m_group+"_EFSA_Pt",-999.);
    auto EFSAEta = Monitored::Scalar<float>(m_group+"_EFSA_Eta",-999.);
    auto EFSAPhi = Monitored::Scalar<float>(m_group+"_EFSA_Phi",-999.);

    EFSAPt = (*muSAEL)->pt()/1e3 * (*muSAEL)->charge(); // convert to GeV
    EFSAEta = (*muSAEL)->eta();
    EFSAPhi = (*muSAEL)->phi();

    fill(m_group, EFSAPt, EFSAEta, EFSAPhi);
  }


  // EFCB
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> > featureContCB = getTrigDecisionTool()->features<xAOD::MuonContainer>( chain, TrigDefs::includeFailedDecisions, "HLT_MuonsCB");
  for (const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer>& muCBLinkInfo : featureContCB) {
    ATH_CHECK( muCBLinkInfo.isValid() );
    const ElementLink<xAOD::MuonContainer> muCBEL = muCBLinkInfo.link;
    if ( ! (*muCBEL)->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle) ) continue;

    // basic EDM variables
    auto EFCBPt = Monitored::Scalar<float>(m_group+"_EFCB_Pt",-999.);
    auto EFCBEta = Monitored::Scalar<float>(m_group+"_EFCB_Eta",-999.);
    auto EFCBPhi = Monitored::Scalar<float>(m_group+"_EFCB_Phi",-999.);

    EFCBPt = (*muCBEL)->pt()/1e3 * (*muCBEL)->charge(); // convert to GeV
    EFCBEta = (*muCBEL)->eta();
    EFCBPhi = (*muCBEL)->phi();

    fill(m_group, EFCBPt, EFCBEta, EFCBPhi);
  }

  return StatusCode::SUCCESS;
}


StatusCode EFMuonMonMT :: fillVariablesPerOfflineMuonPerChain(const EventContext &ctx, const xAOD::Muon* mu, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  const xAOD::TrackParticle* OfflineSATrack = mu->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
  const xAOD::TrackParticle* OfflineCBTrack = mu->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);


  // OfflineSA
  if( OfflineSATrack ){
    auto OfflineSAPt = Monitored::Scalar<float>(m_group+"_OfflineSA_Pt",-999.);
    auto OfflineSAEta = Monitored::Scalar<float>(m_group+"_OfflineSA_Eta",-999.);
    auto OfflineSAPhi = Monitored::Scalar<float>(m_group+"_OfflineSA_Phi",-999.);

    auto matchedEFSA = Monitored::Scalar<bool>(m_group+"_matchedEFSA",false);
    auto matchedL2SA = Monitored::Scalar<bool>(m_group+"_matchedL2SA",false);

    auto OfflineSAmatchedL2SAPt = Monitored::Scalar<float>(m_group+"_OfflineSAmatchedL2SA_Pt",-999.);
    auto OfflineSAmatchedL2SAEta = Monitored::Scalar<float>(m_group+"_OfflineSAmatchedL2SA_Eta",-999.);
    auto OfflineSAmatchedL2SAPhi = Monitored::Scalar<float>(m_group+"_OfflineSAmatchedL2SA_Phi",-999.);
 
    auto MatchedEFSAPt = Monitored::Scalar<float>(m_group+"_MatchedEFSA_Pt",-999.);
    auto MatchedEFSAEta = Monitored::Scalar<float>(m_group+"_MatchedEFSA_Eta",-999.);
    auto MatchedEFSAPhi = Monitored::Scalar<float>(m_group+"_MatchedEFSA_Phi",-999.);

    auto SAdR = Monitored::Scalar<float>(m_group+"_SAdR",1000.);
    auto SAdPt = Monitored::Scalar<float>(m_group+"_SAdPt",-999.);
    auto SAdEta = Monitored::Scalar<float>(m_group+"_SAdEta",-999.);
    auto SAdPhi = Monitored::Scalar<float>(m_group+"_SAdPhi",-999.);

    // basic EDM variables
    OfflineSAPt = OfflineSATrack->pt()/1e3 * OfflineSATrack->charge(); // convert to GeV
    OfflineSAEta = OfflineSATrack->eta();
    OfflineSAPhi = OfflineSATrack->phi();

    // correlation histograms EFSA vs. OfflineSA
    // get the EFSA muon matched to offlineSA muon
    const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> EFSAMuonLinkInfo = m_matchTool->matchEFSALinkInfo(mu, chain); 
    if( EFSAMuonLinkInfo.isValid() ){
      const ElementLink<xAOD::MuonContainer> EFSAMuon = EFSAMuonLinkInfo.link;
      const xAOD::TrackParticle* EFSATrack = (*EFSAMuon)->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
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
    fill(m_group, OfflineSAPt, OfflineSAEta, OfflineSAPhi, OfflineSAmatchedL2SAPt, OfflineSAmatchedL2SAEta, OfflineSAmatchedL2SAPhi,
         MatchedEFSAPt, MatchedEFSAEta, MatchedEFSAPhi, SAdPt, SAdEta, SAdPhi, SAdR, matchedEFSA, matchedL2SA);
  }


  // OfflineCB
  if( OfflineCBTrack ){
    auto OfflineCBPt = Monitored::Scalar<float>(m_group+"_OfflineCB_Pt",-999.);
    auto OfflineCBEta = Monitored::Scalar<float>(m_group+"_OfflineCB_Eta",-999.);
    auto OfflineCBPhi = Monitored::Scalar<float>(m_group+"_OfflineCB_Phi",-999.);

    auto matchedEFCB = Monitored::Scalar<bool>(m_group+"_matchedEFCB",false);
    auto matchedL2CB = Monitored::Scalar<bool>(m_group+"_matchedL2CB",false);

    auto OfflineCBmatchedL2CBPt = Monitored::Scalar<float>(m_group+"_OfflineCBmatchedL2CB_Pt",-999.);
    auto OfflineCBmatchedL2CBEta = Monitored::Scalar<float>(m_group+"_OfflineCBmatchedL2CB_Eta",-999.);
    auto OfflineCBmatchedL2CBPhi = Monitored::Scalar<float>(m_group+"_OfflineCBmatchedL2CB_Phi",-999.);
 
    auto MatchedEFCBPt = Monitored::Scalar<float>(m_group+"_MatchedEFCB_Pt",-999.);
    auto MatchedEFCBEta = Monitored::Scalar<float>(m_group+"_MatchedEFCB_Eta",-999.);
    auto MatchedEFCBPhi = Monitored::Scalar<float>(m_group+"_MatchedEFCB_Phi",-999.);
  
    auto CBdR = Monitored::Scalar<float>(m_group+"_CBdR",1000.);
    auto CBdPt = Monitored::Scalar<float>(m_group+"_CBdPt",-999.);
    auto CBdEta = Monitored::Scalar<float>(m_group+"_CBdEta",-999.);
    auto CBdPhi = Monitored::Scalar<float>(m_group+"_CBdPhi",-999.);

    // basic EDM variables
    OfflineCBPt = OfflineCBTrack->pt()/1e3 * OfflineCBTrack->charge(); // convert to GeV
    OfflineCBEta = OfflineCBTrack->eta();
    OfflineCBPhi = OfflineCBTrack->phi();

    // correlation histograms EFCB vs. OfflineCB
    // get the closest EFCB muon
    const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> EFCBMuonLinkInfo = m_matchTool->matchEFCBLinkInfo(mu, chain);
    if( EFCBMuonLinkInfo.isValid() ){
      const ElementLink<xAOD::MuonContainer> EFCBMuon = EFCBMuonLinkInfo.link;
      const xAOD::TrackParticle* EFCBTrack = (*EFCBMuon)->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);
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
    fill(m_group, OfflineCBPt, OfflineCBEta, OfflineCBPhi, OfflineCBmatchedL2CBPt, OfflineCBmatchedL2CBEta, OfflineCBmatchedL2CBPhi,
         MatchedEFCBPt, MatchedEFCBEta, MatchedEFCBPhi, CBdPt, CBdEta, CBdPhi, CBdR, matchedEFCB, matchedL2CB);
  }

  return StatusCode::SUCCESS;
}
