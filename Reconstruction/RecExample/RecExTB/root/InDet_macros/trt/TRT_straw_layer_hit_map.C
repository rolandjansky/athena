/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TRT_straw_layer_hit_map.C

// Beam profile for the given layer. Histogram.
// Input: straw layer number 0 - 72
// Create histogram with hit map for "continious" layer 
// in both phi-sectors.
// Usage: root> .x TRT_straw_layer_hit_map.C 

void TRT_straw_layer_hit_map(int layer) {
  
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
  
  if (layer < numberOfLayers[0])
  {
    module = 0;
    layerInModule = layer;
    numberOfStraws = numberOfStrawsInLayersA[layerInModule] * 2;
  }
  else if (layer < numberOfLayers[0] + numberOfLayers[1])
    {
      module = 1;
      layerInModule = layer - numberOfLayers[0];
      numberOfStraws = numberOfStrawsInLayersB[layerInModule] * 2;
    }
  else 
    {
      module = 2;
      layerInModule = layer - numberOfLayers[0] - numberOfLayers[1];
      numberOfStraws = numberOfStrawsInLayersC[layerInModule] * 2;
    }
  
  Float_t max = (Float_t) numberOfStraws;
  
  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  sprintf(name2,"Straw layer %d",layer);
  TH1F* hist = new TH1F("trt", name2, numberOfStraws, 0., max);
  
  hist->SetXTitle("Straw number");
  hist->SetYTitle("Hits");
  
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
	
	hist->Fill((Float_t) strawNumber);
	
	++numberOfEntries;
      }
    }
  
  sprintf(name,"strawlayer_hitmaps_%d",layer);
  sprintf(name2,"TRT Offline Monitor - strawlayer hitmap (%d)",layer);
  TCanvas *c11 = new TCanvas(name,name2);
  hist->Draw();

}
