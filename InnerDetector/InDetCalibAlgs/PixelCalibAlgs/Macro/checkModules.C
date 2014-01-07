/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

string run="";

void checkModules() {
  string filename=_file0->GetName();
  int pos = filename.find("run");
  run = filename.substr(pos,9);
  return;
}


void checkModule(string name) {
  string dir;
  string layer = name.substr(0,3);
  if      ( layer == "L0_" ) dir = "_barrel/B-layer";
  else if ( layer == "L1_" ) dir = "_barrel/Layer1";
  else if ( layer == "L2_" ) dir = "_barrel/Layer2";
  else if ( layer == "D1A" ) dir = "_endcapA/Disk1";
  else if ( layer == "D2A" ) dir = "_endcapA/Disk2";
  else if ( layer == "D3A" ) dir = "_endcapA/Disk3";
  else if ( layer == "D1C" ) dir = "_endcapC/Disk1";
  else if ( layer == "D2C" ) dir = "_endcapC/Disk2";
  else if ( layer == "D3C" ) dir = "_endcapC/Disk3";
  else {
    cout << "Unknown layer for module " << name << endl;
    return;
  }
  _file0->cd( (string("LBdep")+dir).c_str() );
  gROOT->ProcessLine((string("TH1D* myh1d = ")+name).c_str());
  _file0->cd( (string("hitMaps")+dir).c_str() );
  gROOT->ProcessLine((string("TH2D* myh2d = ")+name).c_str());
  _file1->cd( (string("LBdep")+dir).c_str() );
  gROOT->ProcessLine((string("TH1D* myh3d = ")+name).c_str());
  _file1->cd( (string("hitMaps")+dir).c_str() );
  gROOT->ProcessLine((string("TH2D* myh4d = ")+name).c_str());
  gStyle->SetPalette(1);
  TCanvas *myC = new TCanvas("myC",name.c_str(),1000,800);
  myC->Divide(2,3);
  // Hitmap from calibration_PixelNoise stream
  TVirtualPad* myPad = myC->cd(1);
  myPad->SetLogy(kFALSE);
  myh2d->SetMaximum(20.);
  myh2d->SetStats(kFALSE);
  myh2d->GetXaxis()->SetTitle("Column");
  myh2d->GetYaxis()->SetTitle("Row");
  myh2d->Draw("COLZ");
  // Hitmap from express_express stream
  myPad = myC->cd(2);
  myPad->SetLogy(kFALSE);
  myh4d->SetStats(kFALSE);
  myh4d->GetXaxis()->SetTitle("Column");
  myh4d->GetYaxis()->SetTitle("Row");
  myh4d->Draw("COLZ");
  // Poisson distribution from calibration_PixelNoise
  myPad = myC->cd(3);
  myPad->SetLogy(kTRUE);
  poissonTest(myh2d,20.);
  // Noise occupancy vs. Lumi-block
  myPad = myC->cd(5);
  myPad->SetLogy(kFALSE);
  myh1d->SetStats(kFALSE);
  myh1d->GetXaxis()->SetTitle("Lumi-block");
  myh1d->GetYaxis()->SetTitle("Hits/Lumi-block");
  myh1d->Draw();  
  // Hit occupancy vs. Lumi-block
  myPad = myC->cd(6);
  myPad->SetLogy(kFALSE);
  myh3d->SetStats(kFALSE);
  myh3d->GetXaxis()->SetTitle("Lumi-block");
  myh3d->GetYaxis()->SetTitle("Hits/Lumi-block");
  myh3d->Draw();  
  // save plot and exit
  myC->SaveAs( (run+"_"+name+".png").c_str() );
  return;
}


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
  myPoisson->SetMarkerStyle(20);
  myPoisson->GetXaxis()->SetTitle("Hits");
  myPoisson->GetYaxis()->SetTitle("Number of pixels");
  myPoisson->Draw("");
  myPoisson->Draw("ESAME");
  myPoisson2->SetLineColor(2);
  myPoisson2->SetLineWidth(2.0);
  myPoisson2->Draw("SAME");
  return fraction;
}


double poissonTest2(TH2D* myModule) {
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
  TH1F* myPoisson3 = new TH1F("myPoisson3","Poisson distribution",hmax+1,-0.5,hmax+0.5);  
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
  double p1  = 320*144*exp(-mean);
  for (int i=0; i<=hmax; i++) {
    myPoisson2->Fill(i,p10+p15+p20+p30);
    myPoisson3->Fill(i,p1);
    p1*=(mean/(i+1));
    p10*=(mu/(i+1));
    p15*=(1.5*mu/(i+1));
    p20*=(2.0*mu/(i+1));
    p30*=(3.0*mu/(i+1));
  }
  myPoisson->SetFillColor(5);
  myPoisson->Draw("");
  myPoisson->Draw("ESAME");
  myPoisson3->SetLineColor(3);
  myPoisson3->Draw("SAME");
  myPoisson2->SetLineColor(2);
  myPoisson2->Draw("SAME");
  return mu;
}
