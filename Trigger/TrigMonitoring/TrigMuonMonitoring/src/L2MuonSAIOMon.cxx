/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "L2MuonSAIOMon.h"

#include "xAODTrigMuon/TrigMuonDefs.h"
#include "MuonMatchingTool.h"
#include "math.h"

L2MuonSAIOMon :: L2MuonSAIOMon(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}

StatusCode L2MuonSAIOMon :: initialize(){

  ATH_CHECK( TrigMuonMonitorAlgorithm::initialize() );
  ATH_CHECK( m_L2MuonCBIOContainerKey.initialize() );

  unsigned int nchains = m_monitored_chains.size();
  if(nchains!=m_monitored_chains_plateau.size()){
    ATH_MSG_ERROR("Configuration seems to be wrong. The size of \"Plateaus\" should be same as \"MonitoredChainds\".");
    return StatusCode::FAILURE;
  } else {
    for(unsigned int ichain=0; ichain<nchains; ++ichain){
      m_plateaus[m_monitored_chains[ichain]] = m_monitored_chains_plateau[ichain] + 2.;
    }
  }
  return StatusCode::SUCCESS;
}



StatusCode L2MuonSAIOMon :: fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  if( chain.find("probe") != std::string::npos ) return StatusCode::SUCCESS; // don't use TagAndProbe chains


  const float ZERO_LIMIT = 0.00001;


  //TDT workaround
  std::vector< const xAOD::L2CombinedMuon* > Trig_L2IOobjects;
  //std::vector< bool > pass_muCombHypo;
  ATH_CHECK( matchL2IO_wContainer(ctx, chain, Trig_L2IOobjects) );
  //bool pass_muCombHypo_evt = muCombHypo_TDTworkaround(chain, Trig_L2IOobjects, pass_muCombHypo);
  ATH_MSG_DEBUG(" Trig_L2IOobjects.size(): " << Trig_L2IOobjects.size() );

  // check basic EDM variables using single muon chain
  for(const auto& Trig_L2IOobject : Trig_L2IOobjects){
    ATH_MSG_DEBUG(" Trig_L2IOobject->muSATrack()->roiWord()/Trig_L2IOobject->pt(): " << Trig_L2IOobject->muSATrack()->roiWord() << "/" << Trig_L2IOobject->pt() );
    // basic EDM variables
    bool mf_failure = false;
    auto cbioPt = Monitored::Scalar<float>(chain+"_Pt",-999.);
    auto cbioEta = Monitored::Scalar<float>(chain+"_Eta",-999.);
    auto cbioPhi = Monitored::Scalar<float>(chain+"_Phi",-999.);
    cbioPt  = Trig_L2IOobject->pt()/1e3 * Trig_L2IOobject->charge(); // convert to GeV
    cbioEta  = Trig_L2IOobject->eta();
    cbioPhi  = Trig_L2IOobject->phi();
    ATH_MSG_DEBUG("cbioPt = " << cbioPt << ", cbioEta =" << cbioEta << ", cbioPhi = " << cbioPhi);
    if(std::abs(cbioPt) < ZERO_LIMIT || std::abs(Trig_L2IOobject->muSATrack()->pt()) < ZERO_LIMIT) mf_failure = true;

    if( mf_failure ) continue;

    // region variables
    auto isBarrel = Monitored::Scalar<bool>(chain+"_isBarrel",false);
    auto isEndcap = Monitored::Scalar<bool>(chain+"_isEndcap",false);

    // define regions
    int saddr = Trig_L2IOobject->muSATrack()->sAddress();
    if(saddr == -1)  isEndcap = true;
    else isBarrel = true;;


    fill(m_group+"_"+chain, cbioPt, isBarrel, isEndcap);
    fill(m_group+"_"+chain, cbioEta);
    fill(m_group+"_"+chain, cbioPhi, isBarrel, isEndcap);

  }

  return StatusCode::SUCCESS;
}


