/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>



#include "TrigTauMonitorAlgorithm.h"

TrigTauMonitorAlgorithm::TrigTauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}


TrigTauMonitorAlgorithm::~TrigTauMonitorAlgorithm() {}


StatusCode TrigTauMonitorAlgorithm::initialize() {

  ATH_CHECK( AthMonitorAlgorithm::initialize() );
  ATH_CHECK( m_offlineTauJetKey.initialize() );
  ATH_CHECK( m_legacyl1TauRoIKey.initialize() );
  ATH_CHECK( m_phase1l1TauRoIKey.initialize() );
  ATH_CHECK( m_hltTauJetKey.initialize() );
  ATH_CHECK( m_hltTauJetCaloMVAOnlyKey.initialize() );
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

  return StatusCode::SUCCESS;
}


StatusCode TrigTauMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  //using namespace Monitored;

  ATH_MSG_DEBUG("Executing TrigTauMonitorAlgorithm");

  if(m_trigDecTool->ExperimentalAndExpertMethods().isHLTTruncated()){
    ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
    return StatusCode::SUCCESS; 
  }
  
  ATH_MSG_DEBUG("Chains for Analysis " << m_trigList);

  std::vector< std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision*>> pairObjs;
 
  std::vector<std::string> L1seed_list;
  bool fill_l1 = false;

  for(const auto& trigger : m_trigList){

    const TrigInfo info = getTrigInfo(trigger);

    if ( executeNavigation( ctx, info.trigName,pairObjs).isFailure() || pairObjs.empty())                
    {            
       ATH_MSG_WARNING("executeNavigation failed");                                                                                                       
       return StatusCode::SUCCESS;                                                                                                                         
    }  

    if(info.trigL1Item.empty()){ // L1 item not found
       fill_l1 = false;
    }
    else if (std::find(L1seed_list.begin(), L1seed_list.end(), info.trigL1Item) == L1seed_list.end())
    {
        // check if L1 seed has been already filled -> L1 seed must be filled only once for triggers with same L1 seed
        fill_l1 = true;
        L1seed_list.push_back(info.trigL1Item);
        
    } else {
        fill_l1 = false;
    }
 
    fillDistributions( ctx, pairObjs, trigger, info.HLTthr); 
    if(fill_l1){
      fillL1Distributions( ctx, pairObjs, trigger ,info.trigL1Item, info.L1thr);  
    }   
    pairObjs.clear();
                                                            
  }
   
  L1seed_list.clear();
  
  return StatusCode::SUCCESS;
}


