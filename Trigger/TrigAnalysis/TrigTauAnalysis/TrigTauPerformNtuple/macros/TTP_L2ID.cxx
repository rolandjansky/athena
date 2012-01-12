/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int TTP_L2ID(TTree * tree)
{
  cout << " TTP_L2ID.cxx draws some quantatites of trig InDet tracks associated with TrigTau (loosest menu)\n";
 TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);
  c1->Divide(3,2);
 
  unsigned int nTracksInDR(double dRCut, double minPt = 1500) const;
  int          chargeInDR(double dRCut, double minPt = 1500) const;
  unsigned int nSlowTracksInDR(double dRCut, double maxPt , double minPt = 1500) const;
  unsigned int nTracksInIsolation(double dRMin, double dRMax, double minPt = 1500) const;

  c1->cd(1) ; tree->Draw("tauL2.nTracks()"); // number of tracks
  c1->cd(2) ; tree->Draw("tauL2.nTracksInDR(0.15)"); // number of tracks in basic area
  c1->cd(3) ; tree->Draw("tauL2.chargeInDR(0.15)");  // charge in basic area
  c1->cd(4) ; tree->Draw("tauL2.nSlowTracksInDR(0.15,7000)");  // charge in basic area
  c1->cd(5) ; tree->Draw("tauL2.nTracksInIsolation(0.15,0.3)");    // phi 
  c1->cd(6) ; tree->Draw("tauL2.leadingTrack().Pt()");    // phi 


  return 0;
}
