/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetMatcherAlg.h"
#include "AsgDataHandles/WriteDecorHandle.h"

//**********************************************************************

JetMatcherAlg::JetMatcherAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthReentrantAlgorithm(name,pSvcLocator)
{
  declareProperty("JetContainerName1",m_jetContainerKey1="NONE");
  declareProperty("JetContainerName2",m_jetContainerKey2="NONE");
}

//**********************************************************************

StatusCode JetMatcherAlg::initialize() {

  ATH_MSG_INFO(" Initializing " << name());

  ATH_CHECK( m_jetContainerKey1.initialize() );
  ATH_CHECK( m_jetContainerKey2.initialize() );

  m_ptDiffKey = m_jetContainerKey1.key()+".ptdiff_" + m_jetContainerKey2.key();
  ATH_CHECK( m_ptDiffKey.initialize() );
  m_matchedKey = m_jetContainerKey1.key()+".matched_" + m_jetContainerKey2.key();
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
  SG::WriteDecorHandle<xAOD::JetContainer, char> matchedHandle(m_matchedKey, ctx);

  // Loop over first jet collection
  for(const xAOD::Jet* j1 : *jets1){
    bool j1matched = false;
    double ptDiff  = 0;
    auto tlvjet1 = TLorentzVector();
    tlvjet1.SetPtEtaPhiE(j1->pt(),j1->eta(),j1->phi(),j1->e());
    // Loop over second jet collectoin
    double Rmin = 1E8;
    for(const xAOD::Jet* j2 : *jets2){
      auto tlvjet2 = TLorentzVector();
      tlvjet2.SetPtEtaPhiE(j2->pt(),j2->eta(),j2->phi(),j2->e());
      // calculate DeltaR(jet1,jet2)
      double dr = tlvjet1.DeltaR(tlvjet2);
      if(dr<m_Rmatch && dr<Rmin){ // decorate matching jets
	j1matched  = true;
	ptDiff     = tlvjet1.Pt()-tlvjet2.Pt();
	Rmin       = dr;
      }
    }
    matchedHandle(*j1) = j1matched;
    ptDiffHandle(*j1)  = ptDiff;
  }

  return StatusCode::SUCCESS;

}
