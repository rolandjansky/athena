/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigEgammaAnalysisTools/TrigEgammaPhysValBase.h"
using namespace std;

TrigEgammaPhysValBase::TrigEgammaPhysValBase(){
 
  m_el_ringsE=nullptr;
  m_trig_L1_thrNames=nullptr;          
  m_trig_L2_calo_energySample=nullptr;
  m_trig_L2_calo_rings=nullptr;
  m_trig_L2_el_trackAlgID=nullptr;
  m_trig_L2_el_pt=nullptr;
  m_trig_L2_el_eta=nullptr;
  m_trig_L2_el_caloEta=nullptr;
  m_trig_L2_el_phi=nullptr;
  m_trig_L2_el_charge=nullptr;
  m_trig_L2_el_nTRTHits=nullptr;
  m_trig_L2_el_nTRTHiThresholdHits=nullptr;
  m_trig_L2_el_etOverPt=nullptr;
  m_trig_L2_el_trkClusDeta=nullptr;
  m_trig_L2_el_trkClusDphi=nullptr;
  m_trig_EF_calo_et=nullptr;
  m_trig_EF_calo_eta=nullptr;
  
  m_doMCDetailed=false;
  m_doL2Detailed=false;
  m_doL2CaloDetailed=false;
  m_doRecoDetailed=false;
}

/*
 * book, link, clear, alloc and release method divide in:
 * trigger, Egamma and MonteCarlo data base.
 */
template <class T> 
void TrigEgammaPhysValBase::InitBranch(TTree* fChain, std::string branch_name, T* param, bool message){

  std::string bname = branch_name;
  if (fChain->GetAlias(bname.c_str()))
     bname = std::string(fChain->GetAlias(bname.c_str()));

  if (!fChain->FindBranch(bname.c_str()) && message) {
    return;  
  }
  fChain->SetBranchStatus(bname.c_str(), 1.);
  fChain->SetBranchAddress(bname.c_str(), param);
}

void TrigEgammaPhysValBase::bookEventBranches(TTree *t){
  
  t->Branch("RunNumber",        &m_runNumber);
  t->Branch("EventNumber",      &m_eventNumber);
  t->Branch("LumiBlock",        &m_lumiBlock);
  t->Branch("avgmu",            &m_avgmu);
}

void TrigEgammaPhysValBase::bookTriggerBranches(TTree *t){

  // Level L1 cluster
  t->Branch( "trig_L1_eta",             &m_trig_L1_eta);
  t->Branch( "trig_L1_phi",             &m_trig_L1_phi);
  t->Branch( "trig_L1_emClus",          &m_trig_L1_emClus);
  t->Branch( "trig_L1_tauClus",         &m_trig_L1_tauClus);
  t->Branch( "trig_L1_emIsol",          &m_trig_L1_emIsol);
  t->Branch( "trig_L1_hadIsol",         &m_trig_L1_hadIsol);
  t->Branch( "trig_L1_hadCore",         &m_trig_L1_hadCore);
  t->Branch( "trig_L2_calo_et",         &m_trig_L2_calo_et);
  t->Branch( "trig_L2_calo_eta",        &m_trig_L2_calo_eta);
  t->Branch( "trig_L2_calo_phi",        &m_trig_L2_calo_phi);
  
  if(m_doL2CaloDetailed){
    t->Branch( "trig_L2_calo_e237",       &m_trig_L2_calo_e237 );
    t->Branch( "trig_L2_calo_e277",       &m_trig_L2_calo_e277 );
    t->Branch( "trig_L2_calo_fracs1",     &m_trig_L2_calo_fracs1);
    t->Branch( "trig_L2_calo_weta2",      &m_trig_L2_calo_weta2);
    t->Branch( "trig_L2_calo_ehad1",      &m_trig_L2_calo_ehad1);
    t->Branch( "trig_L2_calo_emaxs1",     &m_trig_L2_calo_emaxs1);
    t->Branch( "trig_L2_calo_e2tsts1",    &m_trig_L2_calo_e2tsts1);
    t->Branch( "trig_L2_calo_wstot",      &m_trig_L2_calo_wstot);
    t->Branch( "trig_L2_calo_energySample",&m_trig_L2_calo_energySample);
  }

  t->Branch( "trig_L2_calo_rings",      &m_trig_L2_calo_rings ); 
  t->Branch( "trig_L2_calo_rnnOutput",  &m_trig_L2_calo_rnnOutput ); 
 

  if(m_doL2Detailed){
    t->Branch( "trig_L2_el_pt" ,                 &m_trig_L2_el_pt );
    t->Branch( "trig_L2_el_eta",                 &m_trig_L2_el_eta);
    t->Branch( "trig_L2_el_phi",                 &m_trig_L2_el_phi );
    t->Branch( "trig_L2_el_caloEta",             &m_trig_L2_el_caloEta );
    t->Branch( "trig_L2_el_charge",              &m_trig_L2_el_charge);
    t->Branch( "trig_L2_el_nTRTHits",            &m_trig_L2_el_nTRTHits);
    t->Branch( "trig_L2_el_nTRTHiThresholdHits", &m_trig_L2_el_nTRTHiThresholdHits);
    t->Branch( "trig_L2_el_etOverPt" ,           &m_trig_L2_el_etOverPt );
    t->Branch( "trig_L2_el_trkClusDeta" ,        &m_trig_L2_el_trkClusDeta );
    t->Branch( "trig_L2_el_trkClusDphi" ,        &m_trig_L2_el_trkClusDphi );
    t->Branch( "trig_L2_el_trackAlgID" ,         &m_trig_L2_el_trackAlgID );
  }

  t->Branch( "trig_EF_calo_e",            &m_trig_EF_calo_e       );
  t->Branch( "trig_EF_calo_et",           &m_trig_EF_calo_et      );
  t->Branch( "trig_EF_calo_eta",          &m_trig_EF_calo_eta     );
  t->Branch( "trig_EF_calo_phi",          &m_trig_EF_calo_phi     );
  t->Branch( "trig_EF_calo_etaBE2",       &m_trig_EF_calo_etaBE2  );

#define BRANCH(_name_) t->Branch( #_name_ , & m_##_name_ );\

  BRANCH( trig_EF_calo_tight                      );
  BRANCH( trig_EF_calo_medium                     );
  BRANCH( trig_EF_calo_loose                      );
  BRANCH( trig_EF_calo_lhtight                    );
  BRANCH( trig_EF_calo_lhmedium                   );
  BRANCH( trig_EF_calo_lhloose                    );
  BRANCH( trig_EF_calo_lhvloose                   );
 
  BRANCH( trig_EF_el_calo_e                       ); 
  BRANCH( trig_EF_el_calo_et                      ); 
  BRANCH( trig_EF_el_calo_eta                     ); 
  BRANCH( trig_EF_el_calo_phi                     ); 
  BRANCH( trig_EF_el_calo_etaBE2                  ); 

  BRANCH( trig_EF_el_hasCalo                      ); 
  BRANCH( trig_EF_el_hasTrack                     ); 
  BRANCH( trig_EF_el_e                            ); 
  BRANCH( trig_EF_el_et                           ); 
  BRANCH( trig_EF_el_eta                          ); 
  BRANCH( trig_EF_el_phi                          ); 
  BRANCH( trig_EF_el_e277                         ); 
  BRANCH( trig_EF_el_ethad1                       ); 
  BRANCH( trig_EF_el_ehad1                        ); 
  BRANCH( trig_EF_el_f1                           ); 
  BRANCH( trig_EF_el_f3                           ); 
  BRANCH( trig_EF_el_f1core                       ); 
  BRANCH( trig_EF_el_f3core                       ); 
  BRANCH( trig_EF_el_weta1                        ); 
  BRANCH( trig_EF_el_weta2                        ); 
  BRANCH( trig_EF_el_wtots1                       ); 
  BRANCH( trig_EF_el_fracs1                       ); 
  BRANCH( trig_EF_el_Reta                         ); 
  BRANCH( trig_EF_el_Rphi                         ); 
  BRANCH( trig_EF_el_Eratio                       ); 
  BRANCH( trig_EF_el_Rhad                         ); 
  BRANCH( trig_EF_el_Rhad1                        ); 
  BRANCH( trig_EF_el_deta2                        ); 
  BRANCH( trig_EF_el_dphi2                        ); 
  BRANCH( trig_EF_el_dphiresc                     ); 
  BRANCH( trig_EF_el_deltaPhiRescaled2            ); 
  BRANCH( trig_EF_el_deltaEta1                    ); 
  BRANCH( trig_EF_el_deltaE                       ); 
  BRANCH( trig_EF_el_etCone                       ); 
  BRANCH( trig_EF_el_ptCone                       ); 
 
  BRANCH( trig_EF_el_trk_pt                           ); 
  BRANCH( trig_EF_el_trk_eta                         ); 
  BRANCH( trig_EF_el_trk_charge                       ); 
  BRANCH( trig_EF_el_trk_qOverP                       ); 
  BRANCH( trig_EF_el_trk_sigd0                        ); 
  BRANCH( trig_EF_el_trk_d0                           ); 
  BRANCH( trig_EF_el_trk_eProbabilityHT               ); 
  BRANCH( trig_EF_el_trk_transformed_eProbabilityHT   ); 
  BRANCH( trig_EF_el_trk_d0significance               ); 
  BRANCH( trig_EF_el_trk_deltaPOverP                  ); 
  BRANCH( trig_EF_el_trk_summaryValues                ); 
  
  BRANCH( trig_EF_el_tight                        );
  BRANCH( trig_EF_el_medium                       );
  BRANCH( trig_EF_el_loose                        );
  BRANCH( trig_EF_el_lhtight                      );
  BRANCH( trig_EF_el_lhmedium                     );
  BRANCH( trig_EF_el_lhloose                      );
  BRANCH( trig_EF_el_lhvloose                     );

  

#undef BRANCH



}



