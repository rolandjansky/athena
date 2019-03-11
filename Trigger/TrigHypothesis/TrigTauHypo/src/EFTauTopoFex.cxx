/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "TrigTauHypo/EFTauTopoFex.h"

//Constructor
EFTauTopoFex::EFTauTopoFex(const std::string& name, ISvcLocator* pSvcLocator) : HLT::ComboAlgo(name, pSvcLocator){
  
  declareProperty("comb", m_comb_type="ditau");
  m_cont = 0;

}

//Destructor
EFTauTopoFex::~EFTauTopoFex(){ 
}

HLT::ErrorCode EFTauTopoFex::hltInitialize()
{
  ATH_MSG_INFO("in initialize()");
  if (m_comb_type != "ditau" and m_comb_type != "eltau" and m_comb_type != "mutau")
    return HLT::ERROR;

  return HLT::OK;
}

HLT::ErrorCode EFTauTopoFex::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE)
{
  ATH_MSG_DEBUG("EFTauTopoFex Execute");
  
  m_cont=new xAOD::TrigCompositeContainer();
  xAOD::TrigCompositeAuxContainer contaux;
  m_cont->setStore(&contaux);
  for(const auto &dR:m_dR) {
    xAOD::TrigComposite *comp=new xAOD::TrigComposite();
    m_cont->push_back(comp);
    comp->setName("EFTAU_dR");
        comp->setDetail("dR",dR);
  }
  attachFeature(outputTE,m_cont,"EFTAUInfo"); 
  ATH_MSG_DEBUG("EFTAUInfo attached feature");

  return HLT::OK;
}

HLT::ErrorCode EFTauTopoFex::hltFinalize()
{
  ATH_MSG_DEBUG("EFTauTopoFex Finalize");  
  return HLT::OK;
}

HLT::ErrorCode EFTauTopoFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass )
{
  pass = false;
  m_dR.clear();  

  ATH_MSG_DEBUG("Running EFTauTopoFex::acceptInputs");

  if (inputTE.size() != 2) {
    ATH_MSG_ERROR("Got different than 2 number of input TEs: " 
      <<  inputTE.size() 
      << " job badly configured");
    return HLT::BAD_JOB_SETUP;
  }

  if (m_comb_type == "eltau") {
    return accept_eltau_Inputs(inputTE, pass);
  }

  if (m_comb_type == "mutau") {
    return accept_mutau_Inputs(inputTE, pass);
  }
  
  return accept_tautau_Inputs(inputTE, pass);

}

void EFTauTopoFex :: fill_delta_r(const xAOD::IParticleContainer* c1, const xAOD::IParticleContainer * c2)
{

  for (const auto &p1: *c1) {
    for (const auto &p2: *c2) {
      float deta = fabs(p1->eta() - p2->eta());
      float dphi = fabs(HLT::wrapPhi(p1->phi() - p2->phi()));
      float dr = sqrt(deta * deta + dphi * dphi); // compute dR
      m_dR.push_back(dr);
    }
  }

}

HLT::ErrorCode EFTauTopoFex :: accept_tautau_Inputs(HLT::TEConstVec& inputTE, bool& pass)
{

  std::vector<const xAOD::TauJetContainer*> vectorTauContainers1;
  std::vector<const xAOD::TauJetContainer*> vectorTauContainers2;

  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];


  if ( getFeatures(te1, vectorTauContainers1) != HLT::OK || getFeatures(te2, vectorTauContainers2) != HLT::OK )
    {
      ATH_MSG_WARNING("Failed to get xAOD::TauJetContainer collections");
      return HLT::MISSING_FEATURE;
    }
  else { ATH_MSG_DEBUG("xAOD::TauJetContainer collections successfully retrieved"); }
  
  if (vectorTauContainers1.size() < 1 || vectorTauContainers2.size() < 1) {
    ATH_MSG_DEBUG(" empty xAOD::TauJetContainer");
    return HLT::OK;
  }

  const auto container_1 = vectorTauContainers1.back();
  const auto container_2 = vectorTauContainers2.back();
  fill_delta_r(container_1, container_2);  

  pass = true;
  return HLT::OK;

}

HLT::ErrorCode EFTauTopoFex :: accept_eltau_Inputs(HLT::TEConstVec& inputTE, bool& pass)
{

  std::vector<const xAOD::ElectronContainer*> vectorElectronContainers;
  std::vector<const xAOD::TauJetContainer*> vectorTauContainers;

  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];


  if ( getFeatures(te1, vectorElectronContainers) != HLT::OK || getFeatures(te2, vectorTauContainers) != HLT::OK )
    {
      ATH_MSG_WARNING("Failed to get xAOD::ElectronContainer and xAOD::TauJetContainer collections");
      return HLT::MISSING_FEATURE;
    }
  else { ATH_MSG_DEBUG("xAOD::TauJetContainer collections successfully retrieved"); }
  
  if (vectorElectronContainers.size() < 1 || vectorTauContainers.size() < 1) {
    ATH_MSG_DEBUG(" empty xAOD::ElectronContainer or xAOD::TauJetContainer collections");
    return HLT::OK;
  }

  const auto container_1 = vectorElectronContainers.back();
  const auto container_2 = vectorTauContainers.back();
  fill_delta_r(container_1, container_2);  

  pass = true;
  return HLT::OK;

}

HLT::ErrorCode EFTauTopoFex :: accept_mutau_Inputs(HLT::TEConstVec& inputTE, bool& pass)
{

  std::vector<const xAOD::MuonContainer*> vectorMuonContainers;
  std::vector<const xAOD::TauJetContainer*> vectorTauContainers;

  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];


  if ( getFeatures(te1, vectorMuonContainers) != HLT::OK || getFeatures(te2, vectorTauContainers) != HLT::OK )
    {
      ATH_MSG_WARNING("Failed to get xAOD::MuonContainer and xAOD::TauJetContainer collections");
      return HLT::MISSING_FEATURE;
    }
  else { ATH_MSG_DEBUG("xAOD::TauJetContainer collections successfully retrieved"); }
  
  if (vectorMuonContainers.size() < 1 || vectorTauContainers.size() < 1) {
    ATH_MSG_DEBUG(" empty xAOD::MuonContainer or xAOD::TauJetContainer collections");
    return HLT::OK;
  }

  const auto container_1 = vectorMuonContainers.back();
  const auto container_2 = vectorTauContainers.back();
  fill_delta_r(container_1, container_2);  

  pass = true;
  return HLT::OK;

}
