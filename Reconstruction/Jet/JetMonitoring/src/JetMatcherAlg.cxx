/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetMatcherAlg.h"
#include "AsgDataHandles/WriteDecorHandle.h"

//**********************************************************************

JetMatcherAlg::JetMatcherAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthReentrantAlgorithm(name,pSvcLocator)
{
  declareProperty("JetContainerName1",m_jetContainerKey1="NONE");
  declareProperty("JetContainerName2",m_jetContainerKey2="NONE");
  declareProperty("JetCalibScale",    m_calibScale="");
}

//**********************************************************************

StatusCode JetMatcherAlg::initialize() {

  ATH_MSG_INFO(" Initializing " << name());

  ATH_CHECK( m_jetContainerKey1.initialize() );
  ATH_CHECK( m_jetContainerKey2.initialize() );

  std::string keyAppendix = m_jetContainerKey2.key();
  if (m_calibScale != "") keyAppendix = m_calibScale + "_" + m_jetContainerKey2.key();
  m_ptDiffKey = m_jetContainerKey1.key()+".ptdiff_" + keyAppendix;
  ATH_CHECK( m_ptDiffKey.initialize() );
  m_energyDiffKey = m_jetContainerKey1.key()+".energydiff_" + keyAppendix;
  ATH_CHECK( m_energyDiffKey.initialize() );
  m_massDiffKey = m_jetContainerKey1.key()+".massdiff_" + keyAppendix;
  ATH_CHECK( m_massDiffKey.initialize() );
  m_ptRespKey = m_jetContainerKey1.key()+".ptresp_" + keyAppendix;
  ATH_CHECK( m_ptRespKey.initialize() );
  m_energyRespKey = m_jetContainerKey1.key()+".energyresp_" + keyAppendix;
  ATH_CHECK( m_energyRespKey.initialize() );
  m_massRespKey = m_jetContainerKey1.key()+".massresp_" + keyAppendix;
  ATH_CHECK( m_massRespKey.initialize() );
  m_ptRefKey = m_jetContainerKey1.key()+".ptRef_" + keyAppendix;
  ATH_CHECK( m_ptRefKey.initialize() );
  m_etaRefKey = m_jetContainerKey1.key()+".etaRef_" + keyAppendix;
  ATH_CHECK( m_etaRefKey.initialize() );
  m_matchedKey = m_jetContainerKey1.key()+".matched_" + keyAppendix;
  ATH_CHECK( m_matchedKey.initialize() );
  
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetMatcherAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name());
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetMatcherAlg::execute(const EventContext& ctx) const {

  // Retrieve jet containers
  SG::ReadHandle<xAOD::JetContainer> jets1(m_jetContainerKey1, ctx);
  if (!jets1.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey1);
    return StatusCode::FAILURE;
  }
  SG::ReadHandle<xAOD::JetContainer> jets2(m_jetContainerKey2, ctx);
  if (!jets2.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey2);
    return StatusCode::FAILURE;
  }

  SG::WriteDecorHandle<xAOD::JetContainer, double> ptDiffHandle(m_ptDiffKey, ctx);
  SG::WriteDecorHandle<xAOD::JetContainer, double> energyDiffHandle(m_energyDiffKey, ctx);
  SG::WriteDecorHandle<xAOD::JetContainer, double> massDiffHandle(m_massDiffKey, ctx);
  SG::WriteDecorHandle<xAOD::JetContainer, double> ptRespHandle(m_ptRespKey, ctx);
  SG::WriteDecorHandle<xAOD::JetContainer, double> energyRespHandle(m_energyRespKey, ctx);
  SG::WriteDecorHandle<xAOD::JetContainer, double> massRespHandle(m_massRespKey, ctx);
  SG::WriteDecorHandle<xAOD::JetContainer, double> ptRefHandle(m_ptRefKey, ctx);
  SG::WriteDecorHandle<xAOD::JetContainer, double> etaRefHandle(m_etaRefKey, ctx);
  SG::WriteDecorHandle<xAOD::JetContainer, char> matchedHandle(m_matchedKey, ctx);


  std::vector<int> matchedIndices; //remembers which jets in jets2 are already matched, so they are not considered in future matching
  // Loop over first jet collection
  for(const xAOD::Jet* j1 : *jets1){
    bool j1matched = false;
    double ptDiff  = 0., energyDiff = 0., massDiff = 0., ptResp = 0., energyResp = 0., massResp = 0., ptRef = 0., etaRef = 0.;
    auto tlvjet1 = TLorentzVector();
    if (m_calibScale == "" ) tlvjet1.SetPtEtaPhiE(j1->pt(),j1->eta(),j1->phi(),j1->e());
    else { //retrieve fourmomentum at specified calibration scale
      xAOD::JetFourMom_t fourVec;
      bool status = j1->getAttribute<xAOD::JetFourMom_t>( "Jet"+m_calibScale+"Momentum", fourVec );
      if(!status) {
        ATH_MSG_ERROR("evtStore() cannot retrieve JetFourMomentum at " << m_calibScale);
        return StatusCode::FAILURE;
      }
      tlvjet1.SetPtEtaPhiE(fourVec.Pt(),fourVec.Eta(),fourVec.Phi(),fourVec.E());
    }
    // Loop over second jet collectoin
    double Rmin = 1E8;
    int jetIndex = 0, jetMatchIndex = 0;
    for(const xAOD::Jet* j2 : *jets2){
      bool alreadymatched = false;
      for (int jetIndexIterator : matchedIndices) { //Loop over indices of already matched jets to skip them
        if (jetIndex == jetIndexIterator) { alreadymatched = true; break; }
      }
      if (alreadymatched) { jetIndex++; continue; }
      auto tlvjet2 = TLorentzVector();
      tlvjet2.SetPtEtaPhiE(j2->pt(),j2->eta(),j2->phi(),j2->e());
      // calculate DeltaR(jet1,jet2)
      double dr = tlvjet1.DeltaR(tlvjet2);
      if(dr<m_Rmatch && dr<Rmin){ // decorate matching jets
	j1matched     = true;
	ptDiff        = tlvjet1.Pt()-tlvjet2.Pt();
        energyDiff    = tlvjet1.E()-tlvjet2.E();
        massDiff      = tlvjet1.M()-tlvjet2.M();
        ptResp        = ptDiff/tlvjet2.Pt();
        energyResp    = energyDiff/tlvjet2.E();
        massResp      = massDiff/tlvjet2.M();
        ptRef         = tlvjet2.Pt(); //second jet collection (=offline) is taken for reference pT and eta
        etaRef        = tlvjet2.Eta();
	Rmin          = dr;
        jetMatchIndex = jetIndex;
      }
      jetIndex++;
    }//End Loop over second jet collection
    matchedHandle(*j1)    = j1matched;
    ptDiffHandle(*j1)     = ptDiff;
    energyDiffHandle(*j1) = energyDiff;
    massDiffHandle(*j1)   = massDiff;
    ptRespHandle(*j1)     = ptResp;
    energyRespHandle(*j1) = energyResp;
    massRespHandle(*j1)   = massResp;
    ptRefHandle(*j1)      = ptRef;
    etaRefHandle(*j1)     = etaRef;
    if (j1matched) matchedIndices.push_back(jetMatchIndex);
  }//End Loop over first jet collection

  return StatusCode::SUCCESS;

}
