/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JfexMonitorAlgorithm.h"
#include "TMath.h"

JfexMonitorAlgorithm::JfexMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}

StatusCode JfexMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("JfexMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);

  ATH_MSG_DEBUG("m_jFexLRJetContainerKey"<< m_jFexLRJetContainerKey);
  ATH_MSG_DEBUG("m_jFexSRJetContainerKey"<< m_jFexSRJetContainerKey);
  ATH_MSG_DEBUG("m_jFexTauContainerKey"  << m_jFexTauContainerKey);
  ATH_MSG_DEBUG("m_jFexFwdElContainerKey"<< m_jFexFwdElContainerKey);
  ATH_MSG_DEBUG("m_jFexMETContainerKey"  << m_jFexMETContainerKey);
  ATH_MSG_DEBUG("m_jFexSumEtContainerKey"<< m_jFexSumEtContainerKey);

  ATH_CHECK( m_jFexLRJetContainerKey.initialize() );  
  ATH_CHECK( m_jFexSRJetContainerKey.initialize() );  
  ATH_CHECK( m_jFexTauContainerKey.initialize()   );  
  ATH_CHECK( m_jFexFwdElContainerKey.initialize()   );  
  ATH_CHECK( m_jFexMETContainerKey.initialize()   );  
  ATH_CHECK( m_jFexSumEtContainerKey.initialize()   );  

  return AthMonitorAlgorithm::initialize();
}