void TrigEgammaPhysValBase::bookElectronBranches(TTree *t){
  
  t->Branch("el_calo_e",                &m_el_calo_e);
  t->Branch("el_calo_et",               &m_el_calo_et);
  t->Branch("el_calo_eta",              &m_el_calo_eta);
  t->Branch("el_calo_etaBE2",           &m_el_calo_etaBE2);
  t->Branch("el_calo_phi",              &m_el_calo_phi);
 
  t->Branch("el_hasCalo",               &m_el_hasCalo   );
  t->Branch("el_hasTrack",              &m_el_hasTrack  );
  t->Branch("el_e",                     &m_el_e         );
  t->Branch("el_et",                    &m_el_et        );
  t->Branch("el_eta",                   &m_el_eta       );
  t->Branch("el_phi",                   &m_el_phi       );
 
  if(m_doRecoDetailed){
    t->Branch("el_ethad1",                &m_el_ethad1);
    t->Branch("el_ehad1",                 &m_el_ehad1);
    t->Branch("el_e277",                  &m_el_e277);
    t->Branch("el_f1",                    &m_el_f1);
    t->Branch("el_f3",                    &m_el_f3);
    t->Branch("el_f1core",                &m_el_f1core);
    t->Branch("el_f3core",                &m_el_f3core);
    t->Branch("el_weta1",                 &m_el_weta1);
    t->Branch("el_weta2",                 &m_el_weta2);
    t->Branch("el_wtots1",                &m_el_wtots1);
    t->Branch("el_fracs1",                &m_el_fracs1);
    t->Branch("el_Reta",                  &m_el_Reta);
    t->Branch("el_Rphi",                  &m_el_Rphi);
    t->Branch("el_Eratio",                &m_el_Eratio);
    t->Branch("el_Rhad",                  &m_el_Rhad);
    t->Branch("el_Rhad1",                 &m_el_Rhad1);
    t->Branch("el_deta2",                 &m_el_deta2);
    t->Branch("el_dphi2",                 &m_el_dphi2);
    t->Branch("el_dphiresc",              &m_el_dphiresc);
    t->Branch("el_deltaPhiRescaled2"         , &m_el_deltaPhiRescaled2  );
    t->Branch("el_deltaEta1"                 , &m_el_deltaEta1          );
    t->Branch("el_deltaE",                &m_el_deltaE    );
    t->Branch("el_etCone"                 , &m_el_etCone                 );
    t->Branch("el_ptCone"                 , &m_el_ptCone                 );

 
    t->Branch("el_trk_pt",                         &m_el_trk_pt        );
    t->Branch("el_trk_eta",                        &m_el_trk_eta   );
    t->Branch("el_trk_charge",                     &m_el_trk_charge);
    t->Branch("el_trk_qOverP",                     &m_el_trk_qOverP);
    t->Branch("el_trk_sigd0"                     , &m_el_trk_sigd0              );
    t->Branch("el_trk_d0"                        , &m_el_trk_d0                 );
    t->Branch("el_trk_eProbabilityHT"            , &m_el_trk_eProbabilityHT     );
    t->Branch("el_trk_transformed_eProbabilityHT", &m_el_trk_transformed_eProbabilityHT);
    t->Branch("el_trk_d0significance"            , &m_el_trk_d0significance     );
    t->Branch("el_trk_deltaPOverP"               , &m_el_trk_deltaPOverP        );
    t->Branch("el_trk_summaryValues"             , &m_el_trk_summaryValues        );
  }

  t->Branch("el_ringsE",                &m_el_ringsE );
  t->Branch("el_nGoodVtx",              &m_el_nGoodVtx);
  t->Branch("el_nPileupPrimaryVtx",     &m_el_nPileupPrimaryVtx);


  t->Branch("el_loose",                 &m_el_loose  );
  t->Branch("el_medium",                &m_el_medium );
  t->Branch("el_tight",                 &m_el_tight );
  t->Branch("el_lhvloose",              &m_el_lhVLoose );
  t->Branch("el_lhloose",               &m_el_lhLoose );
  t->Branch("el_lhmedium",              &m_el_lhMedium ); 
  t->Branch("el_lhtight",               &m_el_lhTight ); 
  t->Branch("el_multiLepton",           &m_el_multiLepton);



}


