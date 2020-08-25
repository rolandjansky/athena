/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L2muCombMonMT.h"

#include "xAODTrigMuon/TrigMuonDefs.h"
#include "MuonMatchingTool.h"

L2muCombMonMT :: L2muCombMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode L2muCombMonMT :: fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  const float ZERO_LIMIT = 0.00001;


  std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2CombinedMuonContainer> > featureCont = getTrigDecisionTool()->features<xAOD::L2CombinedMuonContainer>( chain, TrigDefs::includeFailedDecisions );
  for(const TrigCompositeUtils::LinkInfo<xAOD::L2CombinedMuonContainer>& muLinkInfo : featureCont){
    ATH_CHECK( muLinkInfo.isValid() );
    const ElementLink<xAOD::L2CombinedMuonContainer> muEL = muLinkInfo.link;

    // get L2SA feature
    const TrigCompositeUtils::Decision* muDecision = muLinkInfo.source;
    const TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> saLinkInfo = TrigCompositeUtils::findLink<xAOD::L2StandAloneMuonContainer>(muDecision, "feature");
    ATH_CHECK( saLinkInfo.isValid() );
    const ElementLink<xAOD::L2StandAloneMuonContainer> saEL = saLinkInfo.link;



    // basic EDM variables
    auto cbPt = Monitored::Scalar<float>(m_group+"_Pt",-999.);
    auto cbEta = Monitored::Scalar<float>(m_group+"_Eta",-999.);
    auto cbPhi = Monitored::Scalar<float>(m_group+"_Phi",-999.);

    cbPt = (*muEL)->pt()/1e3 * (*muEL)->charge(); // convert to GeV
    cbEta = (*muEL)->eta();
    cbPhi = (*muEL)->phi();

    auto saPt = Monitored::Scalar<float>(m_group+"_saPt",-999.);
    auto saEta = Monitored::Scalar<float>(m_group+"_saEta",-999.);
    auto saPhi = Monitored::Scalar<float>(m_group+"_saPhi",-999.);

    saPt = (*saEL)->pt();
    saEta = (*saEL)->eta();
    saPhi = (*saEL)->phi();


    // get L2SA track
    const xAOD::L2StandAloneMuon* SATrack = nullptr;
    float SATrackPt = -999.;
    if( (*muEL)->muSATrackLink().isValid() ) {
      SATrack    = (*muEL)->muSATrack();
      SATrackPt  = SATrack->pt();
    }


    // CB and Offline matching
    auto L2SA_success = Monitored::Scalar<bool>(m_group+"_L2SA_success",false);
    auto L2CB_success = Monitored::Scalar<bool>(m_group+"_L2CB_success",false);
    auto L2CBOFFmatching_failure = Monitored::Scalar<bool>(m_group+"_L2CBOFFmatching_failure",false);
    auto L2CB_failure = Monitored::Scalar<bool>(m_group+"_L2CB_failure",false);
    bool off_cb_match = false;
    bool off_sa_match = false;
    L2SA_success = (std::abs(saPt) > ZERO_LIMIT)? true:false;
    L2CB_success = (std::abs(cbPt) > ZERO_LIMIT)? true:false;


    // matching to offline
    const xAOD::Muon* RecMuonCBmatchL2SA = m_matchTool->matchL2SAtoOff(ctx, (*saEL));
    const xAOD::Muon* RecMuonCBmatchL2CB = m_matchTool->matchL2CBtoOff(ctx, (*muEL));
    if( RecMuonCBmatchL2SA && L2SA_success) off_sa_match = true;
    if( RecMuonCBmatchL2CB && L2CB_success) off_cb_match = true;


    if(L2CB_success){
      if(!off_cb_match) L2CBOFFmatching_failure = true;
    }
    else if (off_sa_match) L2CB_failure = true;

    if( !L2CB_success ){
      fill(m_group, saPt, saEta, saPhi, L2SA_success, L2CB_failure);
      continue;
    }

    fill(m_group, cbPt, cbEta, cbPhi, L2CB_success, L2CBOFFmatching_failure);


    // comparison L2muComb vs L2MuonSA
    auto ptratio_toSA = Monitored::Scalar<float>(m_group+"_ptratio_toSA",-999.);
    auto dEta_toSA = Monitored::Scalar<float>(m_group+"_dEta_toSA",-999.);
    auto dPhi_toSA = Monitored::Scalar<float>(m_group+"_dPhi_toSA",-999.);
    auto dR_toSA = Monitored::Scalar<float>(m_group+"_dR_toSA",-999.);
  
    if( (*muEL)->muSATrackLink().isValid() && std::abs(saPt) > ZERO_LIMIT ){
      ptratio_toSA = std::abs(cbPt / saPt);
      dEta_toSA = cbEta - saEta;
      dPhi_toSA = xAOD::P4Helpers::deltaPhi(cbPhi, saPhi);
      dR_toSA = sqrt(dEta_toSA*dEta_toSA + dPhi_toSA*dPhi_toSA);
     
      fill(m_group, ptratio_toSA, dEta_toSA, dPhi_toSA, dR_toSA);
    }


    // get IDTrack
    auto trkPt = Monitored::Scalar<float>(m_group+"_trkPt",-999.);
    auto trkEta = Monitored::Scalar<float>(m_group+"_trkEta",-999.);
    auto trkPhi = Monitored::Scalar<float>(m_group+"_trkPhi",-999.);
    auto trkZ0 = Monitored::Scalar<float>(m_group+"_trkZ0",-999.);
    auto trkChi2 = Monitored::Scalar<float>(m_group+"_trkChi2",-999.);

    const xAOD::TrackParticle* idtrk = nullptr;
    if( (*muEL)->idTrackLink().isValid() ) {
      idtrk = (*muEL)->idTrack();
      trkPt   = idtrk->pt() / 1e3 * idtrk->charge(); // convert to GeV
      trkEta  = idtrk->eta();
      trkPhi  = idtrk->phi0();
      trkZ0   = idtrk->z0();
      trkChi2 = idtrk->chiSquared();
    }

    fill(m_group, trkPt);
    if( std::abs(trkPt) > ZERO_LIMIT)  fill(m_group, trkEta, trkPhi, trkZ0, trkChi2);


    // comparison L2muComb (IDTrack) vs L2MuonSA
    auto ptratio_TrktoSA = Monitored::Scalar<float>(m_group+"_ptratio_TrktoSA",-999.);
    auto dEta_TrktoSA = Monitored::Scalar<float>(m_group+"_dEta_TrktoSA",-999.);
    auto dPhi_TrktoSA = Monitored::Scalar<float>(m_group+"_dPhi_TrktoSA",-999.);
    auto dR_TrktoSA = Monitored::Scalar<float>(m_group+"_dR_TrktoSA",-999.);
    
    if( (*muEL)->idTrackLink().isValid() && std::abs(saPt) > ZERO_LIMIT ){
      ptratio_TrktoSA = std::abs(cbPt / saPt);
      dEta_TrktoSA = cbEta - saEta;
      dPhi_TrktoSA = xAOD::P4Helpers::deltaPhi(cbPhi, saPhi);
      dR_TrktoSA = sqrt(dEta_TrktoSA*dEta_TrktoSA + dPhi_TrktoSA*dPhi_TrktoSA);
      
      fill(m_group, ptratio_TrktoSA, dEta_TrktoSA, dPhi_TrktoSA, dR_TrktoSA);
    } 


    // Muon Feature error
    std::vector<int> vec_MF_error;
    vec_MF_error.clear();
    auto MF_error = Monitored::Collection(m_group+"_MF_error",vec_MF_error);

    bool error = false;
    if( SATrack ){
      if(std::abs(saPt - SATrackPt) > ZERO_LIMIT){
        vec_MF_error.push_back(2);
        error = true;
      }
    } else{
      vec_MF_error.push_back(1);
      error = true;
    }
    if(std::abs(saPt) < ZERO_LIMIT){
      vec_MF_error.push_back(3);
      error = true;
    }
    if(!error)  vec_MF_error.push_back(0);

    fill(m_group, MF_error);

  }

  return StatusCode::SUCCESS;
}


