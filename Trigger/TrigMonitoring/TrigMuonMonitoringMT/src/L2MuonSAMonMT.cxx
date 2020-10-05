/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L2MuonSAMonMT.h"

#include "xAODTrigMuon/TrigMuonDefs.h"
#include "MuonMatchingTool.h"

L2MuonSAMonMT :: L2MuonSAMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode L2MuonSAMonMT :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();
  ATH_CHECK( m_L2MuonSAContainerKey.initialize() );
  return sc;
}


StatusCode L2MuonSAMonMT :: fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  auto lb = Monitored::Scalar<int>(chain+"_LB",-1.0);
  lb = GetEventInfo(ctx)->lumiBlock();

  const float ZERO_LIMIT = 0.00001;

  std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> > featureCont = getTrigDecisionTool()->features<xAOD::L2StandAloneMuonContainer>( chain, TrigDefs::includeFailedDecisions );
  for(const TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer>& muLinkInfo : featureCont){
    ATH_CHECK( muLinkInfo.isValid() );
    const ElementLink<xAOD::L2StandAloneMuonContainer> muEL = muLinkInfo.link;


    // basic EDM variables
    auto saPt = Monitored::Scalar<float>(chain+"_Pt",-999.);
    auto saEta = Monitored::Scalar<float>(chain+"_Eta",-999.);
    auto saPhi = Monitored::Scalar<float>(chain+"_Phi",-999.);
    auto saddr = Monitored::Scalar<int>(chain+"_saddr",-999.);
    auto roiEta = Monitored::Scalar<float>(chain+"_roiEta",-999.);
    auto roiPhi = Monitored::Scalar<float>(chain+"_roiPhi",-999.);
    auto mf_failure = Monitored::Scalar<bool>(chain+"_mf_failure",false);

    saPt = (*muEL)->pt();
    saEta = (*muEL)->eta();
    saPhi = (*muEL)->phi();
    saddr = (*muEL)->sAddress();
    roiEta = (*muEL)->roiEta();
    roiPhi = (*muEL)->roiPhi();

    ATH_MSG_DEBUG("saPt = " << saPt << ", saEta =" << saEta << ", saPhi = " << saPhi << ", saddr = " << saddr);
    if(std::abs(saPt) < ZERO_LIMIT) mf_failure = true;

    fill(m_group+"_"+chain, roiEta, roiPhi, mf_failure);
    if( mf_failure ) continue;

    // define barrel or endcap
    auto isBarrel = Monitored::Scalar<bool>(chain+"_isBarrel",false);
    auto isEndcap = Monitored::Scalar<bool>(chain+"_isEndcap",false);

    if(saddr == -1)  isEndcap = true;
    else isBarrel = true;;

    fill(m_group+"_"+chain, saPt, saEta, saPhi, saddr, isBarrel, isEndcap);


    // define process flow
    std::vector<int> proc_flow;
    auto mon_proc_flow = Monitored::Collection(chain+"_proc_flow", proc_flow);

    bool isL1hitThere               = false;
    bool isL1emuOkForTriggerPlane   = false;
    bool isMDThitThereForTriggerPlane = false;
    bool isMDTFitOkForTriggerPlane    = false;
    bool isMDTFitOkFor2Plane          = false;


    // detector variables
    // RPC and TGC
    auto nRPC = Monitored::Scalar<int>(chain+"_RPC_Pad_N",0);
    auto TGCMidRhoChi2 = Monitored::Scalar<float>(chain+"_TGC_Mid_rho_chi2",-1.0);
    auto TGCMidPhiChi2 = Monitored::Scalar<float>(chain+"_TGC_Mid_phi_chi2",-1.0);
    auto nTGCMidRho = Monitored::Scalar<int>(chain+"_TGC_Mid_rho_N",0);
    auto nTGCMidPhi = Monitored::Scalar<int>(chain+"_TGC_Mid_phi_N",0);

    nRPC = (*muEL)->rpcHitLayer().size();
    TGCMidRhoChi2 = (*muEL)->tgcMidRhoChi2();
    TGCMidPhiChi2 = (*muEL)->tgcMidPhiChi2();
    nTGCMidRho = (*muEL)->tgcMidRhoN();
    nTGCMidPhi = (*muEL)->tgcMidPhiN();

    fill(m_group+"_"+chain, nRPC, TGCMidRhoChi2, TGCMidPhiChi2, nTGCMidRho, nTGCMidPhi, isBarrel, isEndcap);


    // use process flow
    if(isBarrel){
      if( nRPC > 0 ) isL1hitThere = true;
      float rpcFitMidSlope = (*muEL)->rpcFitMidSlope();
      if( std::abs(rpcFitMidSlope) > ZERO_LIMIT ) isL1emuOkForTriggerPlane = true;
    }
    else {
      if( nTGCMidRho > 0 && nTGCMidPhi > 0 ) isL1hitThere = true;
      float TGCMid1Z  = (*muEL)->tgcMid1Z();
      if( std::abs(TGCMid1Z) > ZERO_LIMIT ) isL1emuOkForTriggerPlane = true;
    }


    // define inner, middle, and outer
    int inner  = 0;
    int middle = 1;
    int outer  = 2;

    if (isEndcap) {
      inner  = xAOD::L2MuonParameters::Chamber::EndcapInner;
      middle = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
      outer  = xAOD::L2MuonParameters::Chamber::EndcapOuter;
    } else {
      inner  = xAOD::L2MuonParameters::Chamber::BarrelInner;
      middle = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
      outer  = xAOD::L2MuonParameters::Chamber::BarrelOuter;
    }

    float sign = 1;
    if( saPhi < 0 ) sign = -1;


    // super point
    std::vector<float> sp_r, sp_z;

    auto mon_sp_r= Monitored::Collection(chain+"_MDTpoints_r", sp_r);
    auto mon_sp_z= Monitored::Collection(chain+"_MDTpoints_z", sp_z);

    if( std::abs((*muEL)->superPointR(inner)) > ZERO_LIMIT ) {
      sp_r.push_back( sign * (*muEL)->superPointR(inner) );
      sp_z.push_back( (*muEL)->superPointZ(inner) );
    }
    if( std::abs((*muEL)->superPointR(middle)) > ZERO_LIMIT ) {
      sp_r.push_back( sign * (*muEL)->superPointR(middle) );
      sp_z.push_back( (*muEL)->superPointZ(middle) );
    }
    if( std::abs((*muEL)->superPointR(outer)) > ZERO_LIMIT ) {
      sp_r.push_back( sign * (*muEL)->superPointR(outer) );
      sp_z.push_back( (*muEL)->superPointZ(outer) );
    }

    fill(m_group+"_"+chain, mon_sp_r, mon_sp_z);


    // MDT
    auto MDTInnChi2 = Monitored::Scalar<float>(chain+"_MDT_Inn_fit_chi2",-1.0);
    auto MDTMidChi2 = Monitored::Scalar<float>(chain+"_MDT_Mid_fit_chi2",-1.0);
    auto MDTOutChi2 = Monitored::Scalar<float>(chain+"_MDT_Out_fit_chi2",-1.0);
    auto n_mdt_hits = Monitored::Scalar<int>(chain+"_MDT_N",0);
    auto n_mdt_hits_inner = Monitored::Scalar<int>(chain+"_MDT_Inn_N",0);
    auto n_mdt_hits_middle = Monitored::Scalar<int>(chain+"_MDT_Mid_N",0);
    auto n_mdt_hits_outer = Monitored::Scalar<int>(chain+"_MDT_Out_N",0);

    MDTInnChi2 = (*muEL)->superPointChi2(inner);
    MDTMidChi2 = (*muEL)->superPointChi2(middle);
    MDTOutChi2 = (*muEL)->superPointChi2(outer);


    std::vector<float> res_inn, res_mid, res_out;

    auto mon_res_inn = Monitored::Collection(chain+"_MDT_Inn_residual",res_inn);
    auto mon_res_mid = Monitored::Collection(chain+"_MDT_Mid_residual",res_mid);
    auto mon_res_out = Monitored::Collection(chain+"_MDT_Out_residual",res_out);


    n_mdt_hits = (*muEL)->nMdtHits();

    for (int i_tube=0; i_tube<n_mdt_hits; i_tube++) {
      float res = (*muEL)->mdtHitResidual(i_tube) / 10 ; // to cm
      int imr = (*muEL)->mdtHitChamber(i_tube);

      if (imr == inner) {
        n_mdt_hits_inner++;
        res_inn.push_back(res);
      }
      else if (imr == middle) {
        n_mdt_hits_middle++;
        res_mid.push_back(res);
      }
      else if (imr == outer) {
        n_mdt_hits_outer++;
        res_out.push_back(res);
      }
    }

    fill(m_group+"_"+chain, MDTInnChi2, MDTMidChi2, MDTOutChi2, n_mdt_hits, n_mdt_hits_inner, n_mdt_hits_middle, n_mdt_hits_outer, mon_res_inn, mon_res_mid, mon_res_out, lb, isBarrel, isEndcap);


    // use process flow
    float MDTInnR = (*muEL)->superPointR(inner);
    float MDTMidR = (*muEL)->superPointR(middle);
    float MDTOutR = (*muEL)->superPointR(outer);

    if( n_mdt_hits_middle > 0 ) isMDThitThereForTriggerPlane = true;
    if( MDTMidR > ZERO_LIMIT ) isMDTFitOkForTriggerPlane = true;
    if( isMDTFitOkForTriggerPlane && (MDTInnR > ZERO_LIMIT || MDTOutR > ZERO_LIMIT) ) isMDTFitOkFor2Plane  = true;

    proc_flow.push_back(1);
    if( isL1hitThere )                 proc_flow.push_back(2);
    if( isL1emuOkForTriggerPlane )     proc_flow.push_back(3);
    if( isMDThitThereForTriggerPlane ) proc_flow.push_back(4);
    if( isMDTFitOkForTriggerPlane )    proc_flow.push_back(5);
    if( isMDTFitOkFor2Plane )          proc_flow.push_back(6);

    fill(m_group+"_"+chain, mon_proc_flow);


    // matching to offline
    const xAOD::Muon* RecMuonCB = m_matchTool->matchL2SAtoOff(ctx, (*muEL));
    if(RecMuonCB == nullptr) continue;

    std::vector<float> res_inn_OffMatch = res_inn;
    std::vector<float> res_mid_OffMatch = res_mid;
    std::vector<float> res_out_OffMatch = res_out;

    auto mon_res_inn_OffMatch = Monitored::Collection(chain+"_MDT_Inn_residual_OffMatch",res_inn_OffMatch);
    auto mon_res_mid_OffMatch = Monitored::Collection(chain+"_MDT_Mid_residual_OffMatch",res_mid_OffMatch);
    auto mon_res_out_OffMatch = Monitored::Collection(chain+"_MDT_Out_residual_OffMatch",res_out_OffMatch);

    fill(m_group+"_"+chain, mon_res_inn_OffMatch, mon_res_mid_OffMatch, mon_res_out_OffMatch, isBarrel, isEndcap);

  }

  return StatusCode::SUCCESS;
}



