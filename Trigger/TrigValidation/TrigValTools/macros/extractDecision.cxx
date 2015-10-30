/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int extractDecision(const char * fileName, const char * tag1="", const char * tag2 = "", const char * tag3 = "" )
{
  if( fileName == "" || tag1 == "" )
    {
      cout << " Usage : .x extractDecision.cxx (\"filename\",\"tag1\"[,\"tag2\",\"tag3\"]) \n";
      return 1;
    }

  TString hname1 = "TrigSteer_L2/chainAcceptance_L2_runsummary";
  TString hname2 = "TrigSteer_EF/chainAcceptance_EF_runsummary";

  TFile file(fileName);
  gROOT->cd();
  TH1F * h1old = (TH1F*)file.Get(hname1);
  TH1F * h2old = (TH1F*)file.Get(hname2);
  if( h1old) h1old=(TH2*)h1old->Clone(); // to get copy of this in gROOT
  if( h2old) h2old=(TH2*)h2old->Clone();

  TH1F * h1, * h2;
  if( tag1 != "" ) {
    h1 = extractFromHist(h1old, tag1, tag2, tag3);
    h2 = extractFromHist(h2old, tag1, tag2, tag3);
  }else{
    h1 = h1old;
    h2 = h2old;
  }


  gStyle->SetPadBottomMargin(0.4);
  gStyle->SetOptLogy();
  TCanvas * c1 = (TCanvas*)gROOT->Get("c1");
  if( c1 == 0 ) c1 = new TCanvas("c1","",10,10,800,500);
  c1->Divide(1,2);

  c1->cd(1);
  if(h1 )
    {
      
      h1->LabelsOption("va");
      h1->SetLabelSize(0.08);
      h1->Draw();
    }else{
    cout << " Cannot find histogram " << hname1 << endl;
  }

  c1->cd(2);
  if(h2 )
    {
      h2->LabelsOption("va");
      h2->SetLabelSize(0.08);
      h2->Draw();
    }else{
    cout << " Cannot find histogram " << hname2 << endl;
  }


}

TH1F * extractFromHist(TH1F * h1, const char * tag1, const char * tag2, const char * tag3)
{
  if( h1 == 0 ) return 0;
  TAxis * axis = h1->GetXaxis();

  std::vector<TString> newLabels;

  int nbins = axis->GetNbins();
  for(unsigned int ibin = 1; ibin <= nbins ; ++ibin)
    {
      TString label(axis->GetBinLabel(ibin));
      //cout<< ibin << " " << label << endl;
      if( label.Contains(tag1) )
	{
	  if( tag2 == "" || label.Contains(tag2) ) {
	    if( tag3 == "" || label.Contains(tag3) ) 
	      newLabels.push_back(label);
	    //	cout << "           taken\n";}
	  }
	}
    }
  
  int nNew = newLabels.size();
  TString newName(h1->GetName());

  newName.Append("_sub");
  TH1F * hNew = (TH1F*)gDirectory->Get(newName);
  if( hNew ) delete hNew;
  hNew = new TH1F(newName,h1->GetTitle(), nNew, 0, nNew);
  for(unsigned int ibin = 1; ibin <= nNew ; ++ibin)
    {
      hNew->GetXaxis()->SetBinLabel(ibin,newLabels[ibin-1]);
      hNew->SetBinContent(ibin,h1->GetBinContent(h1->GetXaxis()->FindBin(newLabels[ibin-1])));
    }

  return hNew;

}