void TrigEgammaPhysValBase::createTDTMetadata( TTree *t, std::vector<std::string> trigList ){

  std::vector<std::string> *trig_tdt_triggerList;
  t->Branch("trig_tdt_triggerList", &trig_tdt_triggerList );
  trig_tdt_triggerList = new std::vector<std::string>();
  for(auto& trigger : trigList ){
    trig_tdt_triggerList->push_back(trigger);
  }
  t->Fill();
  delete trig_tdt_triggerList;
}


void TrigEgammaPhysValBase::bookTDTBranches(TTree *t){

  t->Branch("trig_tdt_L1_calo_accept",       &m_trig_tdt_L1_calo_accept);
  t->Branch("trig_tdt_L2_calo_accept",       &m_trig_tdt_L2_calo_accept);
  t->Branch("trig_tdt_L2_el_accept",         &m_trig_tdt_L2_el_accept  );
  t->Branch("trig_tdt_EF_calo_accept",       &m_trig_tdt_EF_calo_accept);
  t->Branch("trig_tdt_EF_el_accept",         &m_trig_tdt_EF_el_accept  );
  t->Branch("trig_tdt_emu_L1_calo_accept",   &m_trig_tdt_emu_L1_calo_accept);
  t->Branch("trig_tdt_emu_L2_calo_accept",   &m_trig_tdt_emu_L2_calo_accept);
  t->Branch("trig_tdt_emu_L2_el_accept",     &m_trig_tdt_emu_L2_el_accept  );
  t->Branch("trig_tdt_emu_EF_calo_accept",   &m_trig_tdt_emu_EF_calo_accept);
  t->Branch("trig_tdt_emu_EF_el_accept",     &m_trig_tdt_emu_EF_el_accept  );
 
}


  
void TrigEgammaPhysValBase::bookMonteCarloBranches(TTree *t){
  // Monte Carlo
  t->Branch("mc_hasMC",       &m_mc_hasMC);
  t->Branch("mc_pt",          &m_mc_pt);
  t->Branch("mc_eta",         &m_mc_eta);
  t->Branch("mc_phi",         &m_mc_phi);
  
  if(m_doMCDetailed){
    t->Branch("mc_isTop",       &m_mc_isTop);
    t->Branch("mc_isParton",    &m_mc_isParton);
    t->Branch("mc_isMeson",     &m_mc_isMeson);
    t->Branch("mc_isTau",       &m_mc_isTau);
    t->Branch("mc_isMuon",      &m_mc_isMuon);
  }
  
  t->Branch("mc_isPhoton",    &m_mc_isPhoton);
  t->Branch("mc_isElectron",  &m_mc_isElectron);
  t->Branch("mc_hasZMother",  &m_mc_hasZMother);
  t->Branch("mc_hasWMother",  &m_mc_hasWMother);
}


void TrigEgammaPhysValBase::linkTDTBranches(TTree *t){

  InitBranch( t, "trig_tdt_L1_calo_accept",       &m_trig_tdt_L1_calo_accept);
  InitBranch( t, "trig_tdt_L2_calo_accept",       &m_trig_tdt_L2_calo_accept);
  InitBranch( t, "trig_tdt_L2_el_accept",         &m_trig_tdt_L2_el_accept  );
  InitBranch( t, "trig_tdt_EF_calo_accept",       &m_trig_tdt_EF_calo_accept);
  InitBranch( t, "trig_tdt_EF_el_accept",         &m_trig_tdt_EF_el_accept  );
  InitBranch( t, "trig_tdt_emu_L1_calo_accept",   &m_trig_tdt_emu_L1_calo_accept);
  InitBranch( t, "trig_tdt_emu_L2_calo_accept",   &m_trig_tdt_emu_L2_calo_accept);
  InitBranch( t, "trig_tdt_emu_L2_el_accept",     &m_trig_tdt_emu_L2_el_accept  );
  InitBranch( t, "trig_tdt_emu_EF_calo_accept",   &m_trig_tdt_emu_EF_calo_accept);
  InitBranch( t, "trig_tdt_emu_EF_el_accept",     &m_trig_tdt_emu_EF_el_accept  );
 
}



void TrigEgammaPhysValBase::linkEventBranches(TTree *t){
  
  InitBranch( t, "RunNumber",        &m_runNumber);
  InitBranch( t, "EventNumber",      &m_eventNumber);
  InitBranch( t, "LumiBlock",        &m_lumiBlock);
  InitBranch( t, "avgmu",            &m_avgmu);
}

