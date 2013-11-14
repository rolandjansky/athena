/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Fakes.h"
#include "TH2.h"
#include "TProfile.h"
#include "TF1.h"
#include "TDatime.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "math.h"
#include "TMath.h"
#include <iostream>
#include "TPostScript.h"
#include "TPaveLabel.h"
#include "TPaveText.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TGraphErrors.h"


const int nzon = 3;
const float etazon[nzon+2] = { 0.0, 1.0, 2.0, 2.7, 9.9 };

TH1F *simuVSphi[10][nzon];
TH1F *fakeVSphi[10][nzon];
TH1F *chistaMuon, *chistaFake, *chidofMuon, *chidofFake;
TH1F *chicanMuon, *chicanFake, *chifitMuon, *chifitFake, *chiverMuon, *chiverFake;
TH2F *chifitVsdofMuon, *chifitVsdofFake;
TH1F *erphipMB, *ertetpMB, *erphivMB, *ertetvMB, *errqspMB;
TH1F *erphipST, *ertetpST, *erphivST, *ertetvST, *errqspST;
TH1F *erred0IP, *errez0IP, *errphiIP, *errtetIP, *errqspIP;

Fakes::Fakes():Analysis() {

  m_nevt = 0;
  m_Nassoc = 3;
  p_assoc[0] = p_assocMB; m_sing[0] = 0;
  p_assoc[1] = p_assocST; m_sing[1] = 0;
  p_assoc[2] = p_assocMS; m_sing[2] = 0;
}


Fakes::~Fakes() {}

void Fakes::initialize(char* option) {
  printf("Starting Fakes\n");
  for (int i=0; i<m_Nassoc; i++) bookHistos(i);
  chistaMuon = new TH1F("chistaMuon", "chistaMuon", 100, 0., 1000.);
  chistaFake = new TH1F("chistaFake", "chistaFake", 100, 0., 1000.);
  chidofMuon = new TH1F("chidofMuon", "chidofMuon", 100, 0., 50.);
  chidofFake = new TH1F("chidofFake", "chidofFake", 100, 0., 50.);

  chicanMuon = new TH1F("chicanMuon", "chicanMuon", 100, 0., 500.);
  chicanFake = new TH1F("chicanFake", "chicanFake", 100, 0., 500.);
  chifitMuon = new TH1F("chifitMuon", "chifitMuon", 100, 0., 200.);
  chifitFake = new TH1F("chifitFake", "chifitFake", 100, 0., 200.);
  chiverMuon = new TH1F("chiverMuon", "chiverMuon", 100, 0., 100.);
  chiverFake = new TH1F("chiverFake", "chiverFake", 100, 0., 100.);

  erphipMB = new TH1F("erphipMB", "erphipMB", 100, -6., 2.);
  ertetpMB = new TH1F("ertetpMB", "ertetpMB", 100, -6., 2.);
  erphivMB = new TH1F("erphivMB", "erphivMB", 100, -6., 2.);
  ertetvMB = new TH1F("ertetvMB", "ertetvMB", 100, -6., 2.);
  errqspMB = new TH1F("errqspMB", "errqspMB", 100, -8., 0.);
  erphipST = new TH1F("erphipST", "erphipST", 100, -6., 2.);
  ertetpST = new TH1F("ertetpST", "ertetpST", 100, -6., 2.);
  erphivST = new TH1F("erphivST", "erphivST", 100, -6., 2.);
  ertetvST = new TH1F("ertetvST", "ertetvST", 100, -6., 2.);
  errqspST = new TH1F("errqspST", "errqspST", 100, -8., 0.);

  erred0IP = new TH1F("erred0IP", "erred0IP", 100, -2., 6.);
  errez0IP = new TH1F("errez0IP", "errez0IP", 100, -2., 6.);
  errphiIP = new TH1F("errphiIP", "errphiIP", 100, -5., 3.);
  errtetIP = new TH1F("errtetIP", "errtetIP", 100, -6., 2.);
  errqspIP = new TH1F("errqspIP", "errqspIP", 100, -8., 0.);

  chifitVsdofMuon = new TH2F("chifitVsdofMuon", "chifitVsdofMuon", 40,0.,80., 50,0.,200);
  chifitVsdofFake = new TH2F("chifitVsdofFake", "chifitVsdofFake", 40,0.,80., 50,0.,200);
}


