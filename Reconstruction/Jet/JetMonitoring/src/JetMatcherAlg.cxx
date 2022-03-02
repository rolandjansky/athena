/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetMatcherAlg.h"
#include "AsgDataHandles/WriteDecorHandle.h"

//**********************************************************************

JetMatcherAlg::JetMatcherAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthReentrantAlgorithm(name,pSvcLocator)
{
  declareProperty("JetContainerName1"  ,m_jetContainerKey1="NONE");
  declareProperty("L1JetContainerName1",m_l1jetContainerKey1="NONE");
  declareProperty("JetContainerName2"  ,m_jetContainerKey2="NONE");
}

//**********************************************************************

StatusCode JetMatcherAlg::initialize() {

  ATH_MSG_INFO(" Initializing " << name());

  ATH_CHECK( m_jetContainerKey1.initialize() );
  ATH_CHECK( m_l1jetContainerKey1.initialize() );
  ATH_CHECK( m_jetContainerKey2.initialize() );

  std::string prepend, keyAppendix = m_jetContainerKey2.key();
  if (!m_matchL1) {
    if (m_calibScale != "") keyAppendix = m_calibScale + "_" + m_jetContainerKey2.key();
    prepend = m_jetContainerKey1.key();
    
    m_ptDiffKey = prepend+".ptdiff_" + keyAppendix;
    m_energyDiffKey = prepend+".energydiff_" + keyAppendix;
    m_massDiffKey = prepend+".massdiff_" + keyAppendix;
    m_ptRespKey = prepend+".ptresp_" + keyAppendix;
    m_energyRespKey = prepend+".energyresp_" + keyAppendix;
    m_massRespKey = prepend+".massresp_" + keyAppendix;
    m_ptRefKey = prepend+".ptRef_" + keyAppendix;
    m_etaRefKey = prepend+".etaRef_" + keyAppendix;
    m_matchedKey = prepend+".matched_" + keyAppendix;

    m_jetVarHandleKeys.push_back(m_ptDiffKey);
    m_jetVarHandleKeys.push_back(m_energyDiffKey);
    m_jetVarHandleKeys.push_back(m_massDiffKey);
    m_jetVarHandleKeys.push_back(m_ptRespKey);
    m_jetVarHandleKeys.push_back(m_energyRespKey);
    m_jetVarHandleKeys.push_back(m_massRespKey);
    m_jetVarHandleKeys.push_back(m_ptRefKey);
    m_jetVarHandleKeys.push_back(m_etaRefKey);

  } else {
    prepend = m_l1jetContainerKey1.key();

    m_l1ptDiffKey = prepend+".ptdiff_" + keyAppendix;
    m_l1energyDiffKey = prepend+".energydiff_" + keyAppendix;
    m_l1massDiffKey = prepend+".massdiff_" + keyAppendix;
    m_l1ptRespKey = prepend+".ptresp_" + keyAppendix;
    m_l1energyRespKey = prepend+".energyresp_" + keyAppendix;
    m_l1massRespKey = prepend+".massresp_" + keyAppendix;
    m_l1ptRefKey = prepend+".ptRef_" + keyAppendix;
    m_l1etaRefKey = prepend+".etaRef_" + keyAppendix;
    m_l1matchedKey = prepend+".matched_" + keyAppendix;

    m_l1JetVarHandleKeys.push_back(m_l1ptDiffKey);
    m_l1JetVarHandleKeys.push_back(m_l1energyDiffKey);
    m_l1JetVarHandleKeys.push_back(m_l1massDiffKey);
    m_l1JetVarHandleKeys.push_back(m_l1ptRespKey);
    m_l1JetVarHandleKeys.push_back(m_l1energyRespKey);
    m_l1JetVarHandleKeys.push_back(m_l1massRespKey);
    m_l1JetVarHandleKeys.push_back(m_l1ptRefKey);
    m_l1JetVarHandleKeys.push_back(m_l1etaRefKey);

  }
  ATH_CHECK( m_ptDiffKey.initialize( !m_matchL1 ) );
  ATH_CHECK( m_energyDiffKey.initialize( !m_matchL1 ) );
  ATH_CHECK( m_massDiffKey.initialize( !m_matchL1 ) );
  ATH_CHECK( m_ptRespKey.initialize( !m_matchL1 ) );
  ATH_CHECK( m_energyRespKey.initialize( !m_matchL1 ) );
  ATH_CHECK( m_massRespKey.initialize( !m_matchL1 ) );
  ATH_CHECK( m_ptRefKey.initialize( !m_matchL1 ) );
  ATH_CHECK( m_etaRefKey.initialize( !m_matchL1 ) );
  ATH_CHECK( m_matchedKey.initialize( !m_matchL1 ) );
  for ( auto& key : m_jetVarHandleKeys ) ATH_CHECK( key.initialize( !m_matchL1 ) );
  ATH_CHECK( m_l1ptDiffKey.initialize( m_matchL1 ) );
  ATH_CHECK( m_l1energyDiffKey.initialize( m_matchL1 ) );
  ATH_CHECK( m_l1massDiffKey.initialize( m_matchL1 ) );
  ATH_CHECK( m_l1ptRespKey.initialize( m_matchL1 ) );
  ATH_CHECK( m_l1energyRespKey.initialize( m_matchL1 ) );
  ATH_CHECK( m_l1massRespKey.initialize( m_matchL1 ) );
  ATH_CHECK( m_l1ptRefKey.initialize( m_matchL1 ) );
  ATH_CHECK( m_l1etaRefKey.initialize( m_matchL1 ) );
  ATH_CHECK( m_l1matchedKey.initialize( m_matchL1 ) );
  for ( auto& key : m_l1JetVarHandleKeys ) ATH_CHECK( key.initialize( m_matchL1 ) );

  if (m_jetContainerKey1.key() != "NONE" && m_l1jetContainerKey1.key() != "NONE") {
    ATH_MSG_ERROR(" Both JetContainerKey1 and L1JetContainerKey1 were set, but we can only use one of them for jet matching. Fix your settings!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetMatcherAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name());
  return StatusCode::SUCCESS;
}

//**********************************************************************

TLorentzVector JetMatcherAlg::GetTLV(const xAOD::Jet* jet) const {

  TLorentzVector tlv = TLorentzVector(0.,0.,0.,0.);
  if (m_calibScale == "" ) {
    tlv.SetPtEtaPhiE(jet->pt(),jet->eta(),jet->phi(),jet->e());
  } else { //retrieve fourmomentum at specified calibration scale
    xAOD::JetFourMom_t fourVec;
    bool status = jet->getAttribute<xAOD::JetFourMom_t>( "Jet"+m_calibScale+"Momentum", fourVec );
    if(!status) {
      ATH_MSG_ERROR("evtStore() cannot retrieve JetFourMomentum at " << m_calibScale);
      return tlv;
    }
    tlv.SetPtEtaPhiE(fourVec.Pt(),fourVec.Eta(),fourVec.Phi(),fourVec.E());
  }
  return tlv;
}

TLorentzVector JetMatcherAlg::GetTLV(const xAOD::JetRoI* jet) const {

  TLorentzVector tlv = TLorentzVector(0.,0.,0.,0.);
  tlv.SetPtEtaPhiM(jet->et8x8(),jet->eta(),jet->phi(),0.);
  return tlv;
}

//**********************************************************************

template <typename T>
void JetMatcherAlg::jetMatching(SG::ReadHandle<DataVector<T>> jets1, SG::ReadHandle<xAOD::JetContainer> jets2, SG::WriteDecorHandleKey<DataVector<T>> matchedHandleKey, std::vector<SG::WriteDecorHandleKey<DataVector<T>>> varHandleKeys, const EventContext& ctx) const{

  SG::WriteDecorHandle<DataVector<T>, double> ptDiffHandle(varHandleKeys[0], ctx);
  SG::WriteDecorHandle<DataVector<T>, double> energyDiffHandle(varHandleKeys[1], ctx);
  SG::WriteDecorHandle<DataVector<T>, double> massDiffHandle(varHandleKeys[2], ctx);
  SG::WriteDecorHandle<DataVector<T>, double> ptRespHandle(varHandleKeys[3], ctx);
  SG::WriteDecorHandle<DataVector<T>, double> energyRespHandle(varHandleKeys[4], ctx);
  SG::WriteDecorHandle<DataVector<T>, double> massRespHandle(varHandleKeys[5], ctx);
  SG::WriteDecorHandle<DataVector<T>, double> ptRefHandle(varHandleKeys[6], ctx);
  SG::WriteDecorHandle<DataVector<T>, double> etaRefHandle(varHandleKeys[7], ctx);
  SG::WriteDecorHandle<DataVector<T>, char> matchedHandle(matchedHandleKey, ctx);

  std::vector<int> matchedIndices; //remembers which jets in jets2 are already matched, so they are not considered in future matching
  // Loop over first jet collection
  for (const T *j1 : *jets1) {
    TLorentzVector tlvjet1 = GetTLV(j1);
    bool j1matched = false;
    double ptDiff  = -999., energyDiff = -999., massDiff = -999., ptResp = -999., energyResp = -999., massResp = -999., ptRef = -999., etaRef = -999.;
    if (tlvjet1.Pt() < 10000.) { // minimum pT cut of 10 GeV for jet matching
      matchedHandle(*j1)    = j1matched; // set default values for match decorations and move to next jet
      ptDiffHandle(*j1)     = ptDiff;
      energyDiffHandle(*j1) = energyDiff;
      massDiffHandle(*j1)   = massDiff;
      ptRespHandle(*j1)     = ptResp;
      energyRespHandle(*j1) = energyResp;
      massRespHandle(*j1)   = massResp;
      ptRefHandle(*j1)      = ptRef;
      etaRefHandle(*j1)     = etaRef;
      continue;
    }
    double Rmin = 1E8;
    int jetIndex = 0, jetMatchIndex = 0;
    // Loop over second jet collection
    for(const xAOD::Jet* j2 : *jets2){
      if (j2->pt() < 10000.) { jetIndex++; continue; } // minimum pT cut of 10 GeV for jet matching
      bool alreadymatched = false;
      for (int jetIndexIterator : matchedIndices) { //Loop over indices of already matched jets to skip them
        if (jetIndex == jetIndexIterator) { alreadymatched = true; break; }
      }
      if (alreadymatched) { jetIndex++; continue; }
      TLorentzVector tlvjet2 = TLorentzVector();
      tlvjet2.SetPtEtaPhiE(j2->pt(),j2->eta(),j2->phi(),j2->e());
      // calculate DeltaR(jet1,jet2)
      double dr = tlvjet1.DeltaR(tlvjet2);
      if(dr<m_Rmatch && dr<Rmin){ // decorate matching jets
	j1matched     = true;
	ptDiff        = tlvjet1.Pt()-tlvjet2.Pt();
        energyDiff    = tlvjet1.E()-tlvjet2.E();
        massDiff      = tlvjet1.M()-tlvjet2.M();
        ptResp        = ptDiff/tlvjet2.Pt();
        if (tlvjet2.E() == 0.) energyResp = -999.;
        else energyResp = energyDiff/tlvjet2.E();
        if (tlvjet2.M() == 0.) massResp = -999.;
        else massResp = massDiff/tlvjet2.M();
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
  }
  return;

}

//**********************************************************************


StatusCode JetMatcherAlg::execute(const EventContext& ctx) const {

  // Retrieve jet containers and call appropriate matching function
  SG::ReadHandle<xAOD::JetContainer> jets2(m_jetContainerKey2, ctx);
  if (!jets2.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey2);
    return StatusCode::FAILURE;
  }
  if (!m_matchL1) { // perform jet matching for online/offline jets
    SG::ReadHandle<xAOD::JetContainer> jets1(m_jetContainerKey1, ctx);
    if (!jets1.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey1);
      return StatusCode::FAILURE;
    }
    jetMatching(jets1, jets2, m_matchedKey, m_jetVarHandleKeys, ctx);
  } else { // perform jet matching for L1 jets
    SG::ReadHandle<xAOD::JetRoIContainer> jets1(m_l1jetContainerKey1, ctx);
    if (!jets1.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain L1 jet Collection with name "<< m_l1jetContainerKey1);
      return StatusCode::FAILURE;
    }
    jetMatching(jets1, jets2, m_l1matchedKey, m_l1JetVarHandleKeys, ctx);
  }

  return StatusCode::SUCCESS;

}