void TrigEgammaPhysValBase::linkElectronBranches( TTree *t ){
  
  InitBranch( t, "el_calo_e",                &m_el_calo_e);
  InitBranch( t, "el_calo_et",               &m_el_calo_et);
  InitBranch( t, "el_calo_eta",              &m_el_calo_eta);
  InitBranch( t, "el_calo_etaBE2",           &m_el_calo_etaBE2);
  InitBranch( t, "el_calo_phi",              &m_el_calo_phi);
  
  InitBranch( t, "el_hasCalo",               &m_el_hasCalo);
  InitBranch( t, "el_hasTrack",              &m_el_hasTrack);
  InitBranch( t, "el_e",                     &m_el_e);
  InitBranch( t, "el_et",                    &m_el_et);
  InitBranch( t, "el_eta",                   &m_el_eta);
  InitBranch( t, "el_phi",                   &m_el_phi);


  if(m_doRecoDetailed){
    InitBranch( t, "el_ethad1",                &m_el_ethad1);
    InitBranch( t, "el_ehad1",                 &m_el_ehad1);
    InitBranch( t, "el_f1",                    &m_el_f1);
    InitBranch( t, "el_f3",                    &m_el_f3);
    InitBranch( t, "el_f1core",                &m_el_f1core);
    InitBranch( t, "el_f3core",                &m_el_f3core);
    InitBranch( t, "el_weta1",                 &m_el_weta2);
    InitBranch( t, "el_weta2",                 &m_el_weta2);
    InitBranch( t, "el_wtots1",                &m_el_wtots1);
    InitBranch( t, "el_fracs1",                &m_el_fracs1);
    InitBranch( t, "el_Reta",                  &m_el_Reta);
    InitBranch( t, "el_Rphi",                  &m_el_Rphi);
    InitBranch( t, "el_Eratio",                &m_el_Eratio);
    InitBranch( t, "el_Rhad",                  &m_el_Rhad);
    InitBranch( t, "el_Rhad1",                 &m_el_Rhad1);
    InitBranch( t, "el_deta2",                 &m_el_deta2);
    InitBranch( t, "el_dphi2",                 &m_el_dphi2);
    InitBranch( t, "el_dphiresc",              &m_el_dphiresc);
    InitBranch( t, "el_deltaPhiRescaled2"         , &m_el_deltaPhiRescaled2         );
    InitBranch( t, "el_e277"                      , &m_el_e277                      );
    InitBranch( t, "el_deltaE"                    , &m_el_e277                      );
    InitBranch( t, "el_deltaEta1"                 , &m_el_deltaEta1                 );
    InitBranch( t, "el_etCone"                 , &m_el_etCone                 );
    InitBranch( t, "el_ptCone"                 , &m_el_ptCone                 );

    InitBranch( t, "el_trk_pt",                         &m_el_trk_pt);
    InitBranch( t, "el_trk_eta"              ,          &m_el_trk_eta);
    InitBranch( t, "el_trk_charge",                     &m_el_trk_charge);
    InitBranch( t, "el_trk_qOverP",                     &m_el_trk_qOverP);
    InitBranch( t, "el_trk_deltaPOverP"               , &m_el_trk_deltaPOverP               );
    InitBranch( t, "el_trk_sigd0"                     , &m_el_trk_sigd0                     );
    InitBranch( t, "el_trk_d0"                        , &m_el_trk_d0                        );
    InitBranch( t, "el_trk_eProbabilityHT"            , &m_el_trk_eProbabilityHT            );
    InitBranch( t, "el_trk_transformed_eProbabilityHT", &m_el_trk_transformed_eProbabilityHT);
    InitBranch( t, "el_trk_d0significance"            , &m_el_trk_d0significance            );
    InitBranch( t, "el_trk_summaryValues"             , &m_el_trk_summaryValues         );
 
 
  }
  
  InitBranch( t, "el_nGoodVtx",              &m_el_nGoodVtx);
  InitBranch( t, "el_ringsE",                &m_el_ringsE );
  InitBranch( t, "el_nPileupPrimaryVtx",     &m_el_nPileupPrimaryVtx);

  InitBranch( t, "el_loose",                 &m_el_loose  );
  InitBranch( t, "el_medium",                &m_el_medium );
  InitBranch( t, "el_tight",                 &m_el_tight );
  InitBranch( t, "el_lhvloose",              &m_el_lhVLoose );
  InitBranch( t, "el_lhloose",               &m_el_lhLoose );
  InitBranch( t, "el_lhmedium",              &m_el_lhMedium ); 
  InitBranch( t, "el_lhtight",               &m_el_lhTight ); 
  InitBranch( t, "el_multiLepton",           &m_el_multiLepton);
}



