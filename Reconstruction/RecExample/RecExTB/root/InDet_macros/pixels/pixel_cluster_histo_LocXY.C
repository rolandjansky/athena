/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixel_cluster_histo_LocXY(int SaveOpt=0) {

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  const int LAYERS = 3;
  const int MODULES_IN_PHI = 2;
  const int CHIPS_PER_MODULE_IN_COL=8;
  const int CHIPS_PER_MODULE_IN_ROW=2;
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
  tree->SetBranchStatus("pixel_clus_*",1);
  //  tree->SetBranchStatus("Event",1);
  
  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;
  
  // Get pointer to data
  int event, pixel_nhits=0;
  float pixel_LocX[MAX_HITS], pixel_LocY[MAX_HITS];
  int pixel_layer[MAX_HITS], pixel_phi[MAX_HITS];
  
  //  tree->SetBranchAddress("Event", &event);
  tree->SetBranchAddress("pixel_clus_nclusters", &pixel_nhits);
  tree->SetBranchAddress("pixel_clus_layer", pixel_layer);
  tree->SetBranchAddress("pixel_clus_phi", pixel_phi);
  tree->SetBranchAddress("pixel_clus_LocX", pixel_LocX);
  tree->SetBranchAddress("pixel_clus_LocY", pixel_LocY);


  // build Canvas
  TCanvas *c11 = new TCanvas("c11","Pixel Offline Monitor Cluster LocX");
  TCanvas *c12 = new TCanvas("c12","Pixel Offline Monitor Cluster LocY");
  c11->Divide(LAYERS,MODULES_IN_PHI);
  c12->Divide(LAYERS,MODULES_IN_PHI);

  TH1F *histoX[6];
  TH1F *histoY[6];


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
      sprintf(name,"Cluster LocX Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"locX[%d,%d]",layer,phi);
      //gStyle->SetOptStat(100000);

      // get histo for hits
      //histo = new TH1F(name2,name,20*X_RESOLUTION,-10,10);
      histoX[module] = new TH1F(name2, name, 20*X_RESOLUTION, -10, 10);
      //histo[module]->SetStats(kFALSE); // statistics in hostogram off

      //sprintf(name,"c12_%d",i);
      //c12.cd();
      //c12->cd(i++);
      //TPad* subcanvas = (TPad*)(c12->GetPrimitive(name));
      //subcanvas->SetLogz();

      // set name for histo
      sprintf(name,"Cluster LocY Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"locY[%d,%d]",layer,phi);
      //gStyle->SetOptStat(100000);

      // get histo for hits
      //histo = new TH1F(name2,name,20*X_RESOLUTION,-10,10);
      histoY[module] = new TH1F(name2,name, 80*Y_RESOLUTION,-40,40);

    }
  }


  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over all hits
    for (hit=0; hit<pixel_nhits; hit++) {
      // calc module no. of current hit
      module = MODULES_IN_PHI * (int)pixel_layer[hit] + (int)pixel_phi[hit];
      //cout<< "Layer " << pixel_layer[hit] <<", Phi " << pixel_phi[hit] <<", Module " << module <<endl;
      //cout<< "Col " << pixel_col[hit] <<", Row " << pixel_row[hit] <<endl;

      histoX[module]->Fill(pixel_LocX[hit]);
      histoY[module]->Fill(pixel_LocY[hit]);
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
      c11->cd();
      c11->cd(i);
      //TPad* subcanvas = (TPad*)(c11->GetPrimitive(name));
      //subcanvas->SetLogz();



      histoX[module]->DrawCopy();

      // get current subcanvas
      //sprintf(name,"c12_%d",i);
      c12->cd();
      c12->cd(i);
      //TPad* subcanvas = (TPad*)(c12->GetPrimitive(name));
      //subcanvas->SetLogz();



      histoY[module]->DrawCopy();

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
  if (SaveOpt==1) { sprintf(sname,"Pixels_Histo_LocX_LocY.ps("); }
  else if(SaveOpt==2) { sprintf(sname,"Pixels_Histo_LocX_LocY.gif(");}
  else if(SaveOpt==3) { sname = psfilename; };

  if (SaveOpt>0) {
    c11->Print(sname);
    c12->Print(sname);
  }
}


