/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Efficiency.h"
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

const int   nzon = 3;   //  zones en eta pour plots de resolution
const float etazon[nzon+2] = { 0.0, 1.0, 2.0, 2.7, 9.9 };
//
const TString station[61] = { "000",
                              "BIL", "BIS", "BML", "BMS", "BOL", "BOS", "BEE", "BIR", "BMF", "BOF",
                              "BOG", "BOH", "EIC", "EIL", "EEL", "EES", "EMC", "EML", "EMS", "EOC",
                              "EOL", "EOS", "T1C", "T1L", "T1S", "T2C", "T2L", "T2S", "T3C", "T3L",
                              "T3S", "CI1", "CI2", "CI3", "CI4", "FIL", "FIS", "FML", "FMS", "FOL",
                              "FOS", "T1F", "T1E", "T2F", "T2E", "T3F", "T3E", "T4F", "T4E", "EIS",
                              "CSS", "CSL", "BIM", "054", "055", "056", "057", "058", "059", "060" };


const int   neta=30;
const float etabin[neta+1] = { 0.00, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.60,
                                     0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95, 1.00, 1.05, 1.10,
                                     1.15, 1.20, 1.30, 1.40, 1.60, 1.80, 2.00, 2.20, 2.40, 2.70 };


int ngenMS[nzon], nrecMS[nzon], nconMS[nzon], nfakMS[nzon], nfa5MS[nzon], nmuoMS[nzon];
int ngenMB[nzon], nrecMB[nzon], nconMB[nzon], nfakMB[nzon], nfa5MB[nzon], nmuoMB[nzon];
int ngenST[nzon], nrecST[nzon], nconST[nzon], nfakST[nzon], nfa5ST[nzon], nmuoST[nzon];
int               nrecMU[nzon],               nfakMU[nzon], nfa5MU[nzon], nmuoMU[nzon];

TH1F *genVSeta, *recVSeta, *conVSeta, *staVSeta, *stcVSeta, *mtgVSeta;
TH1F *genVSetaPlus, *recVSetaPlus, *genVSetaMoin, *recVSetaMoin;
TH1F *genVSetaMuP, *recVSetaMuP, *genVSetaMuM, *recVSetaMuM;
TH1F *gphi[neta], *cphi[neta], *rphi[neta], *sphi[neta], *mphi[neta];
TH1F *ptgen;

TH1F *etaMuTagMediuFake, *etaMuTagEtaZeFake, *etaMuTagInnBaFake, *etaMuTagInnEcFake, *etaMuTagMulBaFake, *etaMuTagMulEcFake;
TH1F *etaMuTagMediuMuon, *etaMuTagEtaZeMuon, *etaMuTagInnBaMuon, *etaMuTagInnEcMuon, *etaMuTagMulBaMuon, *etaMuTagMulEcMuon;
TH1F *phiMuTagMediuFake, *phiMuTagEtaZeFake, *phiMuTagInnBaFake, *phiMuTagInnEcFake, *phiMuTagMulBaFake, *phiMuTagMulEcFake;
TH1F *phiMuTagMediuMuon, *phiMuTagEtaZeMuon, *phiMuTagInnBaMuon, *phiMuTagInnEcMuon, *phiMuTagMulBaMuon, *phiMuTagMulEcMuon;
TH1F *pttMuTagMediuMuon, *pttMuTagEtaZeMuon, *pttMuTagInnBaMuon, *pttMuTagInnEcMuon, *pttMuTagMulBaMuon, *pttMuTagMulEcMuon;
TH1F *pttMuTagMediuFake, *pttMuTagEtaZeFake, *pttMuTagInnBaFake, *pttMuTagInnEcFake, *pttMuTagMulBaFake, *pttMuTagMulEcFake;
TH1F *mdtMuTagMediuFake, *mdtMuTagEtaZeFake, *mdtMuTagInnBaFake, *mdtMuTagInnEcFake, *mdtMuTagMulBaFake, *mdtMuTagMulEcFake;
TH1F *mdtMuTagMediuMuon, *mdtMuTagEtaZeMuon, *mdtMuTagInnBaMuon, *mdtMuTagInnEcMuon, *mdtMuTagMulBaMuon, *mdtMuTagMulEcMuon;
TH1F *nfiMuTagMediuFake, *nfiMuTagEtaZeFake, *nfiMuTagInnBaFake, *nfiMuTagInnEcFake, *nfiMuTagMulBaFake, *nfiMuTagMulEcFake;
TH1F *nfiMuTagMediuMuon, *nfiMuTagEtaZeMuon, *nfiMuTagInnBaMuon, *nfiMuTagInnEcMuon, *nfiMuTagMulBaMuon, *nfiMuTagMulEcMuon;
TH1F *flgMuTagMediuFake, *flgMuTagEtaZeFake, *flgMuTagInnBaFake, *flgMuTagInnEcFake, *flgMuTagMulBaFake, *flgMuTagMulEcFake;
TH1F *flgMuTagMediuMuon, *flgMuTagEtaZeMuon, *flgMuTagInnBaMuon, *flgMuTagInnEcMuon, *flgMuTagMulBaMuon, *flgMuTagMulEcMuon;   

Efficiency::Efficiency():Analysis() {
  m_Debug = kFALSE;
  m_nevt = 0;
}


Efficiency::~Efficiency() {}


void Efficiency::initialize(char* option) {
  printf("Starting Efficiency\n");
  if( strstr(option, "debug") != NULL )    m_Debug = kTRUE;

  // Book histos
// histos de controle
  ptgen = new TH1F("ptgen","ptgen",500,5.,1005.);

  beginEfficiencies();
}


void Efficiency::execute() {

  m_nevt = m_nevt + 1;
  
  efficiencies();

}


void Efficiency::finalize() {
// plots de controle
  TCanvas *check = new TCanvas("check", "check", 300, 10, m_LARGPS, m_HAUTPS);
  ptgen->Draw(); check->Update();

  endEfficiencies();
}