void Fakes::execute() {
  for (int i=0; i<m_Nassoc; i++) checkAssoc(i);
  for (int i=0; i<m_Nassoc; i++) studyFakes(i);
  m_nevt++;
  Assoc* asso = p_assocMS;
  for (int irec=0; irec<asso->getNrec(); irec++) {
    float chi2 = (p_evt->MboyTrkMS_Chistat)->at(irec);
    if(     asso->isFake(irec))        chistaFake->Fill(chi2);
    else if(asso->isMuon(irec))        chistaMuon->Fill(chi2);
    chi2 = chi2 / (p_evt->MboyTrkMS_Ndof)->at(irec);
    if(     asso->isFake(irec))        chidofFake->Fill(chi2);
    else if(asso->isMuon(irec))        chidofMuon->Fill(chi2);
    chi2 = (p_evt->MboyTrkMS_Chican)->at(irec);
    if(     asso->isFake(irec))        chicanFake->Fill(chi2);
    else if(asso->isMuon(irec))        chicanMuon->Fill(chi2);
    chi2 = (p_evt->MboyTrkMS_Chifit)->at(irec);
    if(     asso->isFake(irec))        chifitFake->Fill(chi2);
    else if(asso->isMuon(irec))        chifitMuon->Fill(chi2);
    int ndof = (p_evt->MboyTrkMS_Ndof)->at(irec);
    if(     asso->isFake(irec))        chifitVsdofFake->Fill(ndof,chi2);
    else if(asso->isMuon(irec))        chifitVsdofMuon->Fill(ndof,chi2);
    if(     asso->isFake(irec))        chifitFake->Fill(chi2);
    else if(asso->isMuon(irec))        chifitMuon->Fill(chi2);
    chi2 = (p_evt->MboyTrkMS_Chiver)->at(irec);
    if(     asso->isFake(irec))        chiverFake->Fill(chi2);
    else if(asso->isMuon(irec))        chiverMuon->Fill(chi2);
  }
  for (int irec=0; irec<p_evt->MboyTrkMS_NTrk; irec++) {
    ertetpMB->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov11)->at(irec) ) ) );
    erphipMB->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov22)->at(irec) ) ) );
    ertetvMB->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov33)->at(irec) ) ) );
    erphivMB->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov44)->at(irec) ) ) );
    errqspMB->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov55)->at(irec) ) ) );
    for (int ista=0; ista<p_evt->staco_nmuon; ista++) {
      if(fabs(p_assocMB->getThetaRec(irec)-(p_evt->staco_ThetaMS)->at(ista))<0.00001 &&
         fabs(p_assocMB->getPhiRec(irec)-(p_evt->staco_PhiMS)->at(ista))<0.00001) {
        ertetpST->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov11)->at(irec) ) ) );
        erphipST->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov22)->at(irec) ) ) );
        ertetvST->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov33)->at(irec) ) ) );
        erphivST->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov44)->at(irec) ) ) );
        errqspST->Fill( log10( sqrt( (p_evt->MboyTrkMS_Cov55)->at(irec) ) ) );
      }
    }
    erred0IP->Fill( log10( sqrt( (p_evt->MboyESD_covr11)->at(irec) ) ) );
    errez0IP->Fill( log10( sqrt( (p_evt->MboyESD_covr22)->at(irec) ) ) );
    errphiIP->Fill( log10( sqrt( (p_evt->MboyESD_covr33)->at(irec) ) ) );
    errtetIP->Fill( log10( sqrt( (p_evt->MboyESD_covr44)->at(irec) ) ) );
    errqspIP->Fill( log10( sqrt( (p_evt->MboyESD_covr55)->at(irec) ) ) );
  }
}


