/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixel_correlation_all(int SaveOpt=0) {

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  const int LAYERS = 3;
  const int MODULES_IN_PHI = 2;
  const int CHIPS_PER_MODULE_IN_COL=8;
  const int CHIPS_PER_MODULE_IN_ROW=2;
  const int MAX_HITS = 200;
  const char HISTOTYPE[10] = "";

  // Enabling only pixel branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("pixel_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

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

  TCanvas *cCol = new TCanvas("cCol","Pixel Offline Monitor: Column Correlations (all combinatorics)");
  cCol->Divide(LAYERS, 2);
  TCanvas *cRow = new TCanvas("cRow","Pixel Offline Monitor: Row Correlations (all combinatorics)");
  cRow->Divide(LAYERS, 2);

  TH2F *histoColPhi0[LAYERS][LAYERS];
  TH2F *histoColPhi1[LAYERS][LAYERS];
  TH2F *histoRowPhi0[LAYERS][LAYERS];
  TH2F *histoRowPhi1[LAYERS][LAYERS];

  for (int n=0; n<LAYERS; n++) {
    for (int m=n+1; m<LAYERS; m++) {
      sprintf(name,"PixCor Column Mod %d - Mod %d", MODULES_IN_PHI * n, MODULES_IN_PHI * m);
      sprintf(name2,"corAllCol[%d,%d]", MODULES_IN_PHI * n, MODULES_IN_PHI * m);
      histoColPhi0[n][m] = new TH2F(name2,name,144,0.,144.,144,0.,144.);
      
      sprintf(name,"PixCor Column Mod %d - Mod %d", MODULES_IN_PHI * n + 1, MODULES_IN_PHI * m + 1);
      sprintf(name2,"corAllCol[%d,%d]", MODULES_IN_PHI * n + 1, MODULES_IN_PHI * m + 1);
      histoColPhi1[n][m] = new TH2F(name2,name,144,0.,144.,144,0.,144.);
      
      sprintf(name,"PixCor Row Mod %d - Mod %d", MODULES_IN_PHI * n, MODULES_IN_PHI * m);
      sprintf(name2,"corAllRow[%d,%d]", MODULES_IN_PHI * n, MODULES_IN_PHI * m);
      histoRowPhi0[n][m] = new TH2F(name2,name,328,0.,328.,328,0.,328.);
      
      sprintf(name,"PixCor Row Mod %d - Mod %d", MODULES_IN_PHI * n + 1, MODULES_IN_PHI * m + 1);
      sprintf(name2,"corAllRow[%d,%d]", MODULES_IN_PHI * n + 1, MODULES_IN_PHI * m + 1);
      histoRowPhi1[n][m] = new TH2F(name2,name,328,0.,328.,328,0.,328.);
    }
  }

  int hit, module;
  int count;
  
  int nhits_per_mod[LAYERS * MODULES_IN_PHI];
  float hitcol[LAYERS * MODULES_IN_PHI][MAX_HITS];
  float hitrow[LAYERS * MODULES_IN_PHI][MAX_HITS];

  //float MaxToTHit[LAYERS * MODULES_IN_PHI][3];

  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    if(event%5 == 0) cout<<"."; fflush(stdout); 

    // reset hit counters
    for (count=0; count<LAYERS * MODULES_IN_PHI; count++) {
      nhits_per_mod[count] = 0;
      //MaxToTHit[count][2] = 0;
    }
    //cout<< "nhits " << pixel_nhits <<endl;

    // loop over all hits
    for (hit=0; hit<pixel_nhits; hit++) {
      // calc module no. of current hit
      module = MODULES_IN_PHI * (int)pixel_layer[hit] + (int)pixel_phi[hit];
      //cout<< "Layer " << pixel_layer[hit] <<", Phi " << pixel_phi[hit] <<", Module " << module <<endl;
      //cout<< "Col " << pixel_col[hit] <<", Row " << pixel_row[hit] <<endl;
      /*if (MaxToTHit[module][2]<pixel_tot[hit]) {
        MaxToTHit[module][0] = pixel_col[hit];
	MaxToTHit[module][1] = pixel_row[hit];
	MaxToTHit[module][2] = pixel_tot[hit];
      }*/
      if (module > 5) {
	cout << "ERROR: Layer " << pixel_layer[hit] <<", Phi " << pixel_phi[hit] <<", Module " << module <<endl;
	continue;
      }
      hitcol[module][nhits_per_mod[module]] = pixel_col[hit];
      hitrow[module][nhits_per_mod[module]] = pixel_row[hit];
      nhits_per_mod[module]++;
    }
    // correlations
    for (int n=0; n<LAYERS; n++) {
      for (int m=n+1; m<LAYERS; m++) {
        // loop over all combinatorics for phi 0
        for (int i=0; i<nhits_per_mod[MODULES_IN_PHI * n + 0]; i++) {
          for (int j=0; j<nhits_per_mod[MODULES_IN_PHI * m + 0]; j++) {
            //cout<< "i " << i <<", j " << j <<endl;
            histoColPhi0[n][m]->Fill(hitcol[MODULES_IN_PHI * n + 0][i], hitcol[MODULES_IN_PHI * m + 0][j]);
            histoRowPhi0[n][m]->Fill(hitrow[MODULES_IN_PHI * n + 0][i], hitrow[MODULES_IN_PHI * m + 0][j]);
          }
        }
        // loop over all combinatorics for phi 1
        for (int i=0; i<nhits_per_mod[MODULES_IN_PHI * n + 1]; i++) {
          for (int j=0; j<nhits_per_mod[MODULES_IN_PHI * m + 1]; j++) {
            //cout<< "i " << i <<", j " << j <<endl;
            histoColPhi1[n][m]->Fill(hitcol[MODULES_IN_PHI * n + 1][i], hitcol[MODULES_IN_PHI * m + 1][j]);
            histoRowPhi1[n][m]->Fill(hitrow[MODULES_IN_PHI * n + 1][i], hitrow[MODULES_IN_PHI * m + 1][j]);
          }
        }      
      }
    }
  }

  //histo->Draw("colz");
  double x[2], y[2];
  for (int n=0; n<LAYERS; n++) {
    for (int m=n+1; m<LAYERS; m++) {
      cCol->cd();
      cCol->cd(n+m);
      histoColPhi0[n][m]->Draw(HISTOTYPE);
      //draw chip borders
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
	cCol->Update();
      }
      cRow->cd();
      cRow->cd(n+m);
      histoRowPhi0[n][m]->Draw(HISTOTYPE);
      //draw chip borders
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
      cRow->Update();

      cCol->cd();
      cCol->cd(n+m+LAYERS);
      histoColPhi1[n][m]->Draw(HISTOTYPE);
      //draw chip borders
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
	cCol->Update();
      }
      cRow->cd();
      cRow->cd(n+m+LAYERS);
      histoRowPhi1[n][m]->Draw(HISTOTYPE);
      //draw chip borders
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
      cRow->Update();
    }
  }


  sprintf(title, "run %d (%d events)", run, entries);
  cCol->cd();
  TPaveText *pt = new TPaveText(0.397, 0.001, 0.6, 0.011);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.013);
  text->SetTextColor(10);
  pt->Draw();
  cCol->Update();

  sprintf(title, "run %d (%d events)", run, entries);
  cRow->cd();
  TPaveText *pt = new TPaveText(0.397, 0.001, 0.6, 0.011);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.013);
  text->SetTextColor(10);
  pt->Draw();
  cRow->Update();

  if (SaveOpt==1) {
    cRow->Print("Pixels_histograms.ps(");
    cCol->Print("Pixels_histograms.ps");
  }
  else if(SaveOpt==2) {
    sprintf(sname,"Pixels_correlation_all_Rows.gif");
    cRow->Print(sname);
    sprintf(sname,"Pixels_correlation_all_Cols.gif");
    cCol->Print(sname);
  }
  else if(SaveOpt==3) {
    cRow->Print(psfilename);
    cCol->Print(psfilename);
  }
  
}

