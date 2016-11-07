/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  gSystem->Load("../lib/libsemilCorr.so");

  float minPt = 15;
  float maxPt = 205;
  int nbins = 38;

  semilCorr* corr = new semilCorr("../inputs/semilCorrHistosBinnedModFrag.root");

  vector<float> etav;
  etav.push_back(0);
  etav.push_back(1.0);
  etav.push_back(1.6);
  etav.push_back(1.8);
  etav.push_back(2.3);
  vector<float> ptv;
  ptv.push_back(4);
  ptv.push_back(10);
  ptv.push_back(20);

  for(int etaI = 0; etaI<etav.size(); etaI++){
    for(int ptI = 0; ptI<ptv.size(); ptI++){

  float eta = etav[etaI];//0,1,1.7,2.3
  float pt = ptv[ptI];//4,10,20
  TLorentzVector mu;
  mu.SetPtEtaPhiM(pt,eta,0,0.105);

  TH1F* central = new TH1F("central","central",nbins,minPt,maxPt);
  TH1F* tagup = new TH1F("tagup","tagup",nbins,minPt,maxPt);
  TH1F* tagdown = new TH1F("tagdown","tagdown",nbins,minPt,maxPt);
  TH1F* fragup = new TH1F("fragup","fragup",nbins,minPt,maxPt);
  TH1F* fragdown = new TH1F("fragdown","fragdown",nbins,minPt,maxPt);
  TH1F* decayup = new TH1F("decayup","decayup",nbins,minPt,maxPt);
  TH1F* decaydown = new TH1F("decaydown","decaydown",nbins,minPt,maxPt);
  TH1F* muup = new TH1F("muup","muup",nbins,minPt,maxPt);
  TH1F* mudown = new TH1F("mudown","mudown",nbins,minPt,maxPt);
  TH1F* idup = new TH1F("idup","idup",nbins,minPt,maxPt);
  TH1F* iddown = new TH1F("iddown","iddown",nbins,minPt,maxPt);
  TH1F* decayrewup = new TH1F("decayrewup","decayrewup",nbins,minPt,maxPt);
  TH1F* decayrewdown = new TH1F("decayrewdown","decayrewdown",nbins,minPt,maxPt);
  TH1F* murewup = new TH1F("murewup","murewup",nbins,minPt,maxPt);
  TH1F* murewdown = new TH1F("murewdown","murewdown",nbins,minPt,maxPt);

  TH1F* idbot = new TH1F("idbot","idbot",nbins,minPt,maxPt);
  TH1F* drbot = new TH1F("drbot","drbot",nbins,minPt,maxPt);
  TH1F* tagbot = new TH1F("tagbot","tagbot",nbins,minPt,maxPt);

  TH1F* allup = new TH1F("allup","allup",nbins,minPt,maxPt);
  TH1F* alldown = new TH1F("alldown","alldown",nbins,minPt,maxPt);

  for(int i = 1; i<=nbins; i++){
    TLorentzVector jet;
    float aup = 0;
    float adown = 0;
    jet.SetPtEtaPhiM(central->GetBinCenter(i),eta,0,2);
    float cval = corr->getSemilCorrToIncl(jet,mu);
    float tup = cval+corr->getSemilCorrToInclSyst(jet,mu,
						  semilCorr::TAGGINGWEIGHT);
    float tdown = cval-corr->getSemilCorrToInclSyst(jet,mu,
						    semilCorr::TAGGINGWEIGHT);
    aup += pow(tup-cval,2);
    adown += pow(cval-tdown,2);
    float dup = cval+corr->getSemilCorrToInclSyst(jet,mu,
						  semilCorr::DECAY);
    float ddown = cval-corr->getSemilCorrToInclSyst(jet,mu,
						    semilCorr::DECAY);
    aup += pow(dup-cval,2);
    adown += pow(cval-ddown,2);
    float fup = cval+corr->getSemilCorrToInclSyst(jet,mu,
						  semilCorr::FRAGMENTATION);
    float fdown = cval-corr->getSemilCorrToInclSyst(jet,mu,
						    semilCorr::FRAGMENTATION);
    aup += pow(fup-cval,2);
    adown += pow(cval-fdown,2);
    float drewUp = cval+corr->getSemilCorrToInclSyst(jet,mu,
						     semilCorr::DECAYREW);
    float drewDown = cval-corr->getSemilCorrToInclSyst(jet,mu,
						       semilCorr::DECAYREW);
    aup += pow(drewUp-cval,2);
    adown += pow(cval-drewDown,2);
    float msUp = cval+corr->getSemilCorrToInclSyst(jet,mu,
						     semilCorr::MSRESO);
    float msDown = cval-corr->getSemilCorrToInclSyst(jet,mu,
						       semilCorr::MSRESO);
    aup += pow(msUp-cval,2);
    adown += pow(cval-msDown,2);
    float idUp = cval+corr->getSemilCorrToInclSyst(jet,mu,
						     semilCorr::IDRESO);
    float idDown = cval-corr->getSemilCorrToInclSyst(jet,mu,
						       semilCorr::IDRESO);
    //if(jet.Pt()>65.5 && jet.Pt()<66) cout << idUp-cval << " " << cval-idDown << " " << cval << endl;
    aup += pow(idUp-cval,2);
    adown += pow(cval-idDown,2);
    float muUp = cval+corr->getSemilCorrToInclSyst(jet,mu,
						   semilCorr::MUONSPECTRUM);
    float muDown = cval-corr->getSemilCorrToInclSyst(jet,mu,
						       semilCorr::MUONSPECTRUM);
    aup += pow(muUp-cval,2);
    adown += pow(cval-muDown,2);

    central->SetBinContent(i,cval);
    central->SetBinError(i,0);
    fragup->SetBinContent(i,fup);
    fragdown->SetBinContent(i,fdown);
    tagup->SetBinContent(i,tup);
    tagdown->SetBinContent(i,tdown);
    decayup->SetBinContent(i,dup);
    decaydown->SetBinContent(i,ddown);
    decayrewup->SetBinContent(i,drewUp);
    decayrewdown->SetBinContent(i,drewDown);
    murewup->SetBinContent(i,muUp);
    murewdown->SetBinContent(i,muDown);
    muup->SetBinContent(i,msUp);
    mudown->SetBinContent(i,msDown);
    idup->SetBinContent(i,idUp);
    iddown->SetBinContent(i,idDown);
    allup->SetBinContent(i,sqrt(aup)+cval);
    alldown->SetBinContent(i,cval-sqrt(adown));
    idup->SetBinContent(i,sqrt(aup-pow(tup-cval,2)-pow(drewUp-cval,2))+cval);
    iddown->SetBinContent(i,cval-sqrt(aup-pow(tup-cval,2)-pow(drewUp-cval,2)));
    idbot->SetBinContent(i,sqrt(aup-pow(tup-cval,2)-pow(drewUp-cval,2))/cval);
    drbot->SetBinContent(i,(drewUp-cval)/cval);
    tagbot->SetBinContent(i,(tup-cval)/cval);
    idbot->SetBinError(i,0);
    drbot->SetBinError(i,0);
    tagbot->SetBinError(i,0);
    if(jet.Pt()>65.5 && jet.Pt()<66) cout << idup->GetBinContent(i) << " " << iddown->GetBinContent(i) << " " << cval << endl;
  }
  tagup->SetLineColor(9);
  tagdown->SetLineColor(9);
  tagbot->SetLineColor(9);
  //tagbot->SetMarkerColor(9);
  decayup->SetLineColor(kRed);
  decaydown->SetLineColor(kRed);
  decayrewup->SetLineColor(kOrange);
  drbot->SetLineColor(kOrange);
  //drbot->SetMarkerColor(kOrange);
  decayrewdown->SetLineColor(kOrange);
  fragup->SetLineColor(kMagenta);
  fragdown->SetLineColor(kMagenta);
  muup->SetLineColor(28);
  mudown->SetLineColor(28);
  idup->SetLineColor(kMagenta);
  idbot->SetLineColor(kMagenta);
  //idbot->SetMarkerColor(kMagenta);
  iddown->SetLineColor(kMagenta);
  /*muup->SetLineStyle(2);
  mudown->SetLineStyle(2);
  idup->SetLineStyle(9);
  iddown->SetLineStyle(9);*/
  murewup->SetLineColor(kOrange);
  murewdown->SetLineColor(kOrange);

  float* x = new float[nbins];
  float* xlow = new float[nbins];
  float* xhigh = new float[nbins];
  float* y = new float[nbins];
  float* ylow = new float[nbins];
  float* yhigh = new float[nbins];

  for(int i = 0; i<nbins; i++){
    x[i] = allup->GetBinCenter(i+1);
    xlow[i] = allup->GetBinWidth(i+1)/2.;
    xhigh[i] = allup->GetBinWidth(i+1)/2.;
    y[i] = central->GetBinContent(i+1);
    yhigh[i] = allup->GetBinContent(i+1) - y[i];
    ylow[i] = y[i]-alldown->GetBinContent(i+1);
  }

  TGraphAsymmErrors* g = new TGraphAsymmErrors(nbins,x,y,xlow,xhigh,ylow,yhigh);
  g->SetFillColor(kGreen+2);
  TCanvas* c = new TCanvas("c1","c1",600,600);
  c.SetBottomMargin(0.35);
  central->SetMinimum(0.94);
  central->SetMaximum(1.2);
  central->GetYaxis()->SetTitle("Correction");
  central->GetYaxis()->SetTitleOffset(1.4);
  central->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  central->Draw("e1p");
  g->Draw("3same");
  central->Draw("e1psame");
  central->Draw("e1psameaxis");
  //tagup->Draw("histsame");
  //tagdown->Draw("histsame");
  /*fragup->Draw("histsame");
  fragdown->Draw("histsame");
  decayup->Draw("histsame");
  decaydown->Draw("histsame");
  murewup->Draw("histsame");
  murewdown->Draw("histsame");*/
  //decayrewup->Draw("histsame");
  //decayrewdown->Draw("histsame");
  //idup->Draw("histsame");
  //iddown->Draw("histsame");
  /*muup->Draw("histsame");
    mudown->Draw("histsame");*/

  allup->Divide(central);
  alldown->Divide(central);

  allup->SetFillColor(kGreen+2);
  alldown->SetFillColor(10);
  allup->SetLineWidth(0);
  alldown->SetLineWidth(0);
  allup->SetLineColor(kGreen+2);
  alldown->SetLineColor(0);
  central->GetXaxis()->SetLabelSize(0.001);

  tagup->SetMarkerStyle(24);
  decayrewup->SetMarkerStyle(25);
  idup->SetMarkerStyle(27);

  tagbot->SetMarkerStyle(24);
  drbot->SetMarkerStyle(25);
  idbot->SetMarkerStyle(27);

  TLegend leg2(0.45,0.77,0.9,0.92);
  leg2.SetFillStyle(0);
  leg2.SetBorderSize(0);
  leg2.AddEntry(allup,"ATLAS Simulation","");
  leg2.AddEntry(allup,"PYTHIA MC11, #sqrt{s} = 7 TeV","");
  leg2.AddEntry(allup,"anti-k_{t} R=0.4","");
  leg2.Draw();

  TLegend leg(0.5,0.38,0.9,0.65);
  leg.SetFillStyle(0);
  leg.SetBorderSize(0);
  leg.AddEntry(central,"Central value","p");
  leg.AddEntry(tagbot,"Tagging Syst.","p");
  //leg.AddEntry(decayup,"Decay BR Syst.","l");
  //leg.AddEntry(fragup,"Fragmentation Syst.","l");
  leg.AddEntry(drbot,"Decay Spectrum Syst.","p");
  //leg.AddEntry(muup,"MS Resolution","l");
  leg.AddEntry(idbot,"Others","p");
  //leg.AddEntry(murewup,"Muon Spectrum Syst.","l");
  leg.AddEntry(g,"Combined Systematic","f");
  string kin = "p_{T}^{#mu}=";
  string etasave,ptsave;

  if(pt == 4){
    kin += "4";
    ptsave = "0";
  }
  else if(pt == 10){
    kin += "10";
    ptsave = "1";
  }
  else if(pt == 20){
    kin += "20";
    ptsave = "2";
  }
  kin+=" GeV, ";
  if(eta==0){
    kin += "|#eta|<0.8";
    etasave = "0";
  }else if(eta==1.0){
    kin += "0.8<|#eta|<1.2";
    etasave = "1";
  }else if(eta>=1.5 && eta<1.7){ 
    kin += "1.2<|#eta|<1.7";
    etasave = "2";
  }else if(eta>=1.7 && eta<=1.9){ 
    kin += "1.7<|#eta|<2.1";
    etasave = "3";
  }else if(eta>2.2 && eta<2.4){
    kin += "2.1<|#eta|<2.5";
    etasave = "4";
  }
  leg.AddEntry(g,kin.c_str(),"");
  leg.Draw();


  TPad* p = new TPad();
  p->SetTopMargin(0.65);
  p->Draw();
  p->cd();
  p->SetFillStyle(0);
  p->SetTicky(1);
  allup->GetYaxis()->SetTitle("Uncertainty");
  allup->GetYaxis()->SetTitleOffset(1.4);
  allup->GetYaxis()->SetNdivisions(504);
  allup->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  alldown->GetYaxis()->SetTitle("Uncertainty");
  alldown->GetYaxis()->SetTitleOffset(1.4);
  alldown->GetYaxis()->SetNdivisions(504);
  alldown->GetXaxis()->SetTitle("p_{T}^{jet} [GeV]");
  allup->SetMaximum(0.05);
  allup->SetMinimum(0);
  for(int i = 1; i<=allup->GetNbinsX(); i++)
    allup->SetBinContent(i,allup->GetBinContent(i)-1);
  allup->Draw("hist");
  drbot->Draw("e0psame");
  tagbot->Draw("e0psame");
  idbot->Draw("e0psame");
  //alldown->Draw("histsame");
  //alldown->Draw("histsameaxis");

  c.SaveAs(("plots/CorrSystsModFrag_pt"+ptsave+"_e"+etasave+".eps").c_str());
  delete c;
  //delete p;
  delete central;
  delete tagup;
  delete tagdown;
  delete fragup;
  delete fragdown;
  delete decayup;
  delete decaydown;
  delete decayrewup;
  delete decayrewdown;
  delete murewup;
  delete murewdown;
  delete muup;
  delete mudown;
  delete idup;
  delete iddown;
  delete allup;
  delete alldown;
  delete tagbot;
  delete drbot;
  delete idbot;
    }
  }
}