StatusCode L2MuonSAIOMon :: fillVariablesPerOfflineMuonPerChain(const EventContext& ctx, const xAOD::Muon* mu, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  const float ZERO_LIMIT = 0.00001;

  // get the best L2Inside-Out object matched to offline muon(dR between L2Inside-Out object and offline muon is minimum)
  const xAOD::L2CombinedMuon* Trig_L2IOobject = searchL2InsideOut(ctx, mu, chain);

  // offline muon variables
  auto offPt = Monitored::Scalar<float>(chain+"_offPt",-999.);
  auto offEta = Monitored::Scalar<float>(chain+"_offEta",-999.);
  auto offPhi = Monitored::Scalar<float>(chain+"_offPhi",-999.);
  offPt = mu->pt()/1e3 * mu->charge(); // convert to GeV
  offEta = mu->eta();
  offPhi = mu->phi();
  float offCharge = mu->charge();


  if( chain.find("probe") != std::string::npos ){ // L2Inside-Out efficiency using Tag&Probe chain
    if ( !getTrigDecisionTool()->isPassed(chain) ) return StatusCode::SUCCESS; // impose trigger pass in order to eliminate bias

    // search tag offline muon
    const xAOD::Muon* tag = searchTagOfflineMuon( ctx, mu );
    if( tag == nullptr ) return StatusCode::SUCCESS; // mu doesn't have no tag muons  


    // efficiency variables
    auto passL2InsideOut = Monitored::Scalar<bool>(chain+"_passL2InsideOut",false);
    auto passL2SA = Monitored::Scalar<bool>(chain+"_passL2SA",false);
    auto offdR = Monitored::Scalar<float>(chain+"_offdR",1000.);
    const ElementLink<xAOD::TrackParticleContainer> &tag_ms_track = tag->muonSpectrometerTrackParticleLink();
    if( !tag_ms_track.isValid() ) return StatusCode::SUCCESS; // tag muon dosen't have ms track
    const ElementLink<xAOD::TrackParticleContainer> &probe_ms_track = mu->muonSpectrometerTrackParticleLink();
    if( !probe_ms_track.isValid() ) return StatusCode::SUCCESS; // probe muon dosen't have ms track
    float tpext_deta = (*tag_ms_track)->eta() - (*probe_ms_track)->eta();
    float tpext_dphi = xAOD::P4Helpers::deltaPhi((*tag_ms_track)->phi(), (*probe_ms_track)->phi());
    offdR = std::sqrt(tpext_deta*tpext_deta + tpext_dphi*tpext_dphi);

    if( Trig_L2IOobject == nullptr ){
      passL2InsideOut = false;
      passL2SA = false;

      // retrieve probe l2SA objects
      int legIndex_probe = 1; // probe
      std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> > featureCont = getTrigDecisionTool()->features<xAOD::L2StandAloneMuonContainer>( chain,
																				   TrigDefs::includeFailedDecisions,
																				   "HLT_MuonL2SAInfo",
																				   TrigDefs::lastFeatureOfType,
																				   TrigCompositeUtils::featureString(),
																				   legIndex_probe );

      for(const TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer>& probe_L2SALinkInfo : featureCont){
	ATH_CHECK( probe_L2SALinkInfo.isValid() );
	const ElementLink<xAOD::L2StandAloneMuonContainer> probe_L2SAobject = probe_L2SALinkInfo.link;
	if( m_matchTool->isMatchedL2SA( (*probe_L2SAobject), mu ) ){
	  if( probe_L2SALinkInfo.state == TrigCompositeUtils::ActiveState::ACTIVE ) passL2SA = true;
	  ATH_MSG_WARNING(" There is at least one ACTIVE probe L2SA object but no L2IO objects." );
	  return StatusCode::SUCCESS;
	}
      }
    }
    else{
      if( !m_matchTool->isMatchedL2InsideOut( Trig_L2IOobject, mu ) ) return StatusCode::SUCCESS; // dR between L2Inside-Out object and offline muon is too large

      passL2InsideOut = isPassedmuCombHypo( chain, Trig_L2IOobject );
      passL2SA = true;
    }

    fill(m_group+"_"+chain, passL2InsideOut, passL2SA, offPt);

    if( mu->pt()/1e3 > m_plateaus.at(chain) ){
      fill(m_group+"_"+chain, passL2InsideOut, passL2SA, offEta, offPhi, offdR);
    }
  }
  else{ // make detail histograms using normal L2Inside-Out chain
    if( Trig_L2IOobject == nullptr ) return StatusCode::SUCCESS; // no L2Inside-Out objects
    if( ! m_matchTool->isMatchedL2InsideOut(Trig_L2IOobject, mu) ) return StatusCode::SUCCESS; // offline muons is not matched to any L2Inside-Out objects


    //L2Muon chamberID index
    enum chamberID {
      Inn_Barrel = 0,
      Mid_Barrel,
      Out_Barrel,
      Inn_Endcap,
      Mid_Endcap,
      Out_Endcap
    };
    std::vector< int > L2Muon_chamberID_index;
    auto mon_L2Muon_chamberID_index = Monitored::Collection(chain+"_L2Muon_chamberID_index",L2Muon_chamberID_index);
    for( int i = 0; i < 6; i++){
      L2Muon_chamberID_index.push_back(i);
    }

    // dR wrt offline
    auto dRmin = Monitored::Scalar<float>(chain+"_dRmin",1000.);
    dRmin = xAOD::P4Helpers::deltaR(mu, Trig_L2IOobject, false);

    // region variables
    const float ETA_OF_BARREL   = 1.05;
    auto regionBE = Monitored::Scalar<int>(chain+"_regionBE",-999);
    auto isBarrel = Monitored::Scalar<bool>(chain+"_isBarrel",false);
    auto isBarrelA = Monitored::Scalar<bool>(chain+"_isBarrelA",false);
    auto isBarrelC = Monitored::Scalar<bool>(chain+"_isBarrelC",false);
    auto isEndcap = Monitored::Scalar<bool>(chain+"_isEndcap",false);
    auto isEndcapA = Monitored::Scalar<bool>(chain+"_isEndcapA",false);
    auto isEndcapC = Monitored::Scalar<bool>(chain+"_isEndcapC",false);

    // offline pt variables
    auto pt4to6 = Monitored::Scalar<bool>(chain+"_pt4to6",false);
    auto pt6to8 = Monitored::Scalar<bool>(chain+"_pt6to8",false);
    auto ptover8 = Monitored::Scalar<bool>(chain+"_ptover8",false);

    // define region
    if( std::abs(offEta) < ETA_OF_BARREL ) {
      regionBE = 0;
      isBarrel = true;
      if( offEta > 0. ) isBarrelA = true;
      else isBarrelC = true;
    }
    else{
      regionBE = 1;
      isEndcap = true;
      if( offEta > 0. ) isEndcapA = true;
      else isEndcapC = true;
    }

    if( std::abs(offPt) > 4 ){
      if( std::abs(offPt) < 6 ) pt4to6 = true;
      else if( std::abs(offPt) < 8 ) pt6to8 = true;
      else ptover8 = true;
    }

    // basic variables
    auto cbioPt = Monitored::Scalar<float>(chain+"_Pt_wrt_offline",-999.);
    auto cbioEta = Monitored::Scalar<float>(chain+"_Eta_wrt_offline",-999.);
    auto cbioPhi = Monitored::Scalar<float>(chain+"_Phi_wrt_offline",-999.);
    cbioPt  = Trig_L2IOobject->pt()/1e3 * Trig_L2IOobject->charge(); // convert to GeV
    cbioEta  = Trig_L2IOobject->eta();
    cbioPhi  = Trig_L2IOobject->phi();

    // L2Inside-Out track multiplicity per L2SA track
    auto L2InsideOut_multiplicity =  Monitored::Scalar<float>(chain+"_L2InsideOut_track_multiplicity",-999.);
    L2InsideOut_multiplicity = 0;
    std::vector< const xAOD::L2CombinedMuon* > Trig_L2IOobjects_tmp;
    ATH_CHECK( matchL2IO_wContainer(ctx, chain, Trig_L2IOobjects_tmp) );
    for(const auto& Trig_L2IOobject_tmp : Trig_L2IOobjects_tmp){
      if( Trig_L2IOobject_tmp->muSATrack()->roiWord() == Trig_L2IOobject->muSATrack()->roiWord() ) L2InsideOut_multiplicity++;
    }

    // pt resolution
    auto ptresol = Monitored::Scalar<float>(chain+"_ptresol",-999.);
    if ( std::abs(offPt) > ZERO_LIMIT && std::abs(cbioPt) > ZERO_LIMIT ){
      ptresol = std::abs(cbioPt)/std::abs(offPt) - 1.;
    }

    std::vector< float > ptresol_pos, ptresol_neg;
    auto mon_ptresol_pos = Monitored::Collection(chain+"_ptresol_pos",ptresol_pos);
    auto mon_ptresol_neg = Monitored::Collection(chain+"_ptresol_neg",ptresol_neg);
    if( offCharge > 0. ) ptresol_pos.push_back(ptresol);
    else ptresol_neg.push_back(ptresol);

    // distance bw FTFroad and offlinesegment
    std::vector< float > distance_bw_FTFroad_and_offlinesegment_Inn_Barrel;
    std::vector< float > distance_bw_FTFroad_and_offlinesegment_Mid_Barrel;
    std::vector< float > distance_bw_FTFroad_and_offlinesegment_Out_Barrel;
    std::vector< float > distance_bw_FTFroad_and_offlinesegment_Inn_Endcap;
    std::vector< float > distance_bw_FTFroad_and_offlinesegment_Mid_Endcap;
    std::vector< float > distance_bw_FTFroad_and_offlinesegment_Out_Endcap;
    auto mon_distance_bw_FTFroad_and_offlinesegment_Inn_Barrel = Monitored::Collection(chain+"_distance_bw_FTFroad_and_offlinesegment_Inn_Barrel",distance_bw_FTFroad_and_offlinesegment_Inn_Barrel);
    auto mon_distance_bw_FTFroad_and_offlinesegment_Mid_Barrel = Monitored::Collection(chain+"_distance_bw_FTFroad_and_offlinesegment_Mid_Barrel",distance_bw_FTFroad_and_offlinesegment_Mid_Barrel);
    auto mon_distance_bw_FTFroad_and_offlinesegment_Out_Barrel = Monitored::Collection(chain+"_distance_bw_FTFroad_and_offlinesegment_Out_Barrel",distance_bw_FTFroad_and_offlinesegment_Out_Barrel);
    auto mon_distance_bw_FTFroad_and_offlinesegment_Inn_Endcap = Monitored::Collection(chain+"_distance_bw_FTFroad_and_offlinesegment_Inn_Endcap",distance_bw_FTFroad_and_offlinesegment_Inn_Endcap);
    auto mon_distance_bw_FTFroad_and_offlinesegment_Mid_Endcap = Monitored::Collection(chain+"_distance_bw_FTFroad_and_offlinesegment_Mid_Endcap",distance_bw_FTFroad_and_offlinesegment_Mid_Endcap);
    auto mon_distance_bw_FTFroad_and_offlinesegment_Out_Endcap = Monitored::Collection(chain+"_distance_bw_FTFroad_and_offlinesegment_Out_Endcap",distance_bw_FTFroad_and_offlinesegment_Out_Endcap);

    std::vector< float > distance_bw_FTFroad_and_offlinesegment_vec;
    std::vector< float > FTFroad_Aw;
    std::vector< float > FTFroad_Bw;
    std::vector< bool > FTFroad_fill;
    for(int i=0; i<6; i++){
      distance_bw_FTFroad_and_offlinesegment_vec.push_back(10000.);
      FTFroad_Aw.push_back(Trig_L2IOobject->muSATrack()->roadAw(i, 0));
      FTFroad_Bw.push_back(Trig_L2IOobject->muSATrack()->roadBw(i, 0));
      FTFroad_fill.push_back(false);
    }

    // MDT hits residual
    std::vector<float> res_Inn_Barrel, res_Mid_Barrel, res_Out_Barrel, res_Inn_Endcap, res_Mid_Endcap, res_Out_Endcap;
    auto mon_res_Inn_Barrel = Monitored::Collection(chain+"_MDT_residual_Inn_Barrel",res_Inn_Barrel);
    auto mon_res_Mid_Barrel = Monitored::Collection(chain+"_MDT_residual_Mid_Barrel",res_Mid_Barrel);
    auto mon_res_Out_Barrel = Monitored::Collection(chain+"_MDT_residual_Out_Barrel",res_Out_Barrel);
    auto mon_res_Inn_Endcap = Monitored::Collection(chain+"_MDT_residual_Inn_Endcap",res_Inn_Endcap);
    auto mon_res_Mid_Endcap = Monitored::Collection(chain+"_MDT_residual_Mid_Endcap",res_Mid_Endcap);
    auto mon_res_Out_Endcap = Monitored::Collection(chain+"_MDT_residual_Out_Endcap",res_Out_Endcap);

    // distance bw MDT hits and offlinesegment
    std::vector< float > distance_bw_MDT_and_offlinesegment_Inn_Barrel;
    std::vector< float > distance_bw_MDT_and_offlinesegment_Mid_Barrel;
    std::vector< float > distance_bw_MDT_and_offlinesegment_Out_Barrel;
    std::vector< float > distance_bw_MDT_and_offlinesegment_Inn_Endcap;
    std::vector< float > distance_bw_MDT_and_offlinesegment_Mid_Endcap;
    std::vector< float > distance_bw_MDT_and_offlinesegment_Out_Endcap;
    auto mon_distance_bw_MDT_and_offlinesegment_Inn_Barrel = Monitored::Collection(chain+"_distance_bw_MDT_and_offlinesegment_Inn_Barrel",distance_bw_MDT_and_offlinesegment_Inn_Barrel);
    auto mon_distance_bw_MDT_and_offlinesegment_Mid_Barrel = Monitored::Collection(chain+"_distance_bw_MDT_and_offlinesegment_Mid_Barrel",distance_bw_MDT_and_offlinesegment_Mid_Barrel);
    auto mon_distance_bw_MDT_and_offlinesegment_Out_Barrel = Monitored::Collection(chain+"_distance_bw_MDT_and_offlinesegment_Out_Barrel",distance_bw_MDT_and_offlinesegment_Out_Barrel);
    auto mon_distance_bw_MDT_and_offlinesegment_Inn_Endcap = Monitored::Collection(chain+"_distance_bw_MDT_and_offlinesegment_Inn_Endcap",distance_bw_MDT_and_offlinesegment_Inn_Endcap);
    auto mon_distance_bw_MDT_and_offlinesegment_Mid_Endcap = Monitored::Collection(chain+"_distance_bw_MDT_and_offlinesegment_Mid_Endcap",distance_bw_MDT_and_offlinesegment_Mid_Endcap);
    auto mon_distance_bw_MDT_and_offlinesegment_Out_Endcap = Monitored::Collection(chain+"_distance_bw_MDT_and_offlinesegment_Out_Endcap",distance_bw_MDT_and_offlinesegment_Out_Endcap);
    std::vector< float > distance_bw_MDT_and_offlinesegment_vec;
    std::vector< int > MDTHitChamber_fill;
    std::vector< int > MDTHitChamber;
    std::vector< float > MDTHitR;
    std::vector< float > MDTHitZ;

    // # of MDT hits
    std::vector<int> MDT_N_Inn_Barrel, MDT_N_Mid_Barrel, MDT_N_Out_Barrel, MDT_N_Inn_Endcap, MDT_N_Mid_Endcap, MDT_N_Out_Endcap;
    auto mon_MDT_N_Inn_Barrel = Monitored::Collection(chain+"_MDT_N_Inn_Barrel",MDT_N_Inn_Barrel);
    auto mon_MDT_N_Mid_Barrel = Monitored::Collection(chain+"_MDT_N_Mid_Barrel",MDT_N_Mid_Barrel);
    auto mon_MDT_N_Out_Barrel = Monitored::Collection(chain+"_MDT_N_Out_Barrel",MDT_N_Out_Barrel);
    auto mon_MDT_N_Inn_Endcap = Monitored::Collection(chain+"_MDT_N_Inn_Endcap",MDT_N_Inn_Endcap);
    auto mon_MDT_N_Mid_Endcap = Monitored::Collection(chain+"_MDT_N_Mid_Endcap",MDT_N_Mid_Endcap);
    auto mon_MDT_N_Out_Endcap = Monitored::Collection(chain+"_MDT_N_Out_Endcap",MDT_N_Out_Endcap);
    int n_mdthits_BI = 0;
    int n_mdthits_BM = 0;
    int n_mdthits_BO = 0;
    int n_mdthits_EI = 0;
    int n_mdthits_EM = 0;
    int n_mdthits_EO = 0;

    int n_mdt_hits = Trig_L2IOobject->muSATrack()->nMdtHits();
    for(int i_tube=0; i_tube<n_mdt_hits; i_tube++){
      if( Trig_L2IOobject->muSATrack()->mdtHitIsOutlier(i_tube) != 0 ) continue;
      float res = Trig_L2IOobject->muSATrack()->mdtHitResidual(i_tube);
      int imr = Trig_L2IOobject->muSATrack()->mdtHitChamber(i_tube);
      MDTHitChamber.push_back(imr);
      MDTHitR.push_back(Trig_L2IOobject->muSATrack()->mdtHitR(i_tube));
      MDTHitZ.push_back(Trig_L2IOobject->muSATrack()->mdtHitZ(i_tube));

      if( imr == Inn_Barrel ){
	n_mdthits_BI++;
	res_Inn_Barrel.push_back(res);
      }
      else if( imr == Mid_Barrel ){
	n_mdthits_BM++;
	res_Mid_Barrel.push_back(res);
      }
      else if( imr == Out_Barrel ){
	n_mdthits_BO++;
	res_Out_Barrel.push_back(res);
      }
      else if( imr == Inn_Endcap ){
	n_mdthits_EI++;
	res_Inn_Endcap.push_back(res);
      }
      else if( imr == Mid_Endcap ){
	n_mdthits_EM++;
	res_Mid_Endcap.push_back(res);
      }
      else if( imr == Out_Endcap ){
	n_mdthits_EO++;
	res_Out_Endcap.push_back(res);
      }
    }

    // reconstruction efficiency of superpoint
    std::vector<bool> superpoint_exist_pt4to6, superpoint_exist_pt6to8, superpoint_exist_ptover8;
    auto mon_superpoint_exist_pt4to6 = Monitored::Collection(chain+"_superpoint_pt4to6",superpoint_exist_pt4to6);
    auto mon_superpoint_exist_pt6to8 = Monitored::Collection(chain+"_superpoint_pt6to8",superpoint_exist_pt6to8);
    auto mon_superpoint_exist_ptover8 = Monitored::Collection(chain+"_superpoint_ptover8",superpoint_exist_ptover8);
    std::vector<bool> segment_superpoint_exist(6, false);
    std::vector<bool> offlinesegment_exist_pt4to6, offlinesegment_exist_pt6to8, offlinesegment_exist_ptover8;
    auto mon_offlinesegment_exist_pt4to6 = Monitored::Collection(chain+"_offlinesegment_pt4to6",offlinesegment_exist_pt4to6);
    auto mon_offlinesegment_exist_pt6to8 = Monitored::Collection(chain+"_offlinesegment_pt6to8",offlinesegment_exist_pt6to8);
    auto mon_offlinesegment_exist_ptover8 = Monitored::Collection(chain+"_offlinesegment_ptover8",offlinesegment_exist_ptover8);

    // # of superpoint
    auto superpoint_multiplicity = Monitored::Scalar<int>(chain+"_superpoint_multiplicity",0);
    std::vector< bool > superpoint_exist;
    std::vector< float > superpointR;
    int Num_L2Muon_chamberID = 12;
    for( int i_chamber = 0; i_chamber < Num_L2Muon_chamberID; i_chamber++){
      if( Trig_L2IOobject->muSATrack()->superPointR(i_chamber) < ZERO_LIMIT ){
	superpoint_exist.push_back(false);
      }
      else{
	superpoint_exist.push_back(true);
	superpoint_multiplicity++;
      }
      superpointR.push_back(Trig_L2IOobject->muSATrack()->superPointR(i_chamber));
    }


    std::vector< bool > segment_exist(6, false);
    for(unsigned int i_seg = 0; i_seg < mu->nMuonSegments(); i_seg++){
      const xAOD::MuonSegment* segment = mu->muonSegment(i_seg);
      if(!segment) continue;
      float segmentX = segment->x();
      float segmentY = segment->y();
      float segmentZ = segment->z();
      float segmentR = std::sqrt( std::pow(segmentX, 2.0) + std::pow(segmentY, 2.0) );
      float segmentPx = segment->px();
      float segmentPy = segment->py();
      float segmentPz = segment->pz();
      float segmentSector = segment->sector();
      int segmentChamberIndex = segment->chamberIndex();
      float distance_bw_FTFroad_and_offlinesegment = 99999.;
      float distance_bw_MDT_and_offlinesegment = 99999.;
      int roadChamberIndex = -1;
      int MDTChamberIndex = -1;
      if( segmentChamberIndex == 0 || segmentChamberIndex == 1 ){ // Inner Barrel
	segment_exist.at(Inn_Barrel) = true;
	roadChamberIndex = Inn_Barrel;
	MDTChamberIndex = Inn_Barrel;
      }
      else if( segmentChamberIndex == 2 || segmentChamberIndex == 3 ){ // Middle Barrel
	segment_exist.at(Mid_Barrel) = true;
	roadChamberIndex = Mid_Barrel;
	MDTChamberIndex = Mid_Barrel;
      }
      else if( segmentChamberIndex == 4 || segmentChamberIndex == 5 ){ // Outer Barrel
	segment_exist.at(Out_Barrel) = true;
	roadChamberIndex = Out_Barrel;
	MDTChamberIndex = Out_Barrel;
      }
      else if( segmentChamberIndex == 7 || segmentChamberIndex == 8 ){ // Inner Endcap
	segment_exist.at(Inn_Endcap) = true;
	roadChamberIndex = Inn_Endcap;
	MDTChamberIndex = Inn_Endcap;
      }
      else if( segmentChamberIndex == 9 || segmentChamberIndex == 10 ){ // Middle Endcap
	segment_exist.at(Mid_Endcap) = true;
	roadChamberIndex = Mid_Endcap;
	MDTChamberIndex = Mid_Endcap;
      }
      else if( segmentChamberIndex == 11 || segmentChamberIndex == 12 ){ // Outer Endcap
	segment_exist.at(Out_Endcap) = true;
	roadChamberIndex = Out_Endcap;
	MDTChamberIndex = Out_Endcap;
      }

      // Calc distance bw FTFroad and offlinesegment
      if( roadChamberIndex != -1 ){
	if( FTFroad_Aw.at(roadChamberIndex) > ZERO_LIMIT || FTFroad_Bw.at(roadChamberIndex) > ZERO_LIMIT ){
	  FTFroad_fill.at(roadChamberIndex) = true;
	  if( FTFroad_Aw.at(roadChamberIndex) < ZERO_LIMIT) distance_bw_FTFroad_and_offlinesegment = segmentR - FTFroad_Bw.at(roadChamberIndex);
	  else{
	    float ia = 1.0/FTFroad_Aw.at(roadChamberIndex);
	    float iaq = ia * ia;
	    distance_bw_FTFroad_and_offlinesegment = (segmentZ - ia * (segmentR - FTFroad_Bw.at(roadChamberIndex)))/std::sqrt(1.0 + iaq);
	  }
	  if( std::abs(distance_bw_FTFroad_and_offlinesegment) < std::abs(distance_bw_FTFroad_and_offlinesegment_vec.at(roadChamberIndex)) )
	    distance_bw_FTFroad_and_offlinesegment_vec.at(roadChamberIndex) = distance_bw_FTFroad_and_offlinesegment;
	}
      }

      // Calc distance bw MDT hits and offlinesegment
      if( MDTChamberIndex != -1 ){
	float sector_phi = M_PI*(segmentSector - 1.0)/8.0;
	float segmentR_projection = segmentX * std::cos(sector_phi) + segmentY * std::sin(sector_phi);
	float segmentPr_projection = segmentPx * std::cos(sector_phi) + segmentPy * std::sin(sector_phi);
	
	for(unsigned int i_tube=0; i_tube<MDTHitChamber.size(); i_tube++){
	  if( MDTHitChamber.at(i_tube) != MDTChamberIndex ) continue;
	  if( MDTChamberIndex < 3 ){ //Barrel
	    if( std::abs(segmentPz) < ZERO_LIMIT ) distance_bw_MDT_and_offlinesegment = MDTHitZ.at(i_tube) - segmentZ;
	    else{
	      float denominator = segmentPr_projection/segmentPz;
	      if( std::abs(denominator) < ZERO_LIMIT ) continue;
	      distance_bw_MDT_and_offlinesegment = MDTHitZ.at(i_tube) - ((MDTHitR.at(i_tube) - segmentR_projection)/denominator  + segmentZ);
	    }
	  }
	  else{ //Endcap
	    if( std::abs(segmentPz) < ZERO_LIMIT ){ 
	      distance_bw_MDT_and_offlinesegment = MDTHitR.at(i_tube) - segmentR;
	    }
	    else{
	      float coeffi = (MDTHitZ.at(i_tube) - segmentZ)/segmentPz;
	      float segmentR_extrapolated = std::sqrt(std::pow(segmentX + coeffi * segmentPx, 2.0) + std::pow(segmentY + coeffi * segmentPy, 2.0));
	      distance_bw_MDT_and_offlinesegment = MDTHitR.at(i_tube) - segmentR_extrapolated;
	    }
	  }
	  distance_bw_MDT_and_offlinesegment_vec.push_back(distance_bw_MDT_and_offlinesegment);
	  MDTHitChamber_fill.push_back(MDTHitChamber.at(i_tube));
	}
      }
    }

    if( FTFroad_fill.at(Inn_Barrel) ){
      distance_bw_FTFroad_and_offlinesegment_Inn_Barrel.push_back(distance_bw_FTFroad_and_offlinesegment_vec.at(Inn_Barrel));
    }
    if( FTFroad_fill.at(Mid_Barrel) ){
      distance_bw_FTFroad_and_offlinesegment_Mid_Barrel.push_back(distance_bw_FTFroad_and_offlinesegment_vec.at(Mid_Barrel));
    }
    if( FTFroad_fill.at(Out_Barrel) ){
      distance_bw_FTFroad_and_offlinesegment_Out_Barrel.push_back(distance_bw_FTFroad_and_offlinesegment_vec.at(Out_Barrel));
    }
    if( FTFroad_fill.at(Inn_Endcap) ){
      distance_bw_FTFroad_and_offlinesegment_Inn_Endcap.push_back(distance_bw_FTFroad_and_offlinesegment_vec.at(Inn_Endcap));
    }
    if( FTFroad_fill.at(Mid_Endcap) ){
      distance_bw_FTFroad_and_offlinesegment_Mid_Endcap.push_back(distance_bw_FTFroad_and_offlinesegment_vec.at(Mid_Endcap));
    }
    if( FTFroad_fill.at(Out_Endcap) ){
      distance_bw_FTFroad_and_offlinesegment_Out_Endcap.push_back(distance_bw_FTFroad_and_offlinesegment_vec.at(Out_Endcap));
    }

    for( unsigned int i = 0; i < distance_bw_MDT_and_offlinesegment_vec.size(); i++ ){
      if( MDTHitChamber_fill.at(i) == Inn_Barrel ){
	distance_bw_MDT_and_offlinesegment_Inn_Barrel.push_back(distance_bw_MDT_and_offlinesegment_vec.at(i));
      }
      else if( MDTHitChamber_fill.at(i) == Mid_Barrel ){
	distance_bw_MDT_and_offlinesegment_Mid_Barrel.push_back(distance_bw_MDT_and_offlinesegment_vec.at(i));
      }
      else if( MDTHitChamber_fill.at(i) == Out_Barrel ){
	distance_bw_MDT_and_offlinesegment_Out_Barrel.push_back(distance_bw_MDT_and_offlinesegment_vec.at(i));
      }
      else if( MDTHitChamber_fill.at(i) == Inn_Endcap ){
	distance_bw_MDT_and_offlinesegment_Inn_Endcap.push_back(distance_bw_MDT_and_offlinesegment_vec.at(i));
      }
      else if( MDTHitChamber_fill.at(i) == Mid_Endcap ){
	distance_bw_MDT_and_offlinesegment_Mid_Endcap.push_back(distance_bw_MDT_and_offlinesegment_vec.at(i));
      }
      else if( MDTHitChamber_fill.at(i) == Out_Endcap ){
	distance_bw_MDT_and_offlinesegment_Out_Endcap.push_back(distance_bw_MDT_and_offlinesegment_vec.at(i));
      }
      else{
	ATH_MSG_WARNING( "undefined chamberID is pushed back into MDTHitChamber_fill" );
      }
    }

    if( segment_exist.at(Inn_Barrel) ){
      MDT_N_Inn_Barrel.push_back(n_mdthits_BI);
      segment_superpoint_exist.at(Inn_Barrel) = superpoint_exist.at(Inn_Barrel);
    }
    if( segment_exist.at(Mid_Barrel) ){
      MDT_N_Mid_Barrel.push_back(n_mdthits_BM);
      segment_superpoint_exist.at(Mid_Barrel) = superpoint_exist.at(Mid_Barrel);
    }
    if( segment_exist.at(Out_Barrel) ){
      MDT_N_Out_Barrel.push_back(n_mdthits_BO);
      segment_superpoint_exist.at(Out_Barrel) = superpoint_exist.at(Out_Barrel);
    }
    if( segment_exist.at(Inn_Endcap) ){
      MDT_N_Inn_Endcap.push_back(n_mdthits_EI);
      segment_superpoint_exist.at(Inn_Endcap) = superpoint_exist.at(Inn_Endcap);
    }
    if( segment_exist.at(Mid_Endcap) ){
      MDT_N_Mid_Endcap.push_back(n_mdthits_EM);
      segment_superpoint_exist.at(Mid_Endcap) = superpoint_exist.at(Mid_Endcap);
    }
    if( segment_exist.at(Out_Endcap) ){
      MDT_N_Out_Endcap.push_back(n_mdthits_EO);
      segment_superpoint_exist.at(Out_Endcap) = superpoint_exist.at(Out_Endcap);
    }

    if( pt4to6 ){
      offlinesegment_exist_pt4to6 = segment_exist;
      superpoint_exist_pt4to6 = segment_superpoint_exist;
    }
    else if( pt6to8 ){
      offlinesegment_exist_pt6to8 = segment_exist;
      superpoint_exist_pt6to8 = segment_superpoint_exist;
    }
    else if( ptover8 ){
      offlinesegment_exist_ptover8 = segment_exist;
      superpoint_exist_ptover8 = segment_superpoint_exist;
    }

    fill(m_group+"_"+chain, dRmin, isBarrel, isEndcap);
    fill(m_group+"_"+chain, cbioPt, isBarrel, isEndcap);
    fill(m_group+"_"+chain, cbioEta);
    fill(m_group+"_"+chain, cbioPhi, isBarrel, isEndcap);
    fill(m_group+"_"+chain, L2InsideOut_multiplicity, offPt, isBarrel, isEndcap);
    fill(m_group+"_"+chain, ptresol, offEta, pt4to6, pt6to8, ptover8);
    fill(m_group+"_"+chain, ptresol, offPt, isBarrelA, isBarrelC, isEndcapA, isEndcapC);
    fill(m_group+"_"+chain, mon_ptresol_pos, mon_ptresol_neg, isBarrelA, isBarrelC, isEndcapA, isEndcapC);
    fill(m_group+"_"+chain, mon_distance_bw_FTFroad_and_offlinesegment_Inn_Barrel, pt4to6, pt6to8, ptover8);
    fill(m_group+"_"+chain, mon_distance_bw_FTFroad_and_offlinesegment_Mid_Barrel, pt4to6, pt6to8, ptover8);
    fill(m_group+"_"+chain, mon_distance_bw_FTFroad_and_offlinesegment_Out_Barrel, pt4to6, pt6to8, ptover8);
    fill(m_group+"_"+chain, mon_distance_bw_FTFroad_and_offlinesegment_Inn_Endcap, pt4to6, pt6to8, ptover8);
    fill(m_group+"_"+chain, mon_distance_bw_FTFroad_and_offlinesegment_Mid_Endcap, pt4to6, pt6to8, ptover8);
    fill(m_group+"_"+chain, mon_distance_bw_FTFroad_and_offlinesegment_Out_Endcap, pt4to6, pt6to8, ptover8);
    fill(m_group+"_"+chain, mon_res_Inn_Barrel);
    fill(m_group+"_"+chain, mon_res_Mid_Barrel);
    fill(m_group+"_"+chain, mon_res_Out_Barrel);
    fill(m_group+"_"+chain, mon_res_Inn_Endcap);
    fill(m_group+"_"+chain, mon_res_Mid_Endcap);
    fill(m_group+"_"+chain, mon_res_Out_Endcap);
    fill(m_group+"_"+chain, mon_distance_bw_MDT_and_offlinesegment_Inn_Barrel);
    fill(m_group+"_"+chain, mon_distance_bw_MDT_and_offlinesegment_Mid_Barrel);
    fill(m_group+"_"+chain, mon_distance_bw_MDT_and_offlinesegment_Out_Barrel);
    fill(m_group+"_"+chain, mon_distance_bw_MDT_and_offlinesegment_Inn_Endcap);
    fill(m_group+"_"+chain, mon_distance_bw_MDT_and_offlinesegment_Mid_Endcap);
    fill(m_group+"_"+chain, mon_distance_bw_MDT_and_offlinesegment_Out_Endcap);
    fill(m_group+"_"+chain, mon_MDT_N_Inn_Barrel);
    fill(m_group+"_"+chain, mon_MDT_N_Mid_Barrel);
    fill(m_group+"_"+chain, mon_MDT_N_Out_Barrel);
    fill(m_group+"_"+chain, mon_MDT_N_Inn_Endcap);
    fill(m_group+"_"+chain, mon_MDT_N_Mid_Endcap);
    fill(m_group+"_"+chain, mon_MDT_N_Out_Endcap);
    fill(m_group+"_"+chain, superpoint_multiplicity, regionBE, pt4to6, pt6to8, ptover8);
    fill(m_group+"_"+chain, mon_L2Muon_chamberID_index, mon_superpoint_exist_pt4to6, mon_offlinesegment_exist_pt4to6);
    fill(m_group+"_"+chain, mon_L2Muon_chamberID_index, mon_superpoint_exist_pt6to8, mon_offlinesegment_exist_pt6to8);
    fill(m_group+"_"+chain, mon_L2Muon_chamberID_index, mon_superpoint_exist_ptover8, mon_offlinesegment_exist_ptover8);
  }

  return StatusCode::SUCCESS;
}


