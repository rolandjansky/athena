/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void trt_hitmaps_y(int SaveOpt=0,int Phi) {

  cout<<"Building the hitmaps..."<<endl;
  
  // Styles
  gStyle->SetOptStat("nemr");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt_DC*",1);

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  TH1F *trthisto[3][30];
  
  for (int i=0;i<TRT_LAYER;i++) {

    for (int ii=0;ii<STRAW_LAYER[i];ii++) {

    if(ii%1 == 0) cout<<"."; fflush(stdout); 

      sprintf(name,"TRT Hitmaps [%d,%d,%d]",i,Phi,ii);
      sprintf(name2,"trt_hitmaps_%d_%d_%d",i,Phi,ii);
      trthisto[i][ii] = new TH1F(name2,name,30,-100.5,100.5);
      
      sprintf(options,"trt_DC_layer==%d && trt_DC_phisector==%d && trt_DC_strawlayer==%d",i,Phi,ii);
      tree->Draw("trt_DC_straw_y",options,"goff");
      
      for (int iii=0;iii<tree->GetSelectedRows();iii++)
	{
	  ystraw=*(tree->GetV1()+iii);
	  trthisto[i][ii]->Fill(ystraw);
	}
      
      if (Phi==0) trthisto[i][ii]->SetFillColor(kRed); 
      else trthisto[i][ii]->SetFillColor(kBlue);

      trthisto[i][ii]->SetXTitle("y coord (mm)");
      trthisto[i][ii]->SetYTitle("hits");
    }    
  
    sprintf(name,"TRT_hitmaps_y_%d_%d",i,Phi);
    sprintf(name2,"TRT Offline Monitor [y coordinate] (Layer %d)",i);
    c11 = new TCanvas(name,name2);
    
    if (i==0) c11->Divide(5,4);
    else if (i==1) c11->Divide(6,4);
    else c11->Divide(6,5);
    
    c11->Clear("D");
    
    for (int ii=0;ii<STRAW_LAYER[i];ii++) {
      c11->cd(ii+1);
      trthisto[i][ii]->Draw();
      c11->Update(); 
    }

    sprintf(title, "run %d (%d events)", run, entries);
    c11->cd();
    TPaveText *pt = new TPaveText(0.401, 0.001, 0.6, 0.012);
    pt->SetBorderSize(0);
    pt->SetFillColor(1);
    TText *text = pt->AddText(title);
    text->SetTextSize(0.01);
    text->SetTextColor(10);
    pt->Draw();
    c11->Update();
    
    if (SaveOpt==1) c11->Print("TRT_histograms.ps(");
    else if(SaveOpt==2) {sprintf(sname,"TRThitmaps.gif"); c11->Print(sname); }
    else if(SaveOpt==3) c11->Print(psfilename);
    
  }
  cout << "!\n" << endl;
}


