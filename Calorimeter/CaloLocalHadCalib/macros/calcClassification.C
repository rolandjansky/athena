/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TFile.h>
#include <TList.h>
#include <TKey.h>
#include <TH2F.h>
#include <TProfile2D.h>
#include <iostream>
#include <fstream>
void calcClassification(char * fnpi="classification_had.root", char *fnpi0="classification_em.root", char *fnew="classification_out.root") {
  const double epsilon = 1e-3;
  TFile * fn   = new TFile(fnew,"RECREATE");
  TFile * fpi  = new TFile(fnpi);
  TFile * fpi0 = new TFile(fnpi0);
  TList * tpi  = fpi->GetListOfKeys();
  while  ( tpi->GetEntries() > 0 ) {
    TKey * key = (TKey *)tpi->First();
    const TH2F *ppi  = (TH2F *)fpi->Get(key->GetName());
    const TH2F *ppi0 = (TH2F *)fpi0->Get(key->GetName());
    fn->cd();
    TString hname(ppi->GetName());
    hname.Replace(0,5,"classification");
    TProfile2D * hn = (TProfile2D *)fn->Get(hname.Data());
    if ( !hn ) {
      hn = new TProfile2D(hname.Data(),
                          hname.Data(),
                          ppi->GetNbinsX(),
			  ppi->GetXaxis()->GetXmin(),
			  ppi->GetXaxis()->GetXmax(),
                          ppi->GetNbinsY(),
			  ppi->GetYaxis()->GetXmin(),
			  ppi->GetYaxis()->GetXmax());
      hn->Reset();
      hn->SetXTitle(ppi->GetXaxis()->GetTitle());
      hn->SetYTitle(ppi->GetYaxis()->GetTitle());
    }
    double ntotpi = ppi->GetSum();
    double ntotpi0 = ppi0->GetSum();
    for (int ix=0;ix<hn->GetNbinsX();ix++) {
      for (int iy=0;iy<hn->GetNbinsY();iy++) {
	double x=ppi->GetXaxis()->GetBinCenter(ix+1);
	double y=ppi->GetYaxis()->GetBinCenter(iy+1);
	double dnpi = ppi->GetBinError(ppi->GetBin(ix+1,iy+1));
	double npi = ppi->GetBinContent(ppi->GetBin(ix+1,iy+1));
	double wpi = npi/(ntotpi>0?ntotpi:1);
	double dnpi0 = ppi0->GetBinError(ppi0->GetBin(ix+1,iy+1));
	double npi0 = ppi0->GetBinContent(ppi0->GetBin(ix+1,iy+1));
	double wpi0 = npi0/(ntotpi0>0?ntotpi0:1);
	double w = (2*wpi+wpi0);
	if ( w > 0 ) {
	  w = wpi0/w;
	}
	else
	  w = 0;
	if ( npi + npi0 > 0 ) {
	  double e = sqrt(w*(1-w)/(dnpi*dnpi+dnpi0*dnpi0));
	  if ( w == 0 ) 
	    w = epsilon;
	  cout << "w = " << w << " e = " << e << " spr = " << e*sqrt(dnpi0*dnpi0+dnpi*dnpi) << " n = " << dnpi0*dnpi0+dnpi*dnpi << endl;
	  hn->Fill(x,y,w-e,(dnpi*dnpi + dnpi0*dnpi0)/2);
	  hn->Fill(x,y,w+e,(dnpi*dnpi + dnpi0*dnpi0)/2);
	}
      }
    }
    tpi->RemoveFirst();
  }
  fn->Write();
  fn->Close();
  fn->Delete();
}
