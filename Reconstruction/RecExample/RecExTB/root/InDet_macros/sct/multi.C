/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void multi(int SaveOpt=0){
  
  char title[200], options[300];
  const int NPLANES = 16;
  int rdo_nhits;
  const int MAX=10000;
  float rdo_strips[MAX],rdo_layer[MAX],rdo_phi[MAX],rdo_side[MAX];
  int ievent;

  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_rdo_*",1);
  tree->SetBranchAddress("Event",&ievent);
  tree->SetBranchAddress("sct_rdo_nhits",&rdo_nhits);
  tree->SetBranchAddress("sct_rdo_strips",rdo_strips);
  tree->SetBranchAddress("sct_rdo_layer",rdo_layer);
  tree->SetBranchAddress("sct_rdo_phi",rdo_phi);
  tree->SetBranchAddress("sct_rdo_side",rdo_side);

  int entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;
 
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(111111);
  gStyle->SetOptStat("nemruo");
  
  int plane, layer, phi, side, strips;
  TH1F *multip[NPLANES];

  
  for ( layer=0; layer<4; layer++){
    for ( phi=0; phi<2; phi++){
      for (side=0; side<2; side++){
	
	plane = 2*(2*layer + phi) + side;
		
	sprintf(title,"multip%d",plane);
	multip[plane]= new TH1F(title,title,51,-0.5,10.5);	

	sprintf(title, "SCT multip plane %d [%d:%d:%d]", plane, layer, phi, side);
	cout << title << endl;
	
	multip[plane]->SetTitle(title); 
	multip[plane]->SetStats(kTRUE); 
	multip[plane]->SetXTitle("hits");   
	if (phi==0) multip[plane]->SetFillColor(kRed); 
	else multip[plane]->SetFillColor(kBlue);
      }
    }
  }
  
  int count[16]={0};  
  //  entries = 1;
  for (Int_t i=0; i<entries; i++) {
    tree->GetEntry(i);
         
    for(int k=0; k<16; k++){  
      count[k]=0;
    }
    
    for(int j=0; j<rdo_nhits; j++) {
      
      strips = rdo_strips[j];
      layer  = rdo_layer[j];
      phi    = rdo_phi[j];
      side   = rdo_side[j]; 
      plane  = 2*(2*layer + phi) + side;

      // masking very noisy channels for [1,0,0] and [1,0,1]
      /*
      if( ((layer==1 && phi==0 && side==0) && ( (strips>639 || strips==366) || strips==376 )) || 
	  ((layer==1 && phi==0 && side==1) && ( (strips>256 && strips<=383) || (strips>=641 &&  strips<=652))) )
	continue;      
      */


      count[plane]++;
    }
    
    //if (count[15]!=0 && count[2]!=0){ 
    for(int j=0; j<16; j++){
      multip[j]->Fill(count[j]);
    }
    //}
  }
  
  c11 = new TCanvas("c1", "SCT multiplicites");
  c11->SetBorderMode(0);
  c11->Divide(4,4);  
  for(int i=0; i<NPLANES; i++){
    c11->cd(i+1);
    //gPad->SetLogy();
    multip[i]->Draw();
  }
  c11->Update();     

  if (SaveOpt==1) c11->Print("SCT_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"multi_sct.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