StatusCode L2MuonSAIOMon :: matchL2IO_wContainer(const EventContext &ctx, const std::string &chain, std::vector< const xAOD::L2CombinedMuon* > &Trig_L2IOobjects) const {

  ATH_MSG_DEBUG ("matchL2IO_wContainer ..." );

  // retrieve l2io objects
  SG::ReadHandle<xAOD::L2CombinedMuonContainer> L2IOobjects( m_L2MuonCBIOContainerKey, ctx );

  // retrieve l2SA objects
  std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> > featureCont;
  if( chain.find("probe") != std::string::npos ){ // if tag & probe chain, retrieve probe L2SA objects
    int legIndex_probe = 1; // probe
    featureCont = getTrigDecisionTool()->features<xAOD::L2StandAloneMuonContainer>( chain,
										    TrigDefs::includeFailedDecisions,
										    "HLT_MuonL2SAInfo",
										    TrigDefs::lastFeatureOfType,
										    TrigCompositeUtils::featureString(),
										    legIndex_probe );
  }
  else{
    featureCont = getTrigDecisionTool()->features<xAOD::L2StandAloneMuonContainer>( chain,
										    TrigDefs::includeFailedDecisions,
										    "HLT_MuonL2SAInfo" );
  }

  // match l2io objects to l2sa objects using roiWord
  std::vector< const xAOD::L2CombinedMuon* > matchSA_L2IOobjects;
  for(const auto L2IOobject : *L2IOobjects){
    ATH_MSG_DEBUG(" L2IOobject->muSATrack()->roiWord()/L2IOobject->pt(): " << L2IOobject->muSATrack()->roiWord() << "/" << L2IOobject->pt() );
    for(const TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer>& L2SALinkInfo : featureCont){
      ATH_CHECK( L2SALinkInfo.isValid() );
      const ElementLink<xAOD::L2StandAloneMuonContainer> L2SAobject = L2SALinkInfo.link;
      if( !L2SAobject.isValid() ) continue;
      ATH_MSG_DEBUG(" L2SAobject->roiWord()/L2SALinkInfo.state: " << (*L2SAobject)->roiWord() << "/" << L2SALinkInfo.state );

      if( L2IOobject->muSATrack()->roiWord() != (*L2SAobject)->roiWord() ) continue;
      if( L2SALinkInfo.state != TrigCompositeUtils::ActiveState::ACTIVE ){
        break;
      }else{
        matchSA_L2IOobjects.push_back(L2IOobject);
        ATH_MSG_DEBUG(" matchSA_L2IOobject->muSATrack()->roiWord()/matchSA_L2IOobject->pt(): " << L2IOobject->muSATrack()->roiWord() << "/" << L2IOobject->pt() );
        break;
      }
    }
  }
  ATH_MSG_DEBUG(" matchSA_L2IOobjects.size(): " << matchSA_L2IOobjects.size() );

  const size_t num_matchSAMuon = matchSA_L2IOobjects.size();
  if( num_matchSAMuon == 0 ){
    ATH_MSG_DEBUG(" NO matchSA_L2IOobjects ");
    return StatusCode::SUCCESS;
  }

  std::vector< bool > isoverlap( num_matchSAMuon, false );
  std::vector< bool > passOR( num_matchSAMuon, true );

  // L2CBOverlapRemover
  ATH_CHECK( L2OverlapRemover( matchSA_L2IOobjects, isoverlap, passOR ) );
  for(unsigned int i=0; i<num_matchSAMuon; i++) { // push back trig Inside-Out objects passing L2CBOverlapRemover
    if( isoverlap[i] && !passOR[i] ) continue;
    Trig_L2IOobjects.push_back(matchSA_L2IOobjects.at(i));
  }

  return StatusCode::SUCCESS;
}