void TrigEgammaPhysValBase::linkTriggerBranches( TTree *t ){

  InitBranch(t, "trig_L1_eta",             &m_trig_L1_eta);
  InitBranch(t, "trig_L1_phi",             &m_trig_L1_phi);
  InitBranch(t, "trig_L1_emClus",          &m_trig_L1_emClus);
  InitBranch(t, "trig_L1_tauClus",         &m_trig_L1_tauClus);
  InitBranch(t, "trig_L1_emIsol",          &m_trig_L1_emIsol);
  InitBranch(t, "trig_L1_hadIsol",         &m_trig_L1_hadIsol);
  InitBranch(t, "trig_L1_hadCore",         &m_trig_L1_hadCore);
  InitBranch(t, "trig_L2_calo_et",         &m_trig_L2_calo_et);
  InitBranch(t, "trig_L2_calo_eta",        &m_trig_L2_calo_eta);
  InitBranch(t, "trig_L2_calo_phi",        &m_trig_L2_calo_phi);
  
  if(m_doL2CaloDetailed){
    InitBranch(t, "trig_L2_calo_e237",       &m_trig_L2_calo_e237 );
    InitBranch(t, "trig_L2_calo_e277",       &m_trig_L2_calo_e277 );
    InitBranch(t, "trig_L2_calo_fracs1",     &m_trig_L2_calo_fracs1);
    InitBranch(t, "trig_L2_calo_weta2",      &m_trig_L2_calo_weta2);
    InitBranch(t, "trig_L2_calo_ehad1",      &m_trig_L2_calo_ehad1);
    InitBranch(t, "trig_L2_calo_emaxs1",     &m_trig_L2_calo_emaxs1);
    InitBranch(t, "trig_L2_calo_e2tsts1",    &m_trig_L2_calo_e2tsts1);
    InitBranch(t, "trig_L2_calo_wstot",      &m_trig_L2_calo_wstot);
    InitBranch(t, "trig_L2_calo_energySample",      &m_trig_L2_calo_energySample);
  }
  
  InitBranch(t, "trig_L2_calo_rings",      &m_trig_L2_calo_rings ); 
  InitBranch(t, "trig_L2_calo_rnnOutput",  &m_trig_L2_calo_rnnOutput);
  
  if(m_doL2Detailed){
    InitBranch(t, "trig_L2_el_trackAlgID" ,         &m_trig_L2_el_trackAlgID );
    InitBranch(t, "trig_L2_el_pt" ,                 &m_trig_L2_el_pt );
    InitBranch(t, "trig_L2_el_eta",                 &m_trig_L2_el_eta);
    InitBranch(t, "trig_L2_el_phi",                 &m_trig_L2_el_phi );
    InitBranch(t, "trig_L2_el_caloEta",             &m_trig_L2_el_caloEta );
    InitBranch(t, "trig_L2_el_charge",              &m_trig_L2_el_charge);
    InitBranch(t, "trig_L2_el_nTRTHits",            &m_trig_L2_el_nTRTHits);
    InitBranch(t, "trig_L2_el_nTRTHiThresholdHits", &m_trig_L2_el_nTRTHiThresholdHits);
    InitBranch(t, "trig_L2_el_etOverPt" ,           &m_trig_L2_el_etOverPt );
    InitBranch(t, "trig_L2_el_trkClusDeta" ,        &m_trig_L2_el_trkClusDeta );
    InitBranch(t, "trig_L2_el_trkClusDphi" ,        &m_trig_L2_el_trkClusDphi );

  }
  


#define INIT(_name_) TrigEgammaPhysValBase::InitBranch(t, #_name_, & m_##_name_);\
  
  INIT( trig_EF_calo_et                         ); 
  INIT( trig_EF_calo_eta                        ); 
  INIT( trig_EF_calo_phi                        ); 
  INIT( trig_EF_calo_etaBE2                     ); 
  INIT( trig_EF_calo_e                          ); 
  
  INIT( trig_EF_el_calo_e                       ); 
  INIT( trig_EF_el_calo_et                      ); 
  INIT( trig_EF_el_calo_eta                     ); 
  INIT( trig_EF_el_calo_phi                     ); 
  INIT( trig_EF_el_calo_etaBE2                  ); 
  
  INIT( trig_EF_el_hasCalo                      ); 
  INIT( trig_EF_el_hasTrack                     ); 
  INIT( trig_EF_el_et                           ); 
  INIT( trig_EF_el_eta                          ); 
  INIT( trig_EF_el_phi                          ); 
  INIT( trig_EF_el_e                            ); 
  INIT( trig_EF_el_ethad1                       ); 
  INIT( trig_EF_el_ehad1                        ); 
  INIT( trig_EF_el_f1                           ); 
  INIT( trig_EF_el_f3                           ); 
  INIT( trig_EF_el_f1core                       ); 
  INIT( trig_EF_el_f3core                       ); 
  INIT( trig_EF_el_weta1                        ); 
  INIT( trig_EF_el_weta2                        ); 
  INIT( trig_EF_el_wtots1                       ); 
  INIT( trig_EF_el_fracs1                       ); 
  INIT( trig_EF_el_Reta                         ); 
  INIT( trig_EF_el_Rphi                         ); 
  INIT( trig_EF_el_Eratio                       ); 
  INIT( trig_EF_el_Rhad                         ); 
  INIT( trig_EF_el_Rhad1                        ); 
  INIT( trig_EF_el_deta2                        ); 
  INIT( trig_EF_el_dphi2                        ); 
  INIT( trig_EF_el_e277                         ); 
  INIT( trig_EF_el_deltaE                       ); 
  INIT( trig_EF_el_dphiresc                     ); 
  INIT( trig_EF_el_deltaPhiRescaled2            ); 
  INIT( trig_EF_el_deltaEta1                    ); 
  INIT( trig_EF_el_etCone                       ); 
  INIT( trig_EF_el_ptCone                       ); 

  INIT( trig_EF_el_trk_pt                           ); 
  INIT( trig_EF_el_trk_eta                          ); 
  INIT( trig_EF_el_trk_charge                       ); 
  INIT( trig_EF_el_trk_qOverP                       ); 
  INIT( trig_EF_el_trk_sigd0                        ); 
  INIT( trig_EF_el_trk_d0                           ); 
  INIT( trig_EF_el_trk_eProbabilityHT               ); 
  INIT( trig_EF_el_trk_transformed_eProbabilityHT   ); 
  INIT( trig_EF_el_trk_d0significance               ); 
  INIT( trig_EF_el_trk_deltaPOverP                  ); 
  INIT( trig_EF_el_trk_summaryValues                ); 
  
  INIT( trig_EF_calo_tight                      );
  INIT( trig_EF_calo_medium                     );
  INIT( trig_EF_calo_loose                      );
  INIT( trig_EF_calo_lhtight                    );
  INIT( trig_EF_calo_lhmedium                   );
  INIT( trig_EF_calo_lhloose                    );
  INIT( trig_EF_calo_lhvloose                   );
  INIT( trig_EF_el_tight                        );
  INIT( trig_EF_el_medium                       );
  INIT( trig_EF_el_loose                        );
  INIT( trig_EF_el_lhtight                      );
  INIT( trig_EF_el_lhmedium                     );
  INIT( trig_EF_el_lhloose                      );
  INIT( trig_EF_el_lhvloose                     );



#undef INIT


}  



void TrigEgammaPhysValBase::linkMonteCarloBranches(TTree *t){
 
  InitBranch(t, "mc_hasMC",       &m_mc_hasMC);
  InitBranch(t, "mc_pt",          &m_mc_pt);
  InitBranch(t, "mc_eta",         &m_mc_eta);
  InitBranch(t, "mc_phi",         &m_mc_phi);
  
  if(m_doMCDetailed){
    InitBranch(t, "mc_isTop",       &m_mc_isTop);
    InitBranch(t, "mc_isParton",    &m_mc_isParton);
    InitBranch(t, "mc_isMeson",     &m_mc_isMeson);
    InitBranch(t, "mc_isTau",       &m_mc_isTau);
    InitBranch(t, "mc_isMuon",      &m_mc_isMuon);
  }

  InitBranch(t, "mc_isPhoton",    &m_mc_isPhoton);
  InitBranch(t, "mc_isElectron",  &m_mc_isElectron);
  InitBranch(t, "mc_hasZMother",  &m_mc_hasZMother);
  InitBranch(t, "mc_hasWMother",  &m_mc_hasWMother); 
}


