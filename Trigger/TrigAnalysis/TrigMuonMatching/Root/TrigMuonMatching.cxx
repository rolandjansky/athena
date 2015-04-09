/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonMatching/TrigMuonMatching.h"

#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#endif // ROOTCORE

#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"

#define MUONMASS 105.65837

namespace Trig {

   TrigMuonMatching::TrigMuonMatching( const std::string& name )
     : asg::AsgMetadataTool( name ),
       m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
   {
     declareProperty( "TriggerTool", m_trigDecTool);
   }

  TrigMuonMatching::~TrigMuonMatching(){
  }

  StatusCode TrigMuonMatching::initialize() {
    // Greet the user:
    ATH_MSG_INFO( "Initialising... " );
    
    ATH_CHECK(m_trigDecTool.retrieve());

    return StatusCode::SUCCESS;
  }

  Bool_t TrigMuonMatching::match(const xAOD::Muon* mu,
				 const std::string &chain,
				 const double mindelR)
  {
    return match(mu->eta(),mu->phi(),chain,mindelR);
  }
  
  Bool_t TrigMuonMatching::matchL1(const xAOD::Muon* mu,
				   const std::string &l1item,
				   const double DelR)
  {
    return matchL1(mu->eta(),mu->phi(),l1item,DelR);
  }

  Double_t TrigMuonMatching::minDelR(const xAOD::Muon* mu,
				     const std::string &chain,
				     const double mindelR)
  {
    Double_t delmin = mindelR;
    EFmuon efmuon,dummy;
    std::string l1item = "";
    return matchedTrackDetail(efmuon, dummy, mu->eta(), mu->phi(), delmin, chain, l1item);
  }

  Double_t TrigMuonMatching::minDelRL1(const xAOD::Muon* mu,
				       const std::string &l1item,
				       const double DelR)
  {
    Double_t l1dr = DelR;
    const xAOD::MuonRoIContainer* muonrois = 0;
    StatusCode sc = evtStore()->retrieve(muonrois,"LVL1MuonRoIs");
    if(!sc){
      ATH_MSG_ERROR("TrigMuonMatching::matchL1 : could not retrieve LVL1MuonRoIs");
      return false;
    }
    Int_t threshold1 = getL1pt(l1item);
    xAOD::MuonRoIContainer::const_iterator muroi_itr = muonrois->begin();
    xAOD::MuonRoIContainer::const_iterator muroi_end = muonrois->end();
    for( ; muroi_itr != muroi_end; ++muroi_itr ) {
      Int_t threshold2 = getL1pt((*muroi_itr)->thrName());
      if(threshold2 >= threshold1){
	Double_t dR = TrigMuonMatching::dR(mu->eta(), mu->phi(), (*muroi_itr)->eta(), (*muroi_itr)->phi());
	if(dR < l1dr){
	  l1dr = dR;
	}
      }
    }
    return l1dr;
  }

  Bool_t TrigMuonMatching::matchDimuon(const xAOD::Muon* mu1,
				       const xAOD::Muon* mu2,
				       const std::string& chain,
				       std::pair<Bool_t, Bool_t>& result1,
				       std::pair<Bool_t, Bool_t>& result2,
				       const Double_t& mindelR)
  {
    TLorentzVector mu_1, mu_2;
    mu_1.SetPtEtaPhiM(mu1->pt(),mu1->eta(),mu1->phi(),MUONMASS);
    mu_2.SetPtEtaPhiM(mu2->pt(),mu2->eta(),mu2->phi(),MUONMASS);
    Bool_t valid = matchDimuon(mu_1,mu_2,chain,result1,result2,mindelR);
    return valid;
  }
  
  Bool_t TrigMuonMatching::match(const double eta,
				 const double phi,
				 const std::string &chain,
				 const double mindelR)
  {    
    if(!m_trigDecTool->isPassed(chain)){
      return false;
    }
    Double_t delmin = mindelR;
    EFmuon efmuon,dummy;

    matchedTrackDetail(efmuon, dummy, eta, phi, delmin, chain, "");
    return efmuon.valid;
  }

  Bool_t TrigMuonMatching::matchL1(const double eta,
				   const double phi,
				   const std::string &l1item,
				   const double DelR)
  {
    if(!m_trigDecTool->isPassed("L1_MU.*")){
      return false;
    }
    Double_t l1dr = DelR;
    const xAOD::MuonRoIContainer* muonrois = 0;
    StatusCode sc = evtStore()->retrieve(muonrois,"LVL1MuonRoIs");
    if(!sc){
      ATH_MSG_ERROR("TrigMuonMatching::matchL1 : could not retrieve LVL1MuonRoIs");
      return false;
    }
    Int_t threshold1 = getL1pt(l1item);
    xAOD::MuonRoIContainer::const_iterator muroi_itr = muonrois->begin();
    xAOD::MuonRoIContainer::const_iterator muroi_end = muonrois->end();
    for( ; muroi_itr != muroi_end; ++muroi_itr ) {
      Int_t threshold2 = getL1pt((*muroi_itr)->thrName());
      if(threshold2 >= threshold1){
	Double_t dR = TrigMuonMatching::dR(eta, phi, (*muroi_itr)->eta(), (*muroi_itr)->phi());
	if(dR < l1dr){
	  return true;
	}
      }
    }
    return false;
  }