void Efficiency::beginEfficiencies() {
  char titre[256], nom[256], bineta[256];

  for (int izon=0; izon < nzon+1; izon++) {
    ngenMS[izon] = 0; 
    nconMS[izon] = 0;
    nrecMS[izon] = 0; 
    nfakMS[izon] = 0;
    nfa5MS[izon] = 0;
    nmuoMS[izon] = 0;
    ngenMB[izon] = 0; 
    nconMB[izon] = 0;
    nrecMB[izon] = 0; 
    nfakMB[izon] = 0;
    nfa5MB[izon] = 0;
    nmuoMB[izon] = 0;
    ngenST[izon] = 0;
    nconST[izon] = 0; 
    nrecST[izon] = 0; 
    nfakST[izon] = 0;
    nfa5ST[izon] = 0;
    nmuoST[izon] = 0;
    nrecMU[izon] = 0; 
    nfakMU[izon] = 0;
    nfa5MU[izon] = 0;
    nmuoMU[izon] = 0;
  }
  sprintf(titre, "effic vs eta %s", m_comment);
  genVSeta = new TH1F("genVSeta",titre,60,-3.,3.0);   genVSeta->Sumw2();
  recVSeta = new TH1F("recVSeta",titre,60,-3.,3.0);   recVSeta->Sumw2();
  conVSeta = new TH1F("conVSeta",titre,60,-3.,3.0);   conVSeta->Sumw2();
  sprintf(titre, "effic vs eta %s", m_comment);
  staVSeta = new TH1F("staVSeta",titre,60,-3.,3.0);   staVSeta->Sumw2();
  stcVSeta = new TH1F("stcVSeta",titre,60,-3.,3.0);   stcVSeta->Sumw2();
  sprintf(titre, "effic vs eta %s", m_comment);
  mtgVSeta = new TH1F("mtgVSeta",titre,60,-3.,3.0);   mtgVSeta->Sumw2();
  sprintf(titre, "effic vs eta %s eta+", m_comment);
  genVSetaPlus = new TH1F("genVSetaPlus",titre,30,0.0,3.0);   genVSetaPlus->Sumw2();
  recVSetaPlus = new TH1F("recVSetaPlus",titre,30,0.0,3.0);   recVSetaPlus->Sumw2();
  sprintf(titre, "effic vs eta %s eta-", m_comment);
  genVSetaMoin = new TH1F("genVSetaMoin",titre,30,0.0,3.0);   genVSetaMoin->Sumw2();
  recVSetaMoin = new TH1F("recVSetaMoin",titre,30,0.0,3.0);   recVSetaMoin->Sumw2();
  sprintf(titre, "effic vs eta %s mu+", m_comment);
  genVSetaMuP  = new TH1F("genVSetaMuP",titre,30,-3.0,3.0);   genVSetaMuP->Sumw2();
  recVSetaMuP  = new TH1F("recVSetaMuP",titre,30,-3.0,3.0);   recVSetaMuP->Sumw2();
  sprintf(titre, "effic vs eta %s mu-", m_comment);
  genVSetaMuM  = new TH1F("genVSetaMuM",titre,30,-3.0,3.0);   genVSetaMuM->Sumw2();
  recVSetaMuM  = new TH1F("recVSetaMuM",titre,30,-3.0,3.0);   recVSetaMuM->Sumw2();

  for (int ieta=0; ieta<neta; ieta++) {
    sprintf(bineta, "(%4.2f<eta<%4.2f)", etabin[ieta], etabin[ieta+1]);
    sprintf(nom,   "gphi%d", ieta);
    sprintf(titre, "simul vs phi %s %s", bineta, m_comment);
    gphi[ieta] = new TH1F(nom,titre,48,-180.,180.);  gphi[ieta]->Sumw2();
    sprintf(nom,   "cphi%d", ieta);
    sprintf(titre, "effic vs phi cone %s %s", bineta, m_comment);
    cphi[ieta] = new TH1F(nom,titre,48,-180.,180.);  cphi[ieta]->Sumw2();
    sprintf(nom,   "rphi%d", ieta);
    sprintf(titre, "effic vs phi %s %s", bineta, m_comment);
    rphi[ieta] = new TH1F(nom,titre,48,-180.,180.);  rphi[ieta]->Sumw2();
    sprintf(nom,   "sphi%d", ieta);
    sprintf(titre, "effic Staco vs phi %s %s", bineta, m_comment);
    sphi[ieta] = new TH1F(nom,titre,48,-180.,180.);  sphi[ieta]->Sumw2();
    sprintf(nom,   "mphi%d", ieta);
    sprintf(titre, "effic MuTag vs phi %s %s", bineta, m_comment);
    mphi[ieta] = new TH1F(nom,titre,48,-180.,180.);  mphi[ieta]->Sumw2();
  }

  etaMuTagMulBaFake = new TH1F("etaMuTagMulBaFake","etaMuTagMulBaFake",30,-3., 3.); etaMuTagMulBaFake->Sumw2();
  etaMuTagMulEcFake = new TH1F("etaMuTagMulEcFake","etaMuTagMulEcFake",30,-3., 3.); etaMuTagMulEcFake->Sumw2();
  etaMuTagMediuFake = new TH1F("etaMuTagMediuFake","etaMuTagMediuFake",30,-3., 3.); etaMuTagMediuFake->Sumw2();
  etaMuTagEtaZeFake = new TH1F("etaMuTagEtaZeFake","etaMuTagEtaZeFake",30,-3., 3.); etaMuTagEtaZeFake->Sumw2();
  etaMuTagInnBaFake = new TH1F("etaMuTagInnBaFake","etaMuTagInnBaFake",30,-3., 3.); etaMuTagInnBaFake->Sumw2();
  etaMuTagInnEcFake = new TH1F("etaMuTagInnEcFake","etaMuTagInnEcFake",30,-3., 3.); etaMuTagInnEcFake->Sumw2();
  etaMuTagMulBaMuon = new TH1F("etaMuTagMulBaMuon","etaMuTagMulBaMuon",30,-3., 3.); etaMuTagMulBaMuon->Sumw2();
  etaMuTagMulEcMuon = new TH1F("etaMuTagMulEcMuon","etaMuTagMulEcMuon",30,-3., 3.); etaMuTagMulEcMuon->Sumw2();
  etaMuTagMediuMuon = new TH1F("etaMuTagMediuMuon","etaMuTagMediuMuon",30,-3., 3.); etaMuTagMediuMuon->Sumw2();
  etaMuTagEtaZeMuon = new TH1F("etaMuTagEtaZeMuon","etaMuTagEtaZeMuon",30,-3., 3.); etaMuTagEtaZeMuon->Sumw2();
  etaMuTagInnBaMuon = new TH1F("etaMuTagInnBaMuon","etaMuTagInnBaMuon",30,-3., 3.); etaMuTagInnBaMuon->Sumw2();
  etaMuTagInnEcMuon = new TH1F("etaMuTagInnEcMuon","etaMuTagInnEcMuon",30,-3., 3.); etaMuTagInnEcMuon->Sumw2();
  phiMuTagMulBaFake = new TH1F("phiMuTagMulBaFake","phiMuTagMulBaFake",32,-3.2, 3.2); phiMuTagMulBaFake->Sumw2();
  phiMuTagMulEcFake = new TH1F("phiMuTagMulEcFake","phiMuTagMulEcFake",32,-3.2, 3.2); phiMuTagMulEcFake->Sumw2();
  phiMuTagMediuFake = new TH1F("phiMuTagMediuFake","phiMuTagMediuFake",32,-3.2, 3.2); phiMuTagMediuFake->Sumw2();
  phiMuTagEtaZeFake = new TH1F("phiMuTagPhiZeFake","phiMuTagPhiZeFake",32,-3.2, 3.2); phiMuTagEtaZeFake->Sumw2();
  phiMuTagInnBaFake = new TH1F("phiMuTagInnBaFake","phiMuTagInnBaFake",32,-3.2, 3.2); phiMuTagInnBaFake->Sumw2();
  phiMuTagInnEcFake = new TH1F("phiMuTagInnEcFake","phiMuTagInnEcFake",32,-3.2, 3.2); phiMuTagInnEcFake->Sumw2();
  phiMuTagMulBaMuon = new TH1F("phiMuTagMulBaMuon","phiMuTagMulBaMuon",32,-3.2, 3.2); phiMuTagMulBaMuon->Sumw2();
  phiMuTagMulEcMuon = new TH1F("phiMuTagMulEcMuon","phiMuTagMulEcMuon",32,-3.2, 3.2); phiMuTagMulEcMuon->Sumw2();
  phiMuTagMediuMuon = new TH1F("phiMuTagMediuMuon","phiMuTagMediuMuon",32,-3.2, 3.2); phiMuTagMediuMuon->Sumw2();
  phiMuTagEtaZeMuon = new TH1F("phiMuTagPhiZeMuon","phiMuTagPhiZeMuon",32,-3.2, 3.2); phiMuTagEtaZeMuon->Sumw2();
  phiMuTagInnBaMuon = new TH1F("phiMuTagInnBaMuon","phiMuTagInnBaMuon",32,-3.2, 3.2); phiMuTagInnBaMuon->Sumw2();
  phiMuTagInnEcMuon = new TH1F("phiMuTagInnEcMuon","phiMuTagInnEcMuon",32,-3.2, 3.2); phiMuTagInnEcMuon->Sumw2();
  pttMuTagMulBaFake = new TH1F("pttMuTagMulBaFake","pttMuTagMulBaFake",50,0.,25.); pttMuTagMulBaFake->Sumw2();
  pttMuTagMulEcFake = new TH1F("pttMuTagMulEcFake","pttMuTagMulEcFake",50,0.,25.); pttMuTagMulEcFake->Sumw2();
  pttMuTagMediuFake = new TH1F("pttMuTagMediuFake","pttMuTagMediuFake",50,0.,25.); pttMuTagMediuFake->Sumw2();
  pttMuTagEtaZeFake = new TH1F("pttMuTagPttZeFake","pttMuTagPttZeFake",50,0.,25.); pttMuTagEtaZeFake->Sumw2();
  pttMuTagInnBaFake = new TH1F("pttMuTagInnBaFake","pttMuTagInnBaFake",50,0.,25.); pttMuTagInnBaFake->Sumw2();
  pttMuTagInnEcFake = new TH1F("pttMuTagInnEcFake","pttMuTagInnEcFake",50,0.,25.); pttMuTagInnEcFake->Sumw2();
  pttMuTagMulBaMuon = new TH1F("pttMuTagMulBaMuon","pttMuTagMulBaMuon",50,0.,25.); pttMuTagMulBaMuon->Sumw2();
  pttMuTagMulEcMuon = new TH1F("pttMuTagMulEcMuon","pttMuTagMulEcMuon",50,0.,25.); pttMuTagMulEcMuon->Sumw2();
  pttMuTagMediuMuon = new TH1F("pttMuTagMediuMuon","pttMuTagMediuMuon",50,0.,25.); pttMuTagMediuMuon->Sumw2();
  pttMuTagEtaZeMuon = new TH1F("pttMuTagPttZeMuon","pttMuTagPttZeMuon",50,0.,25.); pttMuTagEtaZeMuon->Sumw2();
  pttMuTagInnBaMuon = new TH1F("pttMuTagInnBaMuon","pttMuTagInnBaMuon",50,0.,25.); pttMuTagInnBaMuon->Sumw2();
  pttMuTagInnEcMuon = new TH1F("pttMuTagInnEcMuon","pttMuTagInnEcMuon",50,0.,25.); pttMuTagInnEcMuon->Sumw2();
  mdtMuTagMulBaFake = new TH1F("mdtMuTagMulBaFake","mdtMuTagMulBaFake",20,0.,20.); mdtMuTagMulBaFake->Sumw2();
  mdtMuTagMulEcFake = new TH1F("mdtMuTagMulEcFake","mdtMuTagMulEcFake",20,0.,20.); mdtMuTagMulEcFake->Sumw2();
  mdtMuTagMediuFake = new TH1F("mdtMuTagMediuFake","mdtMuTagMediuFake",20,0.,20.); mdtMuTagMediuFake->Sumw2();
  mdtMuTagEtaZeFake = new TH1F("mdtMuTagEtaZeFake","mdtMuTagEtaZeFake",20,0.,20.); mdtMuTagEtaZeFake->Sumw2();
  mdtMuTagInnBaFake = new TH1F("mdtMuTagInnBaFake","mdtMuTagInnBaFake",20,0.,20.); mdtMuTagInnBaFake->Sumw2();
  mdtMuTagInnEcFake = new TH1F("mdtMuTagInnEcFake","mdtMuTagInnEcFake",20,0.,20.); mdtMuTagInnEcFake->Sumw2();
  mdtMuTagMulBaMuon = new TH1F("mdtMuTagMulBaMuon","mdtMuTagMulBaMuon",20,0.,20.); mdtMuTagMulBaMuon->Sumw2();
  mdtMuTagMulEcMuon = new TH1F("mdtMuTagMulEcMuon","mdtMuTagMulEcMuon",20,0.,20.); mdtMuTagMulEcMuon->Sumw2();
  mdtMuTagMediuMuon = new TH1F("mdtMuTagMediuMuon","mdtMuTagMediuMuon",20,0.,20.); mdtMuTagMediuMuon->Sumw2();
  mdtMuTagEtaZeMuon = new TH1F("mdtMuTagEtaZeMuon","mdtMuTagEtaZeMuon",20,0.,20.); mdtMuTagEtaZeMuon->Sumw2();
  mdtMuTagInnBaMuon = new TH1F("mdtMuTagInnBaMuon","mdtMuTagInnBaMuon",20,0.,20.); mdtMuTagInnBaMuon->Sumw2();
  mdtMuTagInnEcMuon = new TH1F("mdtMuTagInnEcMuon","mdtMuTagInnEcMuon",20,0.,20.); mdtMuTagInnEcMuon->Sumw2();
  nfiMuTagMulBaFake = new TH1F("nfiMuTagMulBaFake","nfiMuTagMulBaFake",20,0.,20.); nfiMuTagMulBaFake->Sumw2();
  nfiMuTagMulEcFake = new TH1F("nfiMuTagMulEcFake","nfiMuTagMulEcFake",20,0.,20.); nfiMuTagMulEcFake->Sumw2();
  nfiMuTagMediuFake = new TH1F("nfiMuTagMediuFake","nfiMuTagMediuFake",20,0.,20.); nfiMuTagMediuFake->Sumw2();
  nfiMuTagEtaZeFake = new TH1F("nfiMuTagEtaZeFake","nfiMuTagEtaZeFake",20,0.,20.); nfiMuTagEtaZeFake->Sumw2();
  nfiMuTagInnBaFake = new TH1F("nfiMuTagInnBaFake","nfiMuTagInnBaFake",20,0.,20.); nfiMuTagInnBaFake->Sumw2();
  nfiMuTagInnEcFake = new TH1F("nfiMuTagInnEcFake","nfiMuTagInnEcFake",20,0.,20.); nfiMuTagInnEcFake->Sumw2();
  nfiMuTagMulBaMuon = new TH1F("nfiMuTagMulBaMuon","nfiMuTagMulBaMuon",20,0.,20.); nfiMuTagMulBaMuon->Sumw2();
  nfiMuTagMulEcMuon = new TH1F("nfiMuTagMulEcMuon","nfiMuTagMulEcMuon",20,0.,20.); nfiMuTagMulEcMuon->Sumw2();
  nfiMuTagMediuMuon = new TH1F("nfiMuTagMediuMuon","nfiMuTagMediuMuon",20,0.,20.); nfiMuTagMediuMuon->Sumw2();
  nfiMuTagEtaZeMuon = new TH1F("nfiMuTagEtaZeMuon","nfiMuTagEtaZeMuon",20,0.,20.); nfiMuTagEtaZeMuon->Sumw2();
  nfiMuTagInnBaMuon = new TH1F("nfiMuTagInnBaMuon","nfiMuTagInnBaMuon",20,0.,20.); nfiMuTagInnBaMuon->Sumw2();
  nfiMuTagInnEcMuon = new TH1F("nfiMuTagInnEcMuon","nfiMuTagInnEcMuon",20,0.,20.); nfiMuTagInnEcMuon->Sumw2();
  flgMuTagMulBaFake = new TH1F("flgMuTagMulBaFake","flgMuTagMulBaFake", 5,0., 5.); flgMuTagMulBaFake->Sumw2();
  flgMuTagMulEcFake = new TH1F("flgMuTagMulEcFake","flgMuTagMulEcFake", 5,0., 5.); flgMuTagMulEcFake->Sumw2();
  flgMuTagMediuFake = new TH1F("flgMuTagMediuFake","flgMuTagMediuFake", 5,0., 5.); flgMuTagMediuFake->Sumw2();
  flgMuTagEtaZeFake = new TH1F("flgMuTagEtaZeFake","flgMuTagEtaZeFake", 5,0., 5.); flgMuTagEtaZeFake->Sumw2();
  flgMuTagInnBaFake = new TH1F("flgMuTagInnBaFake","flgMuTagInnBaFake", 5,0., 5.); flgMuTagInnBaFake->Sumw2();
  flgMuTagInnEcFake = new TH1F("flgMuTagInnEcFake","flgMuTagInnEcFake", 5,0., 5.); flgMuTagInnEcFake->Sumw2();
  flgMuTagMulBaMuon = new TH1F("flgMuTagMulBaMuon","flgMuTagMulBaMuon", 5,0., 5.); flgMuTagMulBaMuon->Sumw2();
  flgMuTagMulEcMuon = new TH1F("flgMuTagMulEcMuon","flgMuTagMulEcMuon", 5,0., 5.); flgMuTagMulEcMuon->Sumw2();
  flgMuTagMediuMuon = new TH1F("flgMuTagMediuMuon","flgMuTagMediuMuon", 5,0., 5.); flgMuTagMediuMuon->Sumw2();
  flgMuTagEtaZeMuon = new TH1F("flgMuTagEtaZeMuon","flgMuTagEtaZeMuon", 5,0., 5.); flgMuTagEtaZeMuon->Sumw2();
  flgMuTagInnBaMuon = new TH1F("flgMuTagInnBaMuon","flgMuTagInnBaMuon", 5,0., 5.); flgMuTagInnBaMuon->Sumw2();
  flgMuTagInnEcMuon = new TH1F("flgMuTagInnEcMuon","flgMuTagInnEcMuon", 5,0., 5.); flgMuTagInnEcMuon->Sumw2();

}


