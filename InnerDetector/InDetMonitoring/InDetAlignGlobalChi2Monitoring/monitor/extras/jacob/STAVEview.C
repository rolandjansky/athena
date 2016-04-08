/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void STAVEview(char nname[80]=NULL,    float scale = 100)
{
  gStyle->SetCanvasColor(kWhite);  
  if (nname == NULL) sprintf(nname,"jacobanaOutput.root");
  TFile* f0 = new TFile(nname);

  
  //---------------------------------------
  // crea els histogrames
  //---------------------------------------
  const int MaxDOF=6;
  TH1F *hPIXL0[6];
  TH1F *hPIXL1[6];
  TH1F *hPIXL2[6];
  TH1F *hSCTL0[6];
  TH1F *hSCTL1[6];
  TH1F *hSCTL2[6];
  TH1F *hSCTL3[6];

  char DOFname[10];
  char hTitle[80];
  char hName[80];
  float TwoPi = TMath::TwoPi();
  float Pi = TMath::Pi();
  float DeltaPhi; 
  int NMods;

  for (int iDOF=0; iDOF < MaxDOF; iDOF++) {
    if (iDOF==0) sprintf(DOFname,"%s","Tx");
    if (iDOF==1) sprintf(DOFname,"%s","Ty");
    if (iDOF==2) sprintf(DOFname,"%s","Tz");
    if (iDOF==3) sprintf(DOFname,"%s","Rx");
    if (iDOF==4) sprintf(DOFname,"%s","Ry");
    if (iDOF==5) sprintf(DOFname,"%s","Rz");
    // Pix layer 0 
    sprintf(hTitle,"PIX L0 %s",DOFname);
    sprintf(hName,"hPIXL0%s",DOFname);
    NMods = 22;
    DeltaPhi = Pi/NMods;
    hPIXL0[iDOF] = new TH1F(hName,hTitle,NMods,0.-DeltaPhi,TwoPi-DeltaPhi);
    // Pix layer 1
    sprintf(hTitle,"PIX L1 %s",DOFname);
    sprintf(hName,"hPIXL1%s",DOFname);
    NMods = 38;
    DeltaPhi = Pi/NMods;
    hPIXL1[iDOF] = new TH1F(hName,hTitle,NMods,0.-DeltaPhi,TwoPi-DeltaPhi);
    // Pix layer 2
    sprintf(hTitle,"PIX L2 %s",DOFname);
    sprintf(hName,"hPIXL2%s",DOFname);
    NMods = 52;
    DeltaPhi = Pi/NMods;
    hPIXL2[iDOF] = new TH1F(hName,hTitle,NMods,0.-DeltaPhi,TwoPi-DeltaPhi);
    // SCT layer 0 
    sprintf(hTitle,"SCT L0 %s",DOFname);
    sprintf(hName,"hSCTL0%s",DOFname);
    NMods = 32;
    DeltaPhi = Pi/NMods;
    hSCTL0[iDOF] = new TH1F(hName,hTitle,NMods,0.-DeltaPhi,TwoPi-DeltaPhi);
    // SCT layer 1
    sprintf(hTitle,"SCT L1 %s",DOFname);
    sprintf(hName,"hSCTL1%s",DOFname);
    NMods = 40;
    DeltaPhi = Pi/NMods;
    hSCTL1[iDOF] = new TH1F(hName,hTitle,NMods,0.-DeltaPhi,TwoPi-DeltaPhi);
    // SCT layer 2
    sprintf(hTitle,"SCT L2 %s",DOFname);
    sprintf(hName,"hSCTL2%s",DOFname);
    NMods = 48;
    DeltaPhi = Pi/NMods;
    hSCTL2[iDOF] = new TH1F(hName,hTitle,NMods,0.-DeltaPhi,TwoPi-DeltaPhi);
    // SCT layer 3
    sprintf(hTitle,"SCT L3 %s",DOFname);
    sprintf(hName,"hSCTL3%s",DOFname);
    NMods = 56;
    DeltaPhi = Pi/NMods;
    hSCTL3[iDOF] = new TH1F(hName,hTitle,NMods,0.-DeltaPhi,TwoPi-DeltaPhi);
  }

  
  //---------------------------------------
  // colorins
  //---------------------------------------
  Int_t Color_PIXL0 = kBlue;
  Int_t Color_PIXL1 = kRed;
  Int_t Color_PIXL2 = kGreen;
  Int_t Color_SCTL0 = kViolet;
  Int_t Color_SCTL1 = kBlue;
  Int_t Color_SCTL2 = kCyan+2;
  Int_t Color_SCTL3 = kAzure+10;


  //---------------------------------------
  // Omplie els histogrames
  //---------------------------------------
  TH1F *hConst;
  int chan;

  float value;
  for (int iDOF=0; iDOF < MaxDOF; iDOF++) {
    chan = 0;
    if (iDOF==0) hConst = (TH1F*)f0->Get("jacobana/hTxAliCorr")->Clone();
    if (iDOF==1) hConst = (TH1F*)f0->Get("jacobana/hTyAliCorr")->Clone();
    if (iDOF==2) hConst = (TH1F*)f0->Get("jacobana/hTzAliCorr")->Clone();
    if (iDOF==3) hConst = (TH1F*)f0->Get("jacobana/hRxAliCorr")->Clone();
    if (iDOF==4) hConst = (TH1F*)f0->Get("jacobana/hRyAliCorr")->Clone();
    if (iDOF==5) hConst = (TH1F*)f0->Get("jacobana/hRzAliCorr")->Clone();
    //cout << " NbinsX hConst = " << hConst->GetNbinsX() << endl;
    // jump the PIX EndCap A
    chan++;
    // Pix Layer 0
    for (int bin=1; bin<= hPIXL0[iDOF]->GetNbinsX(); bin++) {
      chan++;
      value = hConst->GetBinContent(chan);
      hPIXL0[iDOF]->SetBinContent(bin,1,value);
    }
    // Pix Layer 1
    for (int bin=1; bin<= hPIXL1[iDOF]->GetNbinsX(); bin++) {
      chan++;
      value = hConst->GetBinContent(chan);
      hPIXL1[iDOF]->SetBinContent(bin,value);
    }
    // Pix Layer 2
    for (int bin=1; bin<= hPIXL2[iDOF]->GetNbinsX(); bin++) {
      chan++;
      value = hConst->GetBinContent(chan);
      hPIXL2[iDOF]->SetBinContent(bin,value);
    }
    // jump the PIX EndCap C
    chan++;
    // jump the SCT EndCap A
    chan++;
    // SCT layer 0
    for (int bin=1; bin<= hSCTL0[iDOF]->GetNbinsX(); bin++) {
      chan++;
      value = hConst->GetBinContent(chan);
      hSCTL0[iDOF]->SetBinContent(bin,value);
    }
    // SCT layer 1
    for (int bin=1; bin<= hSCTL1[iDOF]->GetNbinsX(); bin++) {
      chan++;
      value = hConst->GetBinContent(chan);
      hSCTL1[iDOF]->SetBinContent(bin,value);
    }
    // SCT layer 2
    for (int bin=1; bin<= hSCTL2[iDOF]->GetNbinsX(); bin++) {
      chan++;
      value = hConst->GetBinContent(chan);
      hSCTL2[iDOF]->SetBinContent(bin,value);
    }
    // SCT layer 3
    for (int bin=1; bin<= hSCTL3[iDOF]->GetNbinsX(); bin++) {
      chan++;
      value = hConst->GetBinContent(chan);
      hSCTL3[iDOF]->SetBinContent(bin,value);
    }
    // jump the SCT EndCap C
    chan++;
    //    cout << " chan= " << chan << endl;
  }
  

  //---------------------------------------
  // Pinta els histogrames
  //---------------------------------------
  TCanvas *CSV[7];
  char cName[80];


  for (int layer=0; layer<3+4; layer++) {
    if (layer==0) sprintf(cName,"%s","StavesPIXL0");
    if (layer==1) sprintf(cName,"%s","StavesPIXL1");
    if (layer==2) sprintf(cName,"%s","StavesPIXL2");
    if (layer==3) sprintf(cName,"%s","StavesSCTL0");
    if (layer==4) sprintf(cName,"%s","StavesSCTL1");
    if (layer==5) sprintf(cName,"%s","StavesSCTL2");
    if (layer==6) sprintf(cName,"%s","StavesSCTL3");

    CSV[layer] = new TCanvas(cName,cName,1000,500); 
    CSV[layer]->Divide(3,2);
    
    for (int iDOF=0; iDOF < MaxDOF; iDOF++) {
      CSV[layer]->cd(iDOF+1);
      if (layer==0) hPIXL0[iDOF]->Draw();
      if (layer==1) hPIXL1[iDOF]->Draw();
      if (layer==2) hPIXL2[iDOF]->Draw();
      if (layer==3) hSCTL0[iDOF]->Draw();
      if (layer==4) hSCTL1[iDOF]->Draw();
      if (layer==5) hSCTL2[iDOF]->Draw();
      if (layer==6) hSCTL3[iDOF]->Draw();
    }
  }


  //---------------------------------------
  // Pinta els circulets
  //---------------------------------------
  TCanvas *CanCercles = new TCanvas("StaveCercles","StaveCercles",1150,700);
  CanCercles->Divide(3,2);
  
  // Pinta els cercles de cada layer
  TArc *ArcPIXL0 = new TArc(0,0,50.5);
  ArcPIXL0->SetFillStyle(0);
  ArcPIXL0->SetLineColor(kGray+2);

  TArc *ArcPIXL1 = new TArc(0,0,88.5);
  ArcPIXL1->SetFillStyle(0);
  ArcPIXL1->SetLineColor(kGray+2);
  
  TArc *ArcPIXL2 = new TArc(0,0,122.5);
  ArcPIXL2->SetFillStyle(0);
  ArcPIXL2->SetLineColor(kGray+2);
  
  TArc *ArcSCTL0 = new TArc(0,0,299.0);
  ArcSCTL0->SetFillStyle(0);
  ArcSCTL0->SetLineColor(kGray+2);
  
  TArc *ArcSCTL1 = new TArc(0,0,371.0);
  ArcSCTL1->SetFillStyle(0);
  ArcSCTL1->SetLineColor(kGray+2);
  
  TArc *ArcSCTL2 = new TArc(0,0,443.0);
  ArcSCTL2->SetFillStyle(0);
  ArcSCTL2->SetLineColor(kGray+2);
  
  TArc *ArcSCTL3 = new TArc(0,0,514.0);
  ArcSCTL3->SetFillStyle(0);
  ArcSCTL3->SetLineColor(kGray+2);
  
  float x0, y0, x1, y1;
  //
  TText *Rotllo = new TText();
  Rotllo->SetTextSize(0.025);
  
  TH2F *hDofRef[6];
  for (int iDOF=0; iDOF < MaxDOF; iDOF++) {
    if (iDOF==0) sprintf(DOFname,"%s","Tx (mm)");
    if (iDOF==1) sprintf(DOFname,"%s","Ty (mm)");
    if (iDOF==2) sprintf(DOFname,"%s","Tz (mm)");
    if (iDOF==3) sprintf(DOFname,"%s","Rx (mrad)");
    if (iDOF==4) sprintf(DOFname,"%s","Ry (mrad)");
    if (iDOF==5) sprintf(DOFname,"%s","Rz (mrad)");
   
    hDofRef[iDOF] = new TH2F(DOFname,DOFname,30,-520,520,30,-520,520);
    CanCercles->cd(iDOF+1);
    hDofRef[iDOF]->SetStats(false);
    hDofRef[iDOF]->Draw();

    ArcPIXL0->Draw();
    ArcPIXL1->Draw();
    ArcPIXL2->Draw();
    ArcSCTL0->Draw();
    ArcSCTL1->Draw();
    ArcSCTL2->Draw();
    ArcSCTL3->Draw();
    
    // 
    TArrow *Fletxa = new TArrow(); 
    Fletxa->SetLineWidth(1);

    // PixL0
    for (int i=1; i<=hPIXL0[iDOF]->GetNbinsX(); i++) {
      float value = hPIXL0[iDOF]->GetBinContent(i);
      float phi = hPIXL0[iDOF]->GetBinCenter(i);
      x0 = ArcPIXL0->GetR1() * cos(phi);
      y0 = ArcPIXL0->GetR1() * sin(phi);
      x1 = (ArcPIXL0->GetR1()+value*scale) * cos(phi);
      y1 = (ArcPIXL0->GetR1()+value*scale) * sin(phi);
      Fletxa->SetLineColor(Color_PIXL0);
      Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
    }
    // PixL1
    for (int i=1; i<=hPIXL1[iDOF]->GetNbinsX(); i++) {
      float value = hPIXL1[iDOF]->GetBinContent(i);
      float phi = hPIXL1[iDOF]->GetBinCenter(i);
      x0 = ArcPIXL1->GetR1() * cos(phi);
      y0 = ArcPIXL1->GetR1() * sin(phi);
      x1 = (ArcPIXL1->GetR1()+value*scale) * cos(phi);
      y1 = (ArcPIXL1->GetR1()+value*scale) * sin(phi);
      Fletxa->SetLineColor(Color_PIXL1);
      Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
    }
    // PixL2
    for (int i=1; i<=hPIXL2[iDOF]->GetNbinsX(); i++) {
      float value = hPIXL2[iDOF]->GetBinContent(i);
      float phi = hPIXL2[iDOF]->GetBinCenter(i);
      x0 = ArcPIXL2->GetR1() * cos(phi);
      y0 = ArcPIXL2->GetR1() * sin(phi);
      x1 = (ArcPIXL2->GetR1()+value*scale) * cos(phi);
      y1 = (ArcPIXL2->GetR1()+value*scale) * sin(phi);
      Fletxa->SetLineColor(Color_PIXL2);
      Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
    }
    // SCTL0
    for (int i=1; i<=hSCTL0[iDOF]->GetNbinsX(); i++) {
      float value = hSCTL0[iDOF]->GetBinContent(i);
      float phi = hSCTL0[iDOF]->GetBinCenter(i);
      x0 = ArcSCTL0->GetR1() * cos(phi);
      y0 = ArcSCTL0->GetR1() * sin(phi);
      x1 = (ArcSCTL0->GetR1()+value*scale) * cos(phi);
      y1 = (ArcSCTL0->GetR1()+value*scale) * sin(phi);
      Fletxa->SetLineColor(Color_SCTL0);
      Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
    }
    // SCTL1
    for (int i=1; i<=hSCTL1[iDOF]->GetNbinsX(); i++) {
      float value = hSCTL1[iDOF]->GetBinContent(i);
      float phi = hSCTL1[iDOF]->GetBinCenter(i);
      x0 = ArcSCTL1->GetR1() * cos(phi);
      y0 = ArcSCTL1->GetR1() * sin(phi);
      x1 = (ArcSCTL1->GetR1()+value*scale) * cos(phi);
      y1 = (ArcSCTL1->GetR1()+value*scale) * sin(phi);
      Fletxa->SetLineColor(Color_SCTL1);
      Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
    }
    // SCTL2
    for (int i=1; i<=hSCTL2[iDOF]->GetNbinsX(); i++) {
      float value = hSCTL2[iDOF]->GetBinContent(i);
      float phi = hSCTL2[iDOF]->GetBinCenter(i);
      x0 = ArcSCTL2->GetR1() * cos(phi);
      y0 = ArcSCTL2->GetR1() * sin(phi);
      x1 = (ArcSCTL2->GetR1()+value*scale) * cos(phi);
      y1 = (ArcSCTL2->GetR1()+value*scale) * sin(phi);
      Fletxa->SetLineColor(Color_SCTL2);
      Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
    }
    // SCTL3
    for (int i=1; i<=hSCTL3[iDOF]->GetNbinsX(); i++) {
      float value = hSCTL3[iDOF]->GetBinContent(i);
      float phi = hSCTL3[iDOF]->GetBinCenter(i);
      x0 = ArcSCTL3->GetR1() * cos(phi);
      y0 = ArcSCTL3->GetR1() * sin(phi);
      x1 = (ArcSCTL3->GetR1()+value*scale) * cos(phi);
      y1 = (ArcSCTL3->GetR1()+value*scale) * sin(phi);
      Fletxa->SetLineColor(Color_SCTL3);
      Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
    }

    // i ara cal pintar la referencia
    x0 = 400;
    y0 = 450;
    x1 = x0+1*scale;
    y1 = y0;
    Fletxa->SetLineColor(kBlack);
    Fletxa->DrawArrow(x0,y0,x1,y1,0.002);

    // i lo que es
    if (iDOF<3) {
      Rotllo->DrawText(390,460,"1 mm");
    }
    else {
      Rotllo->DrawText(390,460,"1 mrad");
    }
  }

  //---------------------------------------
  // Pinta els circulets desplac,ament en el planol XY
  //---------------------------------------
  TCanvas *CanTxyCercles = new TCanvas("TxyCercles","TxyCercles",700,700);

  float x0, y0, x1, y1;

  TH2F *hDofRef[6];
  hDofRef[0]->Draw();

  // Pinta els cercles de cada layer
  ArcPIXL0->Draw();
  ArcPIXL1->Draw();
  ArcPIXL2->Draw();
  ArcSCTL0->Draw();
  ArcSCTL1->Draw();
  ArcSCTL2->Draw();
  ArcSCTL3->Draw();
  
  // 
  TArrow *Fletxa = new TArrow(); 
  Fletxa->SetLineWidth(2);
  
  // PixL0
  for (int i=1; i<=hPIXL0[0]->GetNbinsX(); i++) {
    float tx = hPIXL0[0]->GetBinContent(i);
    float tz = hPIXL0[2]->GetBinContent(i);
    float phi = hPIXL0[0]->GetBinCenter(i);
    x0 = ArcPIXL0->GetR1() * cos(phi);
    y0 = ArcPIXL0->GetR1() * sin(phi);
    x1 = (ArcPIXL0->GetR1()+tz*scale) * cos(phi); x1+=(tx*scale)*cos(phi+Pi/2); 
    y1 = (ArcPIXL0->GetR1()+tz*scale) * sin(phi); x1+=(tx*scale)*sin(phi+Pi/2); 
    Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
  }
  // PixL1
  for (int i=1; i<=hPIXL1[0]->GetNbinsX(); i++) {
    float tx = hPIXL1[0]->GetBinContent(i);
    float tz = hPIXL1[2]->GetBinContent(i);
    float phi = hPIXL1[0]->GetBinCenter(i);
    x0 = ArcPIXL1->GetR1() * cos(phi);
    y0 = ArcPIXL1->GetR1() * sin(phi);
    x1 = (ArcPIXL1->GetR1()+tz*scale) * cos(phi); x1+=(tx*scale)*cos(phi+Pi/2); 
    y1 = (ArcPIXL1->GetR1()+tz*scale) * sin(phi); x1+=(tx*scale)*sin(phi+Pi/2); 
    Fletxa->SetLineColor(2);
    Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
  }
  // PixL2
  for (int i=1; i<=hPIXL2[0]->GetNbinsX(); i++) {
    float tx = hPIXL2[0]->GetBinContent(i);
    float tz = hPIXL2[2]->GetBinContent(i);
    float phi = hPIXL2[0]->GetBinCenter(i);
    x0 = ArcPIXL2->GetR1() * cos(phi);
    y0 = ArcPIXL2->GetR1() * sin(phi);
    x1 = (ArcPIXL2->GetR1()+tz*scale) * cos(phi); x1+=(tx*scale)*cos(phi+Pi/2); 
    y1 = (ArcPIXL2->GetR1()+tz*scale) * sin(phi); x1+=(tx*scale)*sin(phi+Pi/2); 
    Fletxa->SetLineColor(3);
    Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
  }
  // SCTL0
  for (int i=1; i<=hSCTL0[0]->GetNbinsX(); i++) {
    float tx = hSCTL0[0]->GetBinContent(i);
    float tz = hSCTL0[2]->GetBinContent(i);
    float phi = hSCTL0[0]->GetBinCenter(i);
    x0 = ArcSCTL0->GetR1() * cos(phi);
    y0 = ArcSCTL0->GetR1() * sin(phi);
    x1 = (ArcSCTL0->GetR1()+tz*scale) * cos(phi); x1+=(tx*scale)*cos(phi+Pi/2); 
    y1 = (ArcSCTL0->GetR1()+tz*scale) * sin(phi); x1+=(tx*scale)*sin(phi+Pi/2); 
    Fletxa->SetLineColor(Color_SCTL0);
    Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
  }
  // SCTL1
  for (int i=1; i<=hSCTL1[0]->GetNbinsX(); i++) {
    float tx = hSCTL1[0]->GetBinContent(i);
    float tz = hSCTL1[2]->GetBinContent(i);
    float phi = hSCTL1[0]->GetBinCenter(i);
    x0 = ArcSCTL1->GetR1() * cos(phi);
    y0 = ArcSCTL1->GetR1() * sin(phi);
    x1 = (ArcSCTL1->GetR1()+tz*scale) * cos(phi); x1+=(tx*scale)*cos(phi+Pi/2); 
    y1 = (ArcSCTL1->GetR1()+tz*scale) * sin(phi); x1+=(tx*scale)*sin(phi+Pi/2); 
    Fletxa->SetLineColor(Color_SCTL1);
    Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
  }
  // SCTL0
  for (int i=1; i<=hSCTL2[0]->GetNbinsX(); i++) {
    float tx = hSCTL2[0]->GetBinContent(i);
    float tz = hSCTL2[2]->GetBinContent(i);
    float phi = hSCTL2[0]->GetBinCenter(i);
    x0 = ArcSCTL2->GetR1() * cos(phi);
    y0 = ArcSCTL2->GetR1() * sin(phi);
    x1 = (ArcSCTL2->GetR1()+tz*scale) * cos(phi); x1+=(tx*scale)*cos(phi+Pi/2); 
    y1 = (ArcSCTL2->GetR1()+tz*scale) * sin(phi); x1+=(tx*scale)*sin(phi+Pi/2); 
    Fletxa->SetLineColor(Color_SCTL2);
    Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
  }
  // SCTL0
  for (int i=1; i<=hSCTL3[0]->GetNbinsX(); i++) {
    float tx = hSCTL3[0]->GetBinContent(i);
    float tz = hSCTL3[2]->GetBinContent(i);
    float phi = hSCTL3[0]->GetBinCenter(i);
    x0 = ArcSCTL3->GetR1() * cos(phi);
    y0 = ArcSCTL3->GetR1() * sin(phi);
    x1 = (ArcSCTL3->GetR1()+tz*scale) * cos(phi); x1+=(tx*scale)*cos(phi+Pi/2); 
    y1 = (ArcSCTL3->GetR1()+tz*scale) * sin(phi); x1+=(tx*scale)*sin(phi+Pi/2); 
    Fletxa->SetLineColor(Color_SCTL3);
    Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
  }

  // i ara cal pintar la referencia
  x0 = 400;
  y0 = 450;
  x1 = x0+1*scale;
  y1 = y0;
  Fletxa->SetLineColor(kBlack);
  Fletxa->DrawArrow(x0,y0,x1,y1,0.002);
  Rotllo->DrawText(390,460,"1 mm");
}
