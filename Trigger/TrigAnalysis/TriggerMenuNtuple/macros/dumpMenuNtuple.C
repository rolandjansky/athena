/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  dumpMenuNtuple.C
*/
#include "TTree.h"
#include "TFile.h"
#include <vector>
#include <iostream>
#include "TriggerMenuNtuple/TMNUtil.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"

using namespace std;


void dumpMenuNtuple(const char* fname="/pcatr-srv1/home/tkohno/athena/15.5.X.Y/run/trigntuple.root", 
		    const char* tname="TrigBjet", 
		    const char* tname_config="TrigBjetMeta/TrigConfTree") {
  TFile *f = new TFile(fname);
  TTree* t = dynamic_cast<TTree*>(f->Get(tname));
  TTree* t_config = dynamic_cast<TTree*>(f->Get(tname_config));

  std::vector<ChainEntry> chain_entries;
  RoILinks roi_links;
  std::vector<ChainEntry>::const_iterator p_chain;
  std::vector<short>* trig_EF_passedPhysics=0;

  TMNUtil util;
  util.initForD3PD(t, t_config);
  t->SetBranchStatus("*", 0);
  t->SetBranchStatus("trig_Nav_*", 1);
  t->SetBranchStatus("trig_RoI_*", 1);
  t->SetBranchStatus("trig_EF_passedPhysics", 1);
  t->SetBranchAddress("trig_EF_passedPhysics", &trig_EF_passedPhysics);

  int nentries = t->GetEntries();
  //nentries = 100;
  int ientry;
  for (ientry=0; ientry<nentries; ++ientry) {
    t->GetEntry(ientry);

    util.clear();
    util.reconstructChainEntryRoILinks(chain_entries, roi_links, 
				       TMNUtil::kD3PD);
    //util.readNavigation();

    bool mu15_mu10 = false;
    for (unsigned int i=0; i<trig_EF_passedPhysics->size(); ++i) {
      if ( (*trig_EF_passedPhysics)[i] == 943) {
	mu15_mu10 = true;
	break;
      }
    }
    cout << "Entry: " << ientry << endl;
    cout << "EF_mu15_mu10_EFFS passed or not? " << mu15_mu10 << endl;
    for (p_chain=chain_entries.begin(); p_chain!=chain_entries.end(); 
	 ++p_chain) {
//       if (p_chain->getName().find("_mu4_j135_a4tc_EFFS") != string::npos || 
// 	  p_chain->getName().find("L2_mu4_j95_L1matched") != string::npos) {
      if (p_chain->getName().find("_mu") != string::npos || 
	  p_chain->getName().find("_2mu") != string::npos) {
	cout << "Dump chain : " << p_chain->getName() << endl;
	cout << (*p_chain) << endl;
	util.dumpChainRoIFeatures(*p_chain, roi_links);
      }
    }
  }

  return;
}
