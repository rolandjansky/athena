/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixel_histo_colrow(int SaveOpt=0) {

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  const int LAYERS = 3;
  const int MODULES_IN_PHI = 2;
  const int CHIPS_PER_MODULE_IN_COL=8;
  const int CHIPS_PER_MODULE_IN_ROW=2;
  const int ROW_RESOLUTION=1;		// no of pixels per bin
  const int COL_RESOLUTION=1;		// no of pixels per bin
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
  // tree->SetBranchStatus("Event",1);

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


  // build Canvas
  TCanvas *cRow = new TCanvas("cRow","Pixel Offline Monitor Hits Row");
  TCanvas *cCol = new TCanvas("cCol","Pixel Offline Monitor Hits Col");
  cRow->Divide(LAYERS,MODULES_IN_PHI);
  cCol->Divide(LAYERS,MODULES_IN_PHI);

  TH1F *histoRow[6];
  TH1F *histoCol[6];


  int hit, module;
  int count;

    // loop over all modules
  for (phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (layer=0;layer<LAYERS;layer++) {

      // calc module no.
      module = MODULES_IN_PHI * layer + phi;

      // set name for histo
      sprintf(name,"Hits in Row Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"Row[%d,%d]",layer,phi);
      // get histo for hits
      histoRow[module] = new TH1F(name2, name, 328, 0., 328.);

      // set name for histo
      sprintf(name,"Hits in Col Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"Col[%d,%d]",layer,phi);
      // get histo for hits
      histoCol[module] = new TH1F(name2,name, 144,0.,144.);

    }
  }


  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over all hits
    for (hit=0; hit<pixel_nhits; hit++) {
      // calc module no. of current hit
      module = MODULES_IN_PHI * pixel_layer[hit] + pixel_phi[hit];
      histoRow[module]->Fill(pixel_row[hit]);
      histoCol[module]->Fill(pixel_col[hit]);
    }
  }

  i=1;
  // loop over all modules
  for (phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (layer=0;layer<LAYERS;layer++) {

      // calc module no.
      module = MODULES_IN_PHI * layer + phi;

      // get current subcanvas
      cRow->cd();
      cRow->cd(i);
      histoRow[module]->DrawCopy();

      // get current subcanvas
      cCol->cd();
      cCol->cd(i);
      histoCol[module]->DrawCopy();

      cRow->Update();
      cCol->Update();
      i++;

    }
  }

  if (SaveOpt==1) { sprintf(sname,"Pixels_Histo_Col_Row.ps("); }
  else if(SaveOpt==2) { sprintf(sname,"Pixels_Histo_Col_Row.gif(");}
  else if(SaveOpt==3) { sname = psfilename; };

  if (SaveOpt>0) {
    cRow->Print(sname);
    cCol->Print(sname);
  }
}


