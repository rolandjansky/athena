/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int plotDecision(const char * fileName = "")
{
  if( fileName == "" ) 
    {
      cout << " Usage : .x plotDecision.cxx (\"..\") \n";
      return 1;
    }
  
  TString hname1 = "TrigSteer_L2/chainAcceptance_L2_runsummary";
  TString hname2 = "TrigSteer_EF/chainAcceptance_EF_runsummary";

  TFile file(fileName);
  gROOT->cd();
  TH1F* h1_0 = (TH1F*)file.Get(hname1);
  TH1F* h2_0 = (TH1F*)file.Get(hname2);

  gStyle->SetPadBottomMargin(0.4);
  gStyle->SetOptLogy();
  TCanvas * c1 = (TCanvas*)gROOT->Get("c1");
  if( c1 == 0 ) c1 = new TCanvas("c1","",10,10,800,500);
  c1->Divide(1,2);

  c1->cd(1);
  if(h1_0 ) 
    {
      TH1F * h1 = h1_0->Clone();
      h1->LabelsOption("v");
      h1->SetLabelSize(0.08);
      h1->Draw();
    }else{
    cout << " Cannot find histogram " << hname1 << endl;
  }

  c1->cd(2);
  if(h2_0 ) 
    {
      TH1F * h1 = h2_0->Clone();
      h2->LabelsOption("v");
      h2->SetLabelSize(0.08);
      h2->Draw();
    }else{
    cout << " Cannot find histogram " << hname2 << endl;
  }




}