StatusCode L2MuonSAMonMT :: fillVariablesPerOfflineMuonPerChain(const EventContext&, const xAOD::Muon* mu, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  const float ZERO_LIMIT = 0.00001;


  // offline muon variables
  auto offEta = Monitored::Scalar<float>(chain+"_offEta",-999.);
  auto offPt_signed = Monitored::Scalar<float>(chain+"_offPt_signed",-999.);
  offEta = mu->eta();

  float offPt = mu->pt()/1e3;
  float offPhi = mu->phi();
  float offCharge = mu->charge();
  offPt_signed = offPt * offCharge;


  // get L2SA muon link
  const TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> muLinkInfo = m_matchTool->searchL2SALinkInfo(mu, chain);
  if ( !muLinkInfo.isValid() )  return StatusCode::SUCCESS;
  const ElementLink<xAOD::L2StandAloneMuonContainer> muEL = muLinkInfo.link;


  // dR wrt offline
  auto dRmin = Monitored::Scalar<float>(chain+"_dRmin",1000.);
  dRmin = xAOD::P4Helpers::deltaR(mu, *muEL, false); 

  fill(m_group+"_"+chain, dRmin);
  if( ! m_matchTool->isMatchedL2SA( *muEL, mu) ) return StatusCode::SUCCESS; // not matched to L2MuonSA

  
  // L1 RoI wrt offline 
  float saPt = (*muEL)->pt();
  float roiEta = (*muEL)->roiEta();
  float roiPhi = (*muEL)->roiPhi();

  auto roidEta = Monitored::Scalar<float>(chain+"_initialRoI_dEta",-999.);
  auto roidPhi = Monitored::Scalar<float>(chain+"_initialRoI_dPhi",-999.);
  auto roidR = Monitored::Scalar<float>(chain+"_initialRoI_dR",-999.);

  roidEta = roiEta - offEta;
  roidPhi = xAOD::P4Helpers::deltaPhi(offPhi, roiPhi);
  roidR = sqrt(roidEta*roidEta + roidPhi*roidPhi);
  
  fill(m_group+"_"+chain, roidEta, roidPhi, roidR, offEta);


  // pt resolution, inverse pt resolution
  auto ptresol = Monitored::Scalar<float>(chain+"_ptresol",-999.);
  auto invptresol = Monitored::Scalar<float>(chain+"_invptresol",-999.);
  if ( std::abs(offPt) > ZERO_LIMIT && std::abs(saPt) > ZERO_LIMIT ) {
    ptresol = std::abs(saPt)/std::abs(offPt) - 1.;
    invptresol = (1./(offPt * offCharge) - 1./saPt) / (1./(offPt * offCharge));
  }


  // inverse pt resolution depends on charge of offline muon
  std::vector<float> invptresol_pos, invptresol_neg;

  auto mon_invptresol_pos = Monitored::Collection(chain+"_invptresol_pos",invptresol_pos);
  auto mon_invptresol_neg = Monitored::Collection(chain+"_invptresol_neg",invptresol_neg);

  if( offCharge > 0. ) invptresol_pos.push_back(invptresol);
  else invptresol_neg.push_back(invptresol);


  // region variables
  const float ETA_OF_BARREL   = 1.05;
  const float ETA_OF_ENDCAP1   = 1.5;
  const float ETA_OF_ENDCAP2   = 2.0;
  const float ETA_OF_ENDCAP3   = 2.5;

  auto isBarrel = Monitored::Scalar<bool>(chain+"_isBarrel",false);
  auto isBarrelA = Monitored::Scalar<bool>(chain+"_isBarrelA",false);
  auto isBarrelC = Monitored::Scalar<bool>(chain+"_isBarrelC",false);
  auto isEndcapA = Monitored::Scalar<bool>(chain+"_isEndcapA",false);
  auto isEndcapC = Monitored::Scalar<bool>(chain+"_isEndcapC",false);
  auto isEndcap1 = Monitored::Scalar<bool>(chain+"_isEndcap1",false);
  auto isEndcap2 = Monitored::Scalar<bool>(chain+"_isEndcap2",false);
  auto isEndcap3 = Monitored::Scalar<bool>(chain+"_isEndcap3",false);
  auto isEndcap1A = Monitored::Scalar<bool>(chain+"_isEndcap1A",false);
  auto isEndcap2A = Monitored::Scalar<bool>(chain+"_isEndcap2A",false);
  auto isEndcap3A = Monitored::Scalar<bool>(chain+"_isEndcap3A",false);
  auto isEndcap1C = Monitored::Scalar<bool>(chain+"_isEndcap1C",false);
  auto isEndcap2C = Monitored::Scalar<bool>(chain+"_isEndcap2C",false);
  auto isEndcap3C = Monitored::Scalar<bool>(chain+"_isEndcap3C",false);

  // offline pt variables
  auto pt4to6 = Monitored::Scalar<bool>(chain+"_pt4to6",false);
  auto pt6to8 = Monitored::Scalar<bool>(chain+"_pt6to8",false);
  auto ptover8 = Monitored::Scalar<bool>(chain+"_ptover8",false);


  // define region
  if( std::abs(offEta) < ETA_OF_BARREL ) {
    if( offEta > 0. ) isBarrelA = true;
    else isBarrelC = true;
  }
  else{
    if( offEta > 0. ) isEndcapA = true;
    else isEndcapC = true;
  }


  if( std::abs(offEta) < ETA_OF_BARREL ){
    isBarrel = true;
    if( offEta > 0. ) isBarrelA = true;
    else isBarrelC = true;
  }
  else if ( std::abs(offEta) < ETA_OF_ENDCAP1 ){
    isEndcap1 = true;
    if( offEta > 0. ) isEndcap1A = true;
    else isEndcap1C = true;
  }
  else if ( std::abs(offEta) < ETA_OF_ENDCAP2 ){
    isEndcap2 = true;
    if( offEta > 0. ) isEndcap2A = true;
    else isEndcap2C = true;
  }
  else if ( std::abs(offEta) < ETA_OF_ENDCAP3 ){
    isEndcap3 = true;
    if( offEta > 0. ) isEndcap3A = true;
    else isEndcap3C = true;
  }


  if( std::abs(offPt) > 4 ){
    if( std::abs(offPt) < 6 ) pt4to6 = true;
    else if( std::abs(offPt) < 8 ) pt6to8 = true;
    else ptover8 = true;
  }


  fill(m_group+"_"+chain, ptresol, offPt_signed, offEta, isBarrel, isEndcap1, isEndcap2, isEndcap3, isBarrelA, isBarrelC, isEndcapA, isEndcapC, pt4to6, pt6to8, ptover8);  
  fill(m_group+"_"+chain, mon_invptresol_pos, mon_invptresol_neg, isBarrelA, isBarrelC, isEndcap1A, isEndcap2A, isEndcap3A, isEndcap1C, isEndcap2C, isEndcap3C);

  
  return StatusCode::SUCCESS;
}



StatusCode L2MuonSAMonMT :: fillVariables(const EventContext &ctx) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  ATH_CHECK( fillVariableEtaPhi<xAOD::L2StandAloneMuon>(ctx, m_L2MuonSAContainerKey, "L2SA"));

  return StatusCode::SUCCESS;

}



StatusCode L2MuonSAMonMT :: fillVariablesPerOfflineMuon(const EventContext &ctx, const xAOD::Muon* mu) const {

  ATH_CHECK( fillVariablesRatioPlots<xAOD::L2StandAloneMuon>(ctx, mu, "L2SA", xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle,
                                                             [this](const EventContext &ctx, const xAOD::Muon *mu){ return m_matchTool->matchL2SAReadHandle(ctx,mu); }
                                                             ));

  return StatusCode::SUCCESS;

}
