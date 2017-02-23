/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define __TrigInfo_cxx__
#include "MuonPtCalibNtupleMaker/TrigInfo.h"

TrigInfo::TrigInfo() {

  std::map< std::string, int > triggers = { { "HLT_mu20_iloose_L1MU15", 1 }, 
                                            { "HLT_mu24_iloose_L1MU15", 1 }, 
                                            { "HLT_mu24_imedium", 1 }, 
                                            { "HLT_mu26_imedium", 1 }, 
                                            { "HLT_mu50", 1 }, { "HLT_2mu14", 2 }, 
                                            { "HLT_2mu4", 2 }, 
                                            { "HLT_2mu4_bJpsimumu", 2 }, 
                                            { "HLT_2mu4_bUpsimumu", 2 },
                                            {"HLT_2mu6_bJpsimumu",2},
                                            //                                            {"HLT_mu6_mu4_bJpsimumu",2},
                                            {"HLT_mu4_bJpsi_Trkloose",-2},
                                            {"HLT_mu10_bJpsi_Trkloose",-2},
                                            {"HLT_mu18_bJpsi_Trkloose",-2},
                                            {"HLT_2mu6_bUpsimumu",2},
                                            {"HLT_2mu6_bJpsimumu",2},
                                            {"HLT_mu24_ivarmedium",1},
                                            {"HLT_2mu10",2},
                                            {"HLT_2mu10_nomucomb",2},
                                            {"HLT_2mu14",2},
                                            {"HLT_2mu14_nomucomb",2},
                                            {"HLT_2mu15",2},
                                            {"HLT_2mu15_nomucomb",2},
                                            {"HLT_mu18_mu8noL1",2},
                                            {"HLT_mu20_mu8noL1",2},
                                            {"HLT_mu22_mu8noL1",2},
                                            {"HLT_mu24_mu8noL1",2},
                                            {"HLT_mu26_mu8noL1",2},
                                            {"HLT_mu20_iloose_L1MU15",1},
                                            {"HLT_mu20_ivarloose_L1MU15",1},
                                            {"HLT_mu24_iloose_L1MU15",1},
                                            {"HLT_mu24_ivarloose_L1MU15",1},
                                            {"HLT_mu24_iloose",1},
                                            {"HLT_mu24_imedium",1},
                                            {"HLT_mu24_ivarloose",1},
                                            {"HLT_mu24_ivarmedium",1},
                                            {"HLT_mu26_imedium",1},
                                            {"HLT_mu26_ivarmedium",1},
                                            {"HLT_mu28_imedium",1},
                                            {"HLT_mu28_ivarmedium",1}
                                            
  };
  std::map< std::string, int >::iterator it;
  for( it = triggers.begin(); it != triggers.end(); ++it ) {
    m_Info[ it->first ] = new TrigItem( it->first, it->second );
  }
  Reset();

}

void TrigInfo::Reset() {

  for( auto& kv: m_Info ) ( kv.second )->Reset();

}

void TrigInfo::Register( TTree* t ) {

  for( auto& kv: m_Info ) {
    ( kv.second )->Register( t );
  }

}

void TrigInfo::Fill( ToolHandle<Trig::TrigDecisionTool>& tdt ) {
   
  for( auto& kv: m_Info ) ( kv.second )->SetDecision( tdt->isPassed( kv.first ) );

}

void TrigInfo::FillMatching( ToolHandle< Trig::ITrigMuonMatching >& mt, const xAOD::Muon* mu1, const xAOD::Muon* mu2 ) {

  for( auto& kv: m_Info ) {
    TrigItem* item = kv.second;
    if( item->GetNumberOfMuons() == 1 ) {
      bool mu1_match = mt->match( mu1, ( kv.first ).c_str() );
      bool mu2_match = mt->match( mu2, ( kv.first ).c_str() );
      item->SetMatching( ( Int_t ) ( mu1_match || mu2_match ) );
    }
    else if( item->GetNumberOfMuons() == 2 ) {
      std::pair< Bool_t, Bool_t > result1, result2;
      mt->matchDimuon( mu1, mu2, kv.first, result1, result2 );
      item->SetMatching( ( Int_t ) ( ( result1.first && result2.second ) || ( result2.first && result1.second ) ) );
    }
    else { 
      item->SetMatching( -1 ); 
    }
  }

}