void Efficiency::efficiencies() { // Study of efficiencies
  int ieta, izon;
  float eta, phi;
  bool accep;
  Assoc* asso;

  for (int irec=0; irec<p_assocMS->getNrec(); irec++) {
    float etams = p_assocMS->getEtaRec(irec);
    float etamb = p_assocMB->getEtaRec(irec);
    int izonms  = getEtaZone( fabs(etams) );
    int izonmb  = getEtaZone( fabs(etamb) );
    if( izonms != izonmb ) cout << " difference " << izonms << " " << etams << " - " << izonmb << " " << etamb << endl;
  }

// First muonboy spectro efficiency
  asso = p_assocMS;  
  for (int isim=0; isim<asso->getNsim(); isim++) {
    if(asso->isMuonDirect(isim)) {
      eta = asso->getEtaSim(isim);
      izon = getEtaZone( fabs(eta) );
      accep = isInsideAcceptance(asso, eta);
      if(accep)  ngenMS[izon] = ngenMS[izon] + 1;
// Is there a reconstructed track inside the cone around this simulated one ?
      if(asso->getNCone(isim) > 0) {
        if(accep)  nconMS[izon] = nconMS[izon] + 1;
// Is the matching in P OK ?
        if(asso->getRec(isim) >= 0) {
          if(accep)  nrecMS[izon] = nrecMS[izon] + 1;
        }
      }
    }
  }
// and fakes 
  for (int irec=0; irec<asso->getNrec(); irec++) {
    izon = getEtaZone( asso->getEtaRec(irec) );
    if(asso->isFake(irec)) {
      if(izon < nzon)  nfakMS[izon] = nfakMS[izon] + 1;
      if(izon < nzon && asso->getPtRec(irec) >= 2500. )  nfa5MS[izon] = nfa5MS[izon] + 1;
    } else {
      if(izon < nzon)  nmuoMS[izon] = nmuoMS[izon] + 1;
    }
  }

// Then muonboy IP efficiency
  asso = p_assocMB;
  for (int isim=0; isim<asso->getNsim(); isim++) {
    if(asso->isMuonDirect(isim)) {
      eta = asso->getEtaSim(isim);
      ieta = getEtaBin( fabs(eta) );
      izon = getEtaZone( fabs(eta) );
      phi = asso->getPhiSim(isim) * 180./m_PI;
      accep = isInsideAcceptance(asso, eta);
      ptgen->Fill( eta );
      genVSeta->Fill( eta );
      if( eta > 0 ) genVSetaPlus->Fill( fabs(eta) );
      else          genVSetaMoin->Fill( fabs(eta) );
      if( asso->getChargeSurPSim(isim) > 0 )  genVSetaMuP->Fill( eta );
      else                                    genVSetaMuM->Fill( eta );
      if(accep)  ngenMB[izon] = ngenMB[izon] + 1;
      if(accep)  gphi[ieta]->Fill(phi);     // effic en phi par tranche de eta
// Is there a reconstructed track inside the cone around this simulated one ?
      if(asso->getNCone(isim) > 0) {
        conVSeta->Fill( eta );
        if(accep)  cphi[ieta]->Fill(phi);
        if(accep)  nconMB[izon] = nconMB[izon] + 1;
// Is the matching in P OK ?
        if(asso->getRec(isim) >= 0) {
          recVSeta->Fill( eta );
          if( eta > 0 ) recVSetaPlus->Fill( fabs(eta) );
          else          recVSetaMoin->Fill( fabs(eta) );
          if( asso->getChargeSurPSim(isim) > 0 )  recVSetaMuP->Fill( eta );
          else                                    recVSetaMuM->Fill( eta );
          if(accep)  nrecMB[izon] = nrecMB[izon] + 1;
          if(accep)  rphi[ieta]->Fill(phi);
        }
      }
// same things with Staco
      accep = isInsideAcceptance(p_assocST, eta);
      if(accep) ngenST[izon] = ngenST[izon] + 1;
      if(p_assocST->getNCone(isim) > 0) {
        stcVSeta->Fill( eta );
        if(accep)  nconST[izon] = nconST[izon] + 1;
        if(p_assocST->getRec(isim) >= 0) {
          staVSeta->Fill( eta );
          if(accep)  nrecST[izon] = nrecST[izon] + 1;
          if(accep)  sphi[ieta]->Fill(phi);
        }
      } else {
        if(accep) cout << " staco parec run-evt-eta " << p_evt->Run << " " << p_evt->Event << " " << p_assocST->getEtaSim(isim) << endl;
      }
// and MuTag
      if(p_assocMU->getRec(isim) >= 0) {
        mtgVSeta->Fill( eta );
        if(accep)  nrecMU[izon] = nrecMU[izon] + 1;
        if(accep)  mphi[ieta]->Fill(phi);
      }
    }
  }

// then fakes 
  for (int irec=0; irec<asso->getNrec(); irec++) {
    izon = getEtaZone( asso->getEtaRec(irec) );
    if(asso->isFake(irec)) {
      if(izon < nzon)  nfakMB[izon] = nfakMB[izon] + 1;
      if(izon < nzon && asso->getPtRec(irec) >= 2500. )  nfa5MB[izon] = nfa5MB[izon] + 1;
      cout << " fake muonboy run-evt-eta " << p_evt->Run << " " << p_evt->Event << " " << asso->getEtaRec(irec) << endl;
      cout << " fake muonboy chican " << (p_evt->MboyTrkMS_Chican)->at(irec) << " chifit " << (p_evt->MboyTrkMS_Chifit)->at(irec) 
                        << " chiver " << (p_evt->MboyTrkMS_Chiver)->at(irec) << " chista " << (p_evt->MboyTrkMS_Chistat)->at(irec)
	   << " ndof " << (p_evt->MboyTrkMS_Ndof)->at(irec) << endl;  
    } else {
      if(izon < nzon)  nmuoMB[izon] = nmuoMB[izon] + 1;
    }
  }
  for (int irec=0; irec<p_assocST->getNrec(); irec++) {
    izon = getEtaZone( p_assocST->getEtaRec(irec) );
    if(p_assocST->isFake(irec)) {
      if(izon < nzon)  nfakST[izon] = nfakST[izon] + 1;
      if(izon < nzon && p_assocST->getPtRec(irec) >= 2500. )  nfa5ST[izon] = nfa5ST[izon] + 1;
    } else {
      if(izon < nzon)  nmuoST[izon] = nmuoST[izon] + 1;
    }
  }
  for (int irec=0; irec<p_assocMU->getNrec(); irec++) {
    izon = getEtaZone( p_assocMU->getEtaRec(irec) );
    if(p_assocMU->isFake(irec)) {
      if(izon < nzon)  nfakMU[izon] = nfakMU[izon] + 1;
      if(izon < nzon && p_assocMU->getPtRec(irec) >= 2500. )  nfa5MU[izon] = nfa5MU[izon] + 1;
    } else {
      if(izon < nzon)  nmuoMU[izon] = nmuoMU[izon] + 1;
    }
    if( p_assocMU->getPtRec(irec) < 2500. ) continue;
    if( irec >= p_evt->staco_nmuon ) {  // MuTag track
      int imu = irec - p_evt->staco_nmuon;
      float eta = p_assocMU->getEtaRec(irec);
      int typ = (p_evt->MuTag_StationTypeSglTag)->at(imu);
      if( p_assocMU->isFake(irec) ) {
        if( (p_evt->MuTag_SegMult)->at(imu) > 1 ) {
          if( fabs(eta) < 1. ) {
            etaMuTagMulBaFake->Fill( eta );
            phiMuTagMulBaFake->Fill( p_assocMU->getPhiRec(irec) );
            pttMuTagMulBaFake->Fill( p_assocMU->getPtRec(irec) / 1000. );
            mdtMuTagMulBaFake->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
            nfiMuTagMulBaFake->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
            flgMuTagMulBaFake->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
	  } else {
            etaMuTagMulEcFake->Fill( eta );
            phiMuTagMulEcFake->Fill( p_assocMU->getPhiRec(irec) );
            pttMuTagMulEcFake->Fill( p_assocMU->getPtRec(irec) / 1000. );
            mdtMuTagMulEcFake->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
            nfiMuTagMulEcFake->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
            flgMuTagMulEcFake->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
	  }
        } else if( typ == 0 || typ == 1 || typ == 7 || typ == 52 ) {  // BIL, BIS, BIR, BIM
          etaMuTagInnBaFake->Fill( eta );
          phiMuTagInnBaFake->Fill( p_assocMU->getPhiRec(irec) );
          pttMuTagInnBaFake->Fill( p_assocMU->getPtRec(irec) / 1000. );
          mdtMuTagInnBaFake->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
          nfiMuTagInnBaFake->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
          flgMuTagInnBaFake->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
	} else if( typ == 13 || typ == 49 ) { // EIL, EIS
          etaMuTagInnEcFake->Fill( eta );
          phiMuTagInnEcFake->Fill( p_assocMU->getPhiRec(irec) );
          pttMuTagInnEcFake->Fill( p_assocMU->getPtRec(irec) / 1000. );
          mdtMuTagInnEcFake->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
          nfiMuTagInnEcFake->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
          flgMuTagInnEcFake->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
	} else if( typ == 17 || typ == 18 ) { // EML, EMS
          etaMuTagMediuFake->Fill( eta );
          phiMuTagMediuFake->Fill( p_assocMU->getPhiRec(irec) );
          pttMuTagMediuFake->Fill( p_assocMU->getPtRec(irec) / 1000. );
          mdtMuTagMediuFake->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
          nfiMuTagMediuFake->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
          flgMuTagMediuFake->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
        } else if( typ == 2 || typ == 4 || typ == 10 ) { // BML, BOL, BOG
          etaMuTagEtaZeFake->Fill( eta );
          phiMuTagEtaZeFake->Fill( p_assocMU->getPhiRec(irec) );
          pttMuTagEtaZeFake->Fill( p_assocMU->getPtRec(irec) / 1000. );
          mdtMuTagEtaZeFake->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
          nfiMuTagEtaZeFake->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
          flgMuTagEtaZeFake->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
        } else {
          cout << " Probleme typ mutag " << typ << endl;
        }
      } else {
        if( (p_evt->MuTag_SegMult)->at(imu) > 1 ) {
          if( fabs(eta) < 1. ) {
            etaMuTagMulBaMuon->Fill( eta );
            phiMuTagMulBaMuon->Fill( p_assocMU->getPhiRec(irec) );
            pttMuTagMulBaMuon->Fill( p_assocMU->getPtRec(irec) / 1000. );
            mdtMuTagMulBaMuon->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
            nfiMuTagMulBaMuon->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
            flgMuTagMulBaMuon->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
	  } else {
            etaMuTagMulEcMuon->Fill( eta );
            phiMuTagMulEcMuon->Fill( p_assocMU->getPhiRec(irec) );
            pttMuTagMulEcMuon->Fill( p_assocMU->getPtRec(irec) / 1000. );
            mdtMuTagMulEcMuon->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
            nfiMuTagMulEcMuon->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
            flgMuTagMulEcMuon->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
	  }
        } else if( typ == 0 || typ == 1 || typ == 7 || typ == 52 ) {  // BIL, BIS, BIR, BIM
          etaMuTagInnBaMuon->Fill( eta );
          phiMuTagInnBaMuon->Fill( p_assocMU->getPhiRec(irec) );
          pttMuTagInnBaMuon->Fill( p_assocMU->getPtRec(irec) / 1000. );
          mdtMuTagInnBaMuon->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
          nfiMuTagInnBaMuon->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
          flgMuTagInnBaMuon->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
        } else if( typ == 13 || typ == 49 ) { // EIL, EIS
          etaMuTagInnEcMuon->Fill( eta );
          phiMuTagInnEcMuon->Fill( p_assocMU->getPhiRec(irec) );
          pttMuTagInnEcMuon->Fill( p_assocMU->getPtRec(irec) / 1000. );
          mdtMuTagInnEcMuon->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
          nfiMuTagInnEcMuon->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
          flgMuTagInnEcMuon->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
        } else if( typ == 17 || typ == 18 ) { // EML, EMS
          etaMuTagMediuMuon->Fill( eta );
          phiMuTagMediuMuon->Fill( p_assocMU->getPhiRec(irec) );
          pttMuTagMediuMuon->Fill( p_assocMU->getPtRec(irec) / 1000. );
          mdtMuTagMediuMuon->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
          nfiMuTagMediuMuon->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
          flgMuTagMediuMuon->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
        } else if( typ == 2 || typ == 4 || typ == 10 ) { // BML, BOL, BOG
          etaMuTagEtaZeMuon->Fill( eta );
          phiMuTagEtaZeMuon->Fill( p_assocMU->getPhiRec(irec) );
          pttMuTagEtaZeMuon->Fill( p_assocMU->getPtRec(irec) / 1000. );
          mdtMuTagEtaZeMuon->Fill( (p_evt->MuTag_numberOfMDTHits)->at(imu) );
          nfiMuTagEtaZeMuon->Fill( (p_evt->MuTag_numberOfTGCPhiHits)->at(imu) + (p_evt->MuTag_numberOfRPCPhiHits)->at(imu) );
          flgMuTagEtaZeMuon->Fill( (p_evt->MuTag_MuonQuality)->at(imu) );
        } else {
          cout << " Probleme typ mutag " << typ << endl;
        }
      }
    }
  }
}


