/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEFFICIENCY_IMUONEFFICIENCYTOOL_H
#define TRIGMUONEFFICIENCY_IMUONEFFICIENCYTOOL_H

#include "AsgTools/IAsgTool.h"

#include "xAODMuon/Muon.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

struct DimuonChainInfo {
public:
  std::string chain;
  std::pair<std::string, std::string> thresholds;
  bool isEFFS;
  bool isSymmetric;
  bool isValid;
  
DimuonChainInfo(const std::string& chain_="") :
  chain(chain_), thresholds(), isEFFS(false), isSymmetric(false), isValid(false) {}

};


namespace Trig {
  
  class ITrigMuonMatching : public virtual asg::IAsgTool {
    
    ASG_TOOL_INTERFACE( Trig::ITrigMuonMatching )
      
      public:
    virtual StatusCode initialize(void) = 0;
    
    virtual Bool_t match(const xAOD::Muon* mu,
			 const std::string &chain,
			 const double mindelR = 0.1) = 0;
    
    virtual Bool_t matchL1(const xAOD::Muon* mu,
			   const std::string &l1item,
			   const double DelR = 0.2) = 0;

    virtual Bool_t matchL2SA(const xAOD::Muon* mu,
			     const std::string &l1item,
			     const std::string & chain,
			     const double DelR = 0.2) = 0;

    virtual Bool_t matchL2CB(const xAOD::Muon* mu,
			     const std::string & chain,
			     const double DelR = 0.2) = 0;

    virtual Double_t minDelR(const xAOD::Muon* mu,
			     const std::string &chain,
			     const double mindelR = 0.1) = 0;
    
    virtual Double_t minDelRL1(const xAOD::Muon* mu,
			       const std::string &l1item,
			       const double DelR = 0.2) = 0;
    
    virtual Bool_t matchDimuon(const xAOD::Muon* mu1,
			       const xAOD::Muon* mu2,
			       const std::string& chain,
			       std::pair<Bool_t, Bool_t>& result1,
			       std::pair<Bool_t, Bool_t>& result2,
			       const Double_t& mindelR = 0.1) = 0;
    
    virtual Bool_t match(const double eta,
			 const double phi,
			 const std::string &chain,
			 const double mindelR = 0.1) = 0;
    
    virtual Bool_t matchL1(const double eta,
			   const double phi,
			   const std::string &l1item,
			   const double DelR = 0.2) = 0;

    virtual Bool_t matchDimuon(const TLorentzVector& muon1,
			       const TLorentzVector& muon2,
			       const std::string& chain,
			       std::pair<Bool_t, Bool_t>& result1,
			       std::pair<Bool_t, Bool_t>& result2,
			       const Double_t& mindelR = 0.1) = 0;

    virtual Bool_t isPassedRerun(const std::string& trigger) = 0;
    
   }; 

} 

#endif 
