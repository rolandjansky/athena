/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauMonitorAlgorithm.h"

TrigTauMonitorAlgorithm::TrigTauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}


TrigTauMonitorAlgorithm::~TrigTauMonitorAlgorithm() {}


StatusCode TrigTauMonitorAlgorithm::initialize() {

  ATH_CHECK( m_offlineTauJetKey.initialize() );
  ATH_CHECK( m_l1TauRoIKey.initialize() );
  ATH_CHECK( m_hltTauJetKey.initialize() );
  ATH_CHECK( m_hltTauJetPrecisionKey.initialize() );
  ATH_CHECK( m_hltTauJetPreselKey.initialize() );
  ATH_CHECK( m_hltTauJetCaloOnlyMVAKey.initialize() );
  ATH_CHECK( m_hltTauJetCaloOnlyKey.initialize() );
  ATH_CHECK( m_hltSeedJetKey.initialize());
  ATH_CHECK( m_trigDecTool.retrieve() );

  for(const auto& trigName:m_trigInputList)
  {
     if(getTrigInfoMap().count(trigName) != 0){
       ATH_MSG_WARNING("Trigger already booked, removing from trigger list " << trigName);
     }else {
       m_trigList.push_back(trigName);
       setTrigInfo(trigName);
     }
  }

  return AthMonitorAlgorithm::initialize();
}


StatusCode TrigTauMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  //using namespace Monitored;

  ATH_MSG_DEBUG("Executing TrigTauMonitorAlgorithm");

  if(m_trigDecTool->ExperimentalAndExpertMethods()->isHLTTruncated()){
    ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
    return StatusCode::SUCCESS; 
  }
  
  ATH_MSG_DEBUG("Chains for Analysis " << m_trigList);

  std::vector< std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision*>> pairObjs;

  for(const auto& trigger : m_trigList){

    const TrigInfo info = getTrigInfo(trigger);

    if ( executeNavigation( ctx, info.trigName,info.HLTthr,info.trigWP, pairObjs).isFailure() )                
    {                                                                                                                                                       
       ATH_MSG_WARNING("executeNavigation failed");                                                                                                       
       return StatusCode::SUCCESS;                                                                                                                         
    }  

    fillDistributions( pairObjs, trigger );                                                                                                           
  }

    return StatusCode::SUCCESS;
}


StatusCode TrigTauMonitorAlgorithm::executeNavigation( const EventContext& ctx, 
                                                                  const std::string trigItem,
                                                                  float etthr,
                                                                  const std::string pidname,
                                                                  std::vector<std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision* >> &pairObjs) const
{

  ATH_MSG_DEBUG("Apply navigation selection "); 

  SG::ReadHandle<xAOD::TauJetContainer> offTaus(m_offlineTauJetKey, ctx);

  const double thresholdOffset{10.0};

  if(!offTaus.isValid())
  {
     ATH_MSG_WARNING("Failed to retrieve offline Taus ");
     return StatusCode::FAILURE;
  }

  for(const auto& Tau : *offTaus ){

    const TrigCompositeUtils::Decision *dec=nullptr; 

    if( !( Tau->p4().Pt()  > (etthr-thresholdOffset)*1.e3) ) continue;

    if((pidname.find("mediumRNN") != std::string::npos) && !Tau->isTau(xAOD::TauJetParameters::JetRNNSigMedium)) continue;

    auto vec = m_trigDecTool->features<xAOD::TauJetContainer>(trigItem,TrigDefs::includeFailedDecisions ,"HLT_TrigTauRecMerged_MVA");      
    for( auto &featLinkInfo : vec ){                                             
      if(! featLinkInfo.isValid() ) continue;
      const auto *feat = *(featLinkInfo.link);                   
      if(!feat) continue;
      float deltaR = dR(Tau->eta(),Tau->phi(), feat->eta(),feat->phi());                                
      if(deltaR < 0.4){            
         dec = featLinkInfo.source;                                                
      }                     
    }


    std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * > pair(Tau,dec);
    pairObjs.push_back(pair);

  }

  ATH_MSG_DEBUG("BaseToolMT::Tau TEs " << pairObjs.size() << " found.");
  return StatusCode::SUCCESS;
}

