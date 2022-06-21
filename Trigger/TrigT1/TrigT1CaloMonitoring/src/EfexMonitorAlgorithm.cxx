/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "EfexMonitorAlgorithm.h"


EfexMonitorAlgorithm::EfexMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}

StatusCode EfexMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("EfexMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  ATH_MSG_DEBUG("m_eFexContainer"<< m_eFexContainerKey); 
  ATH_MSG_DEBUG("m_eFexTauContainer"<< m_eFexTauContainerKey);

  // we initialise all the containers that we need
  ATH_CHECK( m_eFexContainerKey.initialize() );
  ATH_CHECK( m_eFexTauContainerKey.initialize() );
  
  return AthMonitorAlgorithm::initialize();
}

StatusCode EfexMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG("EfexMonitorAlgorithm::fillHistograms");

  // Access eFex EM container
  SG::ReadHandle<xAOD::eFexEMRoIContainer> eFexContainer{m_eFexContainerKey, ctx};
  if(!eFexContainer.isValid()){
    ATH_MSG_ERROR("No eFex EM container found in storegate  "<< m_eFexContainerKey); 
    return StatusCode::SUCCESS;
  }

  // monitored variables for histograms
  auto TOBeT = Monitored::Scalar<float>("TOBTransverseEnergy",0.0);
  auto TOBeta = Monitored::Scalar<float>("TOBEta",0.0);
  auto TOBphi = Monitored::Scalar<float>("TOBPhi",0.0);
  auto TOBeFEXNumber = Monitored::Scalar<float>("TOBeFEXNumber",0.0);
  auto TOBshelfNumber = Monitored::Scalar<float>("TOBshelfNumber",0.0);
  auto TOBReta = Monitored::Scalar<float>("TOBReta",0.0);
  auto TOBRhad = Monitored::Scalar<float>("TOBRhad",0.0);
  auto TOBWstot = Monitored::Scalar<float>("TOBWstot",0.0);

  const xAOD::eFexEMRoI* efexEmRoI = 0;

  for(const auto it :  *eFexContainer){
    efexEmRoI = it;

    TOBeT=efexEmRoI->et();
    fill(m_packageName,TOBeT);

    TOBeta=efexEmRoI->eta();
    TOBphi=efexEmRoI->phi();
    fill(m_packageName,TOBeta,TOBphi);

    uint8_t efexnumber=efexEmRoI->eFexNumber();
    TOBeFEXNumber=int(efexnumber);
    fill(m_packageName,TOBeFEXNumber);

    uint8_t shelfnumber=efexEmRoI->shelfNumber();
    TOBshelfNumber=shelfnumber;
    fill(m_packageName,TOBshelfNumber);

    TOBReta=efexEmRoI->RetaThresholds();
    fill(m_packageName,TOBReta);

    TOBRhad=efexEmRoI->RhadThresholds();
    fill(m_packageName,TOBRhad);

    TOBWstot=efexEmRoI->WstotThresholds();
    fill(m_packageName,TOBWstot);
  }

  // Access eFex Tau container
  SG::ReadHandle<xAOD::eFexTauRoIContainer> eFexTauContainer{m_eFexTauContainerKey, ctx};
  if(!eFexTauContainer.isValid()){
    ATH_MSG_ERROR("No eFex Tau container found in storegate  "<< m_eFexTauContainerKey);
    return StatusCode::SUCCESS;
  }

  // monitored variables for histograms
  auto tauTOBeT = Monitored::Scalar<float>("tauTOBTransverseEnergy",0.0);
  auto tauTOBeta = Monitored::Scalar<float>("tauTOBEta",0.0);
  auto tauTOBphi = Monitored::Scalar<float>("tauTOBPhi",0.0);

  const xAOD::eFexTauRoI* efexTauRoI = 0;

  for(const auto it :  *eFexTauContainer){
    efexTauRoI = it;

    tauTOBeT=efexTauRoI->et();
    fill(m_packageName,tauTOBeT);

    tauTOBeta=efexTauRoI->eta();
    fill(m_packageName,tauTOBeta);

    tauTOBphi=efexTauRoI->phi();
    fill(m_packageName,tauTOBphi);
  }
  
  return StatusCode::SUCCESS;
}


