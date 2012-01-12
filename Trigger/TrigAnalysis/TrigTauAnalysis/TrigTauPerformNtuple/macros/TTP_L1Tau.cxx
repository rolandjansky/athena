/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int TTP_L1Tau(TTree * tree)
{
  cout << " TTP_L1Tau.cxx draws some quantatites of L1\n";
 TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);
  c1->Divide(2,2);
 
  c1->cd(1) ; tree->Draw("EventBranch.tauL1.m_TauEnergy");
  c1->cd(2) ; tree->Draw("EventBranch.tauL1.m_EmIsol");
  c1->cd(3) ; tree->Draw("EventBranch.tauL1.m_HdIsol");
  c1->cd(4) ; tree->Draw("EventBranch.tauL1.m_HdCore");


  return 0;
}
