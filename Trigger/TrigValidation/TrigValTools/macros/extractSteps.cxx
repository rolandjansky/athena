/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int extractSteps(const char * fileName, const char * tag1 = "", const char * tag2 = "", const char * tag3 = "" )
{
  if( fileName == "" || tag1 == "" )
    {
      cout << " Usage : .x extractSteps.cxx (\"filename\",\"tag1\"[,\"tag2\",\"tag3\"]) \n";
      return 1;
    }

  TString hname1 = "TrigSteer_L2/signatureAcceptance_L2_runsummary";
  TString hname2 = "TrigSteer_EF/signatureAcceptance_EF_runsummary";

  TFile file(fileName);
  gROOT->cd();
  TH2 * h1old = (TH2*)file.Get(hname1);
  TH2 * h2old = (TH2*)file.Get(hname2);
  if( h1old) h1old=(TH2*)h1old->Clone();
  if( h2old) h2old=(TH2*)h2old->Clone();

  TH2F * h1, * h2;
  if( tag1 != "" ) {
    h1 = extractFromHist(h1old, tag1, tag2, tag3);
    h2 = extractFromHist(h2old, tag1, tag2, tag3);
  }else{
    h1 = h1old;
    h2 = h2old;
  }


  gStyle->SetPadBottomMargin(0.45);
  gStyle->SetPadGridY(1);
  gStyle->SetOptLogy(0);
  TCanvas * c1 = (TCanvas*)gROOT->Get("c1");
  if( c1 == 0 ) c1 = new TCanvas("c1","",10,10,800,500);
  c1->Divide(1,2);

  c1->cd(1);
  if(h1 )
    {
      
      h1->LabelsOption("av");
      h1->SetLabelSize(0.08);
      h1->Draw("text");
    }else{
    cout << " Cannot find histogram " << hname1 << endl;
  }

  c1->cd(2);
  if(h2 )
    {
      h2->LabelsOption("av");
      h2->SetLabelSize(0.08);
      h2->Draw("text");
    }else{
    cout << " Cannot find histogram " << hname2 << endl;
  }


}

TH2F * extractFromHist(TH2 * h1, const char * tag1, const char * tag2, const char * tag3)
{
  if( h1 == 0 ) return 0;
  TAxis * axis  = h1->GetXaxis();
  TAxis * axisY = h1->GetYaxis();

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

  int nY =  axisY->GetNbins();

  newName.Append("_sub");
  TH2F * hNew = (TH2F*)gDirectory->Get(newName);
  if( hNew ) delete hNew;
  hNew = new TH2F(newName,h1->GetTitle(), nNew, 0, nNew, nY,0, nY);
  hNew->GetYaxis()->ImportAttributes(axisY);

  // name axis
  for(unsigned int ibin = 1; ibin <= nY ; ++ibin)
    hNew->GetYaxis()->SetBinLabel(ibin,axisY->GetBinLabel(ibin));

  for(unsigned int ibin = 1; ibin <= nNew ; ++ibin)
    {
      hNew->GetXaxis()->SetBinLabel(ibin,newLabels[ibin-1]);
      for(unsigned int jbin = 0; jbin <= nY+1; ++jbin )
	hNew->SetBinContent(ibin, jbin,
			    h1->GetBinContent(h1->GetXaxis()->FindBin(newLabels[ibin-1]), jbin));
      

    }


  return hNew;

}
