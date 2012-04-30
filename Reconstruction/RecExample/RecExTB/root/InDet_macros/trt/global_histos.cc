/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void global_histos(int SaveOpt=0) {

  cout<<"Building the histograms..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  entries = tree->GetEntries();
  cout<<entries<<" events"<<endl;

  TCanvas *c11 = new TCanvas("c11","TRT Offline Monitor");
  c11->Divide(4,3);

  for(int k=0; k<11; k++){

    if(k%1 == 0) cout<<"."; fflush(stdout); 

    c11->cd(k+1);

    if (k==0) { sprintf(name,"RDO's"); sprintf(name2,"trt_rdo_nrdos"); }
    else if (k==1) { sprintf(name,"Drift Time"); sprintf(name2,"trt_rdo_drifttime"); }
    else if (k==2) { sprintf(name,"Time Over Threshold"); sprintf(name2,"trt_rdo_timeoverthr"); }
    else if (k==3) { sprintf(name,"High Level"); sprintf(name2,"trt_rdo_highlevel"); }
    else if (k==4) { sprintf(name,"Straw"); sprintf(name2,"trt_rdo_straw"); }
    else if (k==5) { sprintf(name,"Straw Y"); sprintf(name2,"trt_DC_straw_y"); }
    else if (k==6) { sprintf(name,"Straw-layer"); sprintf(name2,"trt_rdo_strawlayer"); }
    else if (k==7) { sprintf(name,"Layer"); sprintf(name2,"trt_rdo_layer"); }
    else if (k==8) { sprintf(name,"Phi"); sprintf(name2,"trt_rdo_phisector"); }
    else if (k==9) { sprintf(name,"Time PLL"); sprintf(name2,"trt_rdo_timepll"); }
    else { sprintf(name,"DC radius"); sprintf(name2,"trt_DC_radius"); }

    tree->Draw(name2);
    TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");
    htemp->SetTitle(name);
    htemp->GetXaxis()->SetTitle(name);
    htemp->GetYaxis()->SetTitle("#");
    htemp->DrawCopy("SAME");
    c11->Update();
  }

  cout << "!\n" << endl;

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
    
  if (SaveOpt==1) c11->Print("TRT_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"TRThitmaps.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}