StatusCode L2MuonSAIOMon :: L2OverlapRemover( std::vector< const xAOD::L2CombinedMuon* > matchSA_L2IOobjects, std::vector< bool > &isoverlap, std::vector< bool > &passOR ) const {

  ATH_MSG_DEBUG ("L2OverlapRemover ..." );

  const size_t numMuon = matchSA_L2IOobjects.size();
  bool errorWhenIdentifyingOverlap = false;

  if(numMuon > 1){
    std::vector<unsigned int> mucombResult;
    //unsigned int i,j;
    for(unsigned int i=0; i<numMuon; i++) {mucombResult.emplace_back(i); }
    for(unsigned int i=0; i<numMuon-1; i++){
      for(unsigned int j=i+1; j<numMuon; j++){
	ATH_MSG_DEBUG("++ i=" << i << " vs j=" << j);
	bool overlapped = isOverlap(matchSA_L2IOobjects.at(i), matchSA_L2IOobjects.at(j));
	ATH_MSG_DEBUG("matchSA_L2IOobjects: i/j/Overlap = " << i << "/" << j << "/" << overlapped );
	if( ! overlapped ){ // judged as different
	  ATH_MSG_DEBUG("   judged as: differenr objects");
	  if( mucombResult[i] == mucombResult[j] ) { // but marked as same by someone
	    ATH_MSG_DEBUG( "inconsistentency in muComb overlap removal for more than two objects" );
	    ATH_MSG_DEBUG( "two objects are judged as different but both were already marked as identical by someone else as: " );
	    ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mucombResult[i] << " / "  << mucombResult[j] );
	    errorWhenIdentifyingOverlap = true;
	  }
	}
	else{ // judged as overlap
	  if( (mucombResult[j] != j && mucombResult[i] != mucombResult[j]) || (mucombResult[j] == j && mucombResult[i] != i) ){
	    ATH_MSG_DEBUG( "inconsistentency in muComb based overlap removal for more than two objects" );
	    ATH_MSG_DEBUG( "two objects are judged as overlap but only either was already marked as overlap to someone else: " );
	    ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mucombResult[i] << " / "  << mucombResult[j] );
	    errorWhenIdentifyingOverlap = true;
	  }
	  ATH_MSG_DEBUG("   judged as: overlapped objects");
	  if( mucombResult[i] == i ) {
	    ATH_MSG_DEBUG( "   i is not yet marked as overlap. so, it is a newly found overlap" );
	    ATH_MSG_DEBUG( "   -> marking mucombResult[j] as i..." );
	    mucombResult[j] = i;
	    isoverlap[i] = true;
            isoverlap[j] = true;
	  } else {
	    ATH_MSG_DEBUG( "   both i/j already marked as overlap by: mucombResult[i]=" << mucombResult[i] );
	    ATH_MSG_DEBUG( "   -> do nothing..." );
	  }
	}
      }
    }


    if( errorWhenIdentifyingOverlap ) {
      ATH_MSG_WARNING( "error when resolving overlap. exitting with all EVs active..." );
    } else {

      unsigned int n_uniqueMuon = 0;
      for(unsigned int i=0; i<numMuon; i++) {
	ATH_MSG_DEBUG( "muComb based results: i=" << i << ": ");
	if( mucombResult[i] != i ) {
	  ATH_MSG_DEBUG( "      overlap to j=" << mucombResult[i] );
	} else {
          n_uniqueMuon++;
	  ATH_MSG_DEBUG( "      unique" );
	}
      }

      ATH_MSG_DEBUG( "nr of unique Muons after muComb-based removal=" << n_uniqueMuon );

      if( numMuon != n_uniqueMuon ){
        ATH_CHECK( chooseBestMuon(matchSA_L2IOobjects, passOR, mucombResult) );
      } else {
	ATH_MSG_DEBUG( "no overlap identified. exitting with all EventViews active" );
      }
    }
  }

  return StatusCode::SUCCESS;
}


