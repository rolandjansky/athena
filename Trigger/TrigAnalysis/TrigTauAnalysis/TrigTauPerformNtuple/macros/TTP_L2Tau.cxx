/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int TTP_L2Tau(TTree * tree)
{
  cout << " TTP_L2Tau.cxx draws some quantatites TrigTau\n"
       <<" Selection correspond to the loosest tau menu \"tauNoCut\"\n";;
  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);
  c1->Divide(2,2);
 
  c1->cd(1) ; tree->Draw("tauL2.m_nMatchedTracks"); // number of matched tracks
  c1->cd(2) ; tree->Draw("tauL2.Pt()");      // pt
  c1->cd(3) ; tree->Draw("tauL2.Eta()","abs(tauL2.Eta())<20"); //  eta
  c1->cd(4) ; tree->Draw("tauL2.Phi()");    // phi 


  return 0;
}