void TrigTauMonitorAlgorithm::fillDistributions(std::vector< std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * >> pairObjs, std::string trigger) const
{
  ATH_MSG_DEBUG ("TrigTauMonitorAlgorithm::fillDistributions");

  std::vector<const xAOD::TauJet*> tau_vec_1p;
  std::vector<const xAOD::TauJet*> tau_vec_np;

  const TrigInfo info = getTrigInfo(trigger);

  // Offline
  std::vector<const xAOD::TauJet*> tau_vec;
  for( auto pairObj: pairObjs )
  {
    int nTracks=-1;
    pairObj.first->detail(xAOD::TauJetParameters::nChargedTracks, nTracks);
    if(nTracks==1){
       tau_vec_1p.push_back(pairObj.first);
    }else if(nTracks>1){
       ATH_MSG_DEBUG("NTracks Offline: " << nTracks);
       tau_vec_np.push_back(pairObj.first);
    }
  }
    
  // Offline
  if(info.isRNN){
    fillRNNInputVars( trigger, tau_vec_1p,"1P", false );
    fillRNNInputVars( trigger, tau_vec_np,"MP", false );
    fillRNNTrack( trigger, tau_vec_1p, false );
    fillRNNTrack( trigger, tau_vec_np, false );
    fillRNNCluster( trigger, tau_vec_1p, false );
    fillRNNCluster( trigger, tau_vec_np, false );
  }

  tau_vec_1p.clear();
  tau_vec_np.clear();

  auto vec =  m_trigDecTool->features<xAOD::TauJetContainer>(trigger,TrigDefs::includeFailedDecisions ,"HLT_TrigTauRecMerged_MVA" ); 
  for( auto &featLinkInfo : vec ){
    const auto *feat = *(featLinkInfo.link);
    if(!feat) continue;
    // If not pass, continue
    int nTracks=-1;
    feat->detail(xAOD::TauJetParameters::nChargedTracks, nTracks);
    if(nTracks==1){
      tau_vec_1p.push_back(feat);
    }else if(nTracks>1){
      ATH_MSG_DEBUG("NTracks Online: " << nTracks);
      tau_vec_np.push_back(feat);
    }
  }
  if(info.isRNN){ 
    fillRNNInputVars( trigger, tau_vec_1p,"1P", true );
    fillRNNInputVars( trigger, tau_vec_np,"MP", true );
    fillRNNTrack( trigger, tau_vec_1p, true );
    fillRNNTrack( trigger, tau_vec_np, true );
    fillRNNCluster( trigger, tau_vec_1p, true );
    fillRNNCluster( trigger, tau_vec_np, true );
  }
  
  
}

