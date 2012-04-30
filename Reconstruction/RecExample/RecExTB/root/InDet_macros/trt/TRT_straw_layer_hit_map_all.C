/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TRT_straw_layer_hit_map.C

// Beam profile for all straw layer. Histograms.
// Create histogram with hit map for "continious" layer 
// in both phi-sectors.

void TRT_straw_layer_hit_map_all() {

  // Styles
  gStyle->SetOptStat("nemruo");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  cout<<"Building the hitmaps..."<<endl;

  sprintf(name,"strawlayer_hitmaps0");
  sprintf(name2,"TRT Offline Monitor - straw layer hitmaps (layer 0)");
  TCanvas *c11 = new TCanvas(name,name2);
  c11->Divide(5,4);    

  sprintf(name,"strawlayer_hitmaps1");
  sprintf(name2,"TRT Offline Monitor - straw layer hitmaps (layer 1)");
  TCanvas *c12 = new TCanvas(name,name2);
  c12->Divide(6,4);    

  sprintf(name,"strawlayer_hitmaps2");
  sprintf(name2,"TRT Offline Monitor - straw layer hitmaps (layer 2)");
  TCanvas *c13 = new TCanvas(name,name2);
  c13->Divide(6,5);    

  TH1F* hist[73];

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;
  

  Int_t numberOfLayers[] = {19, 24, 30};
  
  Int_t numberOfStrawsInLayersA[] = {15, 16, 16, 16, 16, 17, 17, 17, 17, 17,
				     18, 18, 18, 18, 18, 19, 19, 19, 18};
  
  Int_t numberOfStrawsInLayersB[] = {19, 20, 20, 20, 20, 20, 21, 21, 21, 21,
				     21, 22, 22, 22, 22, 22, 23, 23, 23, 23,
				     23, 24, 24, 23};
  
  Int_t numberOfStrawsInLayersC[] = {23, 24, 24, 24, 24, 25, 25, 25, 25, 25,
				     26, 26, 26, 26, 26, 27, 27, 27, 27, 27,
				     28, 28, 28, 28, 28, 29, 29, 29, 29, 28};
  
  Int_t module, layerInModule, numberOfStraws;
  
  
  for(int slayer=0;slayer<73;slayer++) {
    
    if(slayer%3 == 0) cout<<"."; fflush(stdout); 
    
    if (slayer < numberOfLayers[0])
      {
	module = 0;
	layerInModule = slayer;
	numberOfStraws = numberOfStrawsInLayersA[layerInModule] * 2;
      }
    else if (slayer < numberOfLayers[0] + numberOfLayers[1])
      {
	module = 1;
	layerInModule = slayer - numberOfLayers[0];
	numberOfStraws = numberOfStrawsInLayersB[layerInModule] * 2;
      }
    else 
      {
	module = 2;
	layerInModule = slayer - numberOfLayers[0] - numberOfLayers[1];
	numberOfStraws = numberOfStrawsInLayersC[layerInModule] * 2;
      }
    
    Float_t max = (Float_t) numberOfStraws;
        
    sprintf(name2,"Straw layer %d",slayer);
    sprintf(name,"Straw_layer_%d",slayer);
    hist[slayer] = new TH1F(name, name2, numberOfStraws, 0., max);
    
    hist[slayer]->SetXTitle("Straw number");
    hist[slayer]->SetYTitle("Hits");
    
    //  hist->SetStats(kFALSE);
    
    Int_t array[2][3][30][29];
    
    Int_t i, j, k, l;
    
    for (i = 0; i < 2; ++i)
      for (j = 0; j < 3; ++j)
	for (k = 0; k < 30; ++k)
	  for (l = 0; l < 29; ++l)
	    array[i][j][k][l] = 0;
    
    // Rejection flag for some noisy straws
    array[1][1][ 8][13] = 1;
    array[1][1][ 8][14] = 1;
    array[1][1][ 8][16] = 1;
    array[1][1][ 9][16] = 1;
    array[1][1][10][14] = 1;
    array[1][1][10][16] = 1;
    array[1][1][11][15] = 1;
    array[1][1][11][17] = 1;
    
    array[0][0][ 9][12] = 1;
    array[1][2][21][12] = 1;
    array[1][2][18][10] = 1;
    array[1][2][19][12] = 1;
    array[1][2][20][12] = 1;
    array[1][2][21][10] = 1;
    array[1][2][20][10] = 1;
    array[1][2][18][12] = 1;
    array[1][2][19][10] = 1;
    
    array[0][2][29][ 1] = 1;
    array[0][2][29][ 0] = 1;

    Int_t numberOfRDOs;
    
    Int_t phi[2000];
    Int_t mod[2000];
    Int_t lay[2000];
    Int_t str[2000];
    
    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("trt_rdo_nrdos", 1);
    tree->SetBranchStatus("trt_rdo_phisector", 1);
    tree->SetBranchStatus("trt_rdo_layer", 1);
    tree->SetBranchStatus("trt_rdo_strawlayer", 1);
    tree->SetBranchStatus("trt_rdo_straw", 1);
    
    tree->SetBranchAddress("trt_rdo_nrdos", &numberOfRDOs);
    tree->SetBranchAddress("trt_rdo_phisector", phi);
    tree->SetBranchAddress("trt_rdo_layer", mod);
    tree->SetBranchAddress("trt_rdo_strawlayer", lay);
    tree->SetBranchAddress("trt_rdo_straw", str);
    
    Int_t numberOfEntries = 0;
    
    Int_t numberOfEvents = (Int_t) tree->GetEntries();
    
    for (i = 0; i < numberOfEvents; ++i)
      {
	tree->GetEntry(i);
	
	for (Int_t j = 0; j < numberOfRDOs; ++j)
	  {
	    if (mod[j] != module)
	      continue;
	    
	    if (lay[j] != layerInModule)
	      continue;
	    
	    if (array[phi[j]][mod[j]][lay[j]][str[j]] == 1)
	      continue;
	    
	    Int_t strawNumber = str[j];
	    
	    if (phi[j] == 1)
	      {
		if (module == 0)
		  strawNumber += numberOfStrawsInLayersA[layerInModule];
		else if (module == 1)
		  strawNumber += numberOfStrawsInLayersB[layerInModule];
		else 
		  strawNumber += numberOfStrawsInLayersC[layerInModule];
	      }
	    
	    hist[slayer]->Fill((Float_t) strawNumber);
	    
	    ++numberOfEntries;
	  }
      }

    if (slayer<19) {
      c11->cd(slayer+1);    
      hist[slayer]->Draw();
      c11->Update();
    }

    else if (slayer<43 && slayer>18) {
      int cont0 = slayer-18; 
      c12->cd(cont0);
      hist[slayer]->Draw();
      c12->Update();
    }

    else {
      int cont1 = slayer-42; 
      c13->cd(cont1);
      hist[slayer]->Draw();
      c13->Update();
    }

  }

  cout << "!\n" << endl;
  
}
