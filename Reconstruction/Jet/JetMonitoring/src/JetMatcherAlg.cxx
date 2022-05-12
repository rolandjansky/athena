/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetMatcherAlg.h"
#include "AsgDataHandles/WriteDecorHandle.h"

//**********************************************************************

JetMatcherAlg::JetMatcherAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthReentrantAlgorithm(name,pSvcLocator)
{
  declareProperty("JetContainerName1"  ,m_jetContainerKey1="NONE");
  declareProperty("JetContainerName2"  ,m_jetContainerKey2="NONE");

  declareProperty("L1JetContainerName1",m_l1jetContainerKey1="NONE");
  declareProperty("L1jFexSRJetRoIContainerName",m_jFexSRJetRoIKey="NONE");
  declareProperty("L1jFexLRJetRoIContainerName",m_jFexLRJetRoIKey="NONE");
  declareProperty("L1gFexJetRoIContainerName",m_gFexJetRoIKey="NONE");
}

//**********************************************************************

StatusCode JetMatcherAlg::initialize() {
  /* Initialise ReadHandle keys for the two xAOD jet containers 
     and various L1 xAOD jet containers.

     xAOD jet containers are produced by the both hlt and offline
     reconstruction programs. The L1 trigger system produces various
     container types.

     The attribute m_jetContainerKey2 must not be initialised to "NONE".

     An instance of JetMatcher Alg will work with two containers only.
     However, all ReadHandles must be initialised.


     
   */
  
  ATH_MSG_INFO(" Initializing " << name());

  if (m_jetContainerKey2.key() == "NONE") {

    std::string msg = "JetContainerKey2 has not been configured correctly - "
      "has value NONE";
    ATH_MSG_ERROR(msg);
    
    return StatusCode::FAILURE;
  }
      
  int key_count{0};

  if (m_jetContainerKey1.key() != "NONE") {
    ++key_count;
    m_matchType = MatchType::xAODJet;
    ATH_MSG_INFO("will match xAODJet to xAODJet");
  }

  if (m_l1jetContainerKey1.key() != "NONE") {
    ++key_count;
    m_matchType = MatchType::JetRoI;
    ATH_MSG_INFO("will match JetRoI (L1)  to xAODJet");
  }

  if (m_jFexSRJetRoIKey.key() != "NONE") {
    ++key_count;
    m_matchType = MatchType::jFexSRJetRoI;
    ATH_MSG_INFO("will match jFexSRJetRoI (L1)  to xAODJet");
  }

  
  if (m_jFexLRJetRoIKey.key() != "NONE") {
    ++key_count;
    m_matchType = MatchType::jFexLRJetRoI;
    ATH_MSG_INFO("will match jFexLRJetRoI (L1)  to xAODJet");
  }

  
  if (m_gFexJetRoIKey.key() != "NONE") {
    ++key_count;
    m_matchType = MatchType::gFexJetRoI;
    ATH_MSG_INFO("will match gFexSRJetRoI (L1)  to xAODJet");
  }

  if (key_count != 1) {
    ATH_MSG_ERROR(key_count <<
		  " containers requested. This should be exactly 1");
    return StatusCode::FAILURE;
  }

  // code was originally written to use m_matchL1 to determine
  // if matching was to be done with the L1 jet container. At that
  // time, there was a  single L1 jet container type.
  // The existence of m_matchL1 has been maintained to allow
  // older configuration code to continue to work.
  //
  bool l1_match =  
    m_matchType == MatchType::JetRoI ||
    m_matchType == MatchType::jFexSRJetRoI ||
    m_matchType == MatchType::jFexLRJetRoI ||
    m_matchType == MatchType::gFexJetRoI;

  if (l1_match != m_matchL1) {
    ATH_MSG_ERROR("m_matchL1 is misconfigured");
    return StatusCode::FAILURE;
  }
  

  ATH_CHECK( m_jetContainerKey1.initialize() );
  ATH_CHECK( m_jetContainerKey2.initialize() );
  ATH_CHECK( m_l1jetContainerKey1.initialize() );
  ATH_CHECK( m_jFexSRJetRoIKey.initialize() );
  ATH_CHECK( m_jFexLRJetRoIKey.initialize() );
  ATH_CHECK( m_gFexJetRoIKey.initialize() );

  ATH_CHECK(set_xAODJet_varHandleKeys());
  ATH_CHECK(set_JetRoI_varHandleKeys());
  ATH_CHECK(set_jFexSRJetRoI_varHandleKeys());
  ATH_CHECK(set_jFexLRJetRoI_varHandleKeys());
  ATH_CHECK(set_gFexJetRoI_varHandleKeys());

  ATH_CHECK(initialize_varHandleKeys());

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetMatcherAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name());
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetMatcherAlg::GetTLV(const xAOD::Jet* jet, TLorentzVector& tlv) const {

  if (m_calibScale == "" ) {
    tlv.SetPtEtaPhiE(jet->pt(),jet->eta(),jet->phi(),jet->e());
  } else { //retrieve fourmomentum at specified calibration scale
    xAOD::JetFourMom_t fourVec;
    bool status = jet->getAttribute<xAOD::JetFourMom_t>( "Jet"+m_calibScale+"Momentum", fourVec );
    if(!status) {
      ATH_MSG_WARNING("evtStore() cannot retrieve JetFourMomentum at " << m_calibScale);
      return StatusCode::FAILURE;
    }
    tlv.SetPtEtaPhiE(fourVec.Pt(),fourVec.Eta(),fourVec.Phi(),fourVec.E());
  }
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::GetTLV(const xAOD::JetRoI* jet, TLorentzVector& tlv) const {

  tlv.SetPtEtaPhiM(jet->et8x8(),jet->eta(),jet->phi(),0.);
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::GetTLV(const xAOD::jFexSRJetRoI* jet, TLorentzVector& tlv) const {

  tlv.SetPtEtaPhiM(jet->et(),jet->eta(),jet->phi(),0.);
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::GetTLV(const xAOD::jFexLRJetRoI* jet, TLorentzVector& tlv) const {

  tlv.SetPtEtaPhiM(jet->et(),jet->eta(),jet->phi(),0.);
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::GetTLV(const xAOD::gFexJetRoI* jet, TLorentzVector& tlv) const {

  tlv.SetPtEtaPhiM(jet->et(),jet->eta(),jet->phi(),0.);
  return StatusCode::SUCCESS;
}



//**********************************************************************

template <typename T>
StatusCode JetMatcherAlg::jetMatching(SG::ReadHandle<DataVector<T>> jets1,
				SG::ReadHandle<xAOD::JetContainer> jets2,
				SG::WriteDecorHandleKey<DataVector<T>> matchedHandleKey,
				std::vector<std::reference_wrapper<SG::WriteDecorHandleKey<DataVector<T>>>> varHandleKeys,
				const EventContext& ctx) const{

  SG::WriteDecorHandle<DataVector<T>, double> ptDiffHandle(varHandleKeys[0].get(), ctx);
  SG::WriteDecorHandle<DataVector<T>, double> energyDiffHandle(varHandleKeys[1].get(), ctx);
  SG::WriteDecorHandle<DataVector<T>, double> massDiffHandle(varHandleKeys[2].get(), ctx);
  SG::WriteDecorHandle<DataVector<T>, double> ptRespHandle(varHandleKeys[3].get(), ctx);
  SG::WriteDecorHandle<DataVector<T>, double> energyRespHandle(varHandleKeys[4].get(), ctx);
  SG::WriteDecorHandle<DataVector<T>, double> massRespHandle(varHandleKeys[5].get(), ctx);
  SG::WriteDecorHandle<DataVector<T>, double> ptRefHandle(varHandleKeys[6].get(), ctx);
  SG::WriteDecorHandle<DataVector<T>, double> etaRefHandle(varHandleKeys[7].get(), ctx);
  SG::WriteDecorHandle<DataVector<T>, char> matchedHandle(matchedHandleKey, ctx);

  std::vector<int> matchedIndices; //remembers which jets in jets2 are already matched, so they are not considered in future matching
  // Loop over first jet collection
  for (const T *j1 : *jets1) {
    TLorentzVector tlvjet1 = TLorentzVector(); 
    StatusCode status = GetTLV(j1, tlvjet1);
    if (!status) {
        ATH_MSG_WARNING("Could not retrieve full jet 4-momentum. Skipping jet matching.");
        return StatusCode::SUCCESS;
    }
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
  return StatusCode::SUCCESS;

}

//**********************************************************************


StatusCode JetMatcherAlg::execute(const EventContext& ctx) const {

  // Retrieve jet containers and call appropriate matching function
  SG::ReadHandle<xAOD::JetContainer> jets2(m_jetContainerKey2, ctx);
  if (!jets2.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey2);
    return StatusCode::FAILURE;
  }
  
  if (m_matchType == MatchType::xAODJet) { // perform jet matching for online/offline xAODJet containers
    SG::ReadHandle<xAOD::JetContainer> jets1(m_jetContainerKey1, ctx);
    if (!jets1.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey1);
      return StatusCode::FAILURE;
    }
    return jetMatching(jets1, jets2, m_matchedKey, m_jetVarHandleKeys, ctx);

  } else if(m_matchType == MatchType::JetRoI) { // perform jet matching for L1 JetRoI container
    SG::ReadHandle<xAOD::JetRoIContainer> jets1(m_l1jetContainerKey1, ctx);
    if (!jets1.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain L1 jet Collection with name "<< m_l1jetContainerKey1);
      return StatusCode::FAILURE;
    }
    return jetMatching(jets1, jets2, m_l1matchedKey, m_l1JetVarHandleKeys, ctx);

  } else if (m_matchType == MatchType::jFexSRJetRoI) { // perform jet matching for L1 jFexSRJetRoI container
    SG::ReadHandle<xAOD::jFexSRJetRoIContainer> jets1(m_jFexSRJetRoIKey, ctx);
    if (!jets1.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain L1 jet Collection with name "<< m_jFexSRJetRoIKey);
      return StatusCode::FAILURE;
    } 
    return jetMatching(jets1, jets2, m_l1jFexSRmatchedKey, m_l1jFexSRJetVarHandleKeys, ctx);

    
  } else if (m_matchType == MatchType::jFexLRJetRoI) { // perform jet matching for L1 jFexLRJetRoI container
    SG::ReadHandle<xAOD::jFexLRJetRoIContainer> jets1(m_jFexLRJetRoIKey, ctx);
    if (!jets1.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain L1 jet Collection with name "<< m_jFexSRJetRoIKey);
      return StatusCode::FAILURE;
    } 
    return jetMatching(jets1, jets2, m_l1jFexLRmatchedKey, m_l1jFexLRJetVarHandleKeys, ctx);
    
  } else if (m_matchType == MatchType::gFexJetRoI) { // perform jet matching for L1 gFexJetRoI container
    SG::ReadHandle<xAOD::gFexJetRoIContainer> jets1(m_gFexJetRoIKey, ctx);
    if (!jets1.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain L1 jet Collection with name "<< m_gFexJetRoIKey);
      return StatusCode::FAILURE;
    } 
    return jetMatching(jets1, jets2, m_l1gFexmatchedKey, m_l1gFexJetVarHandleKeys, ctx);
  }

  ATH_MSG_ERROR("unsupported type for jet matching targe");
  return StatusCode::FAILURE;

}


StatusCode JetMatcherAlg::set_xAODJet_varHandleKeys() {


  if (m_matchType == MatchType::xAODJet) {
    std::string prepend{m_jetContainerKey1.key()};
    std::string keyAppendix{m_jetContainerKey2.key()};
    if (m_calibScale != "") {
      keyAppendix = m_calibScale + "_" + m_jetContainerKey2.key();
    }

    m_ptDiffKey = prepend+".ptdiff_" + keyAppendix;
    m_energyDiffKey = prepend+".energydiff_" + keyAppendix;
    m_massDiffKey = prepend+".massdiff_" + keyAppendix;
    m_ptRespKey = prepend+".ptresp_" + keyAppendix;
    m_energyRespKey = prepend+".energyresp_" + keyAppendix;
    m_massRespKey = prepend+".massresp_" + keyAppendix;
    m_ptRefKey = prepend+".ptRef_" + keyAppendix;
    m_etaRefKey = prepend+".etaRef_" + keyAppendix;
    m_matchedKey = prepend+".matched_" + keyAppendix;
  }

  m_jetVarHandleKeys = {
    m_ptDiffKey,
    m_energyDiffKey,
    m_massDiffKey,
    m_ptRespKey,
    m_energyRespKey,
    m_massRespKey,
    m_ptRefKey,
    m_etaRefKey
  };
  

  return StatusCode::SUCCESS;

}


StatusCode JetMatcherAlg::set_JetRoI_varHandleKeys() {


  if (m_matchType == MatchType::JetRoI) {
    std::string keyAppendix{m_jetContainerKey2.key()};
    std::string prepend{m_l1jetContainerKey1.key()};
    
    m_l1ptDiffKey = prepend+".ptdiff_" + keyAppendix;
    m_l1energyDiffKey = prepend+".energydiff_" + keyAppendix;
    m_l1massDiffKey = prepend+".massdiff_" + keyAppendix;
    m_l1ptRespKey = prepend+".ptresp_" + keyAppendix;
    m_l1energyRespKey = prepend+".energyresp_" + keyAppendix;
    m_l1massRespKey = prepend+".massresp_" + keyAppendix;
    m_l1ptRefKey = prepend+".ptRef_" + keyAppendix;
    m_l1etaRefKey = prepend+".etaRef_" + keyAppendix;
    m_l1matchedKey = prepend+".matched_" + keyAppendix;
  }
   
  m_l1JetVarHandleKeys = {
    m_l1ptDiffKey,
    m_l1energyDiffKey,
    m_l1massDiffKey,
    m_l1ptRespKey,
    m_l1energyRespKey,
    m_l1massRespKey,
    m_l1ptRefKey,
    m_l1etaRefKey
  };
  
  return StatusCode::SUCCESS;

}


StatusCode JetMatcherAlg::set_jFexSRJetRoI_varHandleKeys() {

  if (m_matchType == MatchType::jFexSRJetRoI) {

  std::string keyAppendix = m_jetContainerKey2.key();
  std::string prepend = m_jFexSRJetRoIKey.key();
  
  m_l1jFexSRptDiffKey = prepend+".ptdiff_" + keyAppendix;
  m_l1jFexSRenergyDiffKey = prepend+".energydiff_" + keyAppendix;
  m_l1jFexSRmassDiffKey = prepend+".massdiff_" + keyAppendix;
  m_l1jFexSRptRespKey = prepend+".ptresp_" + keyAppendix;
  m_l1jFexSRenergyRespKey = prepend+".energyresp_" + keyAppendix;
  m_l1jFexSRmassRespKey = prepend+".massresp_" + keyAppendix;
  m_l1jFexSRptRefKey = prepend+".ptRef_" + keyAppendix;
  m_l1jFexSRetaRefKey = prepend+".etaRef_" + keyAppendix;
  m_l1jFexSRmatchedKey = prepend+".matched_" + keyAppendix;
  }
  
  
  m_l1jFexSRJetVarHandleKeys = {
    m_l1jFexSRptDiffKey,
    m_l1jFexSRenergyDiffKey,
    m_l1jFexSRmassDiffKey,
    m_l1jFexSRptRespKey,
    m_l1jFexSRenergyRespKey,
    m_l1jFexSRmassRespKey,
    m_l1jFexSRptRefKey,
    m_l1jFexSRetaRefKey};
  
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::set_jFexLRJetRoI_varHandleKeys() {

  if (m_matchType == MatchType::jFexLRJetRoI) {

  std::string keyAppendix = m_jetContainerKey2.key();
  std::string prepend = m_jFexLRJetRoIKey.key();
  
  m_l1jFexLRptDiffKey = prepend+".ptdiff_" + keyAppendix;
  m_l1jFexLRenergyDiffKey = prepend+".energydiff_" + keyAppendix;
  m_l1jFexLRmassDiffKey = prepend+".massdiff_" + keyAppendix;
  m_l1jFexLRptRespKey = prepend+".ptresp_" + keyAppendix;
  m_l1jFexLRenergyRespKey = prepend+".energyresp_" + keyAppendix;
  m_l1jFexLRmassRespKey = prepend+".massresp_" + keyAppendix;
  m_l1jFexLRptRefKey = prepend+".ptRef_" + keyAppendix;
  m_l1jFexLRetaRefKey = prepend+".etaRef_" + keyAppendix;
  m_l1jFexLRmatchedKey = prepend+".matched_" + keyAppendix;
  }
  
  
  m_l1jFexLRJetVarHandleKeys = {
    m_l1jFexLRptDiffKey,
    m_l1jFexLRenergyDiffKey,
    m_l1jFexLRmassDiffKey,
    m_l1jFexLRptRespKey,
    m_l1jFexLRenergyRespKey,
    m_l1jFexLRmassRespKey,
    m_l1jFexLRptRefKey,
    m_l1jFexLRetaRefKey};
  
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::set_gFexJetRoI_varHandleKeys() {

  if (m_matchType == MatchType::gFexJetRoI) {

  std::string keyAppendix = m_jetContainerKey2.key();
  std::string prepend = m_gFexJetRoIKey.key();
  
  m_l1gFexptDiffKey = prepend+".ptdiff_" + keyAppendix;
  m_l1gFexenergyDiffKey = prepend+".energydiff_" + keyAppendix;
  m_l1gFexmassDiffKey = prepend+".massdiff_" + keyAppendix;
  m_l1gFexptRespKey = prepend+".ptresp_" + keyAppendix;
  m_l1gFexenergyRespKey = prepend+".energyresp_" + keyAppendix;
  m_l1gFexmassRespKey = prepend+".massresp_" + keyAppendix;
  m_l1gFexptRefKey = prepend+".ptRef_" + keyAppendix;
  m_l1gFexetaRefKey = prepend+".etaRef_" + keyAppendix;
  m_l1gFexmatchedKey = prepend+".matched_" + keyAppendix;
  }
  
  
  m_l1gFexJetVarHandleKeys = {
    m_l1gFexptDiffKey,
    m_l1gFexenergyDiffKey,
    m_l1gFexmassDiffKey,
    m_l1gFexptRespKey,
    m_l1gFexenergyRespKey,
    m_l1gFexmassRespKey,
    m_l1gFexptRefKey,
    m_l1gFexetaRefKey};
  
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::initialize_varHandleKeys(){

  if (m_matchType ==  MatchType::xAODJet) {
    ATH_CHECK(initialize_xAODJet_varHandleKeys(true));
    ATH_CHECK(initialize_JetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_jFexSRJetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_jFexLRJetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_gFexJetRoI_varHandleKeys(false));
  } else if (m_matchType ==  MatchType::JetRoI) {
    ATH_CHECK(initialize_xAODJet_varHandleKeys(false));
    ATH_CHECK(initialize_JetRoI_varHandleKeys(true));
    ATH_CHECK(initialize_jFexSRJetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_jFexLRJetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_gFexJetRoI_varHandleKeys(false));
  } else if (m_matchType ==  MatchType::jFexSRJetRoI) {
    ATH_CHECK(initialize_xAODJet_varHandleKeys(false));
    ATH_CHECK(initialize_JetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_jFexSRJetRoI_varHandleKeys(true));
    ATH_CHECK(initialize_jFexLRJetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_gFexJetRoI_varHandleKeys(false));
  } else if (m_matchType ==  MatchType::jFexLRJetRoI) {
    ATH_CHECK(initialize_xAODJet_varHandleKeys(false));
    ATH_CHECK(initialize_JetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_jFexSRJetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_jFexLRJetRoI_varHandleKeys(true));
    ATH_CHECK(initialize_gFexJetRoI_varHandleKeys(false));
  } else if (m_matchType ==  MatchType::gFexJetRoI) {
    ATH_CHECK(initialize_xAODJet_varHandleKeys(false));
    ATH_CHECK(initialize_JetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_jFexSRJetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_jFexLRJetRoI_varHandleKeys(false));
    ATH_CHECK(initialize_gFexJetRoI_varHandleKeys(true));
  } else {
    ATH_MSG_FATAL("uknown match type");
    return  StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::initialize_xAODJet_varHandleKeys(bool do_it){
  for (auto& key : m_jetVarHandleKeys) {
    ATH_CHECK(key.get().initialize(do_it));
    ATH_CHECK(m_matchedKey.initialize(do_it));
    
  }
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::initialize_JetRoI_varHandleKeys(bool do_it){
  for (auto& key : m_l1JetVarHandleKeys) {
    ATH_CHECK(key.get().initialize(do_it));
    ATH_CHECK(m_l1matchedKey.initialize(do_it));
  }
  return StatusCode::SUCCESS;
}


StatusCode JetMatcherAlg::initialize_jFexSRJetRoI_varHandleKeys(bool do_it){
  for (auto& key : m_l1jFexSRJetVarHandleKeys) {
    ATH_CHECK(key.get().initialize(do_it));
    ATH_CHECK(m_l1jFexSRmatchedKey.initialize(do_it));
 }
  return StatusCode::SUCCESS;
}

StatusCode JetMatcherAlg::initialize_jFexLRJetRoI_varHandleKeys(bool do_it){
  for (auto& key : m_l1jFexLRJetVarHandleKeys) {
    ATH_CHECK(key.get().initialize(do_it));
    ATH_CHECK(m_l1jFexLRmatchedKey.initialize(do_it));
 }
  return StatusCode::SUCCESS;
}


StatusCode JetMatcherAlg::initialize_gFexJetRoI_varHandleKeys(bool do_it){
  for (auto& key : m_l1gFexJetVarHandleKeys) {
    ATH_CHECK(key.get().initialize(do_it));
    ATH_CHECK(m_l1gFexmatchedKey.initialize(do_it));
 }
  return StatusCode::SUCCESS;
}