bool L2MuonSAIOMon :: isOverlap( const xAOD::L2CombinedMuon* matchSA_L2IOobject1, const xAOD::L2CombinedMuon* matchSA_L2IOobject2 ) const {

  ATH_MSG_DEBUG( "   ...matchSA_L2IOobject1: pt/eta/phi=" << matchSA_L2IOobject1->pt()/Gaudi::Units::GeV << " / " << matchSA_L2IOobject1->eta() << " / " << matchSA_L2IOobject1->phi() );
  ATH_MSG_DEBUG( "   ...matchSA_L2IOobject2: pt/eta/phi=" << matchSA_L2IOobject2->pt()/Gaudi::Units::GeV << " / " << matchSA_L2IOobject2->eta() << " / " << matchSA_L2IOobject2->phi() );

  const auto [mu1Pt, mu1Eta, mu1Phi] = L2ORPosForMatchFunc(matchSA_L2IOobject1);
  const auto [mu2Pt, mu2Eta, mu2Phi] = L2ORPosForMatchFunc(matchSA_L2IOobject2);

  // if dR or invMass is necessary but (eta,phi) info is not avaiable
  // (i.e. eta,phi=0,0; rec failed)
  const double ZERO_LIMIT_FOR_ETAPHI = 1e-4;
  if( (std::abs(matchSA_L2IOobject1->eta()) <ZERO_LIMIT_FOR_ETAPHI && std::abs(matchSA_L2IOobject1->phi()) < ZERO_LIMIT_FOR_ETAPHI) ||
      (std::abs(matchSA_L2IOobject2->eta()) <ZERO_LIMIT_FOR_ETAPHI && std::abs(matchSA_L2IOobject2->phi()) < ZERO_LIMIT_FOR_ETAPHI) ) {
    ATH_MSG_DEBUG( "   ...-> (eta,phi) info not available (rec at (eta,phi)=(0,0))" );
    if( m_RequireDR || m_RequireMass ) {
      ATH_MSG_DEBUG( "   ...-> but dR of invMass check is required. cannot judge overlap -> return with false" );
      return false;
    }
  }

  // if charge or invMass is necessary but charge(=pT) info is not avaiable
  const double ZERO_LIMIT_FOR_PT = 1e-4;
  if( (std::abs(matchSA_L2IOobject1->pt()) <ZERO_LIMIT_FOR_PT) || (std::abs(matchSA_L2IOobject2->pt()) < ZERO_LIMIT_FOR_PT) ) {
    ATH_MSG_DEBUG( "   ...-> pT info not available (rec at pT=0)" );
    if( m_RequireSameSign || m_RequireMass ) {
      ATH_MSG_DEBUG( "   ...-> but same sign or invMass check is required. cannot judge overlap -> return with false" );
      return false;
    }
  }


  double absEta = (std::abs(mu1Pt) > std::abs(mu2Pt)) ? std::abs(mu1Eta) : std::abs(mu2Eta);
  unsigned int iThres=0;
  for(unsigned int i=0; i<(m_etaBins.size()-1); i++) {
    if ( m_etaBins[i] <= absEta && absEta < m_etaBins[i+1] ) iThres = i;
  }
  float dRThres   = m_dRCBThres[iThres];
  float dRbySAThres = m_dRbySAThres[iThres];
  float massThres = m_massCBThres[iThres];
  ATH_MSG_DEBUG( "   ...iThres=" << iThres );
  if(m_RequireDR) ATH_MSG_DEBUG( "   ...dR       threshold=" << dRThres     );
  if(m_RequireDRbySA) ATH_MSG_DEBUG( "   ...dR(byMF) threshold=" << dRbySAThres );
  if(m_RequireMass) ATH_MSG_DEBUG( "   ...mass     threshold=" << massThres   );



  // same sign cut
  bool sameSign = false;
  if( m_RequireSameSign ) {
    sameSign = ((mu1Pt*mu2Pt) > 0);
    ATH_MSG_DEBUG( "   ...-> sameSign=" << sameSign );
  }

  // dR cut
  bool dRisClose = false;
  float deta = mu1Eta - mu2Eta;
  float dphi = xAOD::P4Helpers::deltaPhi(mu1Phi, mu2Phi);
  float dR = std::sqrt(deta*deta + dphi*dphi);
  if( m_RequireDR ) {
    if( dR < dRThres ) dRisClose = true;
    ATH_MSG_DEBUG( "   ...-> dR=" << dR << " : dRisClose=" << dRisClose );
  }

  // dR(by L2SA) cut
  bool dRbySAisClose = false;
  const xAOD::L2StandAloneMuon* muSA1 = matchSA_L2IOobject1->muSATrack();
  const xAOD::L2StandAloneMuon* muSA2 = matchSA_L2IOobject1->muSATrack();
  if( m_RequireDRbySA ) {
    // here, we do not check (eta,phi) of mF is not (0,0)
    // (i.e. we apply muComb based cut even if muFast rec is failed)
    float deta = muSA1->etaMS() - muSA2->etaMS();
    float dphi = xAOD::P4Helpers::deltaPhi(muSA1->phiMS(), muSA2->phiMS());
    float dRBySA = std::sqrt(deta*deta + dphi*dphi);
    if( dRBySA < dRbySAThres ) dRbySAisClose = true;
    ATH_MSG_DEBUG( "   ...-> dR(by MF)=" << dRBySA << " : dRbySAisClose=" << dRbySAisClose );
  }

  // mass cut
  const double TRACK_MASS = 0.;  // just assume zero mass
  bool massIsClose = false;
  TLorentzVector lvioobj1, lvioobj2;
  lvioobj1.SetPtEtaPhiM(std::abs(mu1Pt), mu1Eta, mu1Phi, TRACK_MASS);
  lvioobj2.SetPtEtaPhiM(std::abs(mu2Pt), mu2Eta, mu2Phi, TRACK_MASS);
  TLorentzVector lvsum = lvioobj1 + lvioobj2;
  float invMass = lvsum.M();
  if( m_RequireMass ) {
    if( invMass < massThres ) massIsClose = true;
    ATH_MSG_DEBUG( "   ...-> invMass=" << invMass << " : massIsClose=" << massIsClose );
  }


  // total judge
  bool overlap = false;
  if( ((m_RequireSameSign &&   sameSign)     || (! m_RequireSameSign)) &&
      ((m_RequireDR       &&  dRisClose)     || (! m_RequireDR))       &&
      ((m_RequireDRbySA &&  dRbySAisClose) || (!  m_RequireDRbySA)) &&
      ((m_RequireMass     &&  massIsClose)   || (! m_RequireMass)) ) {
    overlap = true;
  }
  ATH_MSG_DEBUG( "   ...=> isOverlap=" << overlap );

  return overlap;
}


