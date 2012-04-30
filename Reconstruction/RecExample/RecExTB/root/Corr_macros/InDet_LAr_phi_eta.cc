/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void InDet_LAr_phi_eta(int SaveOpt=0){

  cout<<"Building the correlations between InDet and LAr phi and eta ..."<<endl;

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
  float trk_Rec_d0[10], trk_Rec_z0[10], trk_Rec_phi0[10], trk_Rec_theta0[10], trk_Rec_chi2[10],trk_Rec_eta[10];

  // LAr variables
  int cl_nc;
  float cl_eta[10],cl_phi[10];

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);
  tree->SetBranchStatus("cl_nc_cmb",1);
  tree->SetBranchStatus("cl_eta_cmb",1);
  tree->SetBranchStatus("cl_phi_cmb",1);


  // Get branches
  tree->SetBranchAddress("Event",&ievent);

  // Inner Detector tracks
  tree->SetBranchAddress("trk_nTracks",&trk_nTracks);
  tree->SetBranchAddress("trk_Rec_d0",&trk_Rec_d0);
  tree->SetBranchAddress("trk_Rec_z0",&trk_Rec_z0);
  tree->SetBranchAddress("trk_Rec_phi0",&trk_Rec_phi0);
  tree->SetBranchAddress("trk_Rec_theta0",&trk_Rec_theta0);
  //tree->SetBranchAddress("trk_Rec_eta",&trk_Rec_eta);
  tree->SetBranchAddress("trk_Rec_chi2",&trk_Rec_chi2);

  // LAr clusters
  tree->SetBranchAddress("cl_nc_cmb",&cl_nc);
  tree->SetBranchAddress("cl_eta_cmb",&cl_eta);
  tree->SetBranchAddress("cl_phi_cmb",&cl_phi);

  TCanvas *c1 = new TCanvas("c1","Inner Detector-Calorimeter correlations");
  c1->Divide(2,1);

  TCanvas *c2 = new TCanvas("c2","Inner Detector-Calorimeter correlations");
  c2->Divide(2,1);

  int entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  //TH2F *h2phi;
  //sprintf(name,"h2phi");
  //sprintf(name2,"h2phi");
  //h2phi = new TH2F(name2,name,100,-0.06,0.06,100, -0.02,0.02);
  //h2phi = new TH2F(name2,name,100,0.16,0.3,100, -0.02,0.02);
  //h2phi->SetXTitle("phi (LAr)");
  //h2phi->SetYTitle("phi (ID)");
  //h2phi->GetXaxis()->SetTitleFont(22);
  //h2phi->GetYaxis()->SetTitleFont(22);

  TH1F *hphi;
  sprintf(name,"phi");
  sprintf(name2,"phi");
  hphi = new TH1F(name2,name,100,-0.02,0.03);
  hphi->SetXTitle("phi");
  hphi->GetXaxis()->SetTitleFont(22);

  TH1F *hphi2;
  sprintf(name,"phi2");
  sprintf(name2,"phi2");
  hphi2 = new TH1F(name2,name,100,-0.02,0.03);
  hphi2->SetXTitle("phi");
  hphi2->GetXaxis()->SetTitleFont(22);

  TH1F *heta;
  sprintf(name,"eta");
  sprintf(name2,"eta");
  heta = new TH1F(name2,name,100,-0.04,0.04);
  heta->SetXTitle("eta");
  heta->GetXaxis()->SetTitleFont(22);

  TH1F *heta2;
  sprintf(name,"eta2");
  sprintf(name2,"eta2");
  heta2 = new TH1F(name2,name,100,-0.04,0.04);
  heta2->SetXTitle("eta");
  heta2->GetXaxis()->SetTitleFont(22);

  TProfile *histo;
  sprintf(name,"ID Tracks-LAr clusters");
  sprintf(name2,"corrIDLAr");

  histo  = new TProfile("histo","Inner Detector-LAr phi",30,-0.05,0.05,-0.05,0.05);
  histo->SetXTitle("phi (LAr)");
  histo->SetYTitle("phi (ID)");
  histo->GetXaxis()->SetTitleFont(22);
  histo->GetYaxis()->SetTitleFont(22);

  histoeta  = new TProfile("histoeta","Inner Detector-LAr eta",3,-0.003,0.015,0.16,0.3);
  histoeta->SetXTitle("eta (ID) ");
  histoeta->SetYTitle("eta (LAr)");
  histoeta->GetXaxis()->SetTitleFont(22);
  histoeta->GetYaxis()->SetTitleFont(22);


  for (int event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over ID tracks
    if (trk_nTracks == 1 && cl_nc == 1) {
    for (int track=0; track<trk_nTracks; track++) {
      //Float_t etaID = trk_Rec_eta[track];    
      Float_t phiID = trk_Rec_phi0[track];    
      for (int cluster=0;cluster<cl_nc;cluster++){
	float phiLAr = cl_phi[cluster];
	float etaLAr = cl_eta[cluster];
	histo->Fill(phiLAr,phiID,1.);
	//h2phi->Fill(phiLAr,phiID);
	//histoeta->Fill(etaID,etaLAr,1.);
	hphi->Fill(phiLAr);
	hphi2->Fill(phiID);
	heta->Fill(etaLAr);
	//heta2->Fill(etaID);
      }     
    }
    }
  }


  //histo->SetMaximum(-40);
  //histo->SetMinimum(-120);

  c1->cd(1);
  histo->SetMarkerStyle(8);

  histoeta->Draw("");

  //histo->Fit("pol1","","",-30,20);
  //histo->GetFunction("pol1")->SetLineColor(kRed);
  //histo->GetFunction("pol1")->SetLineWidth(2);

  c1->Update();

  c1->cd(2);
  histo->Draw("");
  c1->Update();

  hphi2->SetFillColor(kRed);
  hphi->SetFillColor(kBlue);

  heta2->SetFillColor(kRed);
  heta->SetFillColor(kBlue);

  c2->cd(1);
  hphi2->Draw("");
  hphi->Draw("same");
  c2->Update();  

  leg = new TLegend(0.5,0.78,0.8,0.89);
  leg->AddEntry(hphi,"LAr","f");
  leg->AddEntry(hphi2,"Inner Detector","f");
  leg->Draw();
  // and add a header (or "title") for the legend
  leg->SetHeader("LAr - Inner Detector phi");
  leg->Draw();

  c2->cd(2);
  heta->Draw("");
  heta2->Draw("same");

  leg = new TLegend(0.3,0.78,0.6,0.89);
  leg->AddEntry(heta,"LAr","f");
  leg->AddEntry(heta2,"Inner Detector","f");
  leg->Draw();
  // and add a header (or "title") for the legend
  leg->SetHeader("LAr - Inner Detector eta");
  leg->Draw();

  c2->Update();  
  
  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"ELAr_Muons.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);


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


