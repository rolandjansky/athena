/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void hitmaps(int SaveOpt=0) {

  cout<<"Building the hitmaps..."<<endl;

  const int NPLANES = 16;
  int strips;

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_rdo_*",1);
 
  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(111111);
  gStyle->SetOptStat("nemruo");
  
  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;
  
  int module, layer, phi, side;

  TH1F *hitmap[NPLANES];

  for (int i=0; i<NPLANES; i++){
    
    if(i%1 == 0) cout<<"."; fflush(stdout);
    
    sprintf(title,"hitmap%d",i);
    hitmap[i]= new TH1F(title,title,768,-0.5,767.5);
    
    module = i/2;
    layer  = i/4;
    phi    = module%2;
    side   = (i+1)%2;     
    sprintf(title, "SCT hitmap module %d [%d:%d:%d]", i, layer, phi, side);
    hitmap[i]->SetTitle(title); 
    hitmap[i]->SetStats(kTRUE); 
    hitmap[i]->SetXTitle("Strip");   
    hitmap[i]->SetYTitle("Hits");   
    if (phi==0) hitmap[i]->SetFillColor(kRed);
    else hitmap[i]->SetFillColor(kBlue);
    
    
    sprintf(options,"sct_rdo_layer==%d && sct_rdo_phi==%d && sct_rdo_side==%d",layer,phi,side);
    tree->Draw("sct_rdo_strips",options,"goff");
    
    for (int j=0; j<tree->GetSelectedRows();j++){
      strips=*(tree->GetV1()+j);
      hitmap[i]->Fill(strips);
    }
  }
  
  c11 = new TCanvas("c1", "SCT Hitmaps");
  c11->SetBorderMode(0);
  c11->Divide(4,4);  
  
  for(int i=0; i<NPLANES; i++){
    c11->cd(i+1);
    hitmap[i]->Draw();
    c11->Update();
  }  

  sprintf(title, "run %d (%d events)", run, entries);
  c11->cd();
  TPaveText *pt = new TPaveText(0.397, 0.001, 0.6, 0.011);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.013);
  text->SetTextColor(10);
  pt->Draw();
  c11->Update();
  
  cout << "!\n" << endl;
  
  if (SaveOpt==1) c11->Print("SCT_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"SCThitmaps.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
