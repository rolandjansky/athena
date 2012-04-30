/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// pixel residuals and pulls per module with cut on number of pixel hits

void track_pixel_res_modules_nhits(char FILE[100]="/localdisk/tilebeam/RecExTB/Reconstruction/RecExample/RecExTB/RecExTB-00-00-41/run/ntuple_run2100100_pions_KF2.root") {


  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(1111);

  const int NHITS_CUT = 2;
  const int LAYERS = 3;
  const int MODULES_IN_PHI = 2;
  const int MAX_PIX_HITS = 8;           //max no. of pixel hits per track
  const int MAX_TRACKS = 60;            //max no. of tracks per event
  const char HISTOTYPE[10] = "";

  int module;

  // Enabling only tracking branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("trk_mc_*",0);
  tree->SetBranchStatus("Run",1);

  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  int event, ntracks=0;
  int run;
  int pixel_nhits[MAX_TRACKS];
  int pixel_layer[MAX_TRACKS][MAX_PIX_HITS], pixel_phi[MAX_TRACKS][MAX_PIX_HITS];
  float pixel_res_phi[MAX_TRACKS][MAX_PIX_HITS], pixel_res_eta[MAX_TRACKS][MAX_PIX_HITS];
  float pixel_pull_phi[MAX_TRACKS][MAX_PIX_HITS], pixel_pull_eta[MAX_TRACKS][MAX_PIX_HITS];

  tree->SetBranchAddress("Run", &run);
  tree->SetBranchAddress("trk_nTracks", &ntracks);
  tree->SetBranchAddress("trk_nPixelHits", pixel_nhits);
  tree->SetBranchAddress("trk_PixResPhi", pixel_res_phi);
  tree->SetBranchAddress("trk_PixResEta", pixel_res_eta);
  tree->SetBranchAddress("trk_PixPullPhi", pixel_pull_phi);
  tree->SetBranchAddress("trk_PixPullEta", pixel_pull_eta);
  tree->SetBranchAddress("trk_Pix_layer", pixel_layer);
  tree->SetBranchAddress("trk_Pix_phi", pixel_phi);


  TCanvas *cResEta = new TCanvas("cResEta","Tracking Offline Monitor: Pixel Residuals Eta");
  TCanvas *cPullEta = new TCanvas("cPullEta","Tracking Offline Monitor: Pixel Pulls Eta");
  TCanvas *cResPhi = new TCanvas("cResPhi","Tracking Offline Monitor: Pixel Residuals Phi");
  TCanvas *cPullPhi = new TCanvas("cPullPhi","Tracking Offline Monitor: Pixel Pulls Phi");
  cResEta->SetFillColor(10);
  cPullEta->SetFillColor(10);
  cResPhi->SetFillColor(10);
  cPullPhi->SetFillColor(10);
  //sprintf(name,"%d events", entries);
  //cRes->SetLabel(name);

  cResEta->Divide(LAYERS,MODULES_IN_PHI);
  cPullEta->Divide(LAYERS,MODULES_IN_PHI);
  cResPhi->Divide(LAYERS,MODULES_IN_PHI);
  cPullPhi->Divide(LAYERS,MODULES_IN_PHI);

  TPad* subcanvas;

  int i=1;

  TH1F *histoResEta[6];
  TH1F *histoResPhi[6];
  TH1F *histoPullEta[6];
  TH1F *histoPullPhi[6];

	// loop over all modules
  for (int phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (int layer=0;layer<LAYERS;layer++) {

      // calc module no.
      module = MODULES_IN_PHI * layer + phi;

      // get current subcanvas
      sprintf(name,"cResEta_%d",i);
      cResEta->cd(i);
      subcanvas = (TPad*)(cResEta->GetPrimitive(name));
      subcanvas->SetFillStyle(4000);
      sprintf(name,"cResPhi_%d",i);
      cResPhi->cd(i);
      subcanvas = (TPad*)(cResPhi->GetPrimitive(name));
      subcanvas->SetFillStyle(4000);
      sprintf(name,"cPullEta_%d",i);
      cPullEta->cd(i);
      subcanvas = (TPad*)(cPullEta->GetPrimitive(name));
      subcanvas->SetFillStyle(4000);
      sprintf(name,"cPullPhi_%d",i);
      cPullPhi->cd(i);
      subcanvas = (TPad*)(cPullPhi->GetPrimitive(name));
      subcanvas->SetFillStyle(4000);

      // set name for histo
      sprintf(name,"Pixel Residuals Eta Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"PixResEta[%d,%d]",layer,phi);
      histoResEta[module] = new TH1F(name2,name,100,-0.6,0.6);
      histoResEta[module]->SetXTitle("r_{#eta}  = #eta_{track} - #eta_{hit} [mm]");
      //histoResPhi->GetXaxis()->SetTitleFont(22);
      histoResEta[module]->GetXaxis()->SetTitleSize(.04);

      sprintf(name,"Pixel Residuals Phi Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"PixResPhi[%d,%d]",layer,phi);
      histoResPhi[module] = new TH1F(name2,name,100,-0.08,0.08);
      histoResPhi[module]->SetXTitle("r_{#phi}  = #phi_{track} - #phi_{hit} [mm]");
      //histoResPhi->GetXaxis()->SetTitleFont(22);
      histoResPhi[module]->GetXaxis()->SetTitleSize(.04);


      sprintf(name,"Pixel Pull Phi Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"PixPullPhi[%d,%d]",layer,phi);
      histoPullPhi[module] = new TH1F(name2,name,100,-5.,5.);
      histoPullPhi[module]->SetXTitle("#frac{ r_{#phi} }{ #sqrt{ #rho_{track}^{2} + #rho_{hit}^{2} } }");
      histoPullPhi[module]->GetXaxis()->SetTitleSize(0.03);
      histoPullPhi[module]->GetXaxis()->SetTitleOffset(1.0);
      //histoPullPhi->GetXaxis()->SetTitleFont(22);
      //histoPullPhi->GetXaxis()->SetTitleSize(.02);


      sprintf(name,"Pixel Pull Eta Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"PixPullEta[%d,%d]",layer,phi);
      histoPullEta[module] = new TH1F(name2,name,100,-5.,5.);
      histoPullEta[module]->SetXTitle("#frac{ r_{#eta} }{ #sqrt{ #rho_{track}^{2} + #rho_{hit}^{2} } }");
      histoPullEta[module]->GetXaxis()->SetTitleSize(0.03);
      histoPullEta[module]->GetXaxis()->SetTitleOffset(1.0);

      i++;
    }
  }


  int hit;

  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over all tracks
    for (int track=0; track<ntracks; track++) {
      if (pixel_nhits[track] == NHITS_CUT) {
        // loop over all pixel hits of current track
        for (hit=0; hit<pixel_nhits[track]; hit++) {
          module = MODULES_IN_PHI * pixel_layer[track][hit] + pixel_phi[track][hit];
          histoResPhi[module]->Fill(pixel_res_phi[track][hit]);
          histoResEta[module]->Fill(pixel_res_eta[track][hit]);
          histoPullPhi[module]->Fill(pixel_pull_phi[track][hit]);
          histoPullEta[module]->Fill(pixel_pull_eta[track][hit]);
        }
      }
    }
  }


  sprintf(name2,"%d events, Run %d", entries, run);
  sprintf(name,"%s", FILE);

  i=1;
	// loop over all modules
  for (int phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (int layer=0;layer<LAYERS;layer++) {

      // calc module no.
      module = MODULES_IN_PHI * layer + phi;

      cResPhi->cd(i);
      histoResPhi[module]->SetFillStyle(4000);
      histoResPhi[module]->Draw(HISTOTYPE);
      histoResPhi[module]->Fit("gaus");
      histoResPhi[module]->GetFunction("gaus")->SetLineColor(kRed);
      histoResPhi[module]->GetFunction("gaus")->SetLineWidth(2);
      //cResPhi->Update();

      cResEta->cd(i);
      histoResEta[module]->SetFillStyle(4000);
      histoResEta[module]->Draw(HISTOTYPE);
      histoResEta[module]->Fit("gaus");
      histoResEta[module]->GetFunction("gaus")->SetLineColor(kRed);
      histoResEta[module]->GetFunction("gaus")->SetLineWidth(2);
      //cResEta->Update();

      cPullPhi->cd(i);
      histoPullPhi[module]->SetFillStyle(4000);
      histoPullPhi[module]->Draw(HISTOTYPE);
      histoPullPhi[module]->Fit("gaus");
      histoPullPhi[module]->GetFunction("gaus")->SetLineColor(kRed);
      histoPullPhi[module]->GetFunction("gaus")->SetLineWidth(2);
      //cPullPhi->Update();

      cPullEta->cd(i);
      histoPullEta[module]->SetFillStyle(4000);
      histoPullEta[module]->Draw(HISTOTYPE);
      histoPullEta[module]->Fit("gaus");
      histoPullEta[module]->GetFunction("gaus")->SetLineColor(kRed);
      histoPullEta[module]->GetFunction("gaus")->SetLineWidth(2);
      //cPullEta->Update();

      i++;
    }
  }

  TPaveText *pt;

  TText *text;

  cResPhi->cd();
  pt = new TPaveText(0.00230947,0.00322061,0.178984,0.037037,"br");
  pt->SetBorderSize(0);
  pt->SetFillColor(20);
  text = pt->AddText(name2);
  text = pt->AddText(name);
  pt->Draw();
  //cRes->Modified();
  cResPhi->Update();

  cResEta->cd();
  pt = new TPaveText(0.00230947,0.00322061,0.178984,0.037037,"br");
  pt->SetBorderSize(0);
  pt->SetFillColor(20);
  text = pt->AddText(name2);
  text = pt->AddText(name);
  pt->Draw();
  //cRes->Modified();
  cResEta->Update();

  cPullPhi->cd();
  pt = new TPaveText(0.00230947,0.00322061,0.178984,0.037037,"br");
  pt->SetBorderSize(0);
  pt->SetFillColor(20);
  text = pt->AddText(name2);
  text = pt->AddText(name);
  pt->Draw();
  //cRes->Modified();
  cPullPhi->Update();

  cPullEta->cd();
  pt = new TPaveText(0.00230947,0.00322061,0.178984,0.037037,"br");
  pt->SetBorderSize(0);
  pt->SetFillColor(20);
  text = pt->AddText(name2);
  text = pt->AddText(name);
  pt->Draw();
  //cRes->Modified();
  cPullEta->Update();

  sprintf(name,"Cut on trk_nPixelHits == %d", NHITS_CUT);

  cResPhi->cd();
  pt = new TPaveText(0.2,0.00322061,0.4,0.037037,"br");
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText(name);
  pt->Draw();
  //cRes->Modified();
  cResPhi->Update();

  cResEta->cd();
  pt = new TPaveText(0.2,0.00322061,0.4,0.037037,"br");
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText(name);
  pt->Draw();
  //cRes->Modified();
  cResEta->Update();

  cPullPhi->cd();
  pt = new TPaveText(0.2,0.00322061,0.4,0.037037,"br");
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText(name);
  pt->Draw();
  //cRes->Modified();
  cPullPhi->Update();

  cPullEta->cd();
  pt = new TPaveText(0.2,0.00322061,0.4,0.037037,"br");
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText(name);
  pt->Draw();
  //cRes->Modified();
  cPullEta->Update();
}