StatusCode L2MuonSAIOMon :: chooseBestMuon( std::vector< const xAOD::L2CombinedMuon* > matchSA_L2IOobjects, std::vector< bool > &passOR, std::vector< unsigned int > &mucombResult ) const{

  const double ZERO_LIMIT = 1e-4;
  unsigned int i,j,k;

  ATH_MSG_DEBUG( "--- choose best among overlaps & disable EVs (muComb based) ---" );
  for(i=0; i<matchSA_L2IOobjects.size(); i++) {
    ATH_MSG_DEBUG( "++ i=" << i << ": result=" << mucombResult[i] );
    if( mucombResult[i] != i ) {
      ATH_MSG_DEBUG( "   overlap to some one. skip." );
      continue;
    }
    std::vector<unsigned int> others;
    for(j=0; j<matchSA_L2IOobjects.size(); j++) {
      if( mucombResult[j] == mucombResult[i] ) others.emplace_back(j);
    }
    if( others.size() == 1 ) {
      ATH_MSG_DEBUG( "   unique object. keep it active." );
      continue;
    }
    else { // must choose one best
      ATH_MSG_DEBUG( "   overlapped objects among: " << others );
      unsigned int bestMuon = 0;
      float maxPtCombMf  = 0.;
      float mindRRoadRoI = 999.;
      for(k=0; k<others.size(); k++) {
        j=others[k];

        float ptCombMf  = std::abs(matchSA_L2IOobjects.at(j)->pt()/1e3);

	const float roadPhiP = std::atan2(matchSA_L2IOobjects.at(j)->muSATrack()->dirPhiMS(),1.);
        const float roadPhiM = std::atan2(-1*matchSA_L2IOobjects.at(j)->muSATrack()->dirPhiMS(),-1.);
        const float roadPhi = (std::abs(xAOD::P4Helpers::deltaPhi(roadPhiP, matchSA_L2IOobjects.at(j)->muSATrack()->roiPhi()))
			       < std::abs(xAOD::P4Helpers::deltaPhi(roadPhiM, matchSA_L2IOobjects.at(j)->muSATrack()->roiPhi())))? roadPhiP : roadPhiM;
	float roadAw = 0.;
	if(std::abs(matchSA_L2IOobjects.at(j)->muSATrack()->roiEta()) < 1.05) { // barrel
	  if( std::abs(matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(1,0)) > ZERO_LIMIT )      roadAw = matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(1,0);
          else if( std::abs(matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(2,0)) > ZERO_LIMIT ) roadAw = matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(2,0);
          else if( std::abs(matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(0,0)) > ZERO_LIMIT ) roadAw = matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(0,0);
        }
	else { // endcap
	  if( std::abs(matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(4,0)) > ZERO_LIMIT )      roadAw = matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(4,0);
          else if( std::abs(matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(5,0)) > ZERO_LIMIT ) roadAw = matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(5,0);
          else if( std::abs(matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(3,0)) > ZERO_LIMIT ) roadAw = matchSA_L2IOobjects.at(j)->muSATrack()->roadAw(3,0);
        }
	float roadEta = 999.;
        if(std::abs(roadAw) > ZERO_LIMIT) roadEta = -std::log(std::tan(0.5*std::atan(std::abs(roadAw))));
        if(roadAw < 0) roadEta *= -1.;
        float detaRoadRoI = roadEta -  matchSA_L2IOobjects.at(j)->muSATrack()->roiEta();
        float dphiRoadRoI = xAOD::P4Helpers::deltaPhi(roadPhi, matchSA_L2IOobjects.at(j)->muSATrack()->roiPhi());
        float dRRoadRoI = std::sqrt(detaRoadRoI*detaRoadRoI + dphiRoadRoI*dphiRoadRoI);
	ATH_MSG_DEBUG("     j="<< j << " , ptCombMf=" << ptCombMf << ", dRRoadRoI=" << dRRoadRoI);


	if( (ptCombMf > maxPtCombMf) ||
            (std::abs(ptCombMf - maxPtCombMf) < ZERO_LIMIT &&
             dRRoadRoI < mindRRoadRoI) ) {
          maxPtCombMf  = ptCombMf;
          mindRRoadRoI = dRRoadRoI;
          bestMuon  = j;
        }
      }
      ATH_MSG_DEBUG( "      best is: bestMuon/maxPtCombMf=" << bestMuon << " / " <<  maxPtCombMf );

      for(k=0; k<others.size(); k++) {
        j=others[k];
        if( j != bestMuon ) {
          ATH_MSG_DEBUG( "      EventView( j=" << j << " ) is not active" );

          passOR.at(j) = false;
        }
        else{
          ATH_MSG_DEBUG( "      EventView( j=" << j << " ) is best one" );
        }
      }
    }
  }


  return StatusCode::SUCCESS;
}


