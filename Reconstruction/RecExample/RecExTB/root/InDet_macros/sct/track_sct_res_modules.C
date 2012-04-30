/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void track_sct_res_modules(int SaveOpt=0) {

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(1111);

  const int LAYERS = 4;
  const int PLANES_IN_LAYER = 4;
  const int MAX_SCT_HITS = 16;           //max no. of sct hits per track
  const int MAX_TRACKS = 60;            //max no. of tracks per event
  const char HISTOTYPE[10] = "";

  // Enabling only tracking branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("Run",1);

  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  int sct_nhits[MAX_TRACKS];
  int sct_layer[MAX_TRACKS][MAX_SCT_HITS], sct_phi[MAX_TRACKS][MAX_SCT_HITS],sct_side[MAX_TRACKS][MAX_SCT_HITS];
  float sct_res[MAX_TRACKS][MAX_SCT_HITS];
  float sct_pull[MAX_TRACKS][MAX_SCT_HITS];

  tree->SetBranchAddress("Run", &run);
  tree->SetBranchAddress("trk_nTracks", &ntracks);
  tree->SetBranchAddress("trk_nSctHits", sct_nhits);
  tree->SetBranchAddress("trk_SctRes", sct_res);
  tree->SetBranchAddress("trk_SctPull", sct_pull);
  tree->SetBranchAddress("trk_Sct_layer", sct_layer);
  tree->SetBranchAddress("trk_Sct_phi", sct_phi);
  tree->SetBranchAddress("trk_Sct_side", sct_side);


  TCanvas *cRes = new TCanvas("cRes","Tracking Offline Monitor: SCT Residuals");
  TCanvas *cPull = new TCanvas("cPull","Tracking Offline Monitor: SCT Pulls");
  cRes->SetFillColor(10);
  cPull->SetFillColor(10);
   //sprintf(name,"%d events", entries);
  //cRes->SetLabel(name);

  cRes->Divide(LAYERS,PLANES_IN_LAYER);
  cPull->Divide(LAYERS,PLANES_IN_LAYER);

  TPad* subcanvas;

  int i=1;

  TH1F *histoRes[PLANES_IN_LAYER*LAYERS];
  TH1F *histoPull[PLANES_IN_LAYER*LAYERS];

  // loop over all modules
  for (int layer=0;layer<LAYERS;layer++) {
    for (int phi=0;phi<2;phi++) {
      for (int side=0;side<2;side++) {
	// calc module no.
	module = 2*(2*layer+phi)+side;
	cout << " modules " << module << endl;

	// get current subcanvas
	sprintf(name,"cRes_%d",i);
	cRes->cd(i);
	subcanvas = (TPad*)(cRes->GetPrimitive(name));
	subcanvas->SetFillStyle(4000);
	sprintf(name,"cPull_%d",i);
	cPull->cd(i);
	subcanvas = (TPad*)(cPull->GetPrimitive(name));
	subcanvas->SetFillStyle(4000);

	// set name for histo
	sprintf(name,"Sct Residuals Module %d: Layer %d, Phi %d, Side %d",module,layer,phi,side);
	sprintf(name2,"SCTRes[%d,%d,%d]",layer,phi,side);
	histoRes[module] = new TH1F(name2,name,100,-0.5,0.5);
	histoRes[module]->SetXTitle("r_{#eta}  = #phi_{track} - #phi_{hit} [mm]");
	//histoRes->GetXaxis()->SetTitleFont(22);
	histoRes[module]->GetXaxis()->SetTitleSize(.04);

	sprintf(name,"Sct Pull Phi Module %d: Layer %d, Phi %d, Side %d",module,layer,phi,side);
	sprintf(name2,"SCTPull[%d,%d,%d]",layer,phi,side);
	histoPull[module] = new TH1F(name2,name,100,-5.,5.);
	histoPull[module]->SetXTitle("#frac{ r_{#phi} }{ #sqrt{ #rho_{track}^{2} + #rho_{hit}^{2} } }");
	histoPull[module]->GetXaxis()->SetTitleSize(0.03);
	histoPull[module]->GetXaxis()->SetTitleOffset(1.0);

	i++;
      }
    }
  }

  int hit;

  for (ievent=0; ievent<entries; ievent++) {
    // Get data for current event
    tree->GetEntry(ievent);
    //cout << " number of tracks " << ntracks << endl;
    // loop over all tracks
    for (int track=0; track<ntracks; track++) {
      //cout << " number of hits " << sct_nhits[track] << endl;
      // loop over all sct hits of current track
      for (hit=0; hit<sct_nhits[track]; hit++) {

        module = 2*(2*sct_layer[track][hit]+sct_phi[track][hit])+sct_side[track][hit] ;
	//cout << "Filling module " << module << endl;
        histoRes[module]->Fill(sct_res[track][hit]);
        histoPull[module]->Fill(sct_pull[track][hit]);
      }
    }
  }


  //sprintf(name2,"%d events, Run %d", entries, run);
  //sprintf(name,"%s", FILE);
  sprintf(name2,"Run %d", run);
  sprintf(name,"%d events", entries);

  i=1;

  // loop over all modules
  for (int layer=0;layer<LAYERS;layer++) {
    for (int phi=0;phi<2;phi++) {
      for (int side=0;side<2;side++) {
	// calc module no.
	module = 2*(2*layer+phi)+side;
	cRes->cd(i);
	histoRes[module]->SetFillStyle(4000);
	histoRes[module]->Draw(HISTOTYPE);
	//histoRes[module]->Fit("gaus");
	//histoRes[module]->GetFunction("gaus")->SetLineColor(kRed);
	//histoRes[module]->GetFunction("gaus")->SetLineWidth(2);
	//cRes->Update();

	cPull->cd(i);
	histoPull[module]->SetFillStyle(4000);
	histoPull[module]->Draw(HISTOTYPE);
	//histoPull[module]->Fit("gaus");
	//histoPull[module]->GetFunction("gaus")->SetLineColor(kRed);
	//histoPull[module]->GetFunction("gaus")->SetLineWidth(2);
	//cPull->Update();

	i++;
      }
    }
  }

  if (SaveOpt==1) { sprintf(sname,"Track_Scts_Res_Pulls_mod.ps("); }
  else if(SaveOpt==2) { sprintf(sname,"Track_Scts_Res_Pulls_mod.gif(");}
  else if(SaveOpt==3) { sname = psfilename; };

  if (SaveOpt>0) {
    cRes->Print(sname);
    cPull->Print(sname);
  }
}

