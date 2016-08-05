/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEFFICIENCY_MUONEFFICIENCYTOOL_H
#define TRIGMUONEFFICIENCY_MUONEFFICIENCYTOOL_H


#include "TrigMuonMatching/ITrigMuonMatching.h"

#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include <iostream>

#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig {

  class TrigMuonMatching : 
    public asg::AsgMetadataTool,
    public virtual ITrigMuonMatching
    {
      ASG_TOOL_INTERFACE(Trig::TrigMuonMatching)
      ASG_TOOL_CLASS2( TrigMuonMatching, Trig::ITrigMuonMatching, Trig::TrigMuonMatching )
      
   public:

      TrigMuonMatching( const std::string& name );

      virtual ~TrigMuonMatching();

      virtual StatusCode initialize(void);
      
      virtual Bool_t match(const xAOD::Muon* mu,
			   const std::string &chain,
			   const double mindelR = 0.1);
      
      virtual Bool_t matchL1(const xAOD::Muon* mu,
			     const std::string &l1item,
			     const double DelR = 0.2);

      virtual Bool_t matchL2SA(const xAOD::Muon* mu,
			       const std::string &l1item,
			       const std::string & chain,
			       const double DelR = 0.2);

      virtual Bool_t matchL2CB(const xAOD::Muon* mu,
			       const std::string & chain,
			       const double DelR = 0.2);
      
      virtual Double_t minDelR(const xAOD::Muon* mu,
			       const std::string &chain,
			       const double mindelR = 0.1);
      
      virtual Double_t minDelRL1(const xAOD::Muon* mu,
				 const std::string &l1item,
				 const double DelR = 0.2);
      
      virtual Bool_t matchDimuon(const xAOD::Muon* mu1,
				 const xAOD::Muon* mu2,
				 const std::string& chain,
				 std::pair<Bool_t, Bool_t>& result1,
				 std::pair<Bool_t, Bool_t>& result2,
				 const Double_t& mindelR = 0.1);
      
      virtual Bool_t match(const double eta,
			   const double phi,
			   const std::string &chain,
			   const double mindelR = 0.1);
      
      virtual Bool_t matchL1(const double eta,
			     const double phi,
			     const std::string &l1item,
			     const double DelR = 0.2);
      
      virtual Bool_t matchDimuon(const TLorentzVector& muon1,
				 const TLorentzVector& muon2,
				 const std::string& chain,
				 std::pair<Bool_t, Bool_t>& result1,
				 std::pair<Bool_t, Bool_t>& result2,
				 const Double_t& mindelR = 0.1);

      virtual Bool_t isPassedRerun(const std::string& trigger);
      
      struct EFmuon {
	bool valid;
	float pt;
	float eta;
	float phi;
	
      EFmuon() : valid(false), pt(-1.e30), eta(-1.e30), phi(-1.e30) {}
      };
      
    private:

      ToolHandle<Trig::TrigDecisionTool> m_trigDecTool;

      std::map<std::string, DimuonChainInfo> m_DimuonChainMap;

      std::pair<bool,bool> matchDimuon(const TLorentzVector& muon1,
				       const TLorentzVector& muon2,
				       const DimuonChainInfo& chainInfo,
				       const double mindelR);

      double dR(const double eta1,
		const double phi1,
		const double eta2,
		const double phi2);

      int getL1pt(const std::string& l1item);
      
      void tokenize(const std::string& str,
		    std::vector<std::string>& tokens,
		    const std::string& delimiters);
      
      /*
      double matchedTrackDetail(const double eta,
				const double phi,
				const std::string chain,
				const std::string l1item);
      */
      
      Double_t matchedTrackDetail(EFmuon& efMuonId,
				  const EFmuon& usedEFMuonId,
				  const double eta,
				  const double phi,
				  const double mindelR,
				  const std::string& chainEventTrigger);
      
      bool decodeDimuonChain(DimuonChainInfo& chainInfo);

      bool isEqual(const double x,
		   const double y);
      
      
    }; 
} 

#endif 
