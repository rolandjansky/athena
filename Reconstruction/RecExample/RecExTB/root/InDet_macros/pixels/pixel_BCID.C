/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixel_BCID(int SaveOpt=0) {

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  const int LAYERS = 3;
  const int MODULES_IN_PHI = 2;
  const int MAX_HITS=200;

  int layer,module,tot;
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
  int pixel_BCID[MAX_HITS];

//  tree->SetBranchAddress("Event", &event);
  tree->SetBranchAddress("pixel_nhits", &pixel_nhits);
  tree->SetBranchAddress("pixel_layer", pixel_layer);
  tree->SetBranchAddress("pixel_phi", pixel_phi);
  tree->SetBranchAddress("pixel_BCID", pixel_BCID);

  // build Canvas
  TCanvas *bcid = new TCanvas("cBCID","Pixel Offline Monitor BCID");
  bcid->Divide(LAYERS,MODULES_IN_PHI);

  TH1F *histoBCID[6];


  int module;
  int count;

    // loop over all modules
  for (phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (layer=0;layer<LAYERS;layer++) {


      // calc module no.
      module = MODULES_IN_PHI * layer + phi;

      // set name for histo
      sprintf(name,"BCID Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"BCID[%d,%d]",layer,phi);
      // get histo for hits
      histoBCID[module] = new TH1F(name2, name, 8, 0., 8.);
    }
  }


  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over all hits
    for (int hit=0; hit<pixel_nhits; hit++) {
      // calc module no. of current hit
      module = MODULES_IN_PHI * pixel_layer[hit] + pixel_phi[hit];
      if (module > 5) {
	      cout << "ERROR: Layer " << pixel_layer[hit] <<", Phi " << pixel_phi[hit] <<", Module " << module <<endl;
	      continue;
      }
      if (hit >= MAX_HITS) {
	      cout << "Hit number exceeds array " << hit << endl;
	      continue;
      } 
      histoBCID[module]->Fill(pixel_BCID[hit]);
    }
  }

  i=1;
  // loop over all modules
  for (phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (layer=0;layer<LAYERS;layer++) {

      // calc module no.
      module = MODULES_IN_PHI * layer + phi;

      // get current subcanvas
      cBCID->cd();
      cBCID->cd(i);
      histoBCID[module]->DrawCopy();

      cBCID->Update();
      i++;
    }
  }

  if (SaveOpt==1) { sprintf(sname,"Pixels_Histo_BCID.ps("); }
  else if(SaveOpt==2) { sprintf(sname,"Pixels_Histo_BCID.gif(");}
  else if(SaveOpt==3) { sname = psfilename; };

}


