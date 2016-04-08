/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

typedef struct VPeak{
  int canal;
  double valor;
  double error;
};

double QoverPt_fitfun(double *x, double *par);

//
void QoverPtAna(char nname[80], int level=2, int tipus=1, char* rotllo="")
{
  Set_style();
  
  TFile *f0 = new TFile(nname);
  TH2F *HD;
  char nameh[80];

  cout << " --> tipus " << tipus << endl;
  cout << " --> level " << level << endl;

  switch (tipus) {
  case 1:
    sprintf(nameh,"trkana/Track_Parameters/hRecoQoverPt_RecoPhi0_%d",level);
    break;
  case 2:
    sprintf(nameh,"trkana/Track_Parameters/hRecoPt_RecoPhi0_%d",level);
    break;
  case 3:
    sprintf(nameh,"trkana/Track_Parameters/hRecoQoverPt_RecoEta_%d",level);
    break;
  case 4:
    sprintf(nameh,"trkana/Track_Parameters/hRecoPt_RecoEta_%d",level);
    break;
  default:
    sprintf(nameh,"trkana/Track_Parameters/hRecoQoverPt_RecoPhi0_%d",level);
    break;
  }

  cout << " --> Pintant " << nameh << endl;

  // sprintf(nameh,"trkana/hRecoQoverPt_RecoPhi0_FixPt%d",level);

  // pren el fitxer
  HD = (TH2F*)f0->Get(nameh); 
  
  // obri canvas
  TCanvas *CQoverPt = new TCanvas("CQoverPt","QoverPt",585,585);
  HD->Draw("colz");
  HD->GetYaxis()->SetAxisColor(2);
  HD->GetXaxis()->SetAxisColor(2);
  HD->GetYaxis()->SetTitleSize(0.03);
  HD->GetYaxis()->SetTitleOffset(1.5);
  HD->GetXaxis()->SetLabelSize(0.025);
  HD->GetYaxis()->SetLabelSize(0.025);
  HD->GetZaxis()->SetLabelSize(0.025);
  CQoverPt->SetGridx();
  CQoverPt->SetGridy();

  cout << " Rotllo :<" << rotllo <<">" << endl;
  if (strcmp(rotllo,"")>0) {
    TPaveLabel* PRotllo = new TPaveLabel(0.6, 0.2, 0.8, 0.3, rotllo, "NDC");
    PRotllo->Draw();
  }

  CQoverPt->Print("QoverPt.gif");

  // l'analisi sols es fa per a q/pt vs phi
  if (tipus != 1) return;

  // fins aci ja tenim pintat el plot de Q/pt 
  int cx = CQoverPt->GetWindowTopX();
  int cy = CQoverPt->GetWindowTopY();
  int wx = CQoverPt->GetWindowWidth();
  int wy = CQoverPt->GetWindowHeight();
  
  TCanvas *CQCumul = new TCanvas("CQCumul","Q cummlative",cx-5,cy+wy+4,wx,wy/1.8);

  int NBinsX = HD->GetNbinsX();  
  int NBinsY = HD->GetNbinsY();  

  // en Y la mitat de dalt son per a Q positius,
  // mentres que  la mitat de baix per a Q negatius
  int NBinsTemp;
  if (NBinsY%2==0) NBinsTemp = NBinsY/2;
  if (NBinsY%2==1) NBinsTemp = NBinsY/2+1;
  const int NBinsQoPt = NBinsTemp;
  

  // creem les pojeccions en Y
  TH1D* QoverPtPlusProjX[NBinsQoPt+1];
  TH1D* QoverPtMinusProjX[NBinsQoPt+1];

  // histograma on integrem les carregues positives
  TH1F* hEntriesQPlus= new TH1F("hEnQPlus","Entries per q/pt bin", NBinsQoPt,
				0., HD->GetYaxis()->GetXmax());
  hEntriesQPlus->SetLineColor(4);
  hEntriesQPlus->SetXTitle("Q/Pt (GeV/c)^{-1}");
  hEntriesQPlus->SetYTitle("# tracks");
  hEntriesQPlus->SetMinimum(0);

  // histograma on integrem les carregues negatives
  TH1F* hEntriesQMinus = new TH1F("hEnQMinus","Entries per q/pt bin", NBinsQoPt,
				0., HD->GetYaxis()->GetXmax());
  hEntriesQMinus->SetLineColor(2);
  hEntriesQMinus->SetXTitle("Q/Pt (GeV/c)^{-1}");
  hEntriesQMinus->SetYTitle("# tracks");
  hEntriesQMinus->SetMinimum(0);

  cout << " ** QoverPtAna ** projection histograms created.... going to fill them... good luck ! " << endl;

  // Primer traguem les distribucions cumulatives per a particules amb 
  // carrega positiva i carrega negativa
  //
  char Apodo[80];
  // a vore la simetria de entrades entre positives i negatives
  for (int bin=1; bin<=NBinsQoPt; bin++) {
    int binPlus = NBinsY - bin +1;
    int binMinus = bin;
    
    cout << "   -- bin " << bin << "   bin-iu " << binMinus << "  bin+iu " << binPlus << endl;

    sprintf(Apodo,"Projeccio de Q/Pt +iu per al canal %d",binPlus);
    QoverPtPlusProjX[bin] = HD->ProjectionX(Apodo,binPlus,binPlus);
    QoverPtPlusProjX[bin]->Draw();


    sprintf(Apodo,"Projeccio de Q/Pt -iu per al canal %d",binMinus);
    QoverPtMinusProjX[bin] = HD->ProjectionX(Apodo,binMinus,binMinus);

    //    CAna->cd(bin+1);
    QoverPtPlusProjX[bin]->SetLineColor(4);
    QoverPtPlusProjX[bin]->SetMinimum(0);
    QoverPtPlusProjX[bin]->Draw();
    QoverPtMinusProjX[bin]->SetLineColor(2);
    QoverPtMinusProjX[bin]->Draw("same");

    int NenPlus = (int) QoverPtPlusProjX[bin]->GetEntries();
    int NenMinus = (int) QoverPtMinusProjX[bin]->GetEntries();
     
    int binCorr = NBinsQoPt - bin +1 ; 

    hEntriesQPlus->SetBinContent(binCorr,NenPlus);
    hEntriesQMinus->SetBinContent(binCorr,NenMinus);
  }

  cout << " ** QoverPtAna ** projection histograms filled. Well done ! " << endl;
  CQCumul->cd();   
  hEntriesQPlus->Draw();
  hEntriesQMinus->Draw("same");
  CQCumul->Update(); 

  //
  // ara anem a integrar-los
  //
  TH1F* hIntQPlus= new TH1F("hIntQPlus","Intgral de q/pt (pos)", NBinsQoPt,
				0., HD->GetYaxis()->GetXmax());
  TH1F* hIntQMinus= new TH1F("hIntQMinus","Intgral de q/pt (neg)", NBinsQoPt,
				0., HD->GetYaxis()->GetXmax());
  float SumaPlus=0;
  float SumaMinus=0;
  for (int bin=1; bin<=NBinsQoPt; bin++){
    SumaPlus += hEntriesQPlus->GetBinContent(bin); 
    hIntQPlus->SetBinContent(bin,SumaPlus);
    SumaMinus += hEntriesQMinus->GetBinContent(bin); 
    hIntQMinus->SetBinContent(bin,SumaMinus);
  }
  // i anem a pintar-ho en el mateix histo


  float rmax = (hIntQPlus->GetMaximum() > hIntQMinus->GetMaximum()) ? hIntQPlus->GetMaximum() : hIntQMinus->GetMaximum();
  float Fscala = 0.95*gPad->GetUymax()/rmax;
  cout << " RMAX " << rmax << endl;
  cout << " FSCALA " << Fscala << endl;
  hIntQPlus->SetLineColor(214);
  hIntQMinus->SetLineColor(102);
  hIntQPlus->Scale(Fscala);
  hIntQMinus->Scale(Fscala);
  hIntQPlus->Draw("same");
  hIntQMinus->Draw("same");

  TLegend *TRotllo = new TLegend(0.65,0.6,0.8,0.8,"","NDC");
  TRotllo->AddEntry(hEntriesQPlus,"q>0","l");
  TRotllo->AddEntry(hEntriesQMinus,"q<0","l");
  TRotllo->Draw();

  double ResKolmoTest = hIntQPlus->KolmogorovTest(hIntQMinus,"M");
    
  cout << " Resultat test de Kolmogorov " << ResKolmoTest << endl;

  // i finalment pintem l'eix a la dreta
  TGaxis *IntAxis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
			       gPad->GetUxmax(), gPad->GetUymax(),
			       0, rmax, 510, "+L");
  IntAxis->Draw();
  
  CQCumul->Print("QoPt_assym.gif");

  //--------------------------
  // Ara anem a fer talls en Pt
  const int NBinsPhi = NBinsX;
  TCanvas *CTest = new TCanvas("CTest","Test",cx+wx+4,cy+wy+4,wx,wy/1.8);

  TH1D* QoverPtProjY[NBinsPhi];

  TH1F* hQoverPtPlusPeak= new TH1F("hQoverPtPlusPeak","Peak of Q/Pt", NBinsPhi,
				HD->GetXaxis()->GetXmin(), HD->GetXaxis()->GetXmax());
  hQoverPtPlusPeak->SetLineColor(4);
  hQoverPtPlusPeak->SetXTitle(HD->GetXaxis()->GetTitle());
  hQoverPtPlusPeak->SetYTitle(HD->GetYaxis()->GetTitle());

  TH1F* hQoverPtMinusPeak= new TH1F("hQoverPtMinusPeak","Peak of Q/Pt", NBinsPhi,
				HD->GetXaxis()->GetXmin(), HD->GetXaxis()->GetXmax());
  hQoverPtMinusPeak->SetLineColor(2);
  hQoverPtMinusPeak->SetXTitle(HD->GetXaxis()->GetTitle());
  hQoverPtMinusPeak->SetYTitle(HD->GetYaxis()->GetTitle());

  // rang per a dibuixar
  float VYmax = 0.160;
  float VYmin = 0.000;
  hQoverPtPlusPeak->SetMaximum(VYmax);
  hQoverPtPlusPeak->SetMinimum(VYmin);
  hQoverPtMinusPeak->SetMaximum(-VYmin);
  hQoverPtMinusPeak->SetMinimum(-VYmax);


  VPeak PeakPlus, PeakMinus;

  cout << " ** QoverPtAna ** Computing positive and negative peak positions ..." << endl;
  for (int bin=0; bin<NBinsPhi; bin++) {
    //for (int bin=0; bin<=0; bin++) {
    int hbin = bin+1;
    sprintf(Apodo,"Projeccio Y de Q/Pt + per al canal %d  phi= %f",hbin, HD->GetXaxis()->GetBinCenter(hbin));

    cout << endl << Apodo << endl <<endl;

    QoverPtProjY[bin] = HD->ProjectionY(Apodo,hbin,hbin);
    QoverPtProjY[bin]->Draw(); 
    
    PeakPlus = Trau_picPlus(QoverPtProjY[bin]);
    // return;
    PeakMinus = Trau_picMinus(QoverPtProjY[bin]);

    hQoverPtPlusPeak->SetBinContent(hbin, PeakPlus.valor);
    hQoverPtPlusPeak->SetBinError(hbin, PeakPlus.error);

    hQoverPtMinusPeak->SetBinContent(hbin, PeakMinus.valor);
    hQoverPtMinusPeak->SetBinError(hbin, PeakMinus.error);
  }
  cout << " ** QoverPtAna ** positive and negative peak positions computed and histograms filled " << endl;

  
  // definim la funcio a ajustar
  TF1 *QoPtFun = new TF1 ("QoPtFun", QoverPt_fitfun, 0., 0., 3);
  QoPtFun->SetLineColor(1);
  QoPtFun->SetParameters(0.05, 0., 0.);
  QoPtFun->SetParName(0,"QoPt_0");
  QoPtFun->SetParName(1,"SagEff");
  QoPtFun->SetParName(2,"Phi_0");

  // 
  TCanvas *CAna = new TCanvas("AnaCanvas","AnaCanvas",cx+wx+4,cy-22,wx,wy);
  //  CQoverPt->SetGridx();
  CAna->Divide(1,2);
  CAna->cd(1);
  gPad->SetGridy();
  hQoverPtPlusPeak->Fit("QoPtFun");


  // pinta resultat de l'ajust:
  TText *ResPlus = new TText();
  ResPlus->SetNDC();
  char Rotllo[80];
  float TextY = 0.85;
  float TDeltaY = -0.08;
  sprintf(Rotllo,"Q/Pt0= %6.4f +- %6.4f",QoPtFun->GetParameter(0),QoPtFun->GetParError(0));
  ResPlus->DrawText(0.6, TextY, Rotllo);
  sprintf(Rotllo,"SagEff= %6.4f +- %6.4f",fabs(QoPtFun->GetParameter(1)),QoPtFun->GetParError(1));
  TextY += TDeltaY;
  ResPlus->DrawText(0.6, TextY, Rotllo);
  sprintf(Rotllo,"phi00= %5.2f +- %5.2f",QoPtFun->GetParameter(2),QoPtFun->GetParError(2));
  TextY += TDeltaY;
  ResPlus->DrawText(0.6, TextY, Rotllo);

  cout << " Rotllo :<" << rotllo <<">" << endl;
  if (strcmp(rotllo,"")>0) {
    TPaveLabel* PRotllo = new TPaveLabel(0.2, 0.85, 0.35, 0.95, rotllo, "NDC");
    PRotllo->Draw();
  }


  CAna->cd(2);
  gPad->SetGridy();
  hQoverPtMinusPeak->Draw();
  hQoverPtMinusPeak->Fit("QoPtFun");
  TextY = 0.85;
  sprintf(Rotllo,"Q/Pt0= %6.4f +- %6.4f",-QoPtFun->GetParameter(0),QoPtFun->GetParError(0));
  ResPlus->DrawText(0.6, TextY, Rotllo);
  sprintf(Rotllo,"SagEff= %6.4f +- %6.4f",fabs(QoPtFun->GetParameter(1)),QoPtFun->GetParError(1));
  TextY += TDeltaY;
  ResPlus->DrawText(0.6, TextY, Rotllo);
  sprintf(Rotllo,"phi00= %5.2f +- %5.2f",QoPtFun->GetParameter(2),QoPtFun->GetParError(2));
  TextY += TDeltaY;
  ResPlus->DrawText(0.6, TextY, Rotllo);

  cout << " Rotllo :<" << rotllo <<">" << endl;
  if (strcmp(rotllo,"")>0) {
    TPaveLabel* PRotllo = new TPaveLabel(0.2, 0.85, 0.35, 0.95, rotllo, "NDC");
    PRotllo->Draw();
  }

  AnaCanvas->Print("QoPT_Ajust.gif");
}

