/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

double poissonTest(TH2D* myModule, double threshold) {
  int hmax=20;
  TH1F* myPoisson = new TH1F("myPoisson","Hit distribution",hmax+1,-0.5,hmax+0.5);
  for (int row=0; row<160; row++) {
    for (int col=0; col<144; col++) {
      double hits = myModule->GetBinContent(col+1,row+1);
      myPoisson->Fill(hits);
      hits = myModule->GetBinContent(col+1,327-row+1);
      myPoisson->Fill(hits);
    }
  }
  TH1F* myPoisson2 = new TH1F("myPoisson2","Poisson distribution",hmax+1,-0.5,hmax+0.5);  
  double mean = myPoisson->GetMean();
  double fr10 = 16.*156./(160.*18.);
  double fr15 =  2.*156./(160.*18.);
  double fr20 = 16.*4./(160.*18.);
  double fr30 =  2.*4./(160.*18.);
  double mu = mean/(fr10+fr15*1.5+fr20*2.+fr30*3.);
  double p10 = 320*144*fr10*exp(-mu);
  double p15 = 320*144*fr15*exp(-1.5*mu);
  double p20 = 320*144*fr20*exp(-2.*mu);
  double p30 = 320*144*fr30*exp(-3.*mu);
  double fraction=0.;
  for (int i=0; i<=hmax; i++) {
    if (i>threshold) fraction+=(p10+p15+p20+p30); 
    myPoisson2->Fill(i,p10+p15+p20+p30);
    p10*=(mu/(i+1));
    p15*=(1.5*mu/(i+1));
    p20*=(2.0*mu/(i+1));
    p30*=(3.0*mu/(i+1));
  }
  myPoisson->SetFillColor(5);
  myPoisson->Draw("");
  myPoisson->Draw("ESAME");
  myPoisson2->SetLineColor(2);
  myPoisson2->Draw("SAME");
  return fraction;
}
