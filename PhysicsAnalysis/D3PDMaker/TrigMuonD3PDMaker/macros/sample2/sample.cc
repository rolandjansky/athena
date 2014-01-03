/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <vector>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"

#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "TriggerMenuNtuple/TMNUtil.h"

#include "TrigMuonD3PD.h"
#include "TrigConfTree.h"
using namespace std;

void sample(){
  //initialize input
  TChain* ch1 = new TChain("trigger");
  TChain* ch2 = new TChain("triggerMeta/TrigConfTree");
  ch1->Add("../../../work/20101028/trig_muon_d3pd_level0.root");
  ch2->Add("../../../work/20101028/trig_muon_d3pd_level0.root");

  //Important Note: keep the oerder of declaration,
  //TrigMuonD3PD => TMNUtil => TrigConfTree

  //Ntuple
  TrigMuonD3PD* d3pd = new TrigMuonD3PD(ch1);
  //Navigation tool
  vector<string> all_roi_types;
  std::vector<ChainEntry>   vchain;
  RoILinks                  roi_links;
  all_roi_types.push_back("trig_RoI_L2_mu");
  all_roi_types.push_back("trig_RoI_EF_mu");
  TMNUtil* util = new TMNUtil;
  util->setRoITypes(all_roi_types);
  util->init(ch1,ch2,ch2);
  //Metadata
  TrigConfTree* conf = new TrigConfTree(ch2);
  ch2->GetEntry(0);

  //initialize output
  TFile* fout = new TFile("output.root","recreate");
  TH1F* h_L2_mu10MSpassed_L2SAmu_pt = new TH1F("h_L2_mu10MSpassed_L2SAmu_pt","L2_mu10MSpassed_L2SAmu_pt;p_{T} (GeV);entry",100,0,100);
  TH1F* h_L2_mu10MSfailed_L2SAmu_pt = new TH1F("h_L2_mu10MSfailed_L2SAmu_pt","L2_mu10MSfailed_L2SAmu_pt;p_{T} (GeV);entry",100,0,100);
  TH1F* h_EF_mu10MSpassed_EFSAmu_pt = new TH1F("h_EF_mu10MSpassed_EFSAmu_pt","EF_mu10MSpassed_EFSAmu_pt;p_{T} (GeV);entry",100,0,100);
  TH1F* h_EF_mu10MSfailed_EFSAmu_pt = new TH1F("h_EF_mu10MSfailed_EFSAmu_pt","EF_mu10MSfailed_EFSAmu_pt;p_{T} (GeV);entry",100,0,100);

  const int nevent = ch1->GetEntries();
  for(int iev=0; iev<nevent; iev++){//Event Loop
    ch1->GetEntry(iev);
    /*
     *  CombLinks corresponds to "Combination" in TrigDecisionTool
     *  One CombLinks object repersents one trigger chain object
     *
     */
    util->reconstructChainEntryRoILinks(vchain, roi_links, TMNUtil::kD3PD);
    std::vector<CombLinks> CombsL2(0);
    std::vector<CombLinks> CombsEF(0);
    for(unsigned int i_chain=0;i_chain<vchain.size();i_chain++){
      if( vchain[i_chain].getName()=="L2_mu10_MSonly"){
        //Each CombsL2 entry corresponds to L2_mu10 chain object for each RoI
        CombsL2 = roi_links.getCombLinks(vchain[i_chain]);
      }   
      if( vchain[i_chain].getName()=="EF_mu10_MSonly"){
        //Each CombsEF entry corresponds to EF_mu10 chain object for each RoI
        CombsEF = roi_links.getCombLinks(vchain[i_chain]);
      }
    }//Note that in trigger ntuple, L2 and EF must be treated separately 

    for(unsigned int i_L2=0; i_L2<CombsL2.size(); i_L2++){
      const FeatureIndex* fiL2 = CombsL2[i_L2].index("MuonFeature");
      if(fiL2->getStatus())//Status of TE of MuonFeature is true. So in this case the RoI passed L2_mu10
        h_L2_mu10MSpassed_L2SAmu_pt->Fill(  d3pd->trig_L2_muonfeature_pt->at(fiL2->getIndex())  );
      else
        h_L2_mu10MSfailed_L2SAmu_pt->Fill(  d3pd->trig_L2_muonfeature_pt->at(fiL2->getIndex())  );
    }

  }//Event Loop

  fout->Write();
}