void TrigTauMonitorAlgorithm::fillRNNInputVars(const std::string trigger, std::vector<const xAOD::TauJet*> tau_vec,const std::string nProng, bool online) const
{
  ATH_MSG_DEBUG("Fill RNN input variables: " << trigger);

  auto monGroup = getGroup(trigger+( online ? "/RNN/HLT/InputScalar_"+nProng : "/RNN/Offline/InputScalar_"+nProng));  

  auto centFrac           = Monitored::Collection("centFrac", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::centFrac, detail)){
                                                      detail = std::min(detail, 1.0f);    
                                                    } return detail;});
  auto etOverPtLeadTrk    = Monitored::Collection("etOverPtLeadTrk", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, detail)){
                                                      detail = TMath::Log10(std::max(detail, 0.1f));
                                                    } return detail;});
  auto dRmax              = Monitored::Collection("dRmax", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::dRmax, detail)){
                                                    } return detail;});
  auto absipSigLeadTrk    = Monitored::Collection("absipSigLeadTrk", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::ipSigLeadTrk, detail)){
                                                      detail = std::min(TMath::Abs(detail), 30.0f);
                                                    } return detail;});
  auto sumPtTrkFrac       = Monitored::Collection("sumPtTrkFrac", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::SumPtTrkFrac, detail)){
                                                    } return detail;});
  auto emPOverTrkSysP     = Monitored::Collection("emPOverTrkSysP", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, detail)){
                                                      detail = TMath::Log10(std::max(detail, 1e-3f));
                                                    } return detail;});
  auto ptRatioEflowApprox = Monitored::Collection("ptRatioEflowApprox", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, detail)){
                                                      detail = std::min(detail, 4.0f);
                                                    } return detail;});
  auto mEflowApprox       = Monitored::Collection("mEflowApprox", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    float detail = -999;
                                                    if (tau->detail(xAOD::TauJetParameters::mEflowApprox, detail)){
                                                      detail = TMath::Log10(std::max(detail, 140.0f));
                                                    }return detail;});
  auto ptDetectorAxis     = Monitored::Collection("ptDetectorAxis", tau_vec,  [] (const xAOD::TauJet* tau){
                                                    return TMath::Log10(std::min(tau->ptDetectorAxis() / 1000.0, 100.0));});
  auto massTrkSys         = Monitored::Collection("massTrkSys", tau_vec,  [&nProng] (const xAOD::TauJet* tau){
                                                float detail = -999;
                                                if ( tau->detail(xAOD::TauJetParameters::massTrkSys, detail) && nProng.find("MP") != std::string::npos ){
                                                  detail = TMath::Log10(std::max(detail, 140.0f));
                                                }return detail;});

    
  fill(monGroup, centFrac,etOverPtLeadTrk,dRmax,absipSigLeadTrk,sumPtTrkFrac,emPOverTrkSysP,ptRatioEflowApprox,mEflowApprox,ptDetectorAxis,massTrkSys);     
  
}