bool L2MuonSAIOMon :: muCombHypo_TDTworkaround( const std::string &chain, const std::vector< const xAOD::L2CombinedMuon* >& Trig_L2IOobjects, std::vector< bool > &pass_muCombHypo ) const{

  bool pass_muCombHypo_evt = false;

  int requireMuonNum = 1;

  int passHypo_MuonNum = 0;
  for(auto &Trig_L2IOobject : Trig_L2IOobjects){
    bool pass_muCombHypo_obj = isPassedmuCombHypo( chain, Trig_L2IOobject );
    pass_muCombHypo.push_back(pass_muCombHypo_obj);
    if( pass_muCombHypo_obj ) passHypo_MuonNum++; 
  }

  if( passHypo_MuonNum >= requireMuonNum ){
    ATH_MSG_DEBUG("this evt passed muCombhypo");
    pass_muCombHypo_evt = true;
  }

  return pass_muCombHypo_evt;
}


bool L2MuonSAIOMon :: isPassedmuCombHypo( const std::string &chain, const xAOD::L2CombinedMuon* Trig_L2IOobject ) const{

  bool pass_muCombHypo = false;

  // config
  std::vector< float > my_EtaBins = {0, 1.05, 1.5, 2.0, 9.9};
  std::vector< float > my_muCombThres = {0., 0., 0., 0.};
  bool my_pikCuts = true;
  float my_maxPtToApplyPik = 25.;
  float my_chi2MaxID = 3.5;
  ATH_MSG_INFO( "decision_ptthreshold:   " << decision_ptthreshold( chain, my_EtaBins, my_muCombThres, my_pikCuts, my_maxPtToApplyPik, my_chi2MaxID ) );

  bool pikCut = true;
  bool stdCut = true;

  auto ptValue = Trig_L2IOobject->pt() * Trig_L2IOobject->charge()/1e3;
  float fexPt = ptValue;
  if(my_pikCuts && (std::abs(fexPt) < my_maxPtToApplyPik)){
    if(Trig_L2IOobject->idTrack()->chiSquared() > my_chi2MaxID){
      ATH_MSG_DEBUG("this obj failed at Kpi rejection:idTrack_chiSquared =   " << Trig_L2IOobject->idTrack()->chiSquared() );
      pikCut = false;
    }
  }

  float absEta = std::abs(Trig_L2IOobject->eta());
  unsigned int iThres = 0;
  for(unsigned int i=0; i<(my_EtaBins.size()-1); i++) {
    if ( my_EtaBins[i] <= absEta && absEta < my_EtaBins[i+1] ) iThres = i;
  }
  const float muCombThres = my_muCombThres[iThres];
  if(Trig_L2IOobject->pt()/1e3 < muCombThres){
    ATH_MSG_DEBUG("this obj failed at std Pt cut:muCombThres =   " << muCombThres);
    stdCut = false;
  }

  if(stdCut && pikCut){
    ATH_MSG_DEBUG("this obj passed muCombhypo");
    pass_muCombHypo = true;
  }

  return pass_muCombHypo;
}


