/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
gStyle->SetOptStat(1111);
TCanvas *c = new TCanvas("Tracks","Pixel hits on track",600,900);
c->Divide(1,3);
c->cd(1);
TH1F phit_all("phit_all","Hits per track",10,-0.5,9.5);
Track_Chain->Draw("nPixelHits>>phit_all");
c->cd(2);
TH1F phit_ontrack("phit_ontrack","Pixel hits on track",10,-0.5,9.5);
Track_Chain->Draw("nPixelHits>>phit_ontrack","nPixelHits>0");
c->cd(3);
TH1F phit_ongoodtrack("phit_ongoodtrack","Pixel hits after selection",10,-0.5,9.5);
Track_Chain->Draw("nPixelHits>>phit_ongoodtrack","(5*nSCTHits+nTRTHits)>=30&nPixelHits>0");
}
