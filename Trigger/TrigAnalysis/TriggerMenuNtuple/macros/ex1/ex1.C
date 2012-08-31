/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  ex1.C
*/
#include <vector>
#include <set>
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TriggerMenuNtuple/TMNUtil.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "D3PD_NTUP.h"

using namespace std;

void ex1(const std::string& chain_name="L2_e10_medium", 
	 const std::string& fname="d3pd_level1.root") {
  TFile *f = new TFile(fname.c_str(), "READ");
  TTree* t = dynamic_cast<TTree*>(f->Get("t"));
  TTree* t_run = dynamic_cast<TTree*>(f->Get("tMeta/TrigConfMap"));
  TTree* t_config = dynamic_cast<TTree*>(f->Get("tMeta/TrigConfTree"));

  D3PD_NTUP nt(t);

  set<string> rois;
  rois.insert("RoI_L2_e");
  TMNUtil u;
  u.setRoITypes(rois);
  u.init(t, t_run, t_config);
  
  vector<ChainEntry> chain_entries;
  RoILinks roi_links;

  TH1F* h_l2_emcl_pt = new TH1F("h_l2_emcl_pt", "p_{T}^{cluster}", 50, 0, 100);
  TH2F* h_l2_etacorr = new TH2F("h_l2_etacorr", 
				"#eta^{cluster} vs. #eta^{track}", 
				60, -3, 3, 60, -3, 3);
  h_l2_emcl_pt->SetDirectory(gROOT);
  h_l2_etacorr->SetDirectory(gROOT);

  int nentries = t->GetEntries();
  for (int ientry=0; ientry<nentries; ++ientry) {
    nt.GetEntry(ientry);
    u.reconstructChainEntryRoILinks(chain_entries, roi_links);

    for (unsigned int i=0; i<chain_entries.size(); ++i) {
      //      u.dumpChainRoIFeatures(chain_entries[i], roi_links);
      if (chain_entries[i].getName() == chain_name) {
	vector<CombLinks> combs = roi_links.getCombLinks(chain_entries[i]);
	for (unsigned int icomb=0; icomb<combs.size(); ++icomb) {
	  const FeatureIndex* fi_emcl = combs[icomb].index("TrigEMCluster");
	  const vector<FeatureIndex>* fi_tracks = 
	    combs[icomb].indexVec("TrigInDetTrackCollection_TrigSiTrack_eGamma");
	  int i_emcl=0; // index of TrigEMCluster
	  int i_trk=0; // index of TrigInDetTrack
	  if (fi_emcl && (i_emcl = fi_emcl->getIndex()) >= 0) {
	    // TrigEMCluster exists in the RoI

	    h_l2_emcl_pt->Fill( (*nt.l2_emcl_et)[i_emcl]*0.001);
	    if (fi_tracks && fi_tracks->size() > 0) {
	      // At least one TrigInDetTrack exists in the RoI

	      for (unsigned int itr=0; itr<fi_tracks->size(); ++itr){
		i_trk = (*fi_tracks)[itr].getIndex();
		h_l2_etacorr->Fill( (*nt.l2_emcl_eta)[i_emcl], 
				  (*nt.l2_trk_sitrack_eGamma_TrigL2Track_eta)[i_trk]);
	      }
	    }
	  }
	}
      }
    }
  }
}