void TrigEgammaPhysValBase::clear(){

  ///EventInfo
  m_runNumber             = 0;
  m_eventNumber           = 0;
  m_lumiBlock             = 0;
  m_avgmu                 = 0;
  
  ///Egamma
  m_el_hasCalo            = false;
  m_el_hasTrack           = false;
  
  
  m_el_calo_e             =  0; 
  m_el_calo_et            =  0; 
  m_el_calo_eta           =  0; 
  m_el_calo_phi           =  0; 
  m_el_calo_etaBE2        =  0;  
 
  m_el_e                  =  0;
  m_el_et                 =  0;
  m_el_eta                =  0;
  m_el_phi                =  0;
  m_el_ethad1             =  0;
  m_el_ehad1              =  0;
  m_el_f1                 =  0;
  m_el_f3                 =  0;
  m_el_f1core             =  0;
  m_el_f3core             =  0;
  m_el_weta1              =  0;
  m_el_weta2              =  0;
  m_el_wtots1             =  0;
  m_el_fracs1             =  0;
  m_el_e277               =  0;
  m_el_Reta               =  0;
  m_el_Rphi               =  0;
  m_el_Eratio             =  0;
  m_el_Rhad               =  0;
  m_el_Rhad1              =  0;
  m_el_deta2              =  0;
  m_el_dphi2              =  0;
  m_el_dphiresc           =  0;
  m_el_deltaPhiRescaled2  =  0;
  m_el_deltaEta1          =  0;
  m_el_deltaE             =  0;
  m_el_etCone             ->clear();
  m_el_ptCone             ->clear();

  m_el_trk_pt                 =  0;
  m_el_trk_eta                =  0;
  m_el_trk_charge             =  0;
  m_el_trk_sigd0              =  0;
  m_el_trk_d0                 =  0;
  m_el_trk_eProbabilityHT     =  0;
  m_el_trk_transformed_eProbabilityHT=0;
  m_el_trk_d0significance     =  0;
  m_el_trk_deltaPOverP        =  0;
  m_el_trk_qOverP             =  0;
  m_el_trk_summaryValues      ->clear();


  
  m_el_medium             = false; 
  m_el_tight              = false; 
  m_el_lhVLoose           = false; 
  m_el_lhLoose            = false; 
  m_el_lhMedium           = false; 
  m_el_lhTight            = false;  
  m_el_rgLoose            = false; 
  m_el_rgMedium           = false; 
  m_el_rgTight            = false;  
  m_el_multiLepton        = false;
  
  m_el_nGoodVtx           = -1;
  m_el_nPileupPrimaryVtx  = -1;
  m_el_ringsE             ->clear(); 
 
  // Trigger L1
  m_trig_L1_eta           = 0;
  m_trig_L1_phi           = 0;
  m_trig_L1_emClus        = 0;
  m_trig_L1_tauClus       = 0;
  m_trig_L1_emIsol        = 0;
  m_trig_L1_hadIsol       = 0;
  m_trig_L1_hadCore       = 0;
 
  // Trigger L2 Calo
  m_trig_L2_calo_et       = 0;
  m_trig_L2_calo_eta      = 0;
  m_trig_L2_calo_phi      = 0;
  m_trig_L2_calo_e237     = 0;
  m_trig_L2_calo_e277     = 0;
  m_trig_L2_calo_fracs1   = 0;
  m_trig_L2_calo_weta2    = 0;
  m_trig_L2_calo_ehad1    = 0;
  m_trig_L2_calo_emaxs1   = 0;
  m_trig_L2_calo_e2tsts1  = 0;
  m_trig_L2_calo_wstot    = 0; 
  
  
  m_trig_L2_calo_energySample                ->clear();
  m_trig_L2_calo_rings                       ->clear();
  m_trig_L2_calo_rnnOutput                   ->clear();
  
  
  // L2 Electron
  m_trig_L2_el_pt                            ->clear(); 
  m_trig_L2_el_eta                           ->clear(); 
  m_trig_L2_el_phi                           ->clear(); 
  m_trig_L2_el_charge                        ->clear();     
  m_trig_L2_el_caloEta                       ->clear(); 
  m_trig_L2_el_trkClusDeta                   ->clear(); 
  m_trig_L2_el_trkClusDphi                   ->clear(); 
  m_trig_L2_el_etOverPt                      ->clear();         
  m_trig_L2_el_nTRTHits                      ->clear();       
  m_trig_L2_el_nTRTHiThresholdHits           ->clear();       
  m_trig_L2_el_trackAlgID                    ->clear(); 
 
  m_trig_EF_el_hasCalo                       ->clear();
  m_trig_EF_el_hasTrack                      ->clear();
  
  // EF Calo
  m_trig_EF_calo_e                           ->clear();     
  m_trig_EF_calo_et                          ->clear();     
  m_trig_EF_calo_eta                         ->clear();     
  m_trig_EF_calo_phi                         ->clear();     
  m_trig_EF_calo_etaBE2                      ->clear();     
  
  m_trig_EF_calo_tight                       ->clear();
  m_trig_EF_calo_medium                      ->clear();
  m_trig_EF_calo_loose                       ->clear();
  m_trig_EF_calo_lhtight                     ->clear();
  m_trig_EF_calo_lhmedium                    ->clear();
  m_trig_EF_calo_lhloose                     ->clear();
  m_trig_EF_calo_lhvloose                    ->clear();
 
  // HLT electron
  m_trig_EF_el_calo_e                        ->clear(); 
  m_trig_EF_el_calo_et                       ->clear(); 
  m_trig_EF_el_calo_eta                      ->clear(); 
  m_trig_EF_el_calo_phi                      ->clear(); 
  m_trig_EF_el_calo_etaBE2                   ->clear(); 

  m_trig_EF_el_e                             ->clear(); 
  m_trig_EF_el_et                            ->clear(); 
  m_trig_EF_el_eta                           ->clear(); 
  m_trig_EF_el_phi                           ->clear(); 
  m_trig_EF_el_deltaE                        ->clear(); 
  m_trig_EF_el_ethad1                        ->clear(); 
  m_trig_EF_el_ehad1                         ->clear(); 
  m_trig_EF_el_f1                            ->clear(); 
  m_trig_EF_el_f3                            ->clear(); 
  m_trig_EF_el_f1core                        ->clear(); 
  m_trig_EF_el_f3core                        ->clear(); 
  m_trig_EF_el_weta1                         ->clear(); 
  m_trig_EF_el_weta2                         ->clear(); 
  m_trig_EF_el_wtots1                        ->clear(); 
  m_trig_EF_el_fracs1                        ->clear(); 
  m_trig_EF_el_Reta                          ->clear(); 
  m_trig_EF_el_Rphi                          ->clear(); 
  m_trig_EF_el_Eratio                        ->clear(); 
  m_trig_EF_el_Rhad                          ->clear(); 
  m_trig_EF_el_Rhad1                         ->clear(); 
  m_trig_EF_el_e277                          ->clear(); 
  m_trig_EF_el_deta2                         ->clear(); 
  m_trig_EF_el_dphi2                         ->clear(); 
  m_trig_EF_el_dphiresc                      ->clear(); 
  m_trig_EF_el_deltaPhiRescaled2             ->clear(); 
  m_trig_EF_el_deltaEta1                     ->clear(); 
  m_trig_EF_el_etCone                        ->clear();
  m_trig_EF_el_ptCone                        ->clear();
  
  m_trig_EF_el_trk_pt                        ->clear(); 
  m_trig_EF_el_trk_eta                       ->clear(); 
  m_trig_EF_el_trk_charge                    ->clear(); 
  m_trig_EF_el_trk_qOverP                    ->clear(); 
  m_trig_EF_el_trk_sigd0                     ->clear(); 
  m_trig_EF_el_trk_d0                        ->clear(); 
  m_trig_EF_el_trk_eProbabilityHT            ->clear(); 
  m_trig_EF_el_trk_transformed_eProbabilityHT->clear(); 
  m_trig_EF_el_trk_d0significance            ->clear(); 
  m_trig_EF_el_trk_deltaPOverP               ->clear(); 
  m_trig_EF_el_trk_summaryValues             ->clear();


  m_trig_EF_el_tight                         ->clear();
  m_trig_EF_el_medium                        ->clear();
  m_trig_EF_el_loose                         ->clear();
  m_trig_EF_el_lhtight                       ->clear();
  m_trig_EF_el_lhmedium                      ->clear();
  m_trig_EF_el_lhloose                       ->clear();
  m_trig_EF_el_lhvloose                      ->clear();
  
  m_trig_tdt_L1_calo_accept                  ->clear();
  m_trig_tdt_L2_calo_accept                  ->clear();
  m_trig_tdt_L2_el_accept                    ->clear();
  m_trig_tdt_EF_calo_accept                  ->clear();
  m_trig_tdt_EF_el_accept                    ->clear();
  m_trig_tdt_emu_L1_calo_accept              ->clear();
  m_trig_tdt_emu_L2_calo_accept              ->clear();
  m_trig_tdt_emu_L2_el_accept                ->clear();
  m_trig_tdt_emu_EF_calo_accept              ->clear();
  m_trig_tdt_emu_EF_el_accept                ->clear();
  
  ///Monte Carlo
  m_mc_hasMC              = false;
  m_mc_pt                 = -1;
  m_mc_eta                = -1;
  m_mc_phi                = -1;
  m_mc_isTop              = false;
  m_mc_isParton           = false;
  m_mc_isMeson            = false;
  m_mc_isTau              = false;
  m_mc_isMuon             = false;
  m_mc_isPhoton           = false;
  m_mc_isElectron         = false;
  m_mc_hasZMother         = false;
  m_mc_hasWMother         = false;


}

