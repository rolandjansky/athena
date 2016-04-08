/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PIXStave(char nname[80]=NULL,Int_t layer=0, Int_t sector=0,Bool_t DrawTz=false)
{
  gStyle->SetCanvasColor(kWhite);  

  Int_t DetecType = 0;
  enum {PIX=1, SCT=2};

  if (nname == NULL) sprintf(nname,"jacobanaOutput.root");

  TFile* f0 = new TFile(nname);

  Float_t RzMax = 3.5;
  Float_t RzMin = -RzMax;
 
  Float_t TxScaleFactor = 10;

  Float_t TxMin = RzMin * 1000/TxScaleFactor;  // 1000: from mm to microns
  Float_t TxMax = RzMax * 1000/TxScaleFactor;

  // sort out the layer
  if (layer<0) layer = 0;
  if (layer>6) layer = 6;
 
  // layers: 0, 1 & 2 are pixels
  // layers: 3, 4, 5 & 6 are SCT
  if (layer <=2) {
    DetecType = PIX;
  } 
  else {
    DetecType = SCT;
  }

  // sort out the stave for pixels
  if (sector<0) sector = 0;
  if (layer==0 && sector>21) sector = 21;
  if (layer==1 && sector>37) sector = 37;
  if (layer==2 && sector>51) sector = 51;
  
  // sort out the stave for the SCT
  if (layer==3 && sector>31) sector = 31;
  if (layer==4 && sector>39) sector = 39;
  if (layer==5 && sector>47) sector = 47;
  if (layer==6 && sector>51) sector = 51;

  Float_t xModOffset;
  if (DetecType==PIX) {
    xModOffset = (layer>0? 286:0) + (layer>1? 494:0) + (layer>2? 676:0);  // for pixels
  }
  if (DetecType==SCT) {
    xModOffset = 1456;  // for pixels 
    xModOffset += (layer>3? 384:0) + (layer>4? 480:0) + (layer>5? 576:0); 
  }


  // identifiers of the first and last module
  Float_t xModMin;
  Float_t xModMax;
  if (DetecType == PIX) {
    xModMin = xModOffset+13*sector+1;
    xModMax = xModMin+12;  
  }
  if (DetecType == SCT) {
    xModMin = xModOffset+12*sector+1;
    xModMax = xModMin+11;  
  }

  // colorins
  Int_t RzColor = kAzure-2;
  Int_t TxColor = kRed-4;
  Int_t TzColor = kOrange-4;

  cout << " Drawing modules in layer " << layer 
       << " and sector " << sector 
       << " ( " << xModMin << " --> " << xModMax << " ) " << endl;

  TH1F* hRzAliCorr = (TH1F*)f0->Get("jacobana/hRzAliCorr")->Clone();
  hRzAliCorr->SetStats(0);
  hRzAliCorr->GetYaxis()->SetLabelColor(RzColor);
  hRzAliCorr->GetYaxis()->SetTitleColor(RzColor);
  hRzAliCorr->GetYaxis()->SetAxisColor(RzColor);
  hRzAliCorr->GetYaxis()->SetRangeUser(RzMin,RzMax);
  hRzAliCorr->SetLineColor(RzColor);
  hRzAliCorr->SetMarkerColor(RzColor);
  hRzAliCorr->SetMarkerStyle(20);
  hRzAliCorr->GetXaxis()->SetRange(xModMin,xModMax);
  hRzAliCorr->SetXTitle("#eta module id");
  hRzAliCorr->SetLineWidth(2);
  

  char htitle[80];
  if (DetecType == PIX) sprintf(htitle,"PIX modules in layer %d stave %d", layer, sector);
  if (DetecType == SCT) sprintf(htitle,"SCT modules in layer %d stave %d", layer-3, sector); // remind: 3 pix layers
  hRzAliCorr->SetTitle(htitle);

  //x axis
  Int_t BinVal = -6;
  char BinLabel[6];
  for (int bin=xModMin; bin<=xModMax; bin++) {
    sprintf(BinLabel,"%d",BinVal);
    hRzAliCorr->GetXaxis()->SetBinLabel(bin,BinLabel);
    BinVal++;
    if (DetecType==SCT && BinVal==0) BinVal=1; // remind SCT has no ring 0
  }


  hRzAliCorr->Draw();

  // Ara traguem el Tx
  TH1F* hTxAliCorr = (TH1F*)f0->Get("jacobana/hTxAliCorr")->Clone();
  hTxAliCorr->SetLineColor(TxColor);
  hTxAliCorr->SetMarkerColor(TxColor);
  hTxAliCorr->Scale(TxScaleFactor);
  hTxAliCorr->SetLineWidth(2);
  hTxAliCorr->SetMarkerStyle(20);
  hTxAliCorr->Draw("same");

  Float_t rightmax = 1.1*hTxAliCorr->GetMaximum();

  TGaxis *axis = new TGaxis(xModMax+0.5,
			    RzMin,xModMax+0.5,RzMax,
			    TxMin, TxMax,510,"+L");
  
  axis->SetTitle("Tx (#mu m)");
  if (DrawTz)   axis->SetTitle("Tx & Tz (#mu m)");
  axis->SetTitleColor(TxColor);
  axis->SetLineColor(TxColor);
  axis->SetLabelColor(TxColor);
  axis->Draw();

  gPad->SetGridy(1);

  // Ara traguem el Tz
  if (DrawTz) {
    TH1F* hTzAliCorr = (TH1F*)f0->Get("jacobana/hTzAliCorr")->Clone();
    hTzAliCorr->SetLineColor(TzColor);
    hTzAliCorr->SetMarkerColor(TzColor);
    hTzAliCorr->Scale(TxScaleFactor);
    hTzAliCorr->SetLineWidth(2);
    hTzAliCorr->SetMarkerStyle(20);
    hTzAliCorr->Draw("same");
  }

  // legend
  if (DrawTz) {
    TLegend *Rotllo = new TLegend(0.77, 0.75, 0.86, 0.88);
    Rotllo->SetFillStyle(1001);
    Rotllo->SetFillColor(10);
    Rotllo->AddEntry(hTxAliCorr,"Tx","p");
    Rotllo->AddEntry(hTzAliCorr,"Tz","p");
    Rotllo->AddEntry(hRzAliCorr,"Rz","p");
    Rotllo->Draw();    
  }
}
