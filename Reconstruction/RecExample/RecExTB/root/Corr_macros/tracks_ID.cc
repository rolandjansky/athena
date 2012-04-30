/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void tracks_ID(int SaveOpt=0) {

  gROOT->Reset();

  cout<<"Building the correlations between InDet tracks ..."<<endl;

  // Styles
  gStyle->SetOptStat("111111");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(11111);

  const int MAX = 10;

  // Pixel variables
  int trk_nTracksPixel;
  float trk_Rec_d0Pixel[MAX], trk_Rec_z0Pixel[MAX], trk_Rec_phi0Pixel[MAX], trk_Rec_theta0Pixel[MAX], trk_Rec_chi2Pixel[MAX];

  // SCT variables
  int trk_nTracksSCT;
  float trk_Rec_d0SCT[MAX], trk_Rec_z0SCT[MAX], trk_Rec_phi0SCT[MAX], trk_Rec_theta0SCT[MAX], trk_Rec_chi2SCT[MAX];

  // TRT variables
  int trk_nTracksTRT;
  float trk_Rec_d0TRT[MAX], trk_Rec_z0TRT[MAX], trk_Rec_phi0TRT[MAX], trk_Rec_theta0TRT[MAX], trk_Rec_chi2TRT[MAX];

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);

  // Get branches
  tree->SetBranchAddress("Event",&ievent);

  // Pixel tracks
  tree->SetBranchAddress("trk_nTracksPixel",&trk_nTracksPixel);
  tree->SetBranchAddress("trk_Rec_d0Pixel",&trk_Rec_d0Pixel);
  tree->SetBranchAddress("trk_Rec_z0Pixel",&trk_Rec_z0Pixel);
  tree->SetBranchAddress("trk_Rec_phi0Pixel",&trk_Rec_phi0Pixel);
  tree->SetBranchAddress("trk_Rec_theta0Pixel",&trk_Rec_theta0Pixel);
  tree->SetBranchAddress("trk_Rec_chi2Pixel",&trk_Rec_chi2Pixel);

  // SCT tracks
  tree->SetBranchAddress("trk_nTracksSCT",&trk_nTracksSCT);
  tree->SetBranchAddress("trk_Rec_d0SCT",&trk_Rec_d0SCT);
  tree->SetBranchAddress("trk_Rec_z0SCT",&trk_Rec_z0SCT);
  tree->SetBranchAddress("trk_Rec_phi0SCT",&trk_Rec_phi0SCT);
  tree->SetBranchAddress("trk_Rec_theta0SCT",&trk_Rec_theta0SCT);
  tree->SetBranchAddress("trk_Rec_chi2SCT",&trk_Rec_chi2SCT);

  // TRT tracks
  tree->SetBranchAddress("trk_nTracksTRT",&trk_nTracksTRT);
  tree->SetBranchAddress("trk_Rec_d0TRT",&trk_Rec_d0TRT);
  tree->SetBranchAddress("trk_Rec_z0TRT",&trk_Rec_z0TRT);
  tree->SetBranchAddress("trk_Rec_phi0TRT",&trk_Rec_phi0TRT);
  tree->SetBranchAddress("trk_Rec_theta0TRT",&trk_Rec_theta0TRT);
  tree->SetBranchAddress("trk_Rec_chi2TRT",&trk_Rec_chi2TRT);

  TCanvas *c1 = new TCanvas("c1","Inner Detector track correlations");
  c1->Divide(3,1);

  int entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;


  TH2F *hPixelSCT;
  sprintf(name,"PixelSCT");
  sprintf(name2,"PixelSCT");
  hPixelSCT = new TH2F(name2,name,100,-0.01,0.01,100, -0.01,0.01);
  hPixelSCT->SetXTitle("Pixel");
  hPixelSCT->SetYTitle("SCT");
  hPixelSCT->GetXaxis()->SetTitleFont(22);
  hPixelSCT->GetYaxis()->SetTitleFont(22);

  TH2F *hPixelTRT;
  sprintf(name,"PixelTRT");
  sprintf(name2,"PixelTRT");
  hPixelTRT = new TH2F(name2,name,100,-0.01,0.01,100, -0.01,0.01);
  hPixelTRT->SetXTitle("Pixel");
  hPixelTRT->SetYTitle("TRT");
  hPixelTRT->GetXaxis()->SetTitleFont(22);
  hPixelTRT->GetYaxis()->SetTitleFont(22);

  TH2F *hSCTTRT;
  sprintf(name,"SCTTRT");
  sprintf(name2,"SCTTRT");
  hSCTTRT = new TH2F(name2,name,100,-0.01,0.01,100, -0.01,0.01);
  hSCTTRT->SetXTitle("SCT");
  hSCTTRT->SetYTitle("TRT");
  hSCTTRT->GetXaxis()->SetTitleFont(22);
  hSCTTRT->GetYaxis()->SetTitleFont(22);

  for (int event=0; event<entries; event++) {
    // Get data for current event
    cout << " event " << event << endl;
    tree->GetEntry(event);

    cout << "Pixel tracks " << trk_nTracksPixel << endl;
    cout << "SCT tracks " << trk_nTracksSCT << endl;
    cout << "TRT tracks " << trk_nTracksTRT << endl;

    // loop over ID tracks
    if (trk_nTracksPixel == 1 && trk_nTracksSCT == 1){
      cout << "Filling Pixels SCT" << endl;
      hPixelSCT->Fill(trk_Rec_phi0Pixel[0],trk_Rec_phi0SCT[0]);
    }
    if (trk_nTracksPixel == 1 && trk_nTracksTRT == 1){
      cout << "Filling Pixels TRT" << endl;
      hPixelTRT->Fill(trk_Rec_phi0Pixel[0],trk_Rec_phi0TRT[0]);
    }
    if (trk_nTracksSCT == 1 && trk_nTracksTRT == 1){
      cout << "Filling SCT TRT" << endl;
      hSCTTRT->Fill(trk_Rec_phi0SCT[0],trk_Rec_phi0TRT[0]);
    }

  }

  c1->cd(1);
  hPixelSCT->Draw();
  c1->Update();

  c1->cd(2);
  hPixelTRT->Draw();
  c1->Update();

  c1->cd(3);
  hSCTTRT->Draw();
  c1->Update();

}
