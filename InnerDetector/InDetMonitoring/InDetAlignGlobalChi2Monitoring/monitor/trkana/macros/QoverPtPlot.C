/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//

TH2F *HD;
int level;

//
void QoverPtPlot(char nname[80], int nivell = 3,char* rotllo="")
{
  level = nivell;
  
  Set_style();
  
  TFile *f0 = new TFile(nname);
  char nameh[80];


  // histograma de Q/pt vs phi
  sprintf(nameh,"trkana/Track_Parameters/hRecoQoverPt_RecoPhi0_%d",level);
  HD = (TH2F*)f0->Get(nameh);   
  fa_la_faena(HD,"QoverPt_Phi",5,20,rotllo);

  // histograma de Q/pt vs eta
  sprintf(nameh,"trkana/Track_Parameters/hRecoQoverPt_RecoEta_%d",level);
  HD = (TH2F*)f0->Get(nameh);   
  fa_la_faena(HD,"QoverPt_Eta",515,20,rotllo);

  // histograma de Pt vs phi
  sprintf(nameh,"trkana/Track_Parameters/hRecoPt_RecoPhi0_%d",level);
  HD = (TH2F*)f0->Get(nameh);   
  fa_la_faena(HD,"RecoPt_Phi",5,510,rotllo);

  // histograma de Q/pt vs eta
  sprintf(nameh,"trkana/Track_Parameters/hRecoPt_RecoEta_%d",level);
  HD = (TH2F*)f0->Get(nameh);   
  fa_la_faena(HD,"RecoPt_Eta",515,515,rotllo);
}

//////////////////////////////////////////////////
void Set_style()
{
  // style
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(0);
  //gStyle->SetOptFit(10);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(10);
  //  gStyle->SetStatFontSize(0);
  gStyle->SetStatColor(0);
  gStyle->SetStatW(0.22);
  //gStyle->SetStatH(0.22);
}

//////////////////////////////////////////////////
void pinta_m()
{
  HD->Draw("colz");
  HD->GetYaxis()->SetAxisColor(2);
  HD->GetXaxis()->SetAxisColor(2);
  HD->GetYaxis()->SetLabelSize(0.03);
  HD->GetYaxis()->SetTitleSize(0.03);
  HD->GetYaxis()->SetTitleOffset(1.5);
  HD->GetXaxis()->SetLabelSize(0.03);
  HD->GetZaxis()->SetLabelSize(0.022);
}

//////////////////////////////////////////////////
void fa_la_faena(TH2F* Histo, char* nomC="myCanvas", int XC=0, int YC=0, char* rotllo="")
{
  TCanvas *CCanv = new TCanvas(nomC,nomC,XC,YC,500,500);
  pinta_m();
  CCanv->SetGridx();
  CCanv->SetGridy();

  cout << " Rotllo :<" << rotllo <<">" << endl;
  if (strcmp(rotllo,"")>0) {
    TPaveLabel* PRotllo = new TPaveLabel(0.6, 0.2, 0.8, 0.3, rotllo, "NDC");
    PRotllo->Draw();
  }

  char GifFile[80];
  sprintf(GifFile,"%sSel%d.gif",nomC,level);


  CCanv->Print(GifFile);
}