  Bool_t TrigMuonMatching::matchDimuon(const TLorentzVector& muon1,
				       const TLorentzVector& muon2,
				       const std::string& chain,
				       std::pair<Bool_t, Bool_t>& result1,
				       std::pair<Bool_t, Bool_t>& result2,
				       const Double_t& mindelR)
  {
    Double_t delmin = mindelR;
    DimuonChainInfo chainInfo(chain);
    if (not decodeDimuonChain(chainInfo)) {
      ATH_MSG_ERROR("TrigMuonMatching : Failed to decode chain " << chain << " matchDimuon can accept only chains named HLT_2muXX.");
      return false;
    }
    
    std::pair<Bool_t, Bool_t> rc12, rc21;
    rc12 = matchDimuon(muon1, muon2, chainInfo, delmin);
    if(chainInfo.isSymmetric){
      rc21.first = rc12.second; rc21.second = rc12.first;
    }
    /*
      else{
      rc21 = matchDimuon(muon2, muon1, chainInfo);
      }
    */
    result1.first = rc12.first; result1.second = rc21.second;
    result2.first = rc21.first; result2.second = rc12.second;
    return true;
  }


  std::pair<Bool_t, Bool_t> TrigMuonMatching::matchDimuon(const TLorentzVector& muon1,
							  const TLorentzVector& muon2,
							  const DimuonChainInfo& chainInfo,
							  const double mindelR)
  {
    EFmuon trkId1, trkId2, dummy;
    const Double_t dr1 = matchedTrackDetail(trkId1, dummy, muon1.Eta(), muon1.Phi(), mindelR, chainInfo.thresholds.first, "");

    // for full scan trigger.
    
    const Double_t dr2 = matchedTrackDetail(trkId2, dummy, muon2.Eta(), muon2.Phi(), mindelR, chainInfo.thresholds.second, "");
    
    if(trkId1.valid && trkId2.valid &&
       isEqual(trkId1.pt, trkId2.pt) &&
       isEqual(trkId1.eta, trkId2.eta) &&
       isEqual(trkId1.phi, trkId2.phi)){
      if(dr1 > dr2){
	matchedTrackDetail(trkId1, trkId2, muon1.Eta(), muon1.Phi(), mindelR, chainInfo.thresholds.first,"");
      }
      else{
	matchedTrackDetail(trkId2, trkId1, muon2.Eta(), muon2.Phi(), mindelR, chainInfo.thresholds.second,"");
      }
    }
    
    return std::pair<Bool_t, Bool_t> (trkId1.valid, trkId2.valid);
  }


  
  double TrigMuonMatching::dR(const double eta1,
			      const double phi1,
			      const double eta2,
			      const double phi2)
  {
    double deta = fabs(eta1 - eta2);
    double dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath:: \
      Pi() - fabs(phi1 - phi2);
    return sqrt(deta*deta + dphi*dphi);
  }
  
  
  int TrigMuonMatching::getL1pt(const std::string& l1item)
  {
    int rc = -1;
    
    std::vector<std::string> tokens;
    tokenize(l1item, tokens, "_");
    std::string pt;
    if (tokens.size() == 1) {
      pt = tokens.at(0);
      
    } else if ((tokens.size() == 2 ) and (tokens.at(0) == "L1")) {
      pt = tokens.at(1);
      
    } else {
      ATH_MSG_ERROR("TrigMuonMatching::getL1pt : cannot parse " << l1item);
      return rc;
    }
    
    const size_t index = pt.find("MU");
    if (index != std::string::npos) {
      std::istringstream iss(pt.substr(index + 2));
      iss >> rc;
    } else {
      ATH_MSG_ERROR("TrigMuonMatching::getL1pt : cannot parse " << l1item);
    }
    
    return rc;
  }
  
  
  void TrigMuonMatching::tokenize(const std::string& str,
				     std::vector<std::string>& tokens,
				     const std::string& delimiters)
  {
    tokens.clear();
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);
    