void Fakes::finalize() {
  for (int i=0; i<m_Nassoc; i++) closeHistos(i);
}


void Fakes::bookHistos(int i) {
  char titre[80], nom[80];
  char* type = p_assoc[i]->m_type;
  char* lieu = p_assoc[i]->m_lieu; 

  sprintf(nom  , "pullPhiAsso%s%s", type, lieu);
  sprintf(titre, "pullPhi asso %s %s (%s)", type, lieu, m_comment);
  p_pullPhi[i] = new TH1F(nom, titre, 100, -10., 10.);  
  sprintf(nom  , "pullThetaAsso%s%s", type, lieu);
  sprintf(titre, "pullTheta asso %s %s (%s)", type, lieu, m_comment);
  p_pullTheta[i] = new TH1F(nom, titre, 100, -10., 10.); 
  sprintf(nom  , "p_pullPAsso%s%s", type, lieu);
  sprintf(titre, "p_pullP asso %s %s (%s)", type, lieu, m_comment);
  p_pullP[i] = new TH1F(nom, titre, 100, -10., 10.);

  sprintf(nom  , "Nfake%s%s", type, lieu);
  sprintf(titre, "Nfake %s %s (%s)", type, lieu, m_comment);
  p_histNfake[i] = new TH1F(nom, titre, 20, -0.5, 19.5);
  sprintf(nom  , "Naddi%s%s", type, lieu);
  sprintf(titre, "Naddi %s %s (%s)", type, lieu, m_comment);
  p_histNaddi[i] = new TH1F(nom, titre, 20, -0.5, 19.5);
  sprintf(nom  , "Ndupli%s%s", type, lieu);
  sprintf(titre, "Ndupli %s %s (%s)", type, lieu, m_comment);
  p_histNdupli[i] = new TH1F(nom, titre, 20, -0.5, 19.5);
  sprintf(nom  , "Nmuon%s%s", type, lieu);
  sprintf(titre, "Nmuon %s %s (%s)", type, lieu, m_comment);
  p_histNmuon[i] = new TH1F(nom, titre, 20, -0.5, 19.5);
  sprintf(nom  , "fakeVSmuon%s%s", type, lieu);
  sprintf(titre, "fake vs muon %s %s (%s)", type, lieu, m_comment);
  p_histFake[i] = new TH2F(nom, titre, 10,0.,10., 10,0.,10.);

  sprintf(nom  , "simuVSeta%s%s", type, lieu);
  p_simuVSeta[i] = new TH1F(nom, nom, 60, 0.0, 3.0);
  sprintf(nom  , "fakeVSeta%s%s", type, lieu);
  sprintf(titre, "fakes vs eta %s %s (%s)", type, lieu, m_comment);  
  p_fakeVSeta[i] = new TH1F(nom, titre, 60, 0.0, 3.0);
  for (int izon=0; izon<nzon; izon++) {
    sprintf(nom  , "simuVSphi%s%s%d", type, lieu, izon);
    simuVSphi[i][izon] = new TH1F(nom, nom, 32, -180., 180.);
    sprintf(nom  , "fakeVSphi%s%s%d", type, lieu, izon);
    sprintf(titre, "fakes vs phi %s %s (%s) %.1f<abs(eta)<%.1f", type, lieu, m_comment, etazon[izon], etazon[izon+1]);
    fakeVSphi[i][izon] = new TH1F(nom, titre, 32, -180., 180.);
  }
}