StatusCode TrigTauMonitorAlgorithm::executeNavigation( const EventContext& ctx, 
                                                       const std::string& trigItem,
                                                       std::vector<std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision* >> &pairObjs) const
{

  ATH_MSG_DEBUG("Apply navigation selection "); 

  SG::ReadHandle<xAOD::TauJetContainer> offTaus(m_offlineTauJetKey, ctx);

  if(!offTaus.isValid())
  {
     ATH_MSG_WARNING("Failed to retrieve offline Taus ");
     return StatusCode::FAILURE;
  }

  std::string tauContainerName = "HLT_TrigTauRecMerged_MVA";
  if(trigItem.find("LLP_") != std::string::npos){
     tauContainerName="HLT_TrigTauRecMerged_LLP";
  }else if(trigItem.find("ptonly") != std::string::npos) 
     tauContainerName="HLT_TrigTauRecMerged_CaloMVAOnly";

  auto vec =  m_trigDecTool->features<xAOD::TauJetContainer>(trigItem,TrigDefs::Physics , tauContainerName );

  for(const auto *const Tau : *offTaus ){

    const TrigCompositeUtils::Decision *dec=nullptr; 

    // consider only offline taus which pass RNN medium WP 
    if( !Tau->isTau(xAOD::TauJetParameters::JetRNNSigMedium)) continue;

    // consider only offline taus which pass thinning 
    if( Tau->isAvailable<char>("passThinning") && !Tau->auxdata<char>("passThinning") ) continue;

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

void TrigTauMonitorAlgorithm::fillDistributions(const EventContext& ctx, const std::vector< std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * >>& pairObjs, const std::string& trigger, float HLTthr) const
{
  ATH_MSG_DEBUG ("TrigTauMonitorAlgorithm::fillDistributions");

  const double thresholdOffset{10.0};

  std::vector<const xAOD::TauJet*> offline_for_hlt_tau_vec_1p; // offline 1p taus used for studying HLT performance
  std::vector<const xAOD::TauJet*> offline_for_hlt_tau_vec_mp; // offline mp taus used for studying HLT performance
  std::vector<const xAOD::TauJet*> online_tau_vec_0p; // online 0p taus used for studying HLT performance
  std::vector<const xAOD::TauJet*> online_tau_vec_1p; // online 1p taus used for studying HLT performance
  std::vector<const xAOD::TauJet*> online_tau_vec_mp; // online mp taus used for studying HLT performance
  std::vector<const xAOD::TauJet*> online_tau_vec_all; // online hlt taus used for HLT efficiency studies

  const TrigInfo info = getTrigInfo(trigger);

  // Offline
  std::vector<const xAOD::TauJet*> tau_vec;
  for( auto pairObj: pairObjs )
  {
    int nTracks=-1;
    pairObj.first->detail(xAOD::TauJetParameters::nChargedTracks, nTracks);
    ATH_MSG_DEBUG("NTracks Offline: " << nTracks);
    // filling vectors for studying HLT performance
    if(nTracks==1 && ( pairObj.first->pt() > (HLTthr-thresholdOffset)*1.e3)){
       offline_for_hlt_tau_vec_1p.push_back(pairObj.first);
    }else if(nTracks>1 && ( pairObj.first->pt() > (HLTthr-thresholdOffset)*1.e3)){
       offline_for_hlt_tau_vec_mp.push_back(pairObj.first);
    }
  }
  // Offline
  if( !offline_for_hlt_tau_vec_1p.empty()){
    fillRNNInputVars( trigger, offline_for_hlt_tau_vec_1p,"1P", false );
    fillRNNTrack( trigger, offline_for_hlt_tau_vec_1p, false );
    fillRNNCluster( trigger, offline_for_hlt_tau_vec_1p, false );
    fillbasicVars( trigger, offline_for_hlt_tau_vec_1p, "1P", false);
  }

  if( !offline_for_hlt_tau_vec_mp.empty()){ 
    fillRNNInputVars( trigger, offline_for_hlt_tau_vec_mp,"MP", false );
    fillRNNTrack( trigger, offline_for_hlt_tau_vec_mp, false );
    fillRNNCluster( trigger, offline_for_hlt_tau_vec_mp, false );
    fillbasicVars( trigger, offline_for_hlt_tau_vec_mp, "MP", false);
  }

  std::string tauContainerName = "HLT_TrigTauRecMerged_MVA";
  if(trigger.find("LLP_") != std::string::npos){
     tauContainerName="HLT_TrigTauRecMerged_LLP";
  }else if(trigger.find("ptonly") != std::string::npos) 
     tauContainerName="HLT_TrigTauRecMerged_CaloMVAOnly";

  ATH_MSG_DEBUG("Tau ContainerName is: " << tauContainerName);

  auto vec =  m_trigDecTool->features<xAOD::TauJetContainer>(trigger,TrigDefs::Physics , tauContainerName );
  for( auto &featLinkInfo : vec ){
    const auto *feat = *(featLinkInfo.link);
    if(!feat) continue;
    // If not pass, continue
    int nTracks=-1;
    feat->detail(xAOD::TauJetParameters::nChargedTracks, nTracks);
    ATH_MSG_DEBUG("NTracks Online: " << nTracks);
    online_tau_vec_all.push_back(feat);
    if(nTracks==0){
      online_tau_vec_0p.push_back(feat);
    } else if(nTracks==1){
      online_tau_vec_1p.push_back(feat);
    }else{
      online_tau_vec_mp.push_back(feat);
    }
  }

  // fill information for online 0 prong taus
  if(!online_tau_vec_0p.empty()){
     fillbasicVars( trigger, online_tau_vec_0p, "0P", true);
     fillRNNInputVars( trigger, online_tau_vec_0p,"0P", true );
     fillRNNTrack( trigger, online_tau_vec_0p, true );
     fillRNNCluster( trigger, online_tau_vec_0p, true );
  }

  // fill information for online 1 prong taus 
  if(!online_tau_vec_1p.empty()){
     fillbasicVars( trigger, online_tau_vec_1p, "1P", true);
     fillRNNInputVars( trigger, online_tau_vec_1p,"1P", true );
     fillRNNTrack( trigger, online_tau_vec_1p, true );
     fillRNNCluster( trigger, online_tau_vec_1p, true );
  }          
 
  // fill information for online multiprong prong taus 
  if(!online_tau_vec_mp.empty()){
     fillbasicVars( trigger, online_tau_vec_mp, "MP", true);
     fillRNNInputVars( trigger, online_tau_vec_mp,"MP", true );
     fillRNNTrack( trigger, online_tau_vec_mp, true );
     fillRNNCluster( trigger, online_tau_vec_mp, true );
  }

  fillHLTEfficiencies(ctx, trigger, offline_for_hlt_tau_vec_1p, online_tau_vec_all, "1P");
  fillHLTEfficiencies(ctx, trigger, offline_for_hlt_tau_vec_mp, online_tau_vec_all, "MP");

  offline_for_hlt_tau_vec_1p.clear();
  offline_for_hlt_tau_vec_mp.clear();
  online_tau_vec_0p.clear();
  online_tau_vec_1p.clear();
  online_tau_vec_mp.clear();
  online_tau_vec_all.clear();
}

void TrigTauMonitorAlgorithm::fillL1Distributions(const EventContext& ctx, const std::vector< std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * >>& pairObjs, const std::string& trigger,  const std::string& trigL1Item, float L1thr) const
{
    ATH_MSG_DEBUG ("TrigTauMonitorAlgorithm::fillL1Distributions");

    const double thresholdOffset{10.0};
    const TrigInfo info = getTrigInfo(trigger);
    std::vector<const xAOD::TauJet*> tau_vec; //  offline taus
    std::vector<const xAOD::TauJet*> offline_for_l1_tau_vec_1p; // offline 1p taus
    std::vector<const xAOD::TauJet*> offline_for_l1_tau_vec_mp; // offline mp taus
    std::vector<const xAOD::EmTauRoI*> legacyL1rois; //  used for studying legacy L1 performance
    std::vector<const xAOD::eFexTauRoI*> phase1L1rois; // used for studying phase1 L1 performance 

    for( auto pairObj: pairObjs )
    {
      int nTracks=-1;
      pairObj.first->detail(xAOD::TauJetParameters::nChargedTracks, nTracks);
      ATH_MSG_DEBUG("NTracks Offline: " << nTracks);
      // filling vectors 1p and mp
      if(nTracks==1 && ( pairObj.first->pt() > (L1thr-thresholdOffset)*1.e3)){
        offline_for_l1_tau_vec_1p.push_back(pairObj.first);
      }else if(nTracks>1 && ( pairObj.first->pt() > (L1thr-thresholdOffset)*1.e3)){
        offline_for_l1_tau_vec_mp.push_back(pairObj.first); 
      }
    }

  
    if(trigL1Item.find("L1eTAU") != std::string::npos){

      SG::ReadHandle<xAOD::eFexTauRoIContainer> eFexTauRoIs(m_phase1l1TauRoIKey, ctx);
      if(!eFexTauRoIs.isValid())
      {
          ATH_MSG_WARNING("Failed to retrieve offline eFexTauRoI ");
          return;
      }

      for(const auto *eFexTauRoI : *eFexTauRoIs){
      
          if( eFexTauRoI->et()/1e3 > L1thr){
              phase1L1rois.push_back(eFexTauRoI);
          }
      }
     
    }
    else{
    
      SG::ReadHandle<xAOD::EmTauRoIContainer> EmTauRoIs(m_legacyl1TauRoIKey, ctx);
      if(!EmTauRoIs.isValid())
      {
          ATH_MSG_WARNING("Failed to retrieve EmTauRoI ");
          return;
      }

      for(const auto *EmTauRoI : *EmTauRoIs)
      {
          // check which threshold has passed based on the current L1 item under monitoring
          // reference : https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TriggerCommon/TriggerMenuMT/python/L1/Config/ItemDef.py
          bool found_thrName= false;
          for(auto item : EmTauRoI->thrNames()){
            if( trigL1Item.find("L1TAU8") != std::string::npos && item.find("HA8") != std::string::npos)
            {
               found_thrName = true;
               break;
            }
            else if( trigL1Item.find("L1TAU12IM") != std::string::npos && item.find("HA12IM") != std::string::npos)
            {
               found_thrName = true;
               break;
            } 
            else if( trigL1Item.find("L1TAU20IM") != std::string::npos && item.find("HA20IM") != std::string::npos)
            {
               found_thrName = true;
               break;
            }
            else if ( trigL1Item.find("L1TAU60") != std::string::npos && item.find("HA60") != std::string::npos)
            {
               found_thrName = true;
               break;
            }
            else if ( trigL1Item.find("L1TAU100") != std::string::npos && item.find("HA100") != std::string::npos)
            {
               found_thrName = true;
               break;
            }
          }  

          if(found_thrName) legacyL1rois.push_back(EmTauRoI); 
      }

    }    

    fillL1Efficiencies(ctx, offline_for_l1_tau_vec_1p, "1P", trigL1Item, legacyL1rois, phase1L1rois);
    fillL1Efficiencies(ctx, offline_for_l1_tau_vec_mp, "MP", trigL1Item, legacyL1rois, phase1L1rois);

    offline_for_l1_tau_vec_1p.clear();
    offline_for_l1_tau_vec_mp.clear();
    legacyL1rois.clear();
    phase1L1rois.clear();
}

void TrigTauMonitorAlgorithm::fillHLTEfficiencies(const EventContext& ctx, const std::string& trigger, const std::vector<const xAOD::TauJet*>& offline_tau_vec, const std::vector<const xAOD::TauJet*>& online_tau_vec, const std::string& nProng) const
{
  ATH_MSG_DEBUG("Fill HLT efficiencies: " << trigger);

  std::string monGroupName = trigger+"_HLT_Efficiency_"+nProng;

  auto monGroup = getGroup(monGroupName);

  auto tauPt = Monitored::Scalar<float>(monGroupName+"_tauPt",0.0);
  auto tauEta = Monitored::Scalar<float>(monGroupName+"_tauEta",0.0);
  auto tauPhi = Monitored::Scalar<float>(monGroupName+"_tauPhi",0.0);
  auto averageMu = Monitored::Scalar<float>(monGroupName+"_averageMu",0.0); 
  auto HLT_match = Monitored::Scalar<bool>(monGroupName+"_HLTpass",false);
 
  bool hlt_fires = m_trigDecTool->isPassed(trigger, TrigDefs::Physics | TrigDefs::allowResurrectedDecision);

  for(const auto *offline_tau : offline_tau_vec){

       tauPt = offline_tau->pt()/1e3;
       tauEta = offline_tau->eta();
       tauPhi = offline_tau->phi();
       averageMu = lbAverageInteractionsPerCrossing(ctx);
       // HLT matching  : dR matching + HLT fires
       HLT_match = HLTMatching(offline_tau, online_tau_vec, 0.2) && hlt_fires;

       fill(monGroup, tauPt, tauEta, tauPhi, averageMu, HLT_match);
  }

  ATH_MSG_DEBUG("After fill HLT efficiencies: " << trigger);

}

void TrigTauMonitorAlgorithm::fillL1Efficiencies( const EventContext& ctx , const std::vector<const xAOD::TauJet*>& offline_tau_vec, const std::string& nProng, const std::string& trigL1Item, const std::vector<const xAOD::EmTauRoI*>& legacyL1rois, const std::vector<const xAOD::eFexTauRoI*>& phase1L1rois) const
{
  ATH_MSG_DEBUG("Fill L1 efficiencies: " << trigL1Item);
  std::string monGroupName = trigL1Item+"_L1_Efficiency_"+nProng;

  auto monGroup = getGroup(monGroupName);

  auto tauPt = Monitored::Scalar<float>(monGroupName+"_tauPt",0.0);
  auto tauEta = Monitored::Scalar<float>(monGroupName+"_tauEta",0.0);
  auto tauPhi = Monitored::Scalar<float>(monGroupName+"_tauPhi",0.0);
  auto averageMu = Monitored::Scalar<float>(monGroupName+"_averageMu",0.0);
  auto L1_match = Monitored::Scalar<bool>(monGroupName+"_L1pass",false);

  for(const auto *offline_tau : offline_tau_vec){
       tauPt = offline_tau->pt()/1e3;
       tauEta = offline_tau->eta();
       tauPhi = offline_tau->phi();
       averageMu = lbAverageInteractionsPerCrossing(ctx);
  
       L1_match = false;

       if(trigL1Item.find("L1eTAU") != std::string::npos){
          for( const auto *L1roi : phase1L1rois){
             L1_match = phase1L1Matching(offline_tau, L1roi, 0.3 );
             if( L1_match ){
                break;
              }
          }
       } else {
          for( const auto *L1roi : legacyL1rois){
             L1_match = legacyL1Matching(offline_tau, L1roi, 0.3 );
             if( L1_match ){
                break;
              }
           }
       }
    
       fill(monGroup, tauPt, tauEta, tauPhi, averageMu, L1_match);
  }

  fillL1(trigL1Item, legacyL1rois, phase1L1rois, nProng);
} 

void TrigTauMonitorAlgorithm::fillL1(const std::string& trigL1Item, const std::vector<const xAOD::EmTauRoI*>& legacyL1rois, const std::vector<const xAOD::eFexTauRoI*>& phase1L1rois, const std::string& nProng)  const
{
   ATH_MSG_DEBUG("Fill L1: " << trigL1Item);

   std::string monGroupName = trigL1Item+"_L1_"+nProng;
    
   auto monGroup = getGroup(monGroupName);

   if(trigL1Item.find("L1eTAU") != std::string::npos){

       auto L1RoIEt           = Monitored::Collection("L1RoIEt"     , phase1L1rois,  [] (const xAOD::eFexTauRoI* L1roi){ return L1roi->et()/1e3;});
       auto L1RoIEta          = Monitored::Collection("L1RoIEta"    , phase1L1rois,  [] (const xAOD::eFexTauRoI* L1roi){ return L1roi->eta();});
       auto L1RoIPhi          = Monitored::Collection("L1RoIPhi"    , phase1L1rois,  [] (const xAOD::eFexTauRoI* L1roi){ return L1roi->phi();});
   
       fill(monGroup,L1RoIEt,L1RoIEta,L1RoIPhi);
   } else {

       auto L1RoIEt           = Monitored::Collection("L1RoIEt"     , legacyL1rois,  [] (const xAOD::EmTauRoI* L1roi){ return L1roi->eT()/1e3;});
       auto L1RoIEta          = Monitored::Collection("L1RoIEta"    , legacyL1rois,  [] (const xAOD::EmTauRoI* L1roi){ return L1roi->eta();});
       auto L1RoIPhi          = Monitored::Collection("L1RoIPhi"    , legacyL1rois,  [] (const xAOD::EmTauRoI* L1roi){ return L1roi->phi();});
       auto L1RoITauClus      = Monitored::Collection("L1RoITauClus", legacyL1rois,  [] (const xAOD::EmTauRoI* L1roi){ return L1roi->tauClus()/1e3;});
       auto L1RoIEMIsol       = Monitored::Collection("L1RoIEMIsol" , legacyL1rois,  [] (const xAOD::EmTauRoI* L1roi){ return L1roi->emIsol()/1e3;});
       auto L1RoIHadCore      = Monitored::Collection("L1RoIHadCore", legacyL1rois,  [] (const xAOD::EmTauRoI* L1roi){ return L1roi->hadCore()/1e3;});
       auto L1RoIHadIsol      = Monitored::Collection("L1RoIHadIsol", legacyL1rois,  [] (const xAOD::EmTauRoI* L1roi){ return L1roi->hadIsol()/1e3;});

       fill(monGroup,L1RoIEt,L1RoIEta,L1RoIPhi,L1RoITauClus,L1RoIEMIsol,L1RoIHadCore,L1RoIHadIsol);
   }

   ATH_MSG_DEBUG("AFTER L1: " << trigL1Item);

}

void TrigTauMonitorAlgorithm::fillRNNInputVars(const std::string& trigger, const std::vector<const xAOD::TauJet*>& tau_vec,const std::string& nProng, bool online) const
{
  ATH_MSG_DEBUG("Fill RNN input variables: " << trigger);

  auto monGroup = getGroup(trigger+( online ? "_RNN_HLT_InputScalar_"+nProng : "_RNN_Offline_InputScalar_"+nProng));  

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
                                                        float detail = (tau->nTracks()>0) ? std::abs(tau->track(0)->d0SigTJVA()) : 0.;
                                                        detail = std::min(TMath::Abs(detail), 30.0f);
                                                        return detail;});
   
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

  ATH_MSG_DEBUG("After fill RNN input variables: " << trigger);
  
}