StatusCode L2muCombMonMT :: fillVariablesPerOfflineMuonPerChain(const EventContext&, const xAOD::Muon* mu, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  const float ZERO_LIMIT = 0.00001;

  auto offEta = Monitored::Scalar<float>(m_group+"_offEta",-999.);
  auto ptresol = Monitored::Scalar<float>(m_group+"_ptresol",-999.);
  auto dR = Monitored::Scalar<float>(m_group+"_dR",-999.);

  float offPt = mu->pt()/1e3;
  float offPhi = mu->phi();
  offEta = mu->eta();

  
  // get L2CB muon link
  const TrigCompositeUtils::LinkInfo<xAOD::L2CombinedMuonContainer> muLinkInfo = m_matchTool->searchL2CBLinkInfo(mu, chain);
  if ( !muLinkInfo.isValid() )  return StatusCode::SUCCESS;
  const ElementLink<xAOD::L2CombinedMuonContainer> muEL = muLinkInfo.link;


  // dR wrt offline
  auto dRmin = Monitored::Scalar<float>(m_group+"_dRmin",1000.);
  dRmin = xAOD::P4Helpers::deltaR(mu, *muEL, false);
  fill(m_group, dRmin);
  if( ! m_matchTool->isMatchedL2CB(*muEL, mu) ) return StatusCode::SUCCESS; // not matched to L2muComb


  // pt resolution
  float cbPt  = (*muEL)->pt()/1e3;
  if ( std::abs(offPt) > ZERO_LIMIT && std::abs(cbPt) > ZERO_LIMIT ) ptresol = std::abs(cbPt)/std::abs(offPt) - 1.;
  fill(m_group, offEta, ptresol);


  // HLT_Roi_L2SAMuon variables
  const TrigCompositeUtils::Decision* muDecision = muLinkInfo.source;
  const TrigCompositeUtils::LinkInfo<TrigRoiDescriptorCollection> roiLinkInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(muDecision, "roi");
  ATH_CHECK( roiLinkInfo.isValid() );
  const ElementLink<TrigRoiDescriptorCollection> roiEL = roiLinkInfo.link;
  float SAroiEta = (*roiEL)->eta();
  float SAroiPhi = (*roiEL)->phi();

  auto roidEta = Monitored::Scalar<float>(m_group+"_L2SARoI_dEta",-999.);
  auto roidPhi = Monitored::Scalar<float>(m_group+"_L2SARoI_dPhi",-999.);
  auto roidR = Monitored::Scalar<float>(m_group+"_L2SARoI_dR",-999.);

  roidEta = SAroiEta - offEta;
  roidPhi = xAOD::P4Helpers::deltaPhi(offPhi, SAroiPhi);
  roidR = sqrt(roidEta*roidEta + roidPhi*roidPhi);
  
  fill(m_group, roidEta, roidPhi, roidR, offEta);


  return StatusCode::SUCCESS;
}