void Fakes::closeHistos(int i) {
  Assoc* assoc = p_assoc[i];
  TCanvas* canvas;
  bool psOuvert = false;
  char psfile[80], nom[80];
  gStyle->SetOptStat(111110);
  sprintf(nom  , "Fakes%s%s", assoc->m_type, assoc->m_lieu);

  if(m_sing[i] > 10) {
    canvas = new TCanvas(nom, nom, 300, 10, 800, 600);
    canvas->Divide(3, 2);
    canvas->cd(1); gPad->SetLogy(1); p_pullPhi[i]->Draw();
    canvas->cd(2); gPad->SetLogy(1); p_pullTheta[i]->Draw();
    canvas->cd(3); gPad->SetLogy(1); p_pullP[i]->Draw();
    canvas->cd(4);                   p_pullPhi[i]->Draw();
    canvas->cd(5);                   p_pullTheta[i]->Draw();
    canvas->cd(6);                   p_pullP[i]->Draw();
    canvas->Update();
    sprintf(psfile, "%s%s.ps(", nom, m_comment);
    canvas->Print(psfile);
    psOuvert = true;
    delete canvas;
  }
  
  canvas = new TCanvas(nom, nom, 300, 10, 800, 600);
  canvas->Divide(2, 2);
  canvas->cd(1); p_histNfake[i]->Draw();
  canvas->cd(2); p_histNaddi[i]->Draw();
  canvas->cd(3); p_histNdupli[i]->Draw();
  canvas->cd(4); p_histNmuon[i]->Draw();
  canvas->Update();
  if(psOuvert) sprintf(psfile, "%s%s.ps", nom, m_comment);
  else         sprintf(psfile, "%s%s.ps(", nom, m_comment);
  canvas->Print(psfile);

  canvas->cd(1); p_fakeVSeta[i]->Divide(p_simuVSeta[i]); p_fakeVSeta[i]->Draw();
  for (int izon=0; izon<nzon; izon++) {
    if(izon+2 <= 4) { canvas->cd(izon+2); fakeVSphi[i][izon]->Divide(simuVSphi[i][izon]); fakeVSphi[i][izon]->Draw(); }
  } 
  canvas->Update();
  sprintf(psfile, "%s%s.ps)", nom, m_comment);
  canvas->Print(psfile);
  delete canvas;

  if( i == 0 ) {  // only done once
    canvas = new TCanvas("chi2Mboy1", "chi2Mboy1", 300, 10, 800, 600);  
    canvas->Divide(2, 2);
    canvas->cd(1); gPad->SetLogy(1); chistaMuon->Draw();
    canvas->cd(2); gPad->SetLogy(1); chistaFake->Draw();  
    canvas->cd(3); gPad->SetLogy(1); chidofMuon->Draw();
    canvas->cd(4); gPad->SetLogy(1); chidofFake->Draw();
    canvas->Update();
    sprintf(psfile, "chi2Mboy%s.ps(", m_comment);
    canvas->Print(psfile);
    delete canvas;

    canvas = new TCanvas("chi2Mboy2", "chi2Mboy2", 300, 10, 800, 600);  
    canvas->Divide(2, 3);
    canvas->cd(1); gPad->SetLogy(1); chicanMuon->Draw();
    canvas->cd(2); gPad->SetLogy(1); chicanFake->Draw();  
    canvas->cd(3); gPad->SetLogy(1); chifitMuon->Draw();
    canvas->cd(4); gPad->SetLogy(1); chifitFake->Draw();
    canvas->cd(5); gPad->SetLogy(1); chiverMuon->Draw();
    canvas->cd(6); gPad->SetLogy(1); chiverFake->Draw();
    canvas->Update();
    sprintf(psfile, "chi2Mboy%s.ps", m_comment);
    canvas->Print(psfile);
    delete canvas;

    canvas = new TCanvas("chi2Mboy3", "chi2Mboy3", 300, 10, 800, 600);  
    canvas->Divide(2, 1);
    canvas->cd(1); chifitVsdofMuon->Draw();
    canvas->cd(2); chifitVsdofFake->Draw();  
    canvas->Update();
    sprintf(psfile, "chi2Mboy%s.ps)", m_comment);
    canvas->Print(psfile);
    delete canvas;

    canvas = new TCanvas("erreur", "erreur", 300, 10, 800, 600); canvas->Divide(2, 3);
    canvas->cd(1); ertetpMB->SetFillColor(2); ertetpMB->Draw(); ertetpST->Draw("same");
    canvas->cd(2); erphipMB->SetFillColor(2); erphipMB->Draw(); erphipST->Draw("same");
    canvas->cd(3); ertetvMB->SetFillColor(2); ertetvMB->Draw(); ertetvST->Draw("same");
    canvas->cd(4); erphivMB->SetFillColor(2); erphivMB->Draw(); erphivST->Draw("same");
    canvas->cd(5); errqspMB->SetFillColor(2); errqspMB->Draw(); errqspST->Draw("same");
    canvas->Update();
    sprintf(psfile, "erreur%s.ps(", m_comment);
    canvas->Print(psfile);
    delete canvas;
    canvas = new TCanvas("erreur", "erreur", 300, 10, 800, 600); canvas->Divide(2, 3);
    canvas->cd(1); erred0IP->Draw();
    canvas->cd(2); errez0IP->Draw();
    canvas->cd(3); errphiIP->Draw();
    canvas->cd(4); errtetIP->Draw();
    canvas->cd(5); errqspIP->Draw();
    canvas->Update();
    sprintf(psfile, "erreur%s.ps)", m_comment);
    canvas->Print(psfile);
    delete canvas;
  }
}