void TrigTauMonitorAlgorithm::fillRNNTrack(const std::string& trigger, const std::vector<const xAOD::TauJet*>& tau_vec, bool online) const
{
  ATH_MSG_DEBUG("Fill RNN input Track: " << trigger);
  

  auto monGroup = getGroup(trigger+( online ? "_RNN_HLT_InputTrack" : "_RNN_Offline_InputTrack"));

  auto track_pt_jetseed_log           = Monitored::Collection("track_pt_jetseed_log", tau_vec,  [] (const xAOD::TauJet* tau){ return TMath::Log10( tau->ptJetSeed());});
  fill(monGroup,track_pt_jetseed_log);
  
    for(const auto *tau: tau_vec){
      // Don't call ->allTracks() unless the element links are valid
      static const SG::AuxElement::ConstAccessor< std::vector<ElementLink<xAOD::TauTrackContainer>> > tauTrackAcc("tauTrackLinks");
      bool linksValid = true;
      for (const ElementLink<xAOD::TauTrackContainer>& trackEL : tauTrackAcc(*tau)) {
        if (!trackEL.isValid()) {
          linksValid = false;
        }
        break;
      }
      if (!linksValid) {
      	ATH_MSG_WARNING("Invalid track element links from TauJet in " << trigger);
      	continue;
      }

      auto tracks = tau->allTracks();
  
                                 
      auto cmp_pt = [](const xAOD::TauTrack *lhs, const xAOD::TauTrack *rhs) {
        return lhs->pt() > rhs->pt();
      };
      std::sort(tracks.begin(), tracks.end(), cmp_pt);
                              
      /*                                                                                                         
      unsigned int max_tracks = 10;
      if (tracks.size() > max_tracks) {
        tracks.resize(max_tracks);
      }
      */

      auto track_pt_log = Monitored::Collection("track_pt_log", tracks, [](const xAOD::TauTrack *track){return TMath::Log10( track->pt()); }); 
  
      auto track_dEta = Monitored::Collection("track_dEta", tracks, [&tau](const xAOD::TauTrack *track){auto ddeta=track->eta()- tau->eta();return ddeta; });

      auto track_dPhi = Monitored::Collection("track_dPhi", tracks, [&tau](const xAOD::TauTrack *track){return track->p4().DeltaPhi(tau->p4());}); 

      auto track_z0sinThetaTJVA_abs_log = Monitored::Collection("track_z0sinThetaTJVA_abs_log", tracks, [&tau](const xAOD::TauTrack *track){return track->z0sinThetaTJVA(*tau); }); 

      auto track_d0_abs_log = Monitored::Collection("track_d0_abs_log", tracks, [](const xAOD::TauTrack *track){return  TMath::Log10( TMath::Abs(track->track()->d0()) + 1e-6); }); 

      auto track_nIBLHitsAndExp = Monitored::Collection("track_nIBLHitsAndExp", tracks, [](const xAOD::TauTrack *track){
                                                    uint8_t inner_pixel_hits, inner_pixel_exp;
                                                    const auto success1_innerPixel_hits = track->track()->summaryValue(inner_pixel_hits, xAOD::numberOfInnermostPixelLayerHits);
                                                    const auto success2_innerPixel_exp = track->track()->summaryValue(inner_pixel_exp, xAOD::expectInnermostPixelLayerHit);
                                                    float detail = -999;
                                                    if (success1_innerPixel_hits && success2_innerPixel_exp) {detail=inner_pixel_exp ? inner_pixel_hits : 1.;};
                                                    return   detail; });

      auto track_nPixelHitsPlusDeadSensors = Monitored::Collection("track_nPixelHitsPlusDeadSensors", tracks, [](const xAOD::TauTrack *track){
                                                    uint8_t pixel_hits, pixel_dead;
                                                    const auto success1_pixel_hits = track->track()->summaryValue(pixel_hits, xAOD::numberOfPixelHits);
                                                    const auto success2_pixel_dead = track->track()->summaryValue(pixel_dead, xAOD::numberOfPixelDeadSensors);
                                                    float detail = -999;
                                                    if (success1_pixel_hits && success2_pixel_dead) {detail=pixel_hits + pixel_dead;};
                                                    return   detail; });

      auto track_nSCTHitsPlusDeadSensors = Monitored::Collection("track_nSCTHitsPlusDeadSensors", tracks, [](const xAOD::TauTrack *track){
                                                    uint8_t sct_hits, sct_dead;
                                                    const auto success1_sct_hits = track->track()->summaryValue(sct_hits, xAOD::numberOfSCTHits);
                                                    const auto success2_sct_dead = track->track()->summaryValue(sct_dead, xAOD::numberOfSCTDeadSensors);
                                                    float detail = -999;
                                                    if (success1_sct_hits && success2_sct_dead) {detail=sct_hits + sct_dead;};
                                                    return   detail; });
                                                  

      fill(monGroup,track_pt_log,track_dEta,track_dPhi,track_z0sinThetaTJVA_abs_log,track_d0_abs_log,track_nIBLHitsAndExp,track_nPixelHitsPlusDeadSensors,track_nSCTHitsPlusDeadSensors);
    }

  ATH_MSG_DEBUG("After fill  RNN input Track: " << trigger);
  
}

