/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixel_correl_LocX_Row() {

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  const int LAYERS = 3;
  const int MODULES_IN_PHI = 2;
  const int CHIPS_PER_MODULE_IN_COL=8;
  const int CHIPS_PER_MODULE_IN_ROW=2;
  const int ROW_RESOLUTION=2;		// no of pixels per bin
  const int COL_RESOLUTION=2;		// no of pixels per bin

  const int X_RESOLUTION=10;		// no of bin per x unit
  const int Y_RESOLUTION=1;		// no of bin per y unit
  const int MAX_HITS=100;


  int layer,phi,row,col,module,tot;
  int i;
  char options[80];
  float treevariable1;
  float treevariable2;
  char name[80],name2[80];

  // Enabling only pixel branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("pixel_*",1);
  //  tree->SetBranchStatus("Event",1);
  
  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  int event, pixel_nhits=0;
  float pixel_row[MAX_HITS], pixel_col[MAX_HITS];
  float pixel_layer[MAX_HITS], pixel_phi[MAX_HITS];
  
  //  tree->SetBranchAddress("Event", &event);
  tree->SetBranchAddress("pixel_nhits", &pixel_nhits);
  tree->SetBranchAddress("pixel_layer", pixel_layer);
  tree->SetBranchAddress("pixel_phi", pixel_phi);
  tree->SetBranchAddress("pixel_row", pixel_row);
  tree->SetBranchAddress("pixel_col", pixel_col);

  // Get pointer to data
  int event, pixel_clus_nhits=0;
  float pixel_clus_LocX[MAX_HITS], pixel_clus_LocY[MAX_HITS];
  int pixel_clus_layer[MAX_HITS], pixel_clus_phi[MAX_HITS];

//  tree->SetBranchAddress("Event", &event);
  tree->SetBranchAddress("pixel_clus_nclusters", &pixel_clus_nhits);
  tree->SetBranchAddress("pixel_clus_layer", pixel_clus_layer);
  tree->SetBranchAddress("pixel_clus_phi", pixel_clus_phi);
  tree->SetBranchAddress("pixel_clus_LocX", pixel_clus_LocX);
  tree->SetBranchAddress("pixel_clus_LocY", pixel_clus_LocY);

  // build Canvas
  TCanvas *c11 = new TCanvas("c11","Pixel Offline Monitor Correl LocX-Row");
  TCanvas *c12 = new TCanvas("c12","Pixel Offline Monitor Hits LocY-Col");
  c11->Divide(LAYERS,MODULES_IN_PHI);
  c12->Divide(LAYERS,MODULES_IN_PHI);

  TH2F *histoRow[6];
  TH2F *histoCol[6];


  int hit, module;
  //int nhits_per_mod[LAYERS * MODULES_IN_PHI];
  int count;
  //float hitcol[LAYERS * MODULES_IN_PHI];
  //float hitrow[LAYERS * MODULES_IN_PHI];

  //float MaxToTHit[LAYERS * MODULES_IN_PHI][3];

    // loop over all modules
  for (phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (layer=0;layer<LAYERS;layer++) {

      // calc module no.
      module = MODULES_IN_PHI * layer + phi;

      // get current subcanvas
      //sprintf(name,"c11_%d",i);
      //c11.cd();
      //c11->cd(i++);
      //TPad* subcanvas = (TPad*)(c11->GetPrimitive(name));
      //subcanvas->SetLogz();

      // set name for histo
      sprintf(name,"Correlation LocX-Row Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"LocXRow[%d,%d]",layer,phi);
      //gStyle->SetOptStat(100000);

      // get histo for hits
      //histo = new TH1F(name2,name,20*X_RESOLUTION,-10,10);
      histoRow[module] = new TH2F(name2, name, 328/ROW_RESOLUTION, 0, 328, 20*X_RESOLUTION, -10, 10);
      //histo[module]->SetStats(kFALSE); // statistics in hostogram off

      //sprintf(name,"c12_%d",i);
      //c12.cd();
      //c12->cd(i++);
      //TPad* subcanvas = (TPad*)(c12->GetPrimitive(name));
      //subcanvas->SetLogz();

      // set name for histo
      sprintf(name,"Correlation LocY-Col Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"LocYCol[%d,%d]",layer,phi);
      //gStyle->SetOptStat(100000);

      // get histo for hits
      //histo = new TH1F(name2,name,20*X_RESOLUTION,-10,10);
      histoCol[module] = new TH2F(name2,name, 144/COL_RESOLUTION,0,144, 80*Y_RESOLUTION,-40,40);

    }
  }


  int nhits_per_mod[LAYERS * MODULES_IN_PHI];
  float hitcol[LAYERS * MODULES_IN_PHI][MAX_HITS];
  float hitrow[LAYERS * MODULES_IN_PHI][MAX_HITS];

  int clus_nhits_per_mod[LAYERS * MODULES_IN_PHI];
  float hitLocY[LAYERS * MODULES_IN_PHI][MAX_HITS];
  float hitLocX[LAYERS * MODULES_IN_PHI][MAX_HITS];

  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);
    // reset counters
    for (int count=0; count<LAYERS * MODULES_IN_PHI; count++) {
      nhits_per_mod[count] = 0;
      clus_nhits_per_mod[count] = 0;
    }

    // pixel hits
    // loop over all hits
    for (hit=0; hit<pixel_nhits; hit++) {
      // calc module no. of current hit
      module = MODULES_IN_PHI * (int)pixel_layer[hit] + (int)pixel_phi[hit];
      //cout<< "Layer " << pixel_layer[hit] <<", Phi " << pixel_phi[hit] <<", Module " << module <<endl;
      //cout<< "Col " << pixel_col[hit] <<", Row " << pixel_row[hit] <<endl;
      hitcol[module][nhits_per_mod[module]] = pixel_col[hit];
      hitrow[module][nhits_per_mod[module]] = pixel_row[hit];
      nhits_per_mod[module]++;

    }

    // pixel clusters
    // loop over all clusters
    for (hit=0; hit<pixel_clus_nhits; hit++) {
      // calc module no. of current hit
      module = MODULES_IN_PHI * (int)pixel_clus_layer[hit] + (int)pixel_clus_phi[hit];
      //cout<< "Layer " << pixel_layer[hit] <<", Phi " << pixel_phi[hit] <<", Module " << module <<endl;
      //cout<< "Col " << pixel_col[hit] <<", Row " << pixel_row[hit] <<endl;
      hitLocY[module][clus_nhits_per_mod[module]] = pixel_clus_LocY[hit];
      hitLocX[module][clus_nhits_per_mod[module]] = pixel_clus_LocX[hit];
      clus_nhits_per_mod[module]++;

    }

    // fill histos
    for (module=0; module<6; module++){
      for (int i=0; i<nhits_per_mod[module]; i++) {
        for (int j=0; j<clus_nhits_per_mod[module]; j++) {
          //cout<< "i " << i <<", j " << j <<endl;
          histoRow[module]->Fill(hitrow[module][i], hitLocX[module][j]);
	  histoCol[module]->Fill(hitcol[module][i], hitLocY[module][j]);
        }
      }
    }
  }

  i=1;
  // loop over all modules
  for (phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (layer=0;layer<LAYERS;layer++) {

      // calc module no.
      module = MODULES_IN_PHI * layer + phi;

      // get current subcanvas
      //sprintf(name,"c11_%d",i);
      c11.cd();
      c11->cd(i);
      //TPad* subcanvas = (TPad*)(c11->GetPrimitive(name));
      //subcanvas->SetLogz();



      histoRow[module]->DrawCopy("colz");

      // get current subcanvas
      //sprintf(name,"c12_%d",i);
      c12.cd();
      c12->cd(i);
      //TPad* subcanvas = (TPad*)(c12->GetPrimitive(name));
      //subcanvas->SetLogz();



      histoCol[module]->DrawCopy("colz");

      //histo->DrawCopy("surf4");
      //histo->DrawCopy("contz");
      //histo->DrawCopy("lego");

      // now draw histo
      //histo->DrawCopy("colz");

      // draw lines in histo for chip borders
 /*     double x[2], y[2];
      for (int ix=1;ix<CHIPS_PER_MODULE_IN_COL;ix++){
         x[0]=18*ix;
         x[1]=18*ix;
         y[0]=0;
         y[1]=328;
         TGraph *gr1 = new TGraph (2, x, y);
         gr1->SetLineColor(1);
         gr1->Draw();
      }

      x[0]=0;
      x[1]=144;
      y[0]=164;
      y[1]=164;
      TGraph *gr1 = new TGraph (2, x, y);
      gr1->SetLineColor(1);
      gr1->Draw();
   */
      // update canvas
      c11->Update();
      c12->Update();
      i++;

      // activate mouse event
      //subcanvas->AddExec("dynamic","DynamicExec()");
    }
  }
}