void Fakes::checkAssoc(int i) {
  Assoc* asso = p_assoc[i];
// histos de controle sur les single-mus avec 1 seule trace reconstruite
  if(asso->getNsim() == 1 && asso->getNrec() == 1) {
    if(asso->isMuonDirect(0)) {
      p_pullPhi[i]->Fill(asso->getPullPhi(0,0));
      p_pullTheta[i]->Fill(asso->getPullTheta(0,0));
      p_pullP[i]->Fill(asso->getPullUnSurP(0,0));
      m_sing[i]++;
    }
  }
}


void Fakes::studyFakes(int i) {
  float eta;
  Assoc* asso = p_assoc[i];
  int Nmuon=0, Nfake=0, Naddi=0, Ndupli=0;
  for (int isim=0; isim<asso->getNsim(); isim++) {
    if(asso->isMuonDirect(isim)) {
      eta = asso->getEtaSim(isim);
      p_simuVSeta[i]->Fill(fabs(eta));
      int izon = getEtaZone(eta);
      if(izon < nzon)  simuVSphi[i][izon]->Fill(asso->getPhiSim(isim)*180./m_PI);
    }
  }
  for (int irec=0; irec<asso->getNrec(); irec++) {
    if(asso->isFake(irec))        Nfake++;
    if(asso->isAdditional(irec))  Naddi++; 
    if(asso->isDuplicated(irec))  Ndupli++;
    if(asso->isMuon(irec))        Nmuon++;
    if(asso->isFake(irec)) {
      eta = asso->getEtaRec(irec);
      p_fakeVSeta[i]->Fill(fabs(eta));
      int izon = getEtaZone(eta);
      if(izon < nzon)  fakeVSphi[i][izon]->Fill(asso->getPhiRec(irec)*180./m_PI);
    }
  }
  p_histNfake[i]->Fill(Nfake);
  p_histNaddi[i]->Fill(Naddi);
  p_histNdupli[i]->Fill(Ndupli);
  p_histNmuon[i]->Fill(Nmuon);
  p_histFake[i]->Fill(Nmuon, Nfake);
  if( i == 1 && Nfake > 0 ) {
    cout << "--------------- fake staco " << endl;
    p_assocMS->InfoAssociation();
    p_assocMB->InfoAssociation();
    p_assocST->InfoAssociation();
    cout << "-------------------------- " << endl;
  }
}


int Fakes::getEtaZone(float eta) {
  int izon = nzon;
  for (int k=0; k<nzon; k++)  if(fabs(eta) >= etazon[k] && fabs(eta) < etazon[k+1]) izon = k;
  return izon;
}