void TrigTauMonitorAlgorithm::fillRNNCluster(const std::string& trigger, const std::vector<const xAOD::TauJet*>& tau_vec, bool online) const
{
  ATH_MSG_DEBUG("Fill RNN input Cluster: " << trigger << " for online/offline " << online);
  
  auto monGroup = getGroup(trigger+( online ? "_RNN_HLT_InputCluster" : "_RNN_Offline_InputCluster"));
  
  for(const auto *tau: tau_vec){

    auto cluster_pt_jetseed_log = Monitored::Collection("cluster_pt_jetseed_log", tau_vec,  [] (const xAOD::TauJet* tau){ return TMath::Log10( tau->ptJetSeed());});

    std::vector<const xAOD::CaloCluster *> clusters;

    float max_cluster_dr = 1.0;
    
    if(!tau->jetLink().isValid()) {
      continue;
    }

    const xAOD::Jet *jetSeed = tau->jet();
    if (jetSeed==nullptr) {
      ATH_MSG_ERROR("Tau jet link is invalid.");
      continue;
    } 

    if(!jetSeed->getConstituents().isValid()) {
      continue;
    }

    ATH_MSG_DEBUG("Link to constituents is valid");

    for (const auto *const jc : jetSeed->getConstituents()) {
      const auto *cl = dynamic_cast<const xAOD::CaloCluster *>(jc->rawConstituent());
      if (!cl) {
        ATH_MSG_ERROR("Calorimeter cluster is invalid.");
        continue;
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

    /*
    unsigned int max_clusters = 6;
    if (clusters.size() > max_clusters) {
      clusters.resize(max_clusters);
    }
    */

    auto cluster_et_log = Monitored::Collection("cluster_et_log",clusters, [](const xAOD::CaloCluster *cluster){return TMath::Log10( cluster->et()); }); 
    auto cluster_dEta = Monitored::Collection("cluster_dEta", clusters, [&tau](const xAOD::CaloCluster *cluster){auto ddeta=cluster->eta()- tau->eta();return ddeta; });
    auto cluster_dPhi = Monitored::Collection("cluster_dPhi", clusters, [&tau](const xAOD::CaloCluster *cluster){return cluster->p4().DeltaPhi(tau->p4()); }); 
    auto cluster_SECOND_R_log10 = Monitored::Collection("cluster_SECOND_R_log10", clusters, [](const xAOD::CaloCluster *cluster){
                                              double detail = -999.;
                                              const auto success_SECOND_R = cluster->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_R,detail);
                                              if (success_SECOND_R) detail = TMath::Log10(detail + 0.1);
                                              return detail;});

      auto cluster_SECOND_LAMBDA_log10 = Monitored::Collection("cluster_SECOND_LAMBDA_log10", clusters, [](const xAOD::CaloCluster *cluster){
                                              double detail = -999.;
                                              const auto success_SECOND_LAMBDA = cluster->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_LAMBDA, detail);
                                              if (success_SECOND_LAMBDA) detail = TMath::Log10(detail + 0.1); 
                                              return detail;});

      auto cluster_CENTER_LAMBDA_log10 = Monitored::Collection("cluster_CENTER_LAMBDA_log10", clusters, [](const xAOD::CaloCluster *cluster){
                                              double detail = -999.;
                                              const auto success_CENTER_LAMBDA = cluster->retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_LAMBDA, detail);
                                              if (success_CENTER_LAMBDA) detail = TMath::Log10(detail + 1e-6); 
                                              return detail;});                                                  

      fill(monGroup,cluster_pt_jetseed_log,cluster_et_log,cluster_dEta,cluster_dPhi,cluster_SECOND_R_log10,cluster_SECOND_LAMBDA_log10,cluster_CENTER_LAMBDA_log10);
    }

  ATH_MSG_DEBUG("After fill  RNN input Cluster: " << trigger);
}