    while ((std::string::npos != pos) or (std::string::npos != lastPos)) {
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delimiters, pos);
      pos = str.find_first_of(delimiters, lastPos);
    }
  }


  Double_t TrigMuonMatching::matchedTrackDetail(EFmuon& efMuonId,
						const EFmuon& usedEFMuonId,
						const double eta,
						const double phi,
						const double mindelR,
						const std::string& chainForEventTrigger,
						const std::string& l1item)
  {
    efMuonId.valid = false;
    Double_t drmin = mindelR;
    std::string l1trig = l1item;
    
    const std::string eventTrigger = chainForEventTrigger;

    auto cg = m_trigDecTool->getChainGroup(eventTrigger);
    auto fc = cg->features();

#if defined(ASGTOOL_STANDALONE) || defined(XAOD_ANALYSIS)
    auto MuFeatureContainers = fc.containerFeature<xAOD::MuonContainer>();
    //#endif
#else
    const std::vector< Trig::Feature<xAOD::MuonContainer> > MuFeatureContainers = fc.get<xAOD::MuonContainer>();
#endif 
    
    for(auto mucont : MuFeatureContainers){
      for(auto mu : *mucont.cptr()){

	// l1 matching
	
	if(isEqual(usedEFMuonId.pt,  mu->pt()) &&
	   isEqual(usedEFMuonId.eta, mu->eta()) &&
	   isEqual(usedEFMuonId.phi, mu->phi())) continue;
	
	Double_t dr = dR(eta,phi,mu->eta(),mu->phi());
	if(drmin > dr){
	  drmin = dr;
	  efMuonId.pt = mu->pt();
	  efMuonId.eta = mu->eta();
	  efMuonId.phi = mu->phi();
	  efMuonId.valid = true;
	}
      }
    }
    return drmin;
  }
  
  /*
  double TrigMuonMatching::matchedTrackDetail(const double eta,
						 const double phi,
						 const std::string chain,
						 const std::string l1item)
  {
    auto cg = m_trigDecTool->getChainGroup(chain);
    auto fc = cg->features();
    auto MuFeatureContainers = fc.containerFeature<xAOD::MuonContainer>();
    double dR_min = m_matchingDr;
    for(auto mucont : MuFeatureContainers){
    
    	// l1 item matching 
    
      for(auto mu : *mucont.cptr()){
    	
    	// l1 item matching 
    	double dr = dR(eta,phi,mu->eta(),mu->phi());
	if(dr < dR_min ) dR_min = dr;
      }
    }
    return dR_min;
  }
  */

  bool TrigMuonMatching::decodeDimuonChain(DimuonChainInfo& chainInfo)
  {
    chainInfo.isValid = false;
    std::map<std::string, DimuonChainInfo >::const_iterator p = m_DimuonChainMap.find(chainInfo.chain);
    if (p != m_DimuonChainMap.end()) {
      chainInfo = p->second;
      return chainInfo.isValid;
    }
    std::vector<std::string> tokens;
    tokenize(chainInfo.chain, tokens, "_");
    if (tokens.size() < 2) return false;
    if (tokens[0] != "HLT") return false;
    chainInfo.isSymmetric = (tokens[1].substr(0, 3) == "2mu");
    
    if(chainInfo.isSymmetric) {
      std::string threshold = std::string("HLT_" + tokens[1].substr(1));
      chainInfo.thresholds.first = threshold;
      chainInfo.thresholds.second = threshold;
      chainInfo.isValid = true;
      //if (tokens.size() == 3) chainInfo.tightness = tokens[2];
    }
    else {
      /*
      if (tokens.size() < 4) return false;
      if (tokens[3] == "EFFS") {  // 2011
	if ((tokens[1].substr(0,2) != "mu") or (tokens[2].substr(0,2) != "mu")) return false;
	std::string threshold1 = std::string("EF_" + tokens[1]);
	std::string threshold2 = std::string("EF_" + tokens[2]);
	chainInfo.isEFFS = true;
	chainInfo.thresholds.first = threshold1;
	chainInfo.thresholds.second = threshold2;
	if (tokens.size() == 5) chainInfo.tightness = tokens[4];
	chainInfo.l2seed = "L2_" + tokens[1] + (chainInfo.tightness.empty() ? "" : "_" + chainInfo.tightness);
	chainInfo.is2011 = true;
	chainInfo.isValid = true; 
	
      } else if (tokens[4] == "EFFS") { // 2012
	
	if ((tokens[1].substr(0,2) != "mu") or (tokens[3].substr(0,2) != "mu")) return false;
	std::string threshold1 = std::string("EF_" + tokens[1]);
	std::string threshold2 = std::string("EF_" + tokens[3]);
	chainInfo.isEFFS = true;
	chainInfo.thresholds.first = threshold1;
	chainInfo.thresholds.second = threshold2;
	if (tokens.size() == 5) chainInfo.tightness = tokens[2];
	chainInfo.efseed = "EF_" + tokens[1] + (chainInfo.tightness.empty() ? "" : "_" + chainInfo.tightness);
	chainInfo.is2011 = false;
	chainInfo.isValid = true; 
      */
      //}
      chainInfo.isValid = false;
      return chainInfo.isValid;
    }
    m_DimuonChainMap[chainInfo.chain] = chainInfo;
  return chainInfo.isValid;
  }
  
  bool TrigMuonMatching::isEqual(const double x,
				 const double y)
  {
    if (fabs(x - y) < std::numeric_limits<float>::epsilon()) return true;
    return false;
  }
  
} // namespace CP