void TrigEgammaPhysValBase::alloc_space(){
  
  m_el_ringsE                                 = new std::vector<float>();
  m_el_trk_summaryValues                      = new std::vector<uint8_t>();     
  m_el_etCone                                 = new std::vector<float>();     
  m_el_ptCone                                 = new std::vector<float>();     


  m_trig_L2_calo_energySample                 = new std::vector<float>();
  m_trig_L2_calo_rings                        = new std::vector<float>();
  m_trig_L2_calo_rnnOutput                    = new std::vector<float>();
 
  m_trig_L2_el_trackAlgID                     = new std::vector<int>();
  m_trig_L2_el_eta                            = new std::vector<float>();   
  m_trig_L2_el_phi                            = new std::vector<float>();   
  m_trig_L2_el_caloEta                        = new std::vector<float>();   
  m_trig_L2_el_trkClusDeta                    = new std::vector<float>();
  m_trig_L2_el_trkClusDphi                    = new std::vector<float>();
  m_trig_L2_el_pt                             = new std::vector<float>();   
  m_trig_L2_el_etOverPt                       = new std::vector<float>();           
  m_trig_L2_el_nTRTHits                       = new std::vector<float>();         
  m_trig_L2_el_nTRTHiThresholdHits            = new std::vector<float>();         
  m_trig_L2_el_charge                         = new std::vector<float>();       
  
  m_trig_EF_calo_et                           = new std::vector<float>();     
  m_trig_EF_calo_eta                          = new std::vector<float>();     
  m_trig_EF_calo_phi                          = new std::vector<float>();     
  m_trig_EF_calo_etaBE2                       = new std::vector<float>();     
  m_trig_EF_calo_e                            = new std::vector<float>();     
  
  m_trig_EF_el_calo_et                        = new std::vector<float>();     
  m_trig_EF_el_calo_eta                       = new std::vector<float>();     
  m_trig_EF_el_calo_phi                       = new std::vector<float>();     
  m_trig_EF_el_calo_etaBE2                    = new std::vector<float>();     
  m_trig_EF_el_calo_e                         = new std::vector<float>();     
  
  m_trig_EF_el_hasCalo                        = new std::vector<bool>();
  m_trig_EF_el_hasTrack                       = new std::vector<bool>();
 
  m_trig_EF_el_et                             = new std::vector<float>();     
  m_trig_EF_el_eta                            = new std::vector<float>();     
  m_trig_EF_el_phi                            = new std::vector<float>();     
  m_trig_EF_el_e                              = new std::vector<float>();     
  m_trig_EF_el_deltaE                         = new std::vector<float>();     
  m_trig_EF_el_ethad1                         = new std::vector<float>();     
  m_trig_EF_el_ehad1                          = new std::vector<float>();     
  m_trig_EF_el_f1                             = new std::vector<float>();     
  m_trig_EF_el_f3                             = new std::vector<float>();     
  m_trig_EF_el_f1core                         = new std::vector<float>();     
  m_trig_EF_el_f3core                         = new std::vector<float>();     
  m_trig_EF_el_weta1                          = new std::vector<float>();     
  m_trig_EF_el_weta2                          = new std::vector<float>();     
  m_trig_EF_el_wtots1                         = new std::vector<float>();     
  m_trig_EF_el_fracs1                         = new std::vector<float>();     
  m_trig_EF_el_e277                           = new std::vector<float>();     
  m_trig_EF_el_Reta                           = new std::vector<float>();     
  m_trig_EF_el_Rphi                           = new std::vector<float>();     
  m_trig_EF_el_Eratio                         = new std::vector<float>();     
  m_trig_EF_el_Rhad                           = new std::vector<float>();     
  m_trig_EF_el_Rhad1                          = new std::vector<float>();     
  m_trig_EF_el_deta2                          = new std::vector<float>();     
  m_trig_EF_el_dphi2                          = new std::vector<float>();     
  m_trig_EF_el_dphiresc                       = new std::vector<float>();     
  m_trig_EF_el_deltaPhiRescaled2              = new std::vector<float>();   
  m_trig_EF_el_deltaEta1                      = new std::vector<float>();     
  m_trig_EF_el_etCone                         = new std::vector<float>();     
  m_trig_EF_el_ptCone                         = new std::vector<float>();     

  m_trig_EF_el_trk_pt                         = new std::vector<float>();     
  m_trig_EF_el_trk_eta                        = new std::vector<float>();     
  m_trig_EF_el_trk_charge                     = new std::vector<float>();     
  m_trig_EF_el_trk_qOverP                     = new std::vector<float>();     
  m_trig_EF_el_trk_sigd0                      = new std::vector<float>();     
  m_trig_EF_el_trk_d0                         = new std::vector<float>();     
  m_trig_EF_el_trk_eProbabilityHT             = new std::vector<float>();   
  m_trig_EF_el_trk_transformed_eProbabilityHT = new std::vector<float>();   
  m_trig_EF_el_trk_d0significance             = new std::vector<float>();     
  m_trig_EF_el_trk_deltaPOverP                = new std::vector<float>();     
  m_trig_EF_el_trk_summaryValues              = new std::vector<uint8_t>();     
  
  m_trig_EF_calo_tight                        = new std::vector<bool>();
  m_trig_EF_calo_medium                       = new std::vector<bool>();
  m_trig_EF_calo_loose                        = new std::vector<bool>();
  m_trig_EF_calo_lhtight                      = new std::vector<bool>();
  m_trig_EF_calo_lhmedium                     = new std::vector<bool>();
  m_trig_EF_calo_lhloose                      = new std::vector<bool>();
  m_trig_EF_calo_lhvloose                     = new std::vector<bool>();
  
  m_trig_EF_el_tight                          = new std::vector<bool>();
  m_trig_EF_el_medium                         = new std::vector<bool>();
  m_trig_EF_el_loose                          = new std::vector<bool>();
  m_trig_EF_el_lhtight                        = new std::vector<bool>();
  m_trig_EF_el_lhmedium                       = new std::vector<bool>();
  m_trig_EF_el_lhloose                        = new std::vector<bool>();
  m_trig_EF_el_lhvloose                       = new std::vector<bool>();
  
  m_trig_tdt_L1_calo_accept                   = new std::vector<int>();
  m_trig_tdt_L2_calo_accept                   = new std::vector<int>();
  m_trig_tdt_L2_el_accept                     = new std::vector<int>();
  m_trig_tdt_EF_calo_accept                   = new std::vector<int>();
  m_trig_tdt_EF_el_accept                     = new std::vector<int>();

  m_trig_tdt_emu_L1_calo_accept                   = new std::vector<int>();
  m_trig_tdt_emu_L2_calo_accept                   = new std::vector<int>();
  m_trig_tdt_emu_L2_el_accept                     = new std::vector<int>();
  m_trig_tdt_emu_EF_calo_accept                   = new std::vector<int>();
  m_trig_tdt_emu_EF_el_accept                     = new std::vector<int>();



}

