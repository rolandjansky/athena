/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixel_correlation(int SaveOpt=0) {

  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  // Enabling only pixel branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("pixel_*",1);

  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  int event, pixel_nhits=0;

  //  tree->SetBranchAddress("Event", &event);
  tree->SetBranchAddress("pixel_nhits", &pixel_nhits);
  tree->SetBranchAddress("pixel_layer", pixel_layer);
  tree->SetBranchAddress("pixel_phi", pixel_phi);
  tree->SetBranchAddress("pixel_col", pixel_col);
  tree->SetBranchAddress("pixel_row", pixel_row);
  tree->SetBranchAddress("pixel_tot", pixel_tot);
  
  // array for hit

  TCanvas *c11 = new TCanvas("pixel_corr","Pixel Offline Monitor: Correlations");
  c11->Divide(2,2);

  TH2F *histoAllCol = new TH2F(name2,name,144,0.,144.,144,0.,144.);;
  sprintf(name,"Correlation Pixel Col - All Hits - mod0-mod4");
  sprintf(name2,"corAllCol");

  TH2F *histoToTFilterCol = new TH2F(name2,name,144,0.,144.,144,0.,144.);
  sprintf(name,"Correlation Pixel Col - max ToT hit - mod0-mod4");
  sprintf(name2,"corToTCol");

  TH2F *histoAllRow = new TH2F(name2,name,328,0.,328.,328,0.,328.);;
  sprintf(name,"Correlation Pixel Row - All Hits - mod0-mod4");
  sprintf(name2,"corAllRow");

  TH2F *histoToTFilterRow= new TH2F(name2,name,328,0.,328.,328,0.,328.);
  sprintf(name,"Correlation Pixel Row - max ToT hit - mod0-mod4");
  sprintf(name2,"corToTRow");

  int hit{}, module{};
  int nhits_per_mod[LAYERS * MODULES_IN_PHI];
  int count{};
  float hitcol[LAYERS * MODULES_IN_PHI][MAX_HITS];
  float hitrow[LAYERS * MODULES_IN_PHI][MAX_HITS];

  float MaxToTHit[LAYERS * MODULES_IN_PHI][3];

  for (event=0; event<entries; event++) {

    if(event%100 == 0) cout<<"."; fflush(stdout); 

    // Get data for current event
    tree->GetEntry(event);
    // reset hit counters
    for (count=0; count<LAYERS * MODULES_IN_PHI; count++) {
      nhits_per_mod[count] = 0;
      MaxToTHit[count][2] = 0;
    }
    //cout<< "nhits " << pixel_nhits <<endl;

    // loop over all hits
    for (hit=0; hit<pixel_nhits; hit++) {
      // calc module no. of current hit
      module = MODULES_IN_PHI * (int)pixel_layer[hit] + (int)pixel_phi[hit];
      //cout<< "Layer " << pixel_layer[hit] <<", Phi " << pixel_phi[hit] <<", Module " << module <<endl;
      //cout<< "Col " << pixel_col[hit] <<", Row " << pixel_row[hit] <<endl;
      if (MaxToTHit[module][2]<pixel_tot[hit]) {
        MaxToTHit[module][0] = pixel_col[hit];
	MaxToTHit[module][1] = pixel_row[hit];
	MaxToTHit[module][2] = pixel_tot[hit];
      }
      hitcol[module][nhits_per_mod[module]] = pixel_col[hit];
      hitrow[module][nhits_per_mod[module]] = pixel_row[hit];
      nhits_per_mod[module]++;
    }
    // col-correlation layer 0 - layer 1, phi 0
    //cout<< "layer0 " << nhits_per_mod[MODULES_IN_PHI * 0 + 0] <<", layer1 " << nhits_per_mod[MODULES_IN_PHI * 1 + 0] <<endl;
    for (int i=0; i<nhits_per_mod[MODULES_IN_PHI * 0 + 0]; i++) {
      for (int j=0; j<nhits_per_mod[MODULES_IN_PHI * 2 + 0]; j++) {
        //cout<< "i " << i <<", j " << j <<endl;
        histoAllCol->Fill(hitcol[MODULES_IN_PHI * 0 + 0][i], hitcol[MODULES_IN_PHI * 2 + 0][j]);
	histoAllRow->Fill(hitrow[MODULES_IN_PHI * 0 + 0][i], hitrow[MODULES_IN_PHI * 2 + 0][j]);
      }
    }
    if (MaxToTHit[MODULES_IN_PHI * 0 + 0][2] != 0 && MaxToTHit[MODULES_IN_PHI * 2 + 0][2] != 0) {
      histoToTFilterCol->Fill(MaxToTHit[MODULES_IN_PHI * 0 + 0][0], MaxToTHit[MODULES_IN_PHI * 2 + 0][0]);
      histoToTFilterRow->Fill(MaxToTHit[MODULES_IN_PHI * 0 + 0][1], MaxToTHit[MODULES_IN_PHI * 2 + 0][1]);
    }
  }
  //histo->DrawCopy("colz");
  double x[2], y[2];

  c11->cd(1);
  histoAllCol->DrawCopy();
  delete histoAllCol;
  for (int ix=1;ix<CHIPS_PER_MODULE_IN_COL;ix++){
    x[0]=18*ix;
    x[1]=18*ix;
    y[0]=0;
    y[1]=144;
    TGraph *gr1 = new TGraph (2, x, y);
    gr1->SetLineColor(2);
    gr1->Draw();
    TGraph *gr1 = new TGraph (2, y, x);
    gr1->SetLineColor(2);
    gr1->Draw();

  }
  c11->Update();

  c11->cd(3);
  histoToTFilterCol->DrawCopy();
  delete histoToTFilterCol;
  for (int ix=1;ix<CHIPS_PER_MODULE_IN_COL;ix++){
    x[0]=18*ix;
    x[1]=18*ix;
    y[0]=0;
    y[1]=144;
    TGraph *gr1 = new TGraph (2, x, y);
    gr1->SetLineColor(2);
    gr1->Draw();
    TGraph *gr1 = new TGraph (2, y, x);
    gr1->SetLineColor(2);
    gr1->Draw();
  }
  c11->Update();
  
  c11->cd(2);
  histoAllRow->DrawCopy();
  delete histoAllRow;
  x[0]=0;
  x[1]=328;
  y[0]=164;
  y[1]=164;
  TGraph *gr1 = new TGraph (2, x, y);
  gr1->SetLineColor(2);
  gr1->Draw();
  TGraph *gr1 = new TGraph (2, y, x);
  gr1->SetLineColor(2);
  gr1->Draw();
  c11->Update();

  c11->cd(4);
  histoToTFilterRow->DrawCopy();
  delete histoToTFilterRow;
  x[0]=0;
  x[1]=328;
  y[0]=164;
  y[1]=164;
  TGraph *gr1 = new TGraph (2, x, y);
  gr1->SetLineColor(2);
  gr1->Draw();
  TGraph *gr1 = new TGraph (2, y, x);
  gr1->SetLineColor(2);
  gr1->Draw();
  c11->Update();

  cout << "!\n" << endl;

  if (SaveOpt==1) c11->Print("Pixels_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"Pixels_correlations.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}