StatusCode JfexMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  
  ATH_MSG_DEBUG("JfexMonitorAlgorithm::fillHistograms");
  
  SG::ReadHandle<xAOD::jFexSRJetRoIContainer> jFexSRJetContainer{m_jFexSRJetContainerKey, ctx};
  if(!jFexSRJetContainer.isValid()){
    ATH_MSG_ERROR("No jFex SR Jet container found in storegate  "<< m_jFexSRJetContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexLRJetRoIContainer> jFexLRJetContainer{m_jFexLRJetContainerKey, ctx};
  if(!jFexLRJetContainer.isValid()){
    ATH_MSG_ERROR("No jFex LR Jet container found in storegate  "<< m_jFexLRJetContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexTauRoIContainer> jFexTauContainer{m_jFexTauContainerKey, ctx};
  if(!jFexTauContainer.isValid()){
    ATH_MSG_ERROR("No jFex Tau container found in storegate  "<< m_jFexTauContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexFwdElRoIContainer> jFexEMContainer{m_jFexFwdElContainerKey, ctx};
  if(!jFexEMContainer.isValid()){
    ATH_MSG_ERROR("No jFex EM container found in storegate  "<< m_jFexFwdElContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexMETRoIContainer> jFexMETContainer{m_jFexMETContainerKey, ctx};
  if(!jFexMETContainer.isValid()){
    ATH_MSG_ERROR("No jFex MET container found in storegate  "<< m_jFexMETContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexSumETRoIContainer> jFexSumETContainer{m_jFexSumEtContainerKey, ctx};
  if(!jFexSumETContainer.isValid()){
    ATH_MSG_ERROR("No jFex MET container found in storegate  "<< m_jFexSumEtContainerKey);
    return StatusCode::SUCCESS;
  }

  // variables for histograms
  // jJ
  auto jFexSRJetModule  = Monitored::Scalar<int>  ("jJ_jFexNumber",0);
  auto jFexSRJetFPGA    = Monitored::Scalar<int>  ("jJ_fpgaNumber",0);
  auto jFexSRJetEt  = Monitored::Scalar<int>  ("jJ_Et",0);
  auto jFexSRJeteta = Monitored::Scalar<float>("jJ_Eta",0.0);
  auto jFexSRJetphi = Monitored::Scalar<float>("jJ_Phi",0.0);
  auto jFexSRJeteta_glo = Monitored::Scalar<float>("jJ_GlobalEta",0.0);
  auto jFexSRJetphi_glo = Monitored::Scalar<float>("jJ_GlobalPhi",0.0);
  
  // jLJ
  auto jFexLRJetModule  = Monitored::Scalar<int>  ("jLJ_jFexNumber",0);
  auto jFexLRJetFPGA    = Monitored::Scalar<int>  ("jLJ_fpgaNumber",0);
  auto jFexLRJetEt  = Monitored::Scalar<int>  ("jLJ_Et",0);
  auto jFexLRJeteta = Monitored::Scalar<float>("jLJ_Eta",0.0);
  auto jFexLRJetphi = Monitored::Scalar<float>("jLJ_Phi",0.0);
  auto jFexLRJeteta_glo = Monitored::Scalar<float>("jLJ_GlobalEta",0.0);
  auto jFexLRJetphi_glo = Monitored::Scalar<float>("jLJ_GlobalPhi",0.0);
  
  
  // jTau
  auto jFexTauModule  = Monitored::Scalar<int>  ("jTau_jFexNumber",0);
  auto jFexTauFPGA    = Monitored::Scalar<int>  ("jTau_fpgaNumber",0);
  auto jFexTauEt  = Monitored::Scalar<int>  ("jTau_Et",0);
  auto jFexTauIso = Monitored::Scalar<int>  ("jTau_Iso",0);
  auto jFexTaueta = Monitored::Scalar<float>("jTau_Eta",0.0);
  auto jFexTauphi = Monitored::Scalar<float>("jTau_Phi",0.0);
  auto jFexTaueta_glo = Monitored::Scalar<float>("jTau_GlobalEta",0.0);
  auto jFexTauphi_glo = Monitored::Scalar<float>("jTau_GlobalPhi",0.0);
  
  // jEM
  auto jFexEMModule  = Monitored::Scalar<int>  ("jEM_jFexNumber",0);
  auto jFexEMFPGA    = Monitored::Scalar<int>  ("jEM_fpgaNumber",0);
  auto jFexEMEt  = Monitored::Scalar<int>  ("jEM_Et",0);
  auto jFexEMeta = Monitored::Scalar<float>("jEM_Eta",0.0);
  auto jFexEMphi = Monitored::Scalar<float>("jEM_Phi",0.0);
  auto jFexEMeta_glo = Monitored::Scalar<float>("jEM_GlobalEta",0.0);
  auto jFexEMphi_glo = Monitored::Scalar<float>("jEM_GlobalPhi",0.0);
  auto jFexEMIso = Monitored::Scalar<int>  ("jEM_Iso",0);
  auto jFexEMf1  = Monitored::Scalar<int>  ("jEM_f1",0);
  auto jFexEMf2  = Monitored::Scalar<int>  ("jEM_f2",0);
  
  // jXE
  auto jFexMETX   = Monitored::Scalar<int>  ("jXE_X",0);
  auto jFexMETY   = Monitored::Scalar<int>  ("jXE_Y",0);
  auto jFexMET    = Monitored::Scalar<float>("jXE_MET",0.0);
  auto jFexMETphi = Monitored::Scalar<float>("jXE_phi",0.0);
  
  // jTE
  auto jFexSumEt_low   = Monitored::Scalar<int>  ("jTE_low",0);
  auto jFexSumEt_high  = Monitored::Scalar<int>  ("jTE_high",0);
  auto jFexSumEt_total = Monitored::Scalar<float>("jTE_SumEt",0.0);

  for(const xAOD::jFexSRJetRoI* jFexSRJetRoI : *jFexSRJetContainer){
    if(jFexSRJetRoI->tobWord()==0) continue; //remove empty TOBs
    jFexSRJetModule=jFexSRJetRoI->jFexNumber();
    jFexSRJetFPGA=jFexSRJetRoI->fpgaNumber();
    jFexSRJetEt=jFexSRJetRoI->tobEt();
    jFexSRJeteta=jFexSRJetRoI->eta();
    jFexSRJetphi=jFexSRJetRoI->phi();
    jFexSRJeteta_glo=jFexSRJetRoI->globalEta();
    jFexSRJetphi_glo=jFexSRJetRoI->globalPhi();
    fill(m_packageName,jFexSRJetModule,jFexSRJetFPGA,jFexSRJetEt,jFexSRJeteta,jFexSRJetphi,jFexSRJeteta_glo,jFexSRJetphi_glo);
  }
  
  for(const xAOD::jFexLRJetRoI* jFexLRJetRoI : *jFexLRJetContainer){
    if(jFexLRJetRoI->tobWord()==0) continue; //remove empty TOBs
    jFexLRJetModule=jFexLRJetRoI->jFexNumber();
    jFexLRJetFPGA=jFexLRJetRoI->fpgaNumber();
    jFexLRJetEt=jFexLRJetRoI->tobEt();
    jFexLRJeteta=jFexLRJetRoI->eta();
    jFexLRJetphi=jFexLRJetRoI->phi();
    jFexLRJeteta_glo=jFexLRJetRoI->globalEta();
    jFexLRJetphi_glo=jFexLRJetRoI->globalPhi();
    fill(m_packageName,jFexLRJetModule,jFexLRJetFPGA,jFexLRJetEt,jFexLRJeteta,jFexLRJetphi,jFexLRJeteta_glo,jFexLRJetphi_glo);
  }
  
  for(const xAOD::jFexTauRoI* jFexTauRoI : *jFexTauContainer){
    if(jFexTauRoI->tobWord()==0) continue; //remove empty TOBs
    jFexTauModule =jFexTauRoI->jFexNumber();
    jFexTauFPGA =jFexTauRoI->fpgaNumber();
    jFexTauEt =jFexTauRoI->tobEt();
    jFexTauIso=jFexTauRoI->tobIso();
    jFexTaueta=jFexTauRoI->eta();
    jFexTauphi=jFexTauRoI->phi();
    jFexTaueta_glo=jFexTauRoI->globalEta();
    jFexTauphi_glo=jFexTauRoI->globalPhi();
    fill(m_packageName,jFexTauModule,jFexTauFPGA,jFexTauEt,jFexTauIso,jFexTaueta,jFexTauphi,jFexTaueta_glo,jFexTauphi_glo);
  }

  for(const xAOD::jFexFwdElRoI* jFexFwdElRoI : *jFexEMContainer){
    if(jFexFwdElRoI->tobWord()==0) continue; //remove empty TOBs
    jFexEMModule =jFexFwdElRoI->jFexNumber();
    jFexEMFPGA =jFexFwdElRoI->fpgaNumber();
    jFexEMEt =jFexFwdElRoI->tobEt();
    jFexEMeta=jFexFwdElRoI->eta();
    jFexEMphi=jFexFwdElRoI->phi();
    jFexEMeta_glo=jFexFwdElRoI->globalEta();
    jFexEMphi_glo=jFexFwdElRoI->globalPhi();
    jFexEMIso=jFexFwdElRoI->tobEMIso();
    jFexEMf1 =jFexFwdElRoI->tobEMf1();
    jFexEMf2 =jFexFwdElRoI->tobEMf2();
    fill(m_packageName,jFexEMModule,jFexEMFPGA,jFexEMEt,jFexEMeta,jFexEMphi,jFexEMeta_glo,jFexEMphi_glo,jFexEMIso,jFexEMf1,jFexEMf2);
  }

  int metx = 0;
  int mety = 0;
  for(const xAOD::jFexMETRoI* jFexMETRoI : *jFexMETContainer){
    jFexMETX =jFexMETRoI->tobEx();
    jFexMETY =jFexMETRoI->tobEy();
    metx += jFexMETRoI->tobEx();
    mety += jFexMETRoI->tobEy();
    fill(m_packageName,jFexMETX,jFexMETY);
  }
  if(jFexMETContainer->size()>0){
      jFexMET    = TMath::Sqrt(std::pow(metx,2)+std::pow(mety,2));
      jFexMETphi = TMath::ATan2(mety,metx);
      fill(m_packageName,jFexMET,jFexMETphi);      
  }

  
  int sumEt_total = 0;
  for(const xAOD::jFexSumETRoI* jFexSumETRoI : *jFexSumETContainer){
    jFexSumEt_low =jFexSumETRoI->tobEt_lower();
    jFexSumEt_high =jFexSumETRoI->tobEt_upper();
    sumEt_total += jFexSumETRoI->tobEt_lower()+jFexSumETRoI->tobEt_upper();
    fill(m_packageName,jFexSumEt_low,jFexSumEt_high);
  }
  if(jFexSumETContainer->size()>0){
      jFexSumEt_total = sumEt_total;
      fill(m_packageName,jFexSumEt_total);      
  }

    
  return StatusCode::SUCCESS;
}


