/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void InDet_Muon_Track(int SaveOpt=0){

  cout<<"Building the correlations between InDet and Muons tracks ..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(11111);

  const int MAX = 10;
  int ievent;

  // ID variables
  int trk_nTracks;
  float trk_Rec_d0[10], trk_Rec_z0[10], trk_Rec_phi0[10], trk_Rec_theta0[10], trk_Rec_chi2[10];

  // Muon variables
  float betabarrel;
  float alphabarrel;

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);
  tree->SetBranchStatus("betabarrel",1);
  tree->SetBranchStatus("alphabarrel",1);


  // Get branches
  tree->SetBranchAddress("Event",&ievent);

  // Inner Detector tracks
  tree->SetBranchAddress("trk_nTracks",&trk_nTracks);
  tree->SetBranchAddress("trk_Rec_d0",&trk_Rec_d0);
  tree->SetBranchAddress("trk_Rec_z0",&trk_Rec_z0);
  tree->SetBranchAddress("trk_Rec_phi0",&trk_Rec_phi0);
  tree->SetBranchAddress("trk_Rec_theta0",&trk_Rec_theta0);
  tree->SetBranchAddress("trk_Rec_chi2",&trk_Rec_chi2);

  // Muon tracks
  tree->SetBranchAddress("betabarrel",&betabarrel);
  tree->SetBranchAddress("alphabarrel",&alphabarrel);

  TCanvas *c1 = new TCanvas("c1","Inner Detector-Muons track correlations");
  c1->Divide(2,1);

  TCanvas *c2 = new TCanvas("c2","Inner Detector-Muons z0 and dz/dx");
  c2->Divide(2,1);

  int entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  TH2F *hdzalpha;
  sprintf(name,"dzalpha");
  sprintf(name2,"dzalpha");
  hdzalpha = new TH2F(name2,name,100,-0.02,0.02,100, -0.02,0.02);
  hdzalpha->SetXTitle("dz/dx");
  hdzalpha->SetYTitle("alphabarrel");
  hdzalpha->GetXaxis()->SetTitleFont(22);
  hdzalpha->GetYaxis()->SetTitleFont(22);

  TH1F *hdz;
  sprintf(name,"dz");
  sprintf(name2,"dz");
  hdz = new TH1F(name2,name,70,-0.005,0.01);
  hdz->SetXTitle("dz/dx");
  hdz->GetXaxis()->SetTitleFont(22);

  TH1F *hdz2;
  sprintf(name,"dz");
  sprintf(name2,"dz");
  hdz2 = new TH1F(name2,name,70,-0.005,0.01);
  hdz2->SetXTitle("dz/dx");
  hdz2->GetXaxis()->SetTitleFont(22);

  TH1F *hz;
  sprintf(name,"z");
  sprintf(name2,"z");
  hz = new TH1F(name2,name,70,-130.,30.);
  hz->SetXTitle("z0");
  hz->GetXaxis()->SetTitleFont(22);

  TH1F *hz2;
  sprintf(name,"z");
  sprintf(name2,"z");
  hz2 = new TH1F(name2,name,70,-130.,30.);
  hz2->SetXTitle("z0");
  hz2->GetXaxis()->SetTitleFont(22);

  TProfile *histo;
  sprintf(name,"Inner Detector-Muons tracks");
  sprintf(name2,"corr");

  histo  = new TProfile("histo","Inner Detector-Muons tracks z0",30,-45.,35.,-200.,100.);
  histo->SetXTitle("z_{0}(Pixels+TRT Track) [mm]");
  histo->SetYTitle("z_{0}(RPC+MDT Track) [mm]");
  histo->GetXaxis()->SetTitleFont(22);
  histo->GetYaxis()->SetTitleFont(22);

  histodz  = new TProfile("histodz","Inner Detector-Muons tracks dz/dx",3,-0.003,0.012,-0.05,0.05);
  histodz->SetXTitle("dz/dx(Pixels+TRT Track) ");
  histodz->SetYTitle("dz/dx(RPC+MDT Track) ");
  histodz->GetXaxis()->SetTitleFont(22);
  histodz->GetYaxis()->SetTitleFont(22);


  for (int event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over ID tracks
    //    if (trk_nTracks == 1) {
    for (int track=0; track<trk_nTracks; track++) {
      Float_t zID = trk_Rec_z0[track];    
      float z  = trk_Rec_z0[track]+trk_Rec_d0[track]*(tan(trk_Rec_phi0[track])/tan(trk_Rec_theta0[track]));
      if (abs(betabarrel)<500.){
	//histo->Fill(zID,betabarrel,1.);
	histo->Fill(z,betabarrel,1.);
	hz->Fill(z);
	hz2->Fill(betabarrel);
      }
      float gz = cos(trk_Rec_theta0[track])/(sin(trk_Rec_theta0[track])*cos(trk_Rec_phi0[track]));
      if (abs(alphabarrel)<0.05){
	//cout << "gz " << gz << endl;
	//cout << "alphabarrel " << alphabarrel << endl;
	histodz->Fill(gz,alphabarrel,1.);
	hdzalpha->Fill(gz,alphabarrel);
	hdz->Fill(gz);
	hdz2->Fill(alphabarrel);

      }
      //}
    }
  }
  //histo->SetMaximum(-40);
  //histo->SetMinimum(-120);

  c1->cd(1);
  histo->SetMarkerStyle(8);

  histo->Draw("");

  histo->Fit("pol1","","",-30,20);
  histo->GetFunction("pol1")->SetLineColor(kRed);
  histo->GetFunction("pol1")->SetLineWidth(2);

  c1->Update();

  c1->cd(2);
  histodz->SetMarkerStyle(8);
  histodz->Draw("");
  //hdzalpha->Draw("");
  histodz->Fit("pol1","","",-0.05,0.05);
  histodz->GetFunction("pol1")->SetLineColor(kRed);
  histodz->GetFunction("pol1")->SetLineWidth(2);

  c1->Update();

  hdz2->SetFillColor(kRed);
  hdz->SetFillColor(kBlue);

  hz2->SetFillColor(kRed);
  hz->SetFillColor(kBlue);

  c2->cd(1);
  hdz->Draw("");
  hdz2->Draw("same");
  c2->Update();  

  leg = new TLegend(0.3,0.78,0.6,0.89);
  leg->AddEntry(hdz,"Inner Detector","f");
  leg->AddEntry(hdz2,"Muons","f");
   leg->Draw();
  // and add a header (or "title") for the legend
  leg->SetHeader("ID-Muons dz/dx");
  leg->Draw();

  c2->cd(2);
  hz->Draw("");
  hz2->Draw("same");

  leg = new TLegend(0.3,0.78,0.6,0.89);
  leg->AddEntry(hz,"Inner Detector","f");
  leg->AddEntry(hz2,"Muons","f");
   leg->Draw();
  // and add a header (or "title") for the legend
  leg->SetHeader("ID-Muons z0");
  leg->Draw();

  c2->Update();  


  sprintf(title, "run %d (%d events)", run, entries);
  c1->cd();
  TPaveText *pt = new TPaveText(0.397, 0.001, 0.6, 0.011);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.013);
  text->SetTextColor(10);
  pt->Draw();
  c1->Update();

  sprintf(title, "run %d (%d events)", run, entries);
  c2->cd();
  TPaveText *pt = new TPaveText(0.397, 0.001, 0.6, 0.011);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.013);
  text->SetTextColor(10);
  pt->Draw();
  c2->Update();
  
  gStyle->SetOptStat("nemr");

  if (SaveOpt==1) {
    c1->Print("Correlations_histograms.ps"); 
    c2->Print("Correlations_histograms.ps"); 
  }
  else if(SaveOpt==2) {
    sprintf(sname,"InDet_Muon_track.gif");
    c1->Print(sname);
    sprintf(sname,"InDet_Muon_Track2.gif");
    c2->Print(sname);
  }
  else if(SaveOpt==3) {
    c1->Print(psfilename);
    c2->Print(psfilename);
  }

  /*
  // convert track parameters to y,z,gy,gz
  float y  = trk_Rec_d0*(cos(trk_Rec_phi0) + tan(trk_Rec_phi0)*sin(trk_Rec_phi0));
  float z  = trk_Rec_z0+trk_Rec_d0*(tan(trk_Rec_phi0)/tan(trk_Rec_theta0));
  float gy = tan(trk_Rec_phi0);
  float gz = cos(trk_Rec_theta0)/(sin(trk_Rec_theta0)*cos(trk_Rec_phi0));

  // extrapolate to the given x
  float xmuons = 30000;
  float ext[2] = extrapolate(y,z,gy,gz,x);
  std::cout << "ext y " << ext[0] << std::endl;
  std::cout << "ext z " << ext[1] << std::endl;
  */
}

/*
float ext[2] extrapolate(float y,float z, float gy, float gz, float x){

  ext[0] = y + gy*x;
  ext[1] = z + gz*x;
  return ext;
}
*/


