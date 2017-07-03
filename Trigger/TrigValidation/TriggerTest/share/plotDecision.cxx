

int plotDecision(const char * fileName = "")
{
  if( fileName == "" ) 
    {
      cout << " Usage : .x plotDecision.cxx (\"..\") \n";
      return 1;
    }

  TString hname1 = "TrigSteer_HLT/ChainAcceptance";
//  TString hname1 = "TrigSteer_L2/ChainAcceptance_runsummary";
//  TString hname2 = "TrigSteer_EF/ChainAcceptance_runsummary";

  TFile file(fileName);
  gROOT->cd();
  TH1F * h1 = ((TH1F*)file.Get(hname1))->Clone();
//  TH1F * h2 = ((TH1F*)file.Get(hname2))->Clone();

  gStyle->SetPadBottomMargin(0.4);
  gStyle->SetOptLogy();
  TCanvas * c1 = (TCanvas*)gROOT->Get("c1");
  if( c1 == 0 ) c1 = new TCanvas("c1","",10,10,800,500);
//  c1->Divide(1,2);

 // c1->cd(1);
  if(h1 ) 
    {
      h1->LabelsOption("v");
      h1->SetLabelSize(0.08);
      h1->Draw();
    }else{
    cout << " Cannot find histogram " << hname1 << endl;
  }

//  c1->cd(2);
//  if(h2 ) 
//    {
//      h2->LabelsOption("v");
//      h2->SetLabelSize(0.08);
//      h2->Draw();
//    }else{
//    cout << " Cannot find histogram " << hname2 << endl;
//  }


  // write out to graphics file
  c1->SaveAs("trigDecision.gif");

}