void TrigEgammaPhysValBase::release_space(){
  
  delete m_el_ringsE            ;
  delete m_el_trk_summaryValues ;     
  delete m_el_etCone            ;     
  delete m_el_ptCone            ;     


  
  delete m_trig_L2_calo_energySample;
  delete m_trig_L2_calo_rings     ;
  delete m_trig_L2_calo_rnnOutput ;
  delete m_trig_L2_el_trackAlgID  ;
  delete m_trig_L2_el_pt          ;  
  delete m_trig_L2_el_eta         ;   
  delete m_trig_L2_el_caloEta     ;   
  delete m_trig_L2_el_phi         ;   
  delete m_trig_L2_el_charge      ;       
  delete m_trig_L2_el_nTRTHits    ;         
  delete m_trig_L2_el_nTRTHiThresholdHits    ;         
  delete m_trig_L2_el_etOverPt    ;           
  delete m_trig_L2_el_trkClusDeta ;
  delete m_trig_L2_el_trkClusDphi ;
  
  delete m_trig_EF_calo_et                        ;     
  delete m_trig_EF_calo_eta                       ;     
  delete m_trig_EF_calo_phi                       ;     
  delete m_trig_EF_calo_etaBE2                    ;     
  delete m_trig_EF_calo_e                         ;     
  
  delete m_trig_EF_el_calo_et                        ;     
  delete m_trig_EF_el_calo_eta                       ;     
  delete m_trig_EF_el_calo_phi                       ;     
  delete m_trig_EF_el_calo_etaBE2                    ;     
  delete m_trig_EF_el_calo_e                         ;     
 
  delete m_trig_EF_el_et                             ;     
  delete m_trig_EF_el_eta                            ;     
  delete m_trig_EF_el_phi                            ;     
  delete m_trig_EF_el_e                              ;     
  delete m_trig_EF_el_deltaE                         ;     
  delete m_trig_EF_el_ethad1                         ;     
  delete m_trig_EF_el_ehad1                          ;     
  delete m_trig_EF_el_f1                             ;     
  delete m_trig_EF_el_f3                             ;     
  delete m_trig_EF_el_f1core                         ;     
  delete m_trig_EF_el_f3core                         ;     
  delete m_trig_EF_el_weta1                          ;     
  delete m_trig_EF_el_weta2                          ;     
  delete m_trig_EF_el_wtots1                         ;     
  delete m_trig_EF_el_fracs1                         ;     
  delete m_trig_EF_el_e277                           ;     
  delete m_trig_EF_el_Reta                           ;     
  delete m_trig_EF_el_Rphi                           ;     
  delete m_trig_EF_el_Eratio                         ;     
  delete m_trig_EF_el_Rhad                           ;     
  delete m_trig_EF_el_Rhad1                          ;     
  delete m_trig_EF_el_deta2                          ;     
  delete m_trig_EF_el_dphi2                          ;     
  delete m_trig_EF_el_dphiresc                       ;     
  delete m_trig_EF_el_deltaPhiRescaled2              ;   
  delete m_trig_EF_el_deltaEta1                      ;     
  delete m_trig_EF_el_etCone                         ;     
  delete m_trig_EF_el_ptCone                         ;     

  delete m_trig_EF_el_trk_pt                         ;     
  delete m_trig_EF_el_trk_eta                        ;     
  delete m_trig_EF_el_trk_charge                     ;     
  delete m_trig_EF_el_trk_qOverP                     ;     
  delete m_trig_EF_el_trk_sigd0                      ;     
  delete m_trig_EF_el_trk_d0                         ;     
  delete m_trig_EF_el_trk_eProbabilityHT             ;   
  delete m_trig_EF_el_trk_transformed_eProbabilityHT ;   
  delete m_trig_EF_el_trk_d0significance             ;     
  delete m_trig_EF_el_trk_deltaPOverP                ;     
  delete m_trig_EF_el_trk_summaryValues              ;     
  
  delete m_trig_EF_calo_tight                        ;
  delete m_trig_EF_calo_medium                       ;
  delete m_trig_EF_calo_loose                        ;
  delete m_trig_EF_calo_lhtight                      ;
  delete m_trig_EF_calo_lhmedium                     ;
  delete m_trig_EF_calo_lhloose                      ;
  delete m_trig_EF_calo_lhvloose                     ;
  delete m_trig_EF_el_tight                          ;
  delete m_trig_EF_el_medium                         ;
  delete m_trig_EF_el_loose                          ;
  delete m_trig_EF_el_lhtight                        ;
  delete m_trig_EF_el_lhmedium                       ;
  delete m_trig_EF_el_lhloose                        ;
  delete m_trig_EF_el_lhvloose                       ;
  
  delete m_trig_tdt_L1_calo_accept                   ;
  delete m_trig_tdt_L2_calo_accept                   ;
  delete m_trig_tdt_L2_el_accept                     ;
  delete m_trig_tdt_EF_calo_accept                   ;
  delete m_trig_tdt_EF_el_accept                     ;
  delete m_trig_tdt_emu_L1_calo_accept                   ;
  delete m_trig_tdt_emu_L2_calo_accept                   ;
  delete m_trig_tdt_emu_L2_el_accept                     ;
  delete m_trig_tdt_emu_EF_calo_accept                   ;
  delete m_trig_tdt_emu_EF_el_accept                     ;



  delete m_trig_EF_el_hasCalo                        ;
  delete m_trig_EF_el_hasTrack                       ;
 

}


