/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMatchingTool.h"
#include "xAODTrigger/MuonRoIContainer.h"

static const InterfaceID IID_MuonMatchingTool("IID_MuonMatchingTool", 1, 0);

const InterfaceID& MuonMatchingTool::interfaceID() { return IID_MuonMatchingTool; }


MuonMatchingTool :: MuonMatchingTool(const std::string& type, const std::string& name, const IInterface*  parent)
  : AthAlgTool(type, name, parent),
    m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_rndm(nullptr),
    m_ToyDecision(false)
{}


StatusCode MuonMatchingTool :: initialize(){

  ATH_CHECK( m_trigDec.retrieve() );

  m_rndm.reset(new TRandom1());
  m_rndm->SetSeed(0); 

  return StatusCode::SUCCESS;
}



StatusCode MuonMatchingTool :: matchEF(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchEF()");
  return match<xAOD::MuonContainer>( mu, trig, 0.03, pass);
}



StatusCode MuonMatchingTool :: matchSA(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchSA()");
  return match<xAOD::L2StandAloneMuonContainer>( mu, trig, 0.25, pass);
}


StatusCode MuonMatchingTool :: matchCB(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchCB()");
  return match<xAOD::L2CombinedMuonContainer>( mu, trig, 0.03, pass);
}



StatusCode MuonMatchingTool :: matchL1(  const xAOD::Muon *mu, std::string /*trig*/, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchL1()");
  pass = false;
  double muPt = mu->pt()*Gaudi::Units::GeV;
  if(m_ToyDecision){
    double prob = FermiFunction(muPt,20, 5);
    pass = m_rndm->Rndm()<prob;
  }
  return StatusCode::SUCCESS;
}


StatusCode MuonMatchingTool :: matchL1(  const xAOD::Muon *mu, SG::ReadHandle<xAOD::MuonRoIContainer> &murois, std::string trig, bool &pass) const {

  double reqdR = 0.25;
    
  pass = false;
  for(const auto &roi : *murois){
    double roiEta = roi->eta();
    double roiPhi = roi->phi();
    int roiThr = roi->getThrNumber();
    
    double dR = xAOD::P4Helpers::deltaR(*mu, roiEta, roiPhi, false);
    ATH_MSG_VERBOSE("L1 muon candidate eta=" << roiEta << " phi=" << roiPhi  << " dR=" << dR);
    if( dR<reqdR && roiThr>=L1ItemSTI(trig)){
      reqdR = dR;
      pass = true;
      ATH_MSG_DEBUG("* L1 muon eta=" << roiEta << " phi=" << roiPhi  << " dR=" << dR <<  " isPassed=true" ); 
    }
  }

  return StatusCode::SUCCESS;
}



double MuonMatchingTool :: FermiFunction(double x, double x0, double w) const {
  return 1/(1+TMath::Exp(-10*(x-x0)/w));
}


  