void TrigTauMonitorAlgorithm::fillbasicVars(const std::string& trigger, const std::vector<const xAOD::TauJet*>& tau_vec,const std::string& nProng, bool online) const
{
  ATH_MSG_DEBUG("Fill Basic Variables: " << trigger); 

  auto monGroup = getGroup(trigger+( online ? "HLT_basicVars_"+nProng : "Offline_basicVars_"+nProng));  

  auto hEFEt           = Monitored::Collection("hEFEt", tau_vec,  [] (const xAOD::TauJet* tau){return tau->pt()/1000; });
  auto hEFEta          = Monitored::Collection("hEFEta", tau_vec,  [] (const xAOD::TauJet* tau){return tau->eta(); });                                                     

  auto hEFPhi          = Monitored::Collection("hEFPhi", tau_vec,  [] (const xAOD::TauJet* tau){return tau->phi(); });
  auto hEFnTrack       = Monitored::Collection("hEFnTrack", tau_vec,  [] (const xAOD::TauJet* tau){
      int EFnTrack=-1;
      tau->detail(xAOD::TauJetParameters::nChargedTracks, EFnTrack);
      return EFnTrack; }); 
  auto hEFnWideTrack   = Monitored::Collection("hEFnWideTrack", tau_vec,  [] (const xAOD::TauJet* tau){
      int EFWidenTrack(-1);
      EFWidenTrack = tau->nTracksIsolation();
      return EFWidenTrack; }); 

  const TrigInfo info = getTrigInfo(trigger);

  auto hRNNScore = Monitored::Collection("hRNNScore", tau_vec, [] (const xAOD::TauJet* tau){ return tau->discriminant(xAOD::TauJetParameters::RNNJetScore);});
  auto hRNNScoreSigTrans = Monitored::Collection("hRNNScoreSigTrans", tau_vec, [] (const xAOD::TauJet* tau){ return tau->discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans);});

  fill(monGroup, hEFEt,hEFEta,hEFPhi,hEFnTrack,hEFnWideTrack, hRNNScore, hRNNScoreSigTrans);

  ATH_MSG_DEBUG("After fill Basic variables: " << trigger);

}