void Efficiency::endEfficiencies() {
  char titre[256], nom[256];
  sprintf(nom, "dump.effic%s", m_comment);
  FILE *txtFile = fopen(nom, "w");
  float mstot, msrec, msfak, msfa5, msmuo, mbtot, mbrec, mbfak, mbfa5, mbmuo;
  float sttot, strec, stfak, stfa5, stmuo,        murec, mufak, mufa5, mumuo;
  float effms, errms, effmb, errmb, effst, errst, effmu, errmu;

  mstot = 0.; msrec = 0.; msfak = 0.; msfa5 = 0.; msmuo = 0.;
  mbtot = 0.; mbrec = 0.; mbfak = 0.; mbfa5 = 0.; mbmuo = 0.;
  sttot = 0.; strec = 0.; stfak = 0.; stfa5 = 0.; stmuo = 0.;
  murec = 0.;             mufak = 0.; mufa5 = 0.; mumuo = 0.;

  for (int izon=0; izon<nzon; izon++) {
//    fprintf( txtFile, " Probleme MS izon %d :  %d  %d\n", izon, nrecMS[izon], nmuoMS[izon]);
//    fprintf( txtFile, " Probleme MB izon %d :  %d  %d\n", izon, nrecMB[izon], nmuoMB[izon]);
//    fprintf( txtFile, " Probleme ST izon %d :  %d  %d\n", izon, nrecST[izon], nmuoST[izon]);
//    fprintf( txtFile, " Probleme MU izon %d :  %d  %d\n", izon, nrecMU[izon], nmuoMU[izon]);
    mstot = mstot + ngenMS[izon];
    msrec = msrec + nrecMS[izon];
    msfak = msfak + nfakMS[izon];
    msfa5 = msfa5 + nfa5MS[izon];
    msmuo = msmuo + nmuoMS[izon];
    mbtot = mbtot + ngenMB[izon];
    mbrec = mbrec + nrecMB[izon];
    mbfak = mbfak + nfakMB[izon];
    mbfa5 = mbfa5 + nfa5MB[izon];
    mbmuo = mbmuo + nmuoMB[izon];
    sttot = sttot + ngenST[izon];
    strec = strec + nrecST[izon];
    stfak = stfak + nfakST[izon];
    stfa5 = stfa5 + nfa5ST[izon];
    stmuo = stmuo + nmuoST[izon];
    murec = murec + nrecMU[izon];
    mufak = mufak + nfakMU[izon];
    mufa5 = mufa5 + nfa5MU[izon];
    mumuo = mumuo + nmuoMU[izon];
  }
  if(mbtot > 10.) {
    effms = msrec/mstot; errms = sqrt(msrec*(mstot-msrec)/mstot/mstot/mstot); msfak = msfak/m_nevt; msfa5 = msfa5/m_nevt; msmuo = msmuo/m_nevt;
    effmb = mbrec/mbtot; errmb = sqrt(mbrec*(mbtot-mbrec)/mbtot/mbtot/mbtot); mbfak = mbfak/m_nevt; mbfa5 = mbfa5/m_nevt; mbmuo = mbmuo/m_nevt;
    effst = strec/sttot; errst = sqrt(strec*(sttot-strec)/sttot/sttot/sttot); stfak = stfak/m_nevt; stfa5 = stfa5/m_nevt; stmuo = stmuo/m_nevt;
    effmu = murec/sttot; errmu = sqrt(murec*(sttot-murec)/sttot/sttot/sttot); mufak = mufak/m_nevt; mufa5 = mufa5/m_nevt; mumuo = mumuo/m_nevt;
    fprintf( txtFile, "-----------------------------\n");
    fprintf( txtFile, " Statistics : number of events processed = %d\n", m_nevt);
    fprintf( txtFile, "-----------------------------\n");
    fprintf( txtFile, " ==> effic pour %s :\n", m_comment);
    fprintf( txtFile, "-----------------------------\n");
    fprintf( txtFile, " => integree sur eta : %.1f < abs(eta) < %.1f  (%.0f evts)\n", etazon[0], etazon[nzon], mbtot);
    fprintf( txtFile, "  muonboy spectro : %.4f +/- %.4f   avec %.4f fakes/event, %.4f fakes/event(pt>2.5) et %.4f muons/event\n", effms, errms, msfak, msfa5, msmuo);
    fprintf( txtFile, "       muonboy IP : %.4f +/- %.4f   avec %.4f fakes/event, %.4f fakes/event(pt>2.5) et %.4f muons/event\n", effmb, errmb, mbfak, mbfa5, mbmuo);
    fprintf( txtFile, "            staco : %.4f +/- %.4f   avec %.4f fakes/event, %.4f fakes/event(pt>2.5) et %.4f muons/event\n", effst, errst, stfak, stfa5, stmuo);
    fprintf( txtFile, "            mutag : %.4f +/- %.4f   avec %.4f fakes/event, %.4f fakes/event(pt>2.5) et %.4f muons/event\n", effmu, errmu, mufak, mufa5, mumuo);
    for (int izon=0; izon<nzon; izon++) {
      if(ngenMB[izon] > 10) {
        mstot = ngenMS[izon]; msrec = nrecMS[izon]; msfak = nfakMS[izon];  msfa5 = nfa5MS[izon]; msmuo = nmuoMS[izon];
        mbtot = ngenMB[izon]; mbrec = nrecMB[izon]; mbfak = nfakMB[izon];  mbfa5 = nfa5MB[izon]; mbmuo = nmuoMB[izon];
        sttot = ngenST[izon]; strec = nrecST[izon]; stfak = nfakST[izon];  stfa5 = nfa5ST[izon]; stmuo = nmuoST[izon];
                              murec = nrecMU[izon]; mufak = nfakMU[izon];  mufa5 = nfa5MU[izon]; mumuo = nmuoMU[izon];
        effms = msrec/mstot; errms = sqrt(msrec*(mstot-msrec)/mstot/mstot/mstot); msfak = msfak/m_nevt; msfa5 = msfa5/m_nevt; msmuo = msmuo/m_nevt;
        effmb = mbrec/mbtot; errmb = sqrt(mbrec*(mbtot-mbrec)/mbtot/mbtot/mbtot); mbfak = mbfak/m_nevt; mbfa5 = mbfa5/m_nevt; mbmuo = mbmuo/m_nevt;
        effst = strec/sttot; errst = sqrt(strec*(sttot-strec)/sttot/sttot/sttot); stfak = stfak/m_nevt; stfa5 = stfa5/m_nevt; stmuo = stmuo/m_nevt;
        effmu = murec/sttot; errmu = sqrt(murec*(sttot-murec)/sttot/sttot/sttot); mufak = mufak/m_nevt; mufa5 = mufa5/m_nevt; mumuo = mumuo/m_nevt;
        fprintf( txtFile, " => tranche en eta : %.1f < abs(eta) < %.1f  (%.0f evts)\n", etazon[izon], etazon[izon+1], mbtot);
        fprintf( txtFile, "  muonboy spectro : %.4f +/- %.4f   avec %.4f fakes/event, %.4f fakes/event(pt>2.5) et %.4f muons/event\n", effms, errms, msfak, msfa5, msmuo);
        fprintf( txtFile, "       muonboy IP : %.4f +/- %.4f   avec %.4f fakes/event, %.4f fakes/event(pt>2.5) et %.4f muons/event\n", effmb, errmb, mbfak, mbfa5, mbmuo);
        fprintf( txtFile, "            staco : %.4f +/- %.4f   avec %.4f fakes/event, %.4f fakes/event(pt>2.5) et %.4f muons/event\n", effst, errst, stfak, stfa5, stmuo);
        fprintf( txtFile, "            mutag : %.4f +/- %.4f   avec %.4f fakes/event, %.4f fakes/event(pt>2.5) et %.4f muons/event\n", effmu, errmu, mufak, mufa5, mumuo);
      }
    }
  }
  fclose(txtFile);

  char nomCanvas[256];

  int largPNG=400, hautPNG=400;
  TCanvas *simul, *effic;
  TLegend *legen;
  if(mbtot > 10) {
    simul = new TCanvas("generEta", "generated Eta", 60, 10, m_LARGPS, m_HAUTPS);
    genVSeta->Draw("E");
    simul->Update();
    sprintf(titre, "generEta%s.png", m_comment);
    simul->Print(titre);
    delete simul;
    effic = new TCanvas("efficMB", "efficMB", 60, 10, m_LARGPS, m_HAUTPS);
    legen = new TLegend(0.2, 0.2, 0.5, 0.3);
    recVSeta->Divide(recVSeta,genVSeta,1.,1.,"B");
    recVSeta->SetMinimum(0.4); recVSeta->SetMaximum(1.1); recVSeta->SetStats(kFALSE);
    recVSeta->Draw("E"); legen->AddEntry(recVSeta, "Muonboy", "p");
    legen->Draw(); effic->Update();
    sprintf(titre, "efficMboy%s.ps", m_comment);
    effic->Print(titre);
    delete effic; delete legen;
    effic = new TCanvas("efficST", "efficST", 60, 10, m_LARGPS, m_HAUTPS);
    legen = new TLegend(0.2, 0.2, 0.5, 0.3);
    staVSeta->Divide(staVSeta,genVSeta,1.,1.,"B");
    mtgVSeta->Divide(mtgVSeta,genVSeta,1.,1.,"B");
    staVSeta->SetMinimum(0.0); staVSeta->SetMaximum(1.1); staVSeta->SetStats(kFALSE);
    mtgVSeta->SetMarkerColor(2);                        mtgVSeta->SetStats(kFALSE);
    staVSeta->Draw("E");     legen->AddEntry(staVSeta, "Staco", "p");
    mtgVSeta->Draw("Esame"); legen->AddEntry(mtgVSeta, "Staco+MuTag", "p"); 
    legen->Draw(); effic->Update();
    sprintf(titre, "efficStac%s.ps", m_comment);
    effic->Print(titre);
    delete effic; delete legen;
    effic = new TCanvas("efficAll", "efficAll", 60, 10, m_LARGPS, m_HAUTPS);
    legen = new TLegend(0.2, 0.2, 0.5, 0.3);
    recVSeta->SetMarkerColor(4); recVSeta->Draw("E");     legen->AddEntry(recVSeta, "Muonboy", "p");
    staVSeta->SetMarkerColor(2); staVSeta->Draw("Esame"); legen->AddEntry(staVSeta, "Staco", "p");
    mtgVSeta->SetMarkerColor(3); mtgVSeta->Draw("Esame"); legen->AddEntry(mtgVSeta, "Staco+MuTag", "p");
    legen->Draw(); effic->Update();
    sprintf(titre, "efficAll%s.ps", m_comment);
    effic->Print(titre);
    delete effic; delete legen;
    effic = new TCanvas("efficMB", "efficMB", 60, 10, m_LARGPS, m_HAUTPS);
    legen = new TLegend(0.5, 0.2, 0.8, 0.3);
    recVSetaPlus->Divide(recVSetaPlus,genVSetaPlus,1.,1.,"B");
    recVSetaPlus->SetMinimum(0.0); recVSetaPlus->SetMaximum(1.1); recVSetaPlus->SetStats(kFALSE);
    recVSetaPlus->Draw("E"); legen->AddEntry(recVSetaPlus, "Muonboy eta>0", "p");
    recVSetaMoin->Divide(recVSetaMoin,genVSetaMoin,1.,1.,"B"); recVSetaMoin->SetStats(kFALSE);
    recVSetaMoin->SetMarkerColor(2); recVSetaMoin->Draw("Esame"); legen->AddEntry(recVSetaMoin, "Muonboy eta<0", "p");
    legen->Draw(); effic->Update();
    sprintf(titre, "efficMboyCompar%s.ps", m_comment);
    effic->Print(titre);
    delete effic; delete legen;     
    effic = new  TCanvas("efficCharge", "efficCharge", 60, 10, m_LARGPS, m_HAUTPS); effic->Divide(2,2);
    effic->cd(1); genVSetaMuP->Draw("E");
    effic->cd(2); genVSetaMuM->SetMarkerColor(2); genVSetaMuM->Draw("E");
    effic->cd(3);
    legen = new TLegend(0.5, 0.2, 0.8, 0.3);
    recVSetaMuP->Divide(recVSetaMuP,genVSetaMuP,1.,1.,"B");
    recVSetaMuP->SetMinimum(0.0); recVSetaMuP->SetMaximum(1.1); recVSetaMuP->SetStats(kFALSE);
    recVSetaMuP->SetMarkerSize(0.6); recVSetaMuP->Draw("E"); legen->AddEntry(recVSetaMuP, "Muonboy mu+", "p");
    recVSetaMuM->Divide(recVSetaMuM,genVSetaMuM,1.,1.,"B"); recVSetaMuM->SetStats(kFALSE);
    recVSetaMuM->SetMarkerColor(2); recVSetaMuM->SetMarkerSize(0.6); recVSetaMuM->Draw("Esame"); legen->AddEntry(recVSetaMuM, "Muonboy mu-", "p");
    legen->Draw();
    effic->Update();
    sprintf(titre, "efficMboyCharge%s.ps", m_comment);
    effic->Print(titre);    
  }
//======================== effic vs phi par tranche de eta ================================================
  int ieta;
  const int npage = 9;
  const int nfi = neta/npage + 1;
  TCanvas *effPhi[neta];
  for (int ifi=0; ifi<nfi; ifi++) {
    sprintf(nomCanvas,"effPhi%u",ifi); 
    effPhi[ifi] = new TCanvas(nomCanvas, nomCanvas,  60, 10, m_LARGPS, m_HAUTPS); effPhi[ifi]->Divide(3,3);
    for (int k=0; k<npage; k++) {
      ieta = ifi*npage + k;
      if(ieta <= neta-1) {
        effPhi[ifi]->cd(1+k);
        rphi[ieta]->Divide(rphi[ieta],gphi[ieta],1.,1.,"B"); rphi[ieta]->SetMinimum(0.0); rphi[ieta]->SetMaximum(1.1);
        rphi[ieta]->SetMarkerSize(0.6); rphi[ieta]->SetMarkerColor(4); rphi[ieta]->SetStats(kFALSE); rphi[ieta]->Draw("E0");
        sphi[ieta]->Divide(sphi[ieta],gphi[ieta],1.,1.,"B");
        sphi[ieta]->SetMarkerSize(0.6); sphi[ieta]->SetMarkerColor(2); sphi[ieta]->Draw("Esame");
        mphi[ieta]->Divide(mphi[ieta],gphi[ieta],1.,1.,"B");
        mphi[ieta]->SetMarkerSize(0.6); mphi[ieta]->SetMarkerColor(3); mphi[ieta]->Draw("Esame");
      }
    }
    effPhi[ifi]->Update();
  }

  sprintf(nom, "efficPhi%s.ps(", m_comment);
  effPhi[0]->Print(nom);
  sprintf(nom, "efficPhi%s.ps", m_comment);
  for (int ifi=1; ifi<nfi-1; ifi++)  effPhi[ifi]->Print(nom);
  sprintf(nom, "efficPhi%s.ps)", m_comment);
  effPhi[nfi-1]->Print(nom);

  TCanvas *etaMutag, *phiMutag, *pttMutag, *mdtMutag, *nfiMutag, *flgMutag;
  etaMutag = new TCanvas("etaMutag", "etaMutag", 180, 10, m_LARGPS, m_HAUTPS);
  etaMutag->Divide(3,4);
  etaMutag->cd( 1); etaMuTagMulBaFake->Draw("E");
  etaMutag->cd( 2); etaMuTagEtaZeFake->Draw("E");
  etaMutag->cd( 3); etaMuTagInnBaFake->Draw("E");
  etaMutag->cd( 4); etaMuTagMulBaMuon->Draw("E");
  etaMutag->cd( 5); etaMuTagEtaZeMuon->Draw("E");
  etaMutag->cd( 6); etaMuTagInnBaMuon->Draw("E");
  etaMutag->cd( 7); etaMuTagMulEcFake->Draw("E");
  etaMutag->cd( 8); etaMuTagMediuFake->Draw("E");
  etaMutag->cd( 9); etaMuTagInnEcFake->Draw("E");
  etaMutag->cd(10); etaMuTagMulEcMuon->Draw("E");
  etaMutag->cd(11); etaMuTagMediuMuon->Draw("E");
  etaMutag->cd(12); etaMuTagInnEcMuon->Draw("E");
  etaMutag->Update();
  etaMutag->Print("mutag.ps(");
  phiMutag = new TCanvas("phiMutag", "phiMutag", 180, 10, m_LARGPS, m_HAUTPS);
  phiMutag->Divide(3,4);
  phiMutag->cd( 1); phiMuTagMulBaFake->Draw("E");
  phiMutag->cd( 2); phiMuTagEtaZeFake->Draw("E");
  phiMutag->cd( 3); phiMuTagInnBaFake->Draw("E");
  phiMutag->cd( 4); phiMuTagMulBaMuon->Draw("E");
  phiMutag->cd( 5); phiMuTagEtaZeMuon->Draw("E");
  phiMutag->cd( 6); phiMuTagInnBaMuon->Draw("E");
  phiMutag->cd( 7); phiMuTagMulEcFake->Draw("E");
  phiMutag->cd( 8); phiMuTagMediuFake->Draw("E");
  phiMutag->cd( 9); phiMuTagInnEcFake->Draw("E");
  phiMutag->cd(10); phiMuTagMulEcMuon->Draw("E");
  phiMutag->cd(11); phiMuTagMediuMuon->Draw("E");
  phiMutag->cd(12); phiMuTagInnEcMuon->Draw("E");
  phiMutag->Update();
  phiMutag->Print("mutag.ps");
  pttMutag = new TCanvas("pttMutag", "pttMutag", 180, 10, m_LARGPS, m_HAUTPS);
  pttMutag->Divide(3,4);
  pttMutag->cd( 1); pttMuTagMulBaFake->Draw("E");
  pttMutag->cd( 2); pttMuTagEtaZeFake->Draw("E");
  pttMutag->cd( 3); pttMuTagInnBaFake->Draw("E");
  pttMutag->cd( 4); pttMuTagMulBaMuon->Draw("E");
  pttMutag->cd( 5); pttMuTagEtaZeMuon->Draw("E");
  pttMutag->cd( 6); pttMuTagInnBaMuon->Draw("E");
  pttMutag->cd( 7); pttMuTagMulEcFake->Draw("E");
  pttMutag->cd( 8); pttMuTagMediuFake->Draw("E");
  pttMutag->cd( 9); pttMuTagInnEcFake->Draw("E");
  pttMutag->cd(10); pttMuTagMulEcMuon->Draw("E");
  pttMutag->cd(11); pttMuTagMediuMuon->Draw("E");
  pttMutag->cd(12); pttMuTagInnEcMuon->Draw("E");
  pttMutag->Update();
  pttMutag->Print("mutag.ps");
  mdtMutag = new TCanvas("mdtMutag", "mdtMutag", 180, 10, m_LARGPS, m_HAUTPS);
  mdtMutag->Divide(3,4);
  mdtMutag->cd( 1); mdtMuTagMulBaFake->Draw("E");
  mdtMutag->cd( 2); mdtMuTagEtaZeFake->Draw("E");
  mdtMutag->cd( 3); mdtMuTagInnBaFake->Draw("E");
  mdtMutag->cd( 4); mdtMuTagMulBaMuon->Draw("E");
  mdtMutag->cd( 5); mdtMuTagEtaZeMuon->Draw("E");
  mdtMutag->cd( 6); mdtMuTagInnBaMuon->Draw("E");
  mdtMutag->cd( 7); mdtMuTagMulEcFake->Draw("E");
  mdtMutag->cd( 8); mdtMuTagMediuFake->Draw("E");
  mdtMutag->cd( 9); mdtMuTagInnEcFake->Draw("E");
  mdtMutag->cd(10); mdtMuTagMulEcMuon->Draw("E");
  mdtMutag->cd(11); mdtMuTagMediuMuon->Draw("E");
  mdtMutag->cd(12); mdtMuTagInnEcMuon->Draw("E");
  mdtMutag->Update();
  mdtMutag->Print("mutag.ps");
  nfiMutag = new TCanvas("nfiMutag", "nfiMutag", 180, 10, m_LARGPS, m_HAUTPS);
  nfiMutag->Divide(3,4);
  nfiMutag->cd( 1); nfiMuTagMulBaFake->Draw("E");
  nfiMutag->cd( 2); nfiMuTagEtaZeFake->Draw("E");
  nfiMutag->cd( 3); nfiMuTagInnBaFake->Draw("E");
  nfiMutag->cd( 4); nfiMuTagMulBaMuon->Draw("E");
  nfiMutag->cd( 5); nfiMuTagEtaZeMuon->Draw("E");
  nfiMutag->cd( 6); nfiMuTagInnBaMuon->Draw("E");
  nfiMutag->cd( 7); nfiMuTagMulEcFake->Draw("E");
  nfiMutag->cd( 8); nfiMuTagMediuFake->Draw("E");
  nfiMutag->cd( 9); nfiMuTagInnEcFake->Draw("E");
  nfiMutag->cd(10); nfiMuTagMulEcMuon->Draw("E");
  nfiMutag->cd(11); nfiMuTagMediuMuon->Draw("E");
  nfiMutag->cd(12); nfiMuTagInnEcMuon->Draw("E");
  nfiMutag->Update();
  nfiMutag->Print("mutag.ps");
  flgMutag = new TCanvas("flgMutag", "flgMutag", 180, 10, m_LARGPS, m_HAUTPS);
  flgMutag->Divide(3,4);
  flgMutag->cd( 1); flgMuTagMulBaFake->Draw("E");
  flgMutag->cd( 2); flgMuTagEtaZeFake->Draw("E");
  flgMutag->cd( 3); flgMuTagInnBaFake->Draw("E");
  flgMutag->cd( 4); flgMuTagMulBaMuon->Draw("E");
  flgMutag->cd( 5); flgMuTagEtaZeMuon->Draw("E");
  flgMutag->cd( 6); flgMuTagInnBaMuon->Draw("E");
  flgMutag->cd( 7); flgMuTagMulEcFake->Draw("E");
  flgMutag->cd( 8); flgMuTagMediuFake->Draw("E");
  flgMutag->cd( 9); flgMuTagInnEcFake->Draw("E");
  flgMutag->cd(10); flgMuTagMulEcMuon->Draw("E");
  flgMutag->cd(11); flgMuTagMediuMuon->Draw("E");
  flgMutag->cd(12); flgMuTagInnEcMuon->Draw("E");
  flgMutag->Update();
  flgMutag->Print("mutag.ps)");
}


int Efficiency::getEtaBin(float eta) {
  int ieta = neta;
  for (int k=0; k<neta; k++)  if(fabs(eta) >= etabin[k] && fabs(eta) < etabin[k+1])  ieta = k;
  return ieta;
}


int Efficiency::getEtaZone(float eta) {
  int izon = nzon;
  for (int k=0; k<nzon; k++)  if(fabs(eta) >= etazon[k] && fabs(eta) < etazon[k+1]) izon = k;
  return izon;
}
