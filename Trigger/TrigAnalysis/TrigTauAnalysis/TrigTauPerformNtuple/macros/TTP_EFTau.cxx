/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int TTP_EFTau(TTree * tree)
{
  cout << " TTP_EFTau.cxx draws some quantatites of EF tauObject\n";
 TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);
  c1->Divide(2,2);

  TString cut = "eventNumber>0";//tauEF.m_EMRadius<0";
 
  c1->cd(1) ; tree->Draw("tauEF.m_EMRadius", cut);
  c1->cd(2) ; tree->Draw("tauEF.nTracks()", cut);
  c1->cd(3) ; tree->Draw("tauEF.m_isoFrac", cut);
  c1->cd(4) ; tree->Draw("tauEF.Et()", cut);   


  return 0;
}
