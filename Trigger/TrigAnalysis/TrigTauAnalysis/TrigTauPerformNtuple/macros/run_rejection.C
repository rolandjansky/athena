/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//To run this script: root -l -b -q newlibraries.C 'run_rejection.C++()'

#include <iostream>
#include <fstream>
#include <TChain.h>
#include <TFile.h>
#include "TrigTauPerformNtuple/TrigTauEvent.h"
#include "TrigTauPerformAnalysis/TrigTauAnalyzer.h"



int run_rejection(){

  //--------------------
  // read data
  //--------------------
  //TString chain_name = "eventTree";  
  TString chain_name = "Tau/TrigTau";  
  TChain* chain = new TChain( chain_name);

  //c546*: without pileoutnise suppression at HLT
  //chain->Add("root://castoratlas//castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_JetTauEtmiss/00178109/data11_7TeV.00178109.physics_JetTauEtmiss.hltmerge.NTUP_TRIG.c546_c555_m254/*");

  //c570*: with pileup noise correction at HLT
  //chain->Add("root://castoratlas//castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_JetTauEtmiss/00178109/data11_7TeV.00178109.physics_JetTauEtmiss.hltmerge.NTUP_TRIG.c570_c576_m254/*");

  //this should be as c546, trigger not rerun, same as online
  //chain->Add("root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/tau/data11_7TeV/filtered/JetTauEtmiss/178109/*");

  //chain->Add("root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/tau/data11_7TeV/filtered/JetTauEtmiss/178044/anyTauFilter.178044_physics_JetTauEtmiss_f354_m763.idgreen_caloyellow_hlton_7TeV.*.root");
  //chain->Add("root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/tau/data11_7TeV/filtered/JetTauEtmiss/178109/anyTauFilter.178109_physics_JetTauEtmiss_f354_m763.idgreen_caloyellow_hlton_7TeV.*.root");

  //c600: no pileup noise correction
  //chain->Add("root://castoratlas//castor/cern.ch/grid/atlas/caf/atlcal/temp/trigger/data11_7TeV/physics_EnhancedBias/00179939/data11_7TeV.00179939.physics_EnhancedBias.hltmerge.NTUP_TRIG.c600_c601_j2/data11_7TeV.00179939.physics_EnhancedBias.hltmerge.NTUP_TRIG.c600_c601_j2.*.1");

  //c596: with pileup noise correction
  chain->Add("root://castoratlas//castor/cern.ch/grid/atlas/caf/atlcal/temp/trigger/data11_7TeV/physics_EnhancedBias/00179939/data11_7TeV.00179939.physics_EnhancedBias.hltmerge.NTUP_TRIG.c596_c602_j2/data11_7TeV.00179939.physics_EnhancedBias.hltmerge.NTUP_TRIG.c596_c602_j2._*.1");
  
  chain->ls();

  TrigTauEvent* event = new TrigTauEvent();
  chain->SetBranchAddress("EventBranch", &event);
  chain->SetBranchStatus("*", 1);
  
  
  //--------------------
  // set the selection
  //--------------------
  int nItems = 7;
  TrigTauSelectionManager::CUTLEVEL cutLevel[nItems];
  TrigTauFlags::TRIGGERMENU         menu[nItems];
  
  cutLevel[0] = TrigTauSelectionManager::LOOSE;
  menu[0]     = TrigTauFlags::TAU16;

  cutLevel[1] = TrigTauSelectionManager::MEDIUM1;
  menu[1]     = TrigTauFlags::TAU20_MEDIUM1;

  cutLevel[2] = TrigTauSelectionManager::LOOSE;
  menu[2]     = TrigTauFlags::TAU29;

  cutLevel[3] = TrigTauSelectionManager::MEDIUM1;
  menu[3]     = TrigTauFlags::TAU29_MEDIUM1;

  cutLevel[4] = TrigTauSelectionManager::MEDIUM;
  menu[4]     = TrigTauFlags::TAU100_MEDIUM;

  cutLevel[5] = TrigTauSelectionManager::MEDIUM1;
  menu[5]     = TrigTauFlags::TAU100_MEDIUM1;

  cutLevel[6] = TrigTauSelectionManager::MEDIUM;
  menu[6]     = TrigTauFlags::TAU125_MEDIUM;


  //TrigTauFlags::MENUVERSION         menuVersion = TrigTauFlags::MENU1662;//TrigTauHypo-00-02-34: 
  int nMenus = 2;
  TrigTauSelectionManager::MENUVERSION menuVersion[2];
  menuVersion[0] = TrigTauSelectionManager::MENU1666;//TrigTauHypo-00-02-42
  menuVersion[1] = TrigTauSelectionManager::MENUTEST;//TrigTauHypo-00-03-00: new constants new L2


  TrigTauFlags::HLTAUTHOR   chainType   = TrigTauFlags::SITRK;
  

  //--------------------
  // init
  //--------------------
  int nev = int(chain->GetEntries());
  cout << "events" << " " << nev << endl;

  bool L1[nMenus][nItems];
  bool L2[nMenus][nItems];
  bool EF[nMenus][nItems];
  bool L2Cal[nMenus][nItems];
  bool L2Trk[nMenus][nItems];
  
  int evnL1[nMenus][nItems];
  int evnL2[nMenus][nItems];
  int evnEF[nMenus][nItems];
  int evnL2Cal[nMenus][nItems];
  int evnL2Trk[nMenus][nItems];
  
  int trigDecL1[nMenus][nItems];
  int trigDecL2[nMenus][nItems];
  int trigDecEF[nMenus][nItems];

  int evttrigDecL1[nMenus][nItems];
  int evttrigDecL2[nMenus][nItems];
  int evttrigDecEF[nMenus][nItems];


  //--------------------
  // TrigTauAnalyzer analyzer;
  //--------------------
  TrigTauSelectionManager selmanager[nMenus][nItems];
  for (int imenu=0; imenu<2; imenu++) {
    for (int item=0; item<nItems; item++) {
      selmanager[imenu][item].setMenuVersion(menuVersion[imenu]);
      selmanager[imenu][item].setTauMenu(menu[item],cutLevel[item]);
      
      L1[imenu][item]           = false;
      L2[imenu][item]           = false;
      EF[imenu][item]           = false;
      L2Cal[imenu][item]        = false;
      L2Trk[imenu][item]        = false;
      
      evnL1[imenu][item]        = 0;
      evnL2[imenu][item]        = 0;
      evnEF[imenu][item]        = 0;
      evnL2Cal[imenu][item]     = 0;
      evnL2Trk[imenu][item]     = 0;
      
      trigDecL1[imenu][item]    = false;
      trigDecL2[imenu][item]    = false;
      trigDecEF[imenu][item]    = false;
      
      evttrigDecL1[imenu][item] = 0;
      evttrigDecL2[imenu][item] = 0;
      evttrigDecEF[imenu][item] = 0;
    }
  }




  //--------------------
  // loop
  //--------------------
  //nev = 100;
  for(int iev = 0; iev<nev; ++iev) {
    if (iev%10000==0) cout << iev << endl;
    event->reset();
    chain->GetEvent(iev);

    for (int imenu=0; imenu<2; imenu++) {
      for (int item=0; item<nItems; item++) {
        TrigTauTrigChainList chainList;    
        chainList.setRoiWordMatching();
        chainList.setHLTAuthor(chainType);
        chainList.fillChainList(event);
        chainList.fillTriggerList(selmanager[imenu][item]);
        
        L1[imenu][item] = (chainList.nPassL1Chains()>0);
        if ( L1[imenu][item] ) ++evnL1[imenu][item];
        
        trigDecL1[imenu][item] = event->decision.passedL1(menu[item]);
        trigDecL2[imenu][item] = event->decision.passedL2(menu[item]);
        trigDecEF[imenu][item] = event->decision.passedEF(menu[item]);
        
        L2Cal[imenu][item]     = ( chainList.nPassL2CalChains() > 0 );
        L2Trk[imenu][item]     = ( chainList.nPassL2TrkChains() > 0 );
        L2[imenu][item]        = ( chainList.nPassL2Chains() > 0 );
        EF[imenu][item]        = ( chainList.nPassEFChains() > 0 );
        

        if ( L1[imenu][item] && L2Cal[imenu][item] )                 ++evnL2Cal[imenu][item];
        if ( L1[imenu][item] && L2Trk[imenu][item] )                 ++evnL2Trk[imenu][item];    
        if ( L1[imenu][item] && L2[imenu][item]    )                 ++evnL2[imenu][item];
        if ( L1[imenu][item] && L2[imenu][item] && EF[imenu][item] ) ++evnEF[imenu][item];
        
        if (trigDecL1[imenu][item] )                                                     ++evttrigDecL1[imenu][item];
        if (trigDecL1[imenu][item] && trigDecL2[imenu][item] )                           ++evttrigDecL2[imenu][item];
        if (trigDecL1[imenu][item] && trigDecL2[imenu][item] && trigDecEF[imenu][item] ) ++evttrigDecEF[imenu][item];
      }
    }
    
  }  //loop over events
  

  //print out eveything
  cout << "total vents" << " " << nev << endl;
  for (int imenu=0; imenu<2; imenu++) {
    for (int item=0; item<nItems; item++) {
      cout << " menu = " << menuVersion[imenu] << " item = " << menu[item] << " cutLevel = " << cutLevel[item] << endl;
      cout << "   events L1 passed (TTP)    " << "\t\t" << evnL1[imenu][item]    << endl;
      cout << "   events L2 Cal passed (TTP)" << "\t\t" << evnL2Cal[imenu][item] << endl;
      cout << "   events L2 Trk passed (TTP)" << "\t\t" << evnL2Trk[imenu][item] << endl;
      cout << "   events L2 passed (TTP)    " << "\t\t" << evnL2[imenu][item]    << endl;
      cout << "   events EF passed (TTP)    " << "\t\t" << evnEF[imenu][item]    << endl;
      
      cout << "   events L1 passed (TrigDecision)" << "\t" << evttrigDecL1[imenu][item] << endl;
      cout << "   events L2 passed (TrigDecision)" << "\t" << evttrigDecL2[imenu][item] << endl;
      cout << "   events EF passed (TrigDecision)" << "\t" << evttrigDecEF[imenu][item] << endl;
    }
  }
  
  //formatted for the plotter macro
  cout<<endl;
  for (int imenu=0; imenu<2; imenu++) {
    cout << " menu = " << menuVersion[imenu] << endl;
    for (int item=0; item<nItems; item++) {
      cout<<"{"<<nev<<","<<evnL1[imenu][item]<<","<<evnL2Cal[imenu][item]<<","<<evnL2Trk[imenu][item]<<","<<evnL2[imenu][item]<<","<<evnEF[imenu][item]<<"},"<<endl;
    }
    cout<<endl;
  }

  return 1;

}