void TrigTauMonitorAlgorithm::fillRNNTrack(const std::string trigger, std::vector<const xAOD::TauJet*> tau_vec, bool online) const
{
  ATH_MSG_DEBUG("Fill RNN input Track: " << trigger);
  

  auto monGroup = getGroup(trigger+( online ? "/RNN/HLT/InputTrack" : "/RNN/Offline/InputTrack"));
  
    for(auto tau: tau_vec){
      auto tracks = tau->allTracks();
 
      auto track_pt_jetseed_log           = Monitored::Collection("track_pt_jetseed_log", tau_vec,  [] (const xAOD::TauJet* tau){ return TMath::Log10( tau->ptJetSeed());});
      fill(monGroup,track_pt_jetseed_log);
  
                                 
      auto cmp_pt = [](const xAOD::TauTrack *lhs, const xAOD::TauTrack *rhs) {
        return lhs->pt() > rhs->pt();
      };
      std::sort(tracks.begin(), tracks.end(), cmp_pt);
                                                                                                                                       
      unsigned int max_tracks = 10;
      if (tracks.size() > max_tracks) {
        tracks.resize(max_tracks);
      }


      auto track_pt_log = Monitored::Collection("track_pt_log", tracks, [](const xAOD::TauTrack *track){return TMath::Log10( track->pt()); }); 
  
      auto track_dEta = Monitored::Collection("tracks_dEta", tracks, [&tau](const xAOD::TauTrack *track){auto ddeta=track->eta()- tau->eta();return ddeta; });

      auto track_dPhi = Monitored::Collection("tracks_dPhi", tracks, [&tau](const xAOD::TauTrack *track){return track->p4().DeltaPhi(tau->p4()); }); 

      auto track_z0sinThetaTJVA_abs_log = Monitored::Collection("tracks_z0sinThetaTJVA_abs_log", tracks, [&tau](const xAOD::TauTrack *track){return track->z0sinThetaTJVA(*tau); }); 

      auto track_d0_abs_log = Monitored::Collection("tracks_d0_abs_log", tracks, [](const xAOD::TauTrack *track){return  TMath::Log10( TMath::Abs(track->track()->d0()) + 1e-6); }); 

      auto track_nIBLHitsAndExp = Monitored::Collection("tracks_nIBLHitsAndExp", tracks, [](const xAOD::TauTrack *track){
                                                    uint8_t inner_pixel_hits, inner_pixel_exp;
                                                    const auto success1_innerPixel_hits = track->track()->summaryValue(inner_pixel_hits, xAOD::numberOfInnermostPixelLayerHits);
                                                    const auto success2_innerPixel_exp = track->track()->summaryValue(inner_pixel_exp, xAOD::expectInnermostPixelLayerHit);
                                                    float detail = -999;
                                                    if (success1_innerPixel_hits && success2_innerPixel_exp) {detail=inner_pixel_exp ? inner_pixel_hits : 1.;};
                                                    return   detail; });

      auto track_nPixelHitsPlusDeadSensors = Monitored::Collection("tracks_nPixelHitsPlusDeadSensors", tracks, [](const xAOD::TauTrack *track){
                                                    uint8_t pixel_hits, pixel_dead;
                                                    const auto success1_pixel_hits = track->track()->summaryValue(pixel_hits, xAOD::numberOfPixelHits);
                                                    const auto success2_pixel_dead = track->track()->summaryValue(pixel_dead, xAOD::numberOfPixelDeadSensors);
                                                    float detail = -999;
                                                    if (success1_pixel_hits && success2_pixel_dead) {detail=pixel_hits + pixel_dead;};
                                                    return   detail; });

      auto track_nSCTHitsPlusDeadSensors = Monitored::Collection("tracks_nSCTHitsPlusDeadSensors", tracks, [](const xAOD::TauTrack *track){
                                                    uint8_t sct_hits, sct_dead;
                                                    const auto success1_sct_hits = track->track()->summaryValue(sct_hits, xAOD::numberOfSCTHits);
                                                    const auto success2_sct_dead = track->track()->summaryValue(sct_dead, xAOD::numberOfSCTDeadSensors);
                                                    float detail = -999;
                                                    if (success1_sct_hits && success2_sct_dead) {detail=sct_hits + sct_dead;};
                                                    return   detail; });
                                                  

      fill(monGroup,track_pt_log,track_dEta,track_dPhi,track_z0sinThetaTJVA_abs_log,track_d0_abs_log,track_nIBLHitsAndExp,track_nPixelHitsPlusDeadSensors,track_nSCTHitsPlusDeadSensors);
    }
  
}