//////////////////////////////////////////////////
double QoverPt_fitfun(double *x, double *par)
{
  double qopt;

  double qopt_0 = par[0];
  double SagEff = par[1];
  double Phi00 = par[2];

  qopt = qopt_0 + SagEff * sin(x[0]+Phi00);

  return qopt;
}

//////////////////////////////////////////////////
VPeak Trau_picPlus(TH1D* hproj)
{
  VPeak AquestPic;
 
  cout << " En Trau_picPlus ...." << endl;

  // Init values
  AquestPic.valor = 0.;
  AquestPic.error = 0.;

  TH1D* hcopia = new TH1D("hcopia","hcopia",hproj->GetNbinsX(),hproj->GetXaxis()->GetXmin(), hproj->GetXaxis()->GetXmax());  
  for (int bin=1; bin<=hproj->GetNbinsX(); bin++) {
    if (hproj->GetBinCenter(bin)>0. || hproj->GetBinLowEdge(bin)>0.)
      hcopia->SetBinContent(bin,hproj->GetBinContent(bin));
  }

  AquestPic.canal = hcopia->GetMaximumBin();
  AquestPic.valor = hcopia->GetBinCenter(AquestPic.canal); 

  bool Content = false;
  if (AquestPic.valor>0.) Content = true;

  int Contador = 0;

  while (!Content) {
    hcopia->SetBinContent(AquestPic.canal, 0.);
    AquestPic.canal = hcopia->GetMaximumBin();
    AquestPic.valor = hcopia->GetBinCenter(AquestPic.canal); 
    if (AquestPic.valor>0) Content = true;
    Contador++;
    if (Contador==1000) return AquestPic;  // per a si no trova res... s'en eisca
  }

  for (int bin=1; bin<=hcopia->GetNbinsX(); bin++) {
    if (abs(bin-AquestPic.canal)>2) hcopia->SetBinContent(bin,0.);
  }
    
  AquestPic.valor = hcopia->GetMean(); // cout << " valor nou " << AquestPic.valor << endl;
  AquestPic.error = hcopia->GetRMS();

  cout << " init values: mean : " <<  AquestPic.valor << endl;
  cout << "              sigma: " <<  AquestPic.error << endl;

  hcopia->Fit("gaus");
  hcopia->GetFunction("gaus")->SetParameter(1,AquestPic.valor);
  hcopia->GetFunction("gaus")->SetParameter(1,AquestPic.error);

  hcopia->Fit("gaus");
 
  AquestPic.valor = hcopia->GetFunction("gaus")->GetParameter(1);
  AquestPic.error = 2*hcopia->GetFunction("gaus")->GetParError(1);
  
  cout << "=================== final ================0" << endl;
  hcopia->Draw();

  AquestPic.valor = hcopia->GetBinCenter(AquestPic.canal); cout << " valor nou " << AquestPic.valor << endl;
  AquestPic.error = hcopia->GetBinWidth(AquestPic.canal);
  
  // delete hcopia;

  return AquestPic;
}
//////////////////////////////////////////////////
VPeak Trau_picMinus(TH1D* hproj)
{
  VPeak AquestPic;
 
  TH1D* hcopia = new TH1D("hcopia","hcopia",hproj->GetNbinsX(),hproj->GetXaxis()->GetXmin(), hproj->GetXaxis()->GetXmax());  
  for (int bin=1; bin<=hproj->GetNbinsX(); bin++) {
    hcopia->SetBinContent(bin,hproj->GetBinContent(bin));
  }

  hcopia->DrawCopy();

  AquestPic.canal = hcopia->GetMaximumBin();
  AquestPic.valor = hcopia->GetBinCenter(AquestPic.canal); 

  bool Content = false;
  if (AquestPic.valor<0.) Content = true;

  while (!Content) {
    hcopia->SetBinContent(AquestPic.canal, 0.);
    AquestPic.canal = hcopia->GetMaximumBin();
    AquestPic.valor = hcopia->GetBinCenter(AquestPic.canal); 
    if (AquestPic.valor<0) Content = true;
  }

  for (int bin=1; bin<=hproj->GetNbinsX(); bin++) {
    if (abs(bin-AquestPic.canal)>1) hcopia->SetBinContent(bin,0.);
  }
    
  AquestPic.valor = hcopia->GetMean(); // cout << " valor nou " << AquestPic.valor << endl;
  AquestPic.error = hcopia->GetRMS();

  cout << " init values: mean : " <<  AquestPic.valor << endl;
  cout << "              sigma: " <<  AquestPic.error << endl;

  hcopia->Fit("gaus");
  hcopia->GetFunction("gaus")->SetParameter(1,AquestPic.valor);
  hcopia->GetFunction("gaus")->SetParameter(1,AquestPic.error);

  hcopia->Fit("gaus");
 
  AquestPic.valor = hcopia->GetFunction("gaus")->GetParameter(1);
  AquestPic.error = 2*hcopia->GetFunction("gaus")->GetParError(1);
  
  cout << "=================== final ================0" << endl;
  hcopia->Draw();
  AquestPic.valor = hcopia->GetBinCenter(AquestPic.canal); cout << " valor nou " << AquestPic.valor << endl;
  AquestPic.error = hcopia->GetBinWidth(AquestPic.canal);

  delete hcopia;

  return AquestPic;
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
