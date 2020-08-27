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
  ATH_CHECK( m_trigDecTool.retrieve() );

  for(const auto& trigName:m_trigInputList)
  {
     if(getTrigInfoMap().count(trigName) != 0){
       ATH_MSG_WARNING("Trigger already booked, removing from trigger list " << trigName);
     }else {
       ATH_MSG_INFO("Filling trigger info map");
       m_trigList.push_back(trigName);
       ATH_MSG_INFO("Check1");
       setTrigInfo(trigName);
       ATH_MSG_INFO("Check2");
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

  const TrigInfo info = getTrigInfo(trigger);

  // Offline
  std::vector<const xAOD::TauJet*> tau_vec;
  for( auto pairObj: pairObjs )
  {
    tau_vec.push_back(pairObj.first);
  }
    
  // Offline
  if(info.isRNN && tau_vec.size()>0) fillRNNInputVars( trigger, tau_vec, false );

  tau_vec.clear();

  auto vec =  m_trigDecTool->features<xAOD::TauJetContainer>(trigger,TrigDefs::includeFailedDecisions ,"HLT_TrigTauRecMerged_MVA" ); 
  for( auto &featLinkInfo : vec ){
    const auto *feat = *(featLinkInfo.link);
    if(!feat) continue;
    // If not pass, continue
    tau_vec.push_back(feat);
  }
  if(info.isRNN && tau_vec.size()>0) fillRNNInputVars( trigger, tau_vec, true );

  

}

void TrigTauMonitorAlgorithm::fillRNNInputVars(const std::string trigger, std::vector<const xAOD::TauJet*> tau_vec, bool online) const
{
  ATH_MSG_DEBUG("Fill RNN input variables: " << trigger);

  auto monGroup = getGroup(trigger+( online ? "_RNNInputVars_HLT" : "_RNNInputVars_Offline"));  

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
  
  fill(monGroup, centFrac,etOverPtLeadTrk,dRmax,absipSigLeadTrk,sumPtTrkFrac,emPOverTrkSysP,ptRatioEflowApprox,mEflowApprox,ptDetectorAxis);
  
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

