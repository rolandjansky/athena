/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int TTP_L2Calo(TTree * tree)
{
  cout << " TTP_L2Calo.cxx draws variables used to cut on by tau trigger HYPO\n"
       <<" Selection correspond to the loosest tau menu \"tauNoCut\"\n";;
  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);
  c1->Divide(2,2);
 
  TString cut = "tauL2.m_EMRadius<90&&tauL2.m_stripWidth2<90&&tauL2.m_isoFrac>-5";

  c1->cd(1) ; tree->Draw("tauL2.m_EMRadius", cut); // EMradius 2 sampling
  c1->cd(2) ; tree->Draw("tauL2.m_isoFrac", cut); // Isolation fraction
  c1->cd(3) ; tree->Draw("tauL2.m_stripWidth2",cut); // StripWidth
  c1->cd(4) ; tree->Draw("tauL2.m_EtCalib", cut); // CalibEt


  return 0;
}