StatusCode L2MuonSAIOMon :: decision_ptthreshold( const std::string &chain, std::vector< float > &my_EtaBins, std::vector< float > &my_muCombThres,
                                                    bool &my_pikCuts, float &my_maxPtToApplyPik, float &my_chi2MaxID ) const{

  my_maxPtToApplyPik = 25.;
  my_chi2MaxID = 3.5;
  if(chain == "HLT_mu4_l2io_L1MU3V"){
    my_EtaBins = {0, 1.05, 1.5, 2.0, 9.9}; //4GeV_v15a
    my_muCombThres = {3.86, 3.77, 3.69, 3.70}; //4GeV_v15a
    my_pikCuts = false;
  }else if(chain == "HLT_mu26_ivarmedium_mu6_l2io_probe_L1MU14FCH"){
    my_EtaBins = {0, 1.05, 1.5, 2.0, 9.9}; //6GeV_v15a
    my_muCombThres = {5.87, 5.79, 5.70, 5.62}; //6GeV_v15a
    my_pikCuts = false;
  }else{
    ATH_MSG_ERROR("muCombHypo config is NOT defined in this package:chain =   " << chain);
  }

  return StatusCode::SUCCESS;
}


std::tuple<float,float,float> L2MuonSAIOMon :: L2ORPosForMatchFunc(const xAOD::L2StandAloneMuon *trig){
  return std::forward_as_tuple(trig->pt(), trig->etaMS(), trig->phiMS());
}


std::tuple<float,float,float> L2MuonSAIOMon :: L2ORPosForMatchFunc(const xAOD::L2CombinedMuon *trig){
  return std::forward_as_tuple( (trig->pt()/1e3 * trig->charge() ), trig->eta(), trig->phi());
}


const xAOD::L2CombinedMuon* L2MuonSAIOMon :: searchL2InsideOut( const EventContext &ctx, const xAOD::Muon *mu, const std::string& trig) const {
  ATH_MSG_DEBUG("MuonMonitoring::searchL2InsideOut()");

  const xAOD::L2CombinedMuon* offlinematched_L2IOobject = nullptr;

  //TDT workaround
  std::vector< const xAOD::L2CombinedMuon* > Trig_L2IOobjects;
  if( !matchL2IO_wContainer(ctx, trig, Trig_L2IOobjects).isSuccess() ) {
    ATH_MSG_WARNING("matchL2IO_wContainer failed, returning nullptr");
    return offlinematched_L2IOobject;
  }
  if( Trig_L2IOobjects.empty() ) {
    return offlinematched_L2IOobject;
  }

  float reqdR = 1000.;

  double offlEta = mu->eta();
  double offlPhi = mu->phi();

  int loop_counter = 0;
  int match_index = 0;
  for(auto Trig_L2IOobject : Trig_L2IOobjects){
    double trigEta = Trig_L2IOobject->eta();
    double trigPhi = Trig_L2IOobject->phi();
    double deta = offlEta - trigEta;
    double dphi = xAOD::P4Helpers::deltaPhi(offlPhi, trigPhi);
    double dR = std::sqrt(deta*deta + dphi*dphi);

    ATH_MSG_VERBOSE("Trigger muon candidate eta=" << trigEta << " phi=" << trigPhi  << " pt=" << Trig_L2IOobject->pt() << " dR=" << dR);
    if( dR<reqdR ){
      reqdR = dR;
      match_index = loop_counter;
      ATH_MSG_DEBUG("* Trigger muon eta=" << trigEta << " phi=" << trigPhi  << " pt=" << Trig_L2IOobject->pt() << " dR=" << dR );
    }
    loop_counter++;
  }

  offlinematched_L2IOobject = Trig_L2IOobjects.at(match_index);
  return offlinematched_L2IOobject;
}


const xAOD::Muon* L2MuonSAIOMon :: searchTagOfflineMuon( const EventContext& ctx, const xAOD::Muon* probe ) const{
  ATH_MSG_DEBUG("MuonMonitoring::searchTagOfflineMuon()");

  const double ZERO_LIMIT = 0.00001;

  double Jpsimass = 3.0969;
  double Zmass = 91.1876;
  double my_Jpsimass_lowlim = 81.;
  double my_Jpsimass_highlim = 101.;
  double my_Zmass_lowlim = 2.7;
  double my_Zmass_highlim = 3.5;

  const xAOD::Muon *tag = nullptr;

  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
  if (! muons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return tag;
  }

  double mass_diff_min = 999.;
  double tpdR_min = 999.;
  bool tpfromZ = false;
  for( const xAOD::Muon* mu : *muons ){
    if( mu->muonType()>m_muontype ) continue;
    if( mu->quality() != xAOD::Muon::Medium && mu->quality() != xAOD::Muon::Tight ) continue;
    if( mu->charge()*probe->charge() > 0 ) continue;
    const ElementLink<xAOD::TrackParticleContainer> &tag_ms_track = mu->muonSpectrometerTrackParticleLink();
    if( !tag_ms_track.isValid() ) continue; // tag muon dosen't have ms track 
    TLorentzVector lvmu = mu->p4();
    TLorentzVector lvprobe = probe->p4();
    double dimu_mass = (lvmu+lvprobe).M()/1.e3;
    double tpdR = lvmu.DeltaR(lvprobe);
    if( dimu_mass > my_Jpsimass_lowlim && dimu_mass < my_Jpsimass_highlim ){
      if( tpfromZ ) continue; // Z has higher priority than Jpsi
      double mass_diff = std::abs(dimu_mass - Jpsimass);
      if( mass_diff - mass_diff_min < -1.*ZERO_LIMIT ){
	mass_diff_min = mass_diff;
	tpdR_min =tpdR;
	tag = mu;
      }
      else if( std::abs(mass_diff - mass_diff_min) < ZERO_LIMIT){
	if( tpdR - tpdR_min < 0. ){
	  mass_diff_min = mass_diff;
	  tpdR_min = tpdR;
	  tag = mu;
	}
      }
    }
    else if( dimu_mass > my_Zmass_lowlim && dimu_mass < my_Zmass_highlim ){
      tpfromZ = true;
      double mass_diff = std::abs(dimu_mass - Zmass);
      if( mass_diff - mass_diff_min < -1.*ZERO_LIMIT ){
	mass_diff_min = mass_diff;
        tpdR_min =tpdR;
	tag = mu;
      } 
      else if( std::abs(mass_diff - mass_diff_min) < ZERO_LIMIT){
        if( tpdR - tpdR_min < 0. ){
          mass_diff_min = mass_diff;
          tpdR_min =tpdR;
          tag = mu;
        }
      }
    }
  }

  return tag;
}