void TrigTauMonitorAlgorithm::fillRNNCluster(const std::string trigger, std::vector<const xAOD::TauJet*> tau_vec, bool online) const
{
  ATH_MSG_DEBUG("Fill RNN input Cluster: " << trigger);
  
  auto monGroup = getGroup(trigger+( online ? "/RNN/HLT/InputCluster" : "/RNN/Offline/InputCluster"));
  
  for(auto tau: tau_vec){

    auto cluster_pt_jetseed_log = Monitored::Collection("cluster_pt_jetseed_log", tau_vec,  [] (const xAOD::TauJet* tau){ return TMath::Log10( tau->ptJetSeed());});

    std::vector<const xAOD::CaloCluster *> clusters;

    float max_cluster_dr = 1.0;
    
    if(!tau->jetLink().isValid()) {
      continue;
    }

    const xAOD::Jet *jetSeed = tau->jet();
    if (!jetSeed) {
      ATH_MSG_ERROR("Tau jet link is invalid.");
    } 

    if(!jetSeed->getConstituents().isValid()) {
      continue;
    }

    ATH_MSG_DEBUG("Link to constituents is valid");

    for (const auto jc : jetSeed->getConstituents()) {
      auto cl = dynamic_cast<const xAOD::CaloCluster *>(jc->rawConstituent());
      if (!cl) {
	ATH_MSG_ERROR("Calorimeter cluster is invalid.");
      }

      const auto lc_p4 = tau->p4(xAOD::TauJetParameters::DetectorAxis);
      if (lc_p4.DeltaR(cl->p4()) < max_cluster_dr) {
	clusters.push_back(cl);
      }
    }
  
    auto et_cmp = [](const xAOD::CaloCluster *lhs,
		     const xAOD::CaloCluster *rhs) {
      return lhs->et() > rhs->et();
    };
    std::sort(clusters.begin(), clusters.end(), et_cmp);

    unsigned int max_clusters = 6;
    if (clusters.size() > max_clusters) {
      clusters.resize(max_clusters);
    }


    auto cluster_et_log = Monitored::Collection("cluster_et_log",clusters, [](const xAOD::CaloCluster *cluster){return TMath::Log10( cluster->et()); }); 
    auto cluster_dEta = Monitored::Collection("cluster_dEta", clusters, [&tau](const xAOD::CaloCluster *cluster){auto ddeta=cluster->eta()- tau->eta();return ddeta; });
    auto cluster_dPhi = Monitored::Collection("cluster_dPhi", clusters, [&tau](const xAOD::CaloCluster *cluster){return cluster->p4().DeltaPhi(tau->p4()); }); 
    auto cluster_log_SECOND_R = Monitored::Collection("cluster_log_SECOND_R", clusters, [](const xAOD::CaloCluster *cluster){
                                              double detail = -999.;
                                              const auto success_SECOND_R = cluster->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_R,detail);
                                              if (success_SECOND_R) detail = TMath::Log10(detail + 0.1);
                                              return detail;});

      auto cluster_SECOND_LAMBDA = Monitored::Collection("cluster_SECOND_LAMBDA", clusters, [](const xAOD::CaloCluster *cluster){
                                              double detail = -999.;
                                              const auto success_SECOND_LAMBDA = cluster->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_LAMBDA, detail);
                                              if (success_SECOND_LAMBDA) detail = TMath::Log10(detail + 0.1); 
                                              return detail;});

      auto cluster_CENTER_LAMBDA = Monitored::Collection("cluster_CENTER_LAMBDA", clusters, [](const xAOD::CaloCluster *cluster){
                                              double detail = -999.;
                                              const auto success_CENTER_LAMBDA = cluster->retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_LAMBDA, detail);
                                              if (success_CENTER_LAMBDA) detail = TMath::Log10(detail + 1e-6); 
                                              return detail;});                                                  

      fill(monGroup,cluster_pt_jetseed_log,cluster_et_log,cluster_dEta,cluster_dPhi,cluster_log_SECOND_R,cluster_SECOND_LAMBDA,cluster_CENTER_LAMBDA);
    }

}

TrigInfo TrigTauMonitorAlgorithm::getTrigInfo(const std::string trigger) const{ 
  return m_trigInfo.at(trigger); 
}


void TrigTauMonitorAlgorithm::setTrigInfo(const std::string trigger)
{ 

  std::string idwp="",type="",l1item="",l1type="";
  float hlthr=0.,l1thr=0.;
  bool isRNN=false,isBDT=false,isPerf=false,isL1=false;

  size_t l=trigger.length();
  size_t pos=trigger.find("_");
  std::string substr =trigger.substr(pos+1,l);
  std::vector<std::string> names;
  names.push_back(trigger.substr(0,pos));

  while(substr.find("_")!=std::string::npos)
  {
    pos = substr.find("_");
    names.push_back(substr.substr(0,pos));
    substr = substr.substr(pos+1,substr.length());    
  }

  names.push_back(substr);

  hlthr = std::stof(names[1].substr(3,names[1].length()));
  idwp=names[2];

  if(idwp=="perf" || idwp=="idperf") isPerf=true;
  else if(idwp.find("RNN")!=std::string::npos) isRNN=true;
  else isBDT=true;

  type=names[4];
  if(names[0].find("L1")!=std::string::npos) isL1=true;

  TrigInfo info{trigger,idwp,l1item,l1type,type,isL1,isRNN,isBDT,isPerf,hlthr,l1thr,false};

  m_trigInfo[trigger] = info;
}

