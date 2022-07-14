/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GfexMonitorAlgorithm.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloGain.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include <istream>


GfexMonitorAlgorithm::GfexMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}

StatusCode GfexMonitorAlgorithm::initialize() {
  ATH_MSG_DEBUG("GfexMonitorAlgorithm::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  
  ATH_MSG_DEBUG("m_gFexLRJetContainerKey"<< m_gFexLRJetContainerKey);
  ATH_MSG_DEBUG("m_gFexSRJetContainerKey"<< m_gFexSRJetContainerKey);
  ATH_MSG_DEBUG("m_gFexRhoContainerKey"<< m_gFexRhoContainerKey);
  ATH_MSG_DEBUG("m_gScalarEJwojContainerKey"<< m_gScalarEJwojContainerKey);
  ATH_MSG_DEBUG("m_gMETComponentsJwojContainerKey"<< m_gMETComponentsJwojContainerKey);
  ATH_MSG_DEBUG("m_gMHTComponentsJwojContainerKey"<< m_gMHTComponentsJwojContainerKey);
  ATH_MSG_DEBUG("m_gMSTComponentsJwojContainerKey"<< m_gMSTComponentsJwojContainerKey);

  ATH_CHECK( m_gFexLRJetContainerKey.initialize() );  
  ATH_CHECK( m_gFexSRJetContainerKey.initialize() );  
  ATH_CHECK( m_gFexRhoContainerKey.initialize() );  

  ATH_CHECK( m_gScalarEJwojContainerKey.initialize() );  
  ATH_CHECK( m_gMETComponentsJwojContainerKey.initialize() );  
  ATH_CHECK( m_gMHTComponentsJwojContainerKey.initialize() );  
  ATH_CHECK( m_gMSTComponentsJwojContainerKey.initialize() );  

  ATH_CHECK( m_gMETComponentsNoiseCutContainerKey.initialize() );
  ATH_CHECK( m_gMETComponentsRmsContainerKey.initialize() );
  ATH_CHECK( m_gScalarENoiseCutContainerKey.initialize() );
  ATH_CHECK( m_gScalarERmsContainerKey.initialize() );

  return AthMonitorAlgorithm::initialize();
}

StatusCode GfexMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  ATH_MSG_DEBUG("GfexMonitorAlgorithm::fillHistograms");
  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;

  SG::ReadHandle<xAOD::gFexJetRoIContainer> gFexLRJetContainer{m_gFexLRJetContainerKey, ctx};
  if(!gFexLRJetContainer.isValid()){
    ATH_MSG_ERROR("No gFex LR Jet container found in storegate  "<< m_gFexLRJetContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexJetRoIContainer> gFexSRJetContainer{m_gFexSRJetContainerKey, ctx};
  if(!gFexSRJetContainer.isValid()){
    ATH_MSG_ERROR("No gFex SR Jet container found in storegate  "<< m_gFexSRJetContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexJetRoIContainer> gFexRhoContainer{m_gFexRhoContainerKey, ctx};
  if(!gFexRhoContainer.isValid()){
    ATH_MSG_ERROR("No gFex rho container found in storegate  "<< m_gFexRhoContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gScalarEJwojContainer{m_gScalarEJwojContainerKey, ctx};
  if(!gScalarEJwojContainer.isValid()){
    ATH_MSG_ERROR("No scalar energy jets without jets container found in storegate  "<< m_gScalarEJwojContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMETComponentsJwojContainer{m_gMETComponentsJwojContainerKey, ctx};
  if(!gMETComponentsJwojContainer.isValid()){
    ATH_MSG_ERROR("No MET components for jets without jets container found in storegate  "<< m_gMETComponentsJwojContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMHTComponentsJwojContainer{m_gMHTComponentsJwojContainerKey, ctx};
  if(!gMHTComponentsJwojContainer.isValid()){
    ATH_MSG_ERROR("No MHT components for jets without jets container found in storegate  "<< m_gMHTComponentsJwojContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMSTComponentsJwojContainer{m_gMSTComponentsJwojContainerKey, ctx};
  if(!gMSTComponentsJwojContainer.isValid()){
    ATH_MSG_ERROR("No MST components jets without jets container found in storegate  "<< m_gMSTComponentsJwojContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMETComponentsNoiseCutContainer{m_gMETComponentsNoiseCutContainerKey, ctx};
  if(!gMETComponentsNoiseCutContainer.isValid()){
    ATH_MSG_ERROR("No MET components calculated with Noise Cut Algorithm in storegate  "<< m_gMETComponentsNoiseCutContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMETComponentsRmsContainer{m_gMETComponentsRmsContainerKey, ctx};
  if(!gMETComponentsRmsContainer.isValid()){
    ATH_MSG_ERROR("No MET components calculated with Rms Algorithm in storegate  "<< m_gMETComponentsRmsContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gScalarENoiseCutContainer{m_gScalarENoiseCutContainerKey, ctx};
  if(!gScalarENoiseCutContainer.isValid()){
    ATH_MSG_ERROR("No scalar energy Noise Cut container found in storegate  "<< m_gScalarENoiseCutContainerKey);
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gScalarERmsContainer{m_gScalarERmsContainerKey, ctx};
  if(!gScalarENoiseCutContainer.isValid()){
    ATH_MSG_ERROR("No scalar energy Noise Cut container found in storegate  "<< m_gScalarENoiseCutContainerKey);
    return StatusCode::SUCCESS;
  }

  // variables for histograms
  auto gtype = Monitored::Scalar<int>("gFexType",0.0);
  auto gFexLRJeteT = Monitored::Scalar<float>("gFexLRJetTransverseEnergy",0.0);
  auto gFexLRJeteta = Monitored::Scalar<float>("gFexLRJetEta",0.0);
  auto gFexLRJetphi = Monitored::Scalar<float>("gFexLRJetPhi",0.0);
  auto gFexSRJeteT = Monitored::Scalar<float>("gFexSRJetTransverseEnergy",0.0);
  auto gFexSRJeteta = Monitored::Scalar<float>("gFexSRJetEta",0.0);
  auto gFexSRJetphi = Monitored::Scalar<float>("gFexSRJetPhi",0.0);
  auto gFexRhoeT = Monitored::Scalar<float>("gFexRhoTransverseEnergy",0.0);
  auto gFexMet = Monitored::Scalar<float>("gFexMET",0.0);
  auto gFexSumEt = Monitored::Scalar<float>("gFexSumET",0.0);
  auto METx = Monitored::Scalar<float>("gFexMETx",0.0);
  auto METy = Monitored::Scalar<float>("gFexMETy",0.0);
  auto MHTx = Monitored::Scalar<float>("gFexMHTx",0.0);
  auto MHTy = Monitored::Scalar<float>("gFexMHTy",0.0);
  auto MSTx = Monitored::Scalar<float>("gFexMSTx",0.0);
  auto MSTy = Monitored::Scalar<float>("gFexMSTy",0.0);
  auto METx_NoiseCut = Monitored::Scalar<float>("gFexMETx_NoiseCut",0.0);
  auto METy_NoiseCut = Monitored::Scalar<float>("gFexMETy_NoiseCut",0.0);
  auto METx_Rms = Monitored::Scalar<float>("gFexMETx_Rms",0.0);
  auto METy_Rms = Monitored::Scalar<float>("gFexMETy_Rms",0.0);
  auto gFexMet_NoiseCut = Monitored::Scalar<float>("gFexMET_NoiseCut",0.0);
  auto gFexSumEt_NoiseCut = Monitored::Scalar<float>("gFexSumET_NoiseCut",0.0);
  auto gFexMet_Rms = Monitored::Scalar<float>("gFexMET_Rms",0.0);
  auto gFexSumEt_Rms = Monitored::Scalar<float>("gFexSumET_Rms",0.0);

  for(const xAOD::gFexJetRoI* gFexLRJetRoI : *gFexLRJetContainer){
    gFexLRJeteT=gFexLRJetRoI->gFexTobEt();
    gFexLRJeteta=gFexLRJetRoI->eta();
    gFexLRJetphi=gFexLRJetRoI->phi();
    gtype=gFexLRJetRoI->gFexType();
    fill(m_packageName,gFexLRJeteta,gFexLRJetphi,gFexLRJeteT,gtype);
  }

  for(const xAOD::gFexJetRoI* gFexSRJetRoI : *gFexSRJetContainer){
    gFexSRJeteT=gFexSRJetRoI->gFexTobEt();
    gFexSRJeteta=gFexSRJetRoI->eta();
    gFexSRJetphi=gFexSRJetRoI->phi();
    gtype=gFexSRJetRoI->gFexType();
    fill(m_packageName,gFexSRJeteta,gFexSRJetphi,gFexSRJeteT,gtype);
  }

  for(const xAOD::gFexJetRoI* gFexRhoRoI : *gFexRhoContainer){
    gFexRhoeT=gFexRhoRoI->gFexTobEt();
    gtype=gFexRhoRoI->gFexType();
    fill(m_packageName,gFexRhoeT, gtype);
  }

  for(const xAOD::gFexGlobalRoI* gScalarEJwoj : *gScalarEJwojContainer){
    gFexSumEt=gScalarEJwoj->METquantityTwo();
    gFexMet=gScalarEJwoj->METquantityOne();
    fill(m_packageName,gFexMet,gFexSumEt);
  }

  for (const xAOD::gFexGlobalRoI* gMETComponentsJwoj : *gMETComponentsJwojContainer) {
    METx=gMETComponentsJwoj->METquantityOne();
    METy=gMETComponentsJwoj->METquantityTwo();
    fill(m_packageName,METx);
    fill(m_packageName,METy);
  }

  for (const xAOD::gFexGlobalRoI* gMHTComponentsJwoj : *gMHTComponentsJwojContainer) {
    MHTx=gMHTComponentsJwoj->METquantityOne();
    MHTy=gMHTComponentsJwoj->METquantityTwo();
    fill(m_packageName,MHTx,MHTy);
  }

  for (const xAOD::gFexGlobalRoI* gMSTComponentsJwoj : *gMSTComponentsJwojContainer) {
    MSTx=gMSTComponentsJwoj->METquantityOne();
    MSTy=gMSTComponentsJwoj->METquantityTwo();
    fill(m_packageName,MSTx,MSTy);
  }

  for (const xAOD::gFexGlobalRoI* gMETComponentsNoiseCut : *gMETComponentsNoiseCutContainer) {
    METx_NoiseCut=gMETComponentsNoiseCut->METquantityOne();
    METy_NoiseCut=gMETComponentsNoiseCut->METquantityTwo();
    fill(m_packageName,METx_NoiseCut,METy_NoiseCut);
  }

  for (const xAOD::gFexGlobalRoI* gMETComponentsRms : *gMETComponentsRmsContainer) {
    METx_Rms=gMETComponentsRms->METquantityOne();
    METy_Rms=gMETComponentsRms->METquantityTwo();
    fill(m_packageName,METx_Rms,METy_Rms);
  }

  for (const xAOD::gFexGlobalRoI* gScalarENoiseCut : *gScalarENoiseCutContainer) {
    gFexMet_NoiseCut=gScalarENoiseCut->METquantityOne();
    gFexSumEt_NoiseCut=gScalarENoiseCut->METquantityTwo();
    fill(m_packageName,gFexMet_NoiseCut,gFexSumEt_NoiseCut);
  }

  for (const xAOD::gFexGlobalRoI* gScalarERms : *gScalarERmsContainer) {
    gFexMet_Rms=gScalarERms->METquantityOne();
    gFexSumEt_Rms=gScalarERms->METquantityTwo();
    fill(m_packageName,gFexMet_Rms,gFexSumEt_Rms);
  }

  fill(m_packageName,variables);
  variables.clear();
  return StatusCode::SUCCESS;
}

