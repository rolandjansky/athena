/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "TrigMuonMatching_example.h"


#include <cmath>

namespace Trig {

   TrigMuonMatching_example::TrigMuonMatching_example( const std::string& name, ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
	m_matchTool("CP::TrigMuonMatching/TrigMuonMatching",this)
   {
     declareProperty("MuonMatchTool",m_matchTool);
   }

   StatusCode TrigMuonMatching_example::initialize()
   {

     ATH_CHECK(m_matchTool.retrieve());
      
      return StatusCode::SUCCESS;
   }


  StatusCode TrigMuonMatching_example::execute()
  {

    const xAOD::MuonContainer* muons = 0;
    ATH_CHECK( evtStore()->retrieve( muons, "Muons" ) );
    ATH_MSG_INFO( "Number of muons: " << muons->size() );

    xAOD::MuonContainer *goodMuon = new xAOD::MuonContainer;
    xAOD::MuonAuxContainer *goodMuonAux = new xAOD::MuonAuxContainer;
    goodMuon -> setStore(goodMuonAux);
    CHECK( evtStore()->record(goodMuon,"GoodMuon"));
    CHECK( evtStore()->record(goodMuonAux,"GoodMuonAux."));

    for(const auto* mu: *muons){
      Bool_t ismu26_imedium = false;
      Bool_t ismu50 = false;
      Bool_t isL2SA = false;
      Bool_t isL2CB = false;
      Bool_t isL1 = false;
      ismu26_imedium = m_matchTool->match(mu,"HLT_mu26_imedium");
      ismu50 = m_matchTool->match(mu,"HLT_mu50");
      isL2SA = m_matchTool->matchL2SA(mu, "L1_MU20", "HLT_mu26_imedium");
      isL2CB = m_matchTool->matchL2CB(mu, "HLT_mu26_imedium");
      isL1 = m_matchTool->matchL1(mu,"L1_MU20");
      if(ismu26_imedium)std::cout << "HLT_mu26_imedium matched." << std::endl;
      if(ismu50)std::cout << "HLT_mu50 matched." << std::endl;
      if(isL2SA) std::cout << "L2 SA matched" << std::endl;
      if(isL2CB) std::cout << "L2 CB matched" << std::endl;
      if(isL1) std::cout << "L1_MU20 matched" << std::endl;
      
      Double_t dR = m_matchTool->minDelR(mu,"HLT_mu26_imedium");
      std::cout << "DeltaR(mu26_imedium) " << dR << std::endl;
      Double_t dRL1 = m_matchTool->minDelRL1(mu,"L1_MU20");
      std::cout << "DeltaR(L1_MU20) " << dRL1 << std::endl;

      xAOD::Muon* newMuon = new xAOD::Muon;
      goodMuon->push_back(newMuon);
      *newMuon = *mu;
    }
    
    const xAOD::MuonContainer* SelectedMuons = 0;
    ATH_CHECK(evtStore()->retrieve(SelectedMuons,"GoodMuon"));
    ATH_MSG_INFO( "Number of selected muons: " << SelectedMuons->size() );
    if(SelectedMuons->size()==2){
      const xAOD::Muon *mu1 = (*SelectedMuons)[0];
      const xAOD::Muon *mu2 = (*SelectedMuons)[1];
      if (mu1 && mu2) {
        std::pair<Bool_t,Bool_t> result1, result2;
        m_matchTool->matchDimuon(mu1, mu2, "HLT_2mu14", result1, result2);
        if(result1.first && result2.first)std::cout << "HLT_2mu14 matched" << std::endl;
      }
    }
    
    return StatusCode::SUCCESS;
  }
  
}
