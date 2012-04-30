/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void HL_histo(){

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
 
  char name0[80],name1[80],name[80],name2[80];
  const int MAX_TRT_HITS = 100;           //max no. of trt hits per track
  const int MAX_TRACKS = 60;            //max no. of tracks per event
  
  int n_tracks,n_hits[MAX_TRACKS],hl[MAX_TRACKS][MAX_TRT_HITS];
 
  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;
  
  //Get branches needed 
  tree->SetBranchAddress("trk_nTracks", &n_tracks);
  tree->SetBranchAddress("trk_nTrtHits",n_hits);
  tree->SetBranchAddress("trk_Trt_HL",hl);
 
  //Defining the Canvas
  TCanvas *c11 = new TCanvas("c11","Tracking Offline Monitor: HL histograms");
  c11->Divide(2,1);
  
  TH1F *histo[2];
  sprintf(name0,"TRT histogram: HL=0");
  sprintf(name1,"TRT histogram: HL=1");
  sprintf(name,"HL=0");
  sprintf(name2,"HL=1");
  
  histo[0] = new TH1F(name,name0,60,0,60);
  histo[1] = new TH1F(name2,name1,60,0,60);
  
  //Filling the histogram
  for (int event=0; event<entries; event++) {
    tree->GetEntry(event);  
    for (int track=0; track<n_tracks; track++) {
      for (int hit=0; hit<n_hits[track]; hit++) {
	if(hl[track][hit] == 0){
	  histo[0]->Fill(n_hits[track]);
	}
	else if{
	  histo[1]->Fill(n_hits[track]);
	}
      }
    }
  }
  
  c11->cd(1);
  histo[0]->Draw();
  c11->Update();
  
  c11->cd(2);
  histo[1]->Draw();
  c11->Update();
}