TrigInfo TrigTauMonitorAlgorithm::getTrigInfo(const std::string& trigger) const{ 
  return m_trigInfo.at(trigger); 
}


void TrigTauMonitorAlgorithm::setTrigInfo(const std::string& trigger)
{ 

  std::string idwp="",type="",l1item="",l1type="";
  float hlthr=0.,l1thr=0.;
  bool isRNN=false,isPerf=false,isL1=false;

  size_t l=trigger.length();
  size_t pos=trigger.find('_');
  std::string substr =trigger.substr(pos+1,l);
  std::vector<std::string> names;
  names.push_back(trigger.substr(0,pos));

  while(substr.find('_')!=std::string::npos)
  {
    pos = substr.find('_');
    names.push_back(substr.substr(0,pos));
    substr = substr.substr(pos+1,substr.length());    
  }

  names.push_back(substr);

  hlthr = std::stof(names[1].substr(3,names[1].length()));
 
  idwp=names[2];

  if(idwp=="perf" || idwp=="idperf") isPerf=true;
  else if(idwp.find("RNN")!=std::string::npos) isRNN=true;

  if(names[0].find("L1")!=std::string::npos) isL1=true;

  if(names.size() <= 4)
  {
    type=names[2];
    l1item =names[3];
     
    if(names[3].find("L1TAU") !=std::string::npos){
      l1thr = std::stof(names[3].substr(5,names[3].length()));
    } 
    else if (names[3].find("L1eTAU") !=std::string::npos){
      l1thr = std::stof(names[3].substr(6,names[3].length()));
    }
   
  } 
  else if ( names[4].find("L1TAU") !=std::string::npos || names[4].find("L1eTAU") !=std::string::npos )  {
    type=names[3];
    l1item =names[4];

    if(names[4].find("L1TAU") !=std::string::npos){
      l1thr = std::stof(names[4].substr(5,names[4].length())); 
    }
    else if(names[4].find("L1eTAU") !=std::string::npos){
      l1thr = std::stof(names[4].substr(6,names[4].length()));
    }
  }

  TrigInfo info{trigger,idwp,l1item,l1type,type,isL1,isRNN,isPerf,hlthr,l1thr,false};

  m_trigInfo[trigger] = info;
}

