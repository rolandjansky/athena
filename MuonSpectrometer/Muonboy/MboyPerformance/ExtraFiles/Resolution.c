/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Resolution.h"
#include "AANTMisc.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TF1.h"
#include "TDatime.h"
#include "TStyle.h"
#include "TPostScript.h"
#include "TPaveLabel.h"
#include "TPaveText.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TPaveStats.h"
#include "TGraphErrors.h"

const int   nzon = 3;   //  zones en eta pour plots de resolution
const float etazon[nzon+2] = { 0.0, 1.0, 2.0, 2.7, 9.9 };

int   nreso;
TH1F *IPresD[nzon], *MSresD[nzon], *STresD[nzon];
TH1F *IPpulD[nzon], *MSpulD[nzon], *STpulD[nzon];
TH1F *IPresZ[nzon], *MSresZ[nzon], *STresZ[nzon];
TH1F *IPpulZ[nzon], *MSpulZ[nzon], *STpulZ[nzon];
TH1F *IPbiaP[nzon], *MSbiaP[nzon], *STbiaP[nzon];
TH1F *IPresP[nzon], *MSresP[nzon], *STresP[nzon];
TH1F *IPpulP[nzon], *MSpulP[nzon], *STpulP[nzon];
TH1F *IPresF[nzon], *MSresF[nzon], *STresF[nzon];
TH1F *IPpulF[nzon], *MSpulF[nzon], *STpulF[nzon];
TH1F *IPresT[nzon], *MSresT[nzon], *STresT[nzon];
TH1F *IPpulT[nzon], *MSpulT[nzon], *STpulT[nzon];
TProfile *PosVsEtaMS, *NegVsEtaMS, *PosVsEtaIP, *NegVsEtaIP, *AllVsEtaMS, *AllVsPhiMS, *AllVsPhiIP;
TProfile *ForVsPhiMS, *BacVsPhiMS, *AllVsPhiM2, *AllVsPhiM3, *AllVsPhiM4, *AllVsPhiM5;
TH1F *momMBreso[nzon], *momCOreso[nzon], *momSTreso[nzon];
TH2F *errP[nzon], *errcsc, *nbrcsc, *nbocsc;

const int   nzona = 12;   //  zones en eta pour plots de Psim-Prec
const float etazona[nzona+1] = { 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.7, 2.0, 2.3, 2.6, 2.7 };
TProfile *dPMSvsPhi[nzona], *dPIPvsPhi[nzona];

TProfile *d1SPvsP[nzon], *EsSEr[nzon];

FILE *debst;
int npr = 0;

Resolution::Resolution():Analysis() {
//
  m_Debug = kFALSE;
//
  m_nevt = 0;
//
}

Resolution::~Resolution() {}

void Resolution::initialize(char* option) {
//
  printf("Starting Resolution\n");
  if( strstr(option, "debug") != NULL )   m_Debug = kTRUE;

  char titre[256], nom[256];
  nreso = 0;
  errcsc = new TH2F("errcsc", "resolution vs Min(log10(error) on clusters)",100,-2.,2., 100, -3.,5.);
  nbrcsc = new TH2F("nbrcsc", "resolution vs nber of CSC Z clusters",100,-2.,2., 100, -1.,9.);
  nbocsc = new TH2F("nbocsc", "resolution vs nber of unspoiled CSC Z clusters (if >= 4 clusters)",100,-2.,2., 100, -1.,9.);
  for (int izon=0; izon<nzon; izon++) {

    sprintf(nom,   "momMBreso%d", izon);
    sprintf(titre, "reso dP/P IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    momMBreso[izon] = new TH1F(nom,titre,100,-2.0,2.0);
    sprintf(nom,   "momCOreso%d", izon);
    sprintf(titre, "reso dP/P IPcomb (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    momCOreso[izon] = new TH1F(nom,titre,100,-2.0,2.0);
    sprintf(nom,   "momSTreso%d", izon);
    sprintf(titre, "reso dP/P ST (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    momSTreso[izon] = new TH1F(nom,titre,100,-2.0,2.0);
    sprintf(nom,   "errP%d", izon);
    sprintf(titre, "reso vs log10(err 1/P) IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    errP[izon] = new TH2F(nom,titre, 100,-2.0,2.0, 100,-9.,-3.);

    sprintf(nom,   "IPbiaP%d", izon);
    sprintf(titre, "dP/P IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    IPbiaP[izon] = new TH1F(nom,titre,80,-0.40,0.40);
    sprintf(nom,   "IPresP%d", izon);
    sprintf(titre, "sdP/P IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    IPresP[izon] = new TH1F(nom,titre,80,-0.40,0.40);
    sprintf(nom,   "IPresF%d", izon);
    sprintf(titre, "dPhi   IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    IPresF[izon] = new TH1F(nom,titre,80,-0.05,0.05);
    sprintf(nom,   "IPresT%d", izon);
    sprintf(titre, "dTheta IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    IPresT[izon] = new TH1F(nom,titre,80,-0.01,0.01);
    sprintf(nom,   "MSbiaP%d", izon);
    sprintf(titre, "dP/P IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    MSbiaP[izon] = new TH1F(nom,titre,80,-0.40,0.40);
    sprintf(nom,   "MSresP%d", izon);
    sprintf(titre, "sdP/P MS (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    MSresP[izon] = new TH1F(nom,titre,80,-0.40,0.40);
    sprintf(nom,   "MSresF%d", izon);
    sprintf(titre, "dPhi   MS (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    MSresF[izon] = new TH1F(nom,titre,80,-0.05,0.05);
    sprintf(nom,   "MSresT%d", izon);
    sprintf(titre, "dTheta  MS (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    MSresT[izon] = new TH1F(nom,titre,80,-0.01,0.01); 
    sprintf(nom,   "IPpulP%d", izon);
    sprintf(titre, "pull 1/P IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    IPpulP[izon] = new TH1F(nom,titre,80,-5.0,5.0);
    sprintf(nom,   "IPpulF%d", izon);
    sprintf(titre, "pull Phi IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    IPpulF[izon] = new TH1F(nom,titre,80,-5.0,5.0);
    sprintf(nom,   "IPpulT%d", izon);
    sprintf(titre, "pull Theta IP (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    IPpulT[izon] = new TH1F(nom,titre,80,-5.0,5.0);
    sprintf(nom,   "MSpulP%d", izon);
    sprintf(titre, "pull 1/P MS (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    MSpulP[izon] = new TH1F(nom,titre,80,-5.0,5.0);
    sprintf(nom,   "MSpulF%d", izon);
    sprintf(titre, "pull Phi MS (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    MSpulF[izon] = new TH1F(nom,titre,80,-5.0,5.0);
    sprintf(nom,   "MSpulT%d", izon);
    sprintf(titre, "pull Theta MS (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    MSpulT[izon] = new TH1F(nom,titre,80,-5.0,5.0);
    sprintf(nom,   "STbiaP%d", izon);
    sprintf(titre, "dP/P ST (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    STbiaP[izon] = new TH1F(nom,titre,80,-0.40,0.40);
    sprintf(nom,   "STresP%d", izon);
    sprintf(titre, "sdP/P ST (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    STresP[izon] = new TH1F(nom,titre,80,-0.40,0.40);
    sprintf(nom,   "STresF%d", izon);
    sprintf(titre, "dPhi   ST (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    STresF[izon] = new TH1F(nom,titre,80,-0.002,0.002);
    sprintf(nom,   "STresT%d", izon);
    sprintf(titre, "dTheta  ST (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    STresT[izon] = new TH1F(nom,titre,80,-0.005,0.005);
    sprintf(nom,   "STpulP%d", izon);
    sprintf(titre, "pull 1/P ST (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    STpulP[izon] = new TH1F(nom,titre,80,-5.0,5.0);
    sprintf(nom,   "STpulF%d", izon);
    sprintf(titre, "pull Phi ST (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    STpulF[izon] = new TH1F(nom,titre,80,-5.0,5.0);
    sprintf(nom,   "STpulT%d", izon);
    sprintf(titre, "pull Theta ST (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment);
    STpulT[izon] = new TH1F(nom,titre,80,-5.0,5.0);
  }

// histos speciaux pour Muonboy entree du spectro
  PosVsEtaMS = new TProfile("PosVsEtaMS", "Psim-Prec MS vs Eta", 30, -3., 3., " "); 
  NegVsEtaMS = new TProfile("NegVsEtaMS", "Psim-Prec MS vs Eta", 30, -3., 3., " ");
  AllVsEtaMS = new TProfile("AllVsEtaMS", "Psim-Prec MS vs |Eta|", 15, 0., 3., " ");
  PosVsEtaIP = new TProfile("PosVsEtaIP", "Psim-Prec IP vs Eta", 30, -3., 3., " ");
  NegVsEtaIP = new TProfile("NegVsEtaIP", "Psim-Prec IP vs Eta", 30, -3., 3., " ");
  AllVsPhiMS = new TProfile("AllVsPhiMS", "Psim-Prec MS vs Phi 2.5<|eta|<2.7", 20,  0.,45, " ");
  AllVsPhiM2 = new TProfile("AllVsPhiM2", "Psim-Prec MS vs Phi 1.4<|eta|<1.7", 20,  0.,45, " ");
  AllVsPhiM3 = new TProfile("AllVsPhiM3", "Psim-Prec MS vs Phi 2.5<|eta|",     20,  0.,45, " ");
  AllVsPhiM4 = new TProfile("AllVsPhiM4", "Psim-Prec MS vs Phi 2.0<|eta|<2.6", 45,  -180.,180, " ");
  AllVsPhiM5 = new TProfile("AllVsPhiM5", "Psim-Prec MS vs Phi 1.4<|eta|<1.7", 45,  -180.,180, " ");
  AllVsPhiIP = new TProfile("AllVsPhiIP", "Psim-Prec IP vs Phi |eta|>2.", 20,  0.,45., " ");
  ForVsPhiMS = new TProfile("ForVsPhiMS", "Psim-Prec MS vs Phi eta>2.",   20,  0.,45., " ");
  BacVsPhiMS = new TProfile("BacVsPhiMS", "Psim-Prec MS vs Phi eta<-2.",  20,  0.,45., " ");

  for (int izon=0; izon<nzona; izon++) {
    sprintf(nom,   "dPMSvsPhi%d", izon);
    sprintf(titre, "Psim-Prec MB (%.1f<abs(eta)<%.1f) %s", etazona[izon], etazona[izon+1], m_comment);
    dPMSvsPhi[izon] = new TProfile(nom, titre, 45, -180., 180., " ");
    sprintf(nom,   "dPIPvsPhi%d", izon);
    sprintf(titre, "Psim-Prec IP (%.1f<abs(eta)<%.1f) %s", etazona[izon], etazona[izon+1], m_comment);
    dPIPvsPhi[izon] = new TProfile(nom, titre, 45, -180., 180., " ");
  }

  sprintf(titre, "dump.Staco%s", m_comment);
  debst = fopen(titre, "w");

// histos eloss
  for (int izon=0; izon<nzon; izon++) {
    sprintf(nom, "d1SPvsP%d", izon);
    sprintf(titre,"d(1/P) vs P (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment); 
    d1SPvsP[izon] = new TProfile(nom, titre, 20, 0., 600., " ");
    sprintf(nom, "EsimSErecVsP%d", izon);
    sprintf(titre,"Esim/Erec vs P (%.1f<abs(eta)<%.1f) %s", etazon[izon], etazon[izon+1], m_comment); 
    EsSEr[izon] = new TProfile(nom, titre, 20, 0., 600., " ");
  }
}

void Resolution::execute() {
//
  m_nevt = m_nevt + 1;
  
  int isim, izon;
  float eta, dP;
  Assoc* asso;
  for (int irec=0; irec<p_evt->MboyTrkMB_NTrk; irec++) {
    asso = p_assocMS;
    isim = asso->getSim(irec);
    if( isim >= 0 ) {
      eta = asso->getEtaSim(isim);
      izon = getEtaZone(eta);
      if(izon < nzon) {
        MSbiaP[izon]->Fill( asso->getDeltaPSurPnoSign(irec,isim) );  
        MSresP[izon]->Fill( asso->getDeltaPSurP(irec,isim) );
        MSpulP[izon]->Fill( asso->getPullUnSurP(irec,isim) );
        MSresF[izon]->Fill( asso->getDeltaPhi(irec,isim) );
        MSpulF[izon]->Fill( asso->getPullPhi(irec,isim) );
        MSresT[izon]->Fill( asso->getDeltaTheta(irec,isim) );
        MSpulT[izon]->Fill( asso->getPullTheta(irec,isim) );
        if( p_evt->MboyTrkMB_NTrk == 1 ) {
          if( fabs(asso->getPullPhi(irec,isim)) > 20. || fabs(asso->getPullTheta(irec,isim)) > 20. ) {
            cout << " probleme recons " << p_evt->Run << " " << p_evt->Event << " " << eta << " " << asso->getPullPhi(irec,isim) 
                                                      << " " << asso->getPullTheta(irec,isim) << endl;
          }
        }
        int ismb = p_assocMB->getSim(irec);
        if( ismb >= 0 ) {
          float Erec = fabs(1./p_assocMB->getChargeSurPRec(irec)) - fabs(1./p_assocMS->getChargeSurPRec(irec));
          float Esim = fabs(1./p_assocMB->getChargeSurPSim(ismb)) - fabs(1./p_assocMS->getChargeSurPSim(isim));
          EsSEr[izon]->Fill( fabs(0.001/p_assocMB->getChargeSurPSim(ismb)), Esim/Erec );
        }
      }
      if(asso->isMuonDirect(isim) && asso->getNrec() == 1) { // necessaire quand assoc relachee
//        dP = asso->getDeltaP(irec,isim)/1000.;        
        dP = ( asso->getChargeSurPRec(irec) - asso->getChargeSurPSim(isim) ) / 1000.;
        if( asso->getChargeSurPSim(isim) < 0. )  dP = -dP;
        dP = dP / asso->getChargeSurPSim(isim) / asso->getChargeSurPSim(isim);
        AllVsEtaMS->Fill(fabs(eta), dP);
        if(asso->getChargeSurPSim(isim) > 0.)  PosVsEtaMS->Fill(eta, dP);
        else                                   NegVsEtaMS->Fill(eta, dP);
        if( fabs(eta) > 2.0 && fabs(eta) < 2.5 ) {
          float phisec = asso->getPhiSim(isim)*180./m_PI;
          AllVsPhiM4->Fill(phisec, dP);
          phisec = phisec + 180.;
          while( phisec >= 45.)  phisec = phisec - 45.;
          AllVsPhiMS->Fill(phisec, dP);
          if( eta  >= 0.)    ForVsPhiMS->Fill(phisec, dP);
          else               BacVsPhiMS->Fill(phisec, dP);
        } else if( fabs(eta) > 1.4 && fabs(eta) < 1.7 ) {
          float phisec = asso->getPhiSim(isim)*180./m_PI;
          AllVsPhiM5->Fill(phisec, dP);
          phisec = phisec + 180.;
          while( phisec >= 45.)  phisec = phisec - 45.;
          AllVsPhiM2->Fill(phisec, dP);
        } else if( fabs(eta) > 2.5 ) {
          float phisec = 180. + asso->getPhiSim(isim)*180./m_PI;
          while( phisec >= 45.)  phisec = phisec - 45.;
          AllVsPhiM3->Fill(phisec, dP);
        }
        for (int izon=0; izon<nzona; izon++) {
          if( fabs(eta) >= etazona[izon] && fabs(eta) < etazona[izon+1] ) {
            dPMSvsPhi[izon]->Fill(asso->getPhiSim(isim)*180./m_PI, dP);
          }
        }
      }
    }
    asso = p_assocMB;
    isim = asso->getSim(irec);
    if(isim >= 0) {
      eta = asso->getEtaSim(isim);
      if(asso->isMuonDirect(isim) && asso->getNrec() == 1) { // necessaire quand assoc relachee
//        dP = asso->getDeltaP(irec,isim)/1000.;        
        dP = ( asso->getChargeSurPRec(irec) - asso->getChargeSurPSim(isim) ) / 1000.;
        if( asso->getChargeSurPSim(isim) < 0. )  dP = -dP;
        dP = dP / asso->getChargeSurPSim(isim) / asso->getChargeSurPSim(isim);
        if( asso->getChargeSurPSim(isim) > 0. )  PosVsEtaIP->Fill(eta, dP);
        else                                     NegVsEtaIP->Fill(eta, dP);
        if( fabs(eta) > 2. ) {
          float phisec = 180. + asso->getPhiSim(isim)*180./m_PI;
          while( phisec >= 45.)  phisec = phisec - 45.;
//          if( phisec > 22.5 )    phisec = 45. - phisec;
          AllVsPhiIP->Fill(phisec, dP);
        }
        for (int izon=0; izon<nzona; izon++) {
          if( fabs(eta) >= etazona[izon] && fabs(eta) < etazona[izon+1] ) {
            dPIPvsPhi[izon]->Fill(asso->getPhiSim(isim)*180./m_PI, dP);
          }
        }
      }
    }
  }

  bool aGarder;
  int irmb, irid;
  for (int irec=0; irec<p_assocST->getNrec(); irec++) {
    isim = p_assocST->getSim(irec);
    if(isim >= 0) {
      eta = p_assocST->getEtaSim(isim);
      izon = getEtaZone(eta);
      if(izon < nzon) {
        aGarder = false;
        for (int iid=0; iid<p_evt->Trk_totalNumTracks && iid<500; iid++) {  // bug avec ID ?? limité a 500 traces
          if(fabs((p_evt->Trk_theta)->at(iid)-(p_evt->staco_ThetaID)->at(irec))<0.00001 &&
             fabs((p_evt->Trk_phi)->at(iid)-(p_evt->staco_PhiID)->at(irec))<0.00001) {
            for (int imb=0; imb<p_assocMB->getNrec(); imb++) {
              if(fabs(p_assocMB->getThetaRec(imb)-(p_evt->staco_ThetaMS)->at(irec))<0.00001 &&
                 fabs(p_assocMB->getPhiRec(imb)-(p_evt->staco_PhiMS)->at(irec))<0.00001) {
                aGarder = true;
                irmb = imb;
                irid = iid;
              }
            }
            if( !aGarder ) {    // ancien samples = bug staco inversion MS<->MB
              cout << " sample avec bug ???" << endl;
              for (int imb=0; imb<p_assocMS->getNrec(); imb++) {
                if(fabs(p_assocMS->getThetaRec(imb)-(p_evt->staco_ThetaMS)->at(irec))<0.00001 &&
                   fabs(p_assocMS->getPhiRec(imb)-(p_evt->staco_PhiMS)->at(irec))<0.00001) {
                  aGarder = true;
                  irmb = imb;
                  irid = iid;
                }
              }
            }
          }
        }
        if(aGarder) {
          STbiaP[izon]->Fill( p_assocST->getDeltaPSurPnoSign(irec,isim) );
          STresP[izon]->Fill( p_assocST->getDeltaPSurP(irec,isim) );
          STpulP[izon]->Fill( p_assocST->getPullUnSurP(irec,isim) );
          STresF[izon]->Fill( p_assocST->getDeltaPhi(irec,isim) );
          STpulF[izon]->Fill( p_assocST->getPullPhi(irec,isim) );
          STresT[izon]->Fill( p_assocST->getDeltaTheta(irec,isim) );
          STpulT[izon]->Fill( p_assocST->getPullTheta(irec,isim) );
          IPbiaP[izon]->Fill( p_assocMB->getDeltaPSurPnoSign(irmb,isim) );
          IPresP[izon]->Fill( p_assocMB->getDeltaPSurP(irmb,isim) );
          IPpulP[izon]->Fill( p_assocMB->getPullUnSurP(irmb,isim) );
          IPresF[izon]->Fill( p_assocMB->getDeltaPhi(irmb,isim) );
          IPpulF[izon]->Fill( p_assocMB->getPullPhi(irmb,isim) );
          IPresT[izon]->Fill( p_assocMB->getDeltaTheta(irmb,isim) );
          IPpulT[izon]->Fill( p_assocMB->getPullTheta(irmb,isim) );
          if( fabs(p_assocMB->getPullUnSurP(irmb,isim)) < 5. ) {
            float P2d1SP = p_assocMB->getChargeSurPRec(irmb) - p_assocMB->getChargeSurPSim(isim);
            if( p_assocMB->getChargeSurPSim(isim) < 0. ) P2d1SP = -P2d1SP;
            P2d1SP = P2d1SP / ( p_assocMB->getChargeSurPSim(isim) * p_assocMB->getChargeSurPSim(isim) );
            d1SPvsP[izon]->Fill( fabs(0.001/p_assocMB->getChargeSurPSim(isim)), 0.001*P2d1SP);
          } 
          nreso = nreso + 1;
        } // else  cout << " Warning aGarder " << aGarder << endl;
      }
    }
  }

  float ptref = (p_evt->PtGen)->at(0);
  for (int irec=0; irec<p_assocST->getNrec(); irec++) {
    eta = p_assocST->getEtaRec(irec);
    izon = getEtaZone(eta);
    if(izon < nzon) {
      aGarder = false;
      for (int imb=0; imb<p_assocMB->getNrec(); imb++) {
        if(fabs(p_assocMB->getThetaRec(imb)-(p_evt->staco_ThetaMS)->at(irec))<0.00001 &&
           fabs(p_assocMB->getPhiRec(imb)-(p_evt->staco_PhiMS)->at(irec))<0.00001) {
          for (int iid=0; iid<p_evt->Trk_totalNumTracks && iid<500; iid++) {
            if(fabs((p_evt->Trk_theta)->at(iid)-(p_evt->staco_ThetaID)->at(irec))<0.00001 &&
               fabs((p_evt->Trk_phi)->at(iid)-(p_evt->staco_PhiID)->at(irec))<0.00001) {
              aGarder = true;
              irmb = imb;
              irid = iid;
            }
          }
        }
      }
    }
  }

// histos sans coupures
  for (int irec=0; irec<p_assocST->getNrec(); irec++) {
    isim = p_assocST->getSim(irec);
    if(isim >= 0) {
      eta = p_assocST->getEtaSim(isim);
      izon = getEtaZone(eta);
      if(izon < nzon) {
        momSTreso[izon]->Fill( p_assocST->getDeltaPSurP(irec,isim) );
        for (int imb=0; imb<p_assocMB->getNrec(); imb++) {
          if(fabs(p_assocMB->getThetaRec(imb)-(p_evt->staco_ThetaMS)->at(irec))<0.00001 &&
             fabs(p_assocMB->getPhiRec(imb)-(p_evt->staco_PhiMS)->at(irec))<0.00001) {
            momCOreso[izon]->Fill( p_assocMB->getDeltaPSurP(imb,isim) );
          }
        }
      }
    }
  }
  for (int irec=0; irec<p_assocMB->getNrec(); irec++) {
    isim = p_assocMB->getSim(irec);
    if(isim >= 0 ) {
      eta = p_assocMB->getEtaSim(isim);
      izon = getEtaZone(eta);
      if(izon < nzon) {
        momMBreso[izon]->Fill( p_assocMB->getDeltaPSurP(irec,isim) );
        errP[izon]->Fill( p_assocMB->getDeltaPSurP(irec,isim) , log10( sqrt((p_evt->MboyESD_covr55)->at(irec)) ) );
      }
      if( izon == 2 ) {
        float errmin = 10000.;
        float nnnn = 0.;
        float nbon = 0.;
        int ndig = (p_evt->MboyTrkVT_digits)->at(irec);
        int fdig = (p_evt->MboyTrkVT_HitPosition)->at(irec) - 1;
        for (int i=fdig; i<fdig+ndig; i++) {
          int itec = (p_evt->MboyTrkVT_TrHi_ChamberTech)->at(i) - 1;
          if( itec == 5 && (p_evt->MboyTrkVT_TrHi_MeasuresPhi)->at(i) == 0 ) {   // CSC z cluster
            nnnn = nnnn + 1.;
            if( (p_evt->MboyTrkVT_TrHi_EZSfit)->at(i) < 0.2 ) nbon = nbon + 1;
            if( (p_evt->MboyTrkVT_TrHi_EZSfit)->at(i) < errmin ) errmin = (p_evt->MboyTrkVT_TrHi_EZSfit)->at(i);
          }
        }
        errcsc->Fill( p_assocMB->getDeltaPSurP(irec,isim) , log10(errmin) ); 
        nbrcsc->Fill( p_assocMB->getDeltaPSurP(irec,isim) , nnnn );
        if( nnnn > 3. ) nbocsc->Fill( p_assocMB->getDeltaPSurP(irec,isim) , nbon );
      }
    }
  }
}

void Resolution::finalize() {
//

  char titre[256], nom[256];
  int ipos;
  FILE *txtFile;
  gStyle->SetOptStat(110010); gStyle->SetOptFit(11);
  TCanvas *resoIP, *resoMS, *resoST, *pullIP, *pullMS, *pullST;
  TCanvas *canvasMS, *canvasIP, *biaisP, *MSdPvsPhi1, *MSdPvsPhi2, *IPdPvsPhi1, *IPdPvsPhi2;
  TCanvas *toto, *eloss, *resol, *errp, *ercsc;
  TLine *line;

  fclose(debst);


  canvasMS = new TCanvas("canvasMS", "canvasMS", 180, 10, m_LARGPS, m_HAUTPS); canvasMS->Divide(2,2);
  canvasMS->cd(1); AllVsEtaMS->SetMinimum(-2.); AllVsEtaMS->SetMaximum(4.); AllVsEtaMS->SetStats(kFALSE); AllVsEtaMS->Draw("E");
  line = new TLine( 0.,0., 3.,0.); line->SetLineStyle(2); line->Draw(); 
  canvasMS->cd(2); AllVsPhiM2->SetMinimum(-4.); AllVsPhiM2->SetMaximum(4.); AllVsPhiM2->SetStats(kFALSE); AllVsPhiM2->Draw("E");
  line = new TLine( 0.,0., 45.,0.); line->SetLineStyle(2); line->Draw();
  canvasMS->cd(3); AllVsPhiMS->SetMinimum(-2.); AllVsPhiMS->SetMaximum(4.); AllVsPhiMS->SetStats(kFALSE); AllVsPhiMS->Draw("E");
  line = new TLine( 0.,0., 45.,0.); line->SetLineStyle(2); line->Draw();
  canvasMS->cd(4); AllVsPhiM3->SetMinimum(-4.); AllVsPhiM3->SetMaximum(4.); AllVsPhiM3->SetStats(kFALSE); AllVsPhiM3->Draw("E");
  line = new TLine( 0.,0., 45.,0.); line->SetLineStyle(2); line->Draw();
  canvasMS->Update();

  canvasIP = new TCanvas("canvasIP", "canvasIP", 180, 10, m_LARGPS, m_HAUTPS); canvasIP->Divide(1,2);
  canvasIP->cd(1); PosVsEtaIP->Draw("E"); NegVsEtaIP->SetMarkerColor(2); NegVsEtaIP->Draw("Esame");
  canvasIP->cd(2); AllVsPhiIP->SetMinimum(-2.); AllVsPhiIP->SetMaximum(4.); AllVsPhiIP->Draw("E");
  canvasIP->Update();

  int nnn = (nzona+1)/2;
  MSdPvsPhi1 = new TCanvas("MSdPvsPhi1", "MSdPvsPhi1", 180, 10, m_LARGPS, m_HAUTPS); MSdPvsPhi1->Divide(2,3);
  MSdPvsPhi2 = new TCanvas("MSdPvsPhi2", "MSdPvsPhi2", 180, 10, m_LARGPS, m_HAUTPS); MSdPvsPhi2->Divide(2,3);
  toto = MSdPvsPhi1;
  int j=0;
  for (int i=0; i<nzona; i++) {
    if( i == 6 ) { toto = MSdPvsPhi2; j = 0; }
    toto->cd(j+1); dPMSvsPhi[i]->SetMarkerSize(0.6);
    if( i <= 5 )       { dPMSvsPhi[i]->SetMinimum(-0.4); dPMSvsPhi[i]->SetMaximum(+1.0); }
    else if( i == 6  ) { dPMSvsPhi[i]->SetMinimum(-1.0); dPMSvsPhi[i]->SetMaximum(+1.5); }
    else if( i == 7  ) { dPMSvsPhi[i]->SetMinimum(-1.5); dPMSvsPhi[i]->SetMaximum(+1.0); }
    else if( i == 8  ) { dPMSvsPhi[i]->SetMinimum(-0.4); dPMSvsPhi[i]->SetMaximum(+1.0); }
    else if( i == 9  ) { dPMSvsPhi[i]->SetMinimum(-0.4); dPMSvsPhi[i]->SetMaximum(+1.0); }
    else if( i == 10 ) { dPMSvsPhi[i]->SetMinimum(-0.4); dPMSvsPhi[i]->SetMaximum(+1.0); }
    else               { dPMSvsPhi[i]->SetMinimum(-1.5); dPMSvsPhi[i]->SetMaximum(+2.0); }
    dPMSvsPhi[i]->Draw("E0"); gStyle->SetOptStat(10);
    j++;
  }
  MSdPvsPhi1->Update();
  MSdPvsPhi2->Update();
  IPdPvsPhi1 = new TCanvas("IPdPvsPhi1", "IPdPvsPhi1", 180, 10, m_LARGPS, m_HAUTPS); IPdPvsPhi1->Divide(2,3);
  IPdPvsPhi2 = new TCanvas("IPdPvsPhi2", "IPdPvsPhi2", 180, 10, m_LARGPS, m_HAUTPS); IPdPvsPhi2->Divide(2,3);
  toto = IPdPvsPhi1;
  j = 0;
  for (int i=0; i<nzona; i++) {
    if( i == 6 ) { toto = IPdPvsPhi2; j = 0; }
    toto->cd(j+1);      dPIPvsPhi[i]->SetMarkerSize(0.6);
    if( i <= 5 )      { dPIPvsPhi[i]->SetMinimum(-0.5); dPIPvsPhi[i]->SetMaximum(+1.0); }
    else              { dPIPvsPhi[i]->SetMinimum(-3.0); dPIPvsPhi[i]->SetMaximum(+2.0); }
    dPIPvsPhi[i]->Draw("E0"); gStyle->SetOptStat(10);
    j++;
  }
  IPdPvsPhi1->Update();
  IPdPvsPhi2->Update();
  gStyle->SetPalette(1);

  if( nreso > 10 ) {

    biaisP = new TCanvas("biaisP", "biaisP", 180, 10, m_LARGPS, m_HAUTPS);
    sprintf(titre, "dump.biaisP%s", m_comment);
    txtFile = fopen(titre,"w");
    biaisP->Divide(nzon,2); ipos = 0;
    for (int k=0; k<nzon; k++) { ipos++; biaisP->cd(ipos); fitHist(IPbiaP[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; biaisP->cd(ipos); fitHist(STbiaP[k], txtFile); }
    fclose(txtFile);
    biaisP->Update();

    resoIP = new TCanvas("resoIP", "resoIP", 180, 10, m_LARGPS, m_HAUTPS);
    sprintf(titre, "dump.resoIP%s", m_comment);
    txtFile = fopen(titre,"w");
    resoIP->Divide(nzon,3); ipos = 0;
    for (int k=0; k<nzon; k++) { ipos++; resoIP->cd(ipos); fitHist(IPresP[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; resoIP->cd(ipos); fitHist(IPresF[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; resoIP->cd(ipos); fitHist(IPresT[k], txtFile); }
    fclose(txtFile);
    resoIP->Update();

    resoMS = new TCanvas("resoMS", "resoMS", 180, 10, m_LARGPS, m_HAUTPS);
    sprintf(titre, "dump.resoMS%s", m_comment);
    txtFile = fopen(titre,"w");
    resoMS->Divide(nzon,3); ipos = 0;
    for (int k=0; k<nzon; k++) { ipos++; resoMS->cd(ipos); fitHist(MSresP[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; resoMS->cd(ipos); fitHist(MSresF[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; resoMS->cd(ipos); fitHist(MSresT[k], txtFile); }
    fclose(txtFile);
    resoMS->Update();

    resoST = new TCanvas("resoST", "resoST", 180, 10, m_LARGPS, m_HAUTPS);
    sprintf(titre, "dump.resoST%s", m_comment);
    txtFile = fopen(titre,"w");
    resoST->Divide(nzon,3); ipos = 0;
    for (int k=0; k<nzon; k++) { ipos++; resoST->cd(ipos); fitHist(STresP[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; resoST->cd(ipos); fitHist(STresF[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; resoST->cd(ipos); fitHist(STresT[k], txtFile); }
    fclose(txtFile);
    resoST->Update();

    pullIP = new TCanvas("pullIP", "pullIP", 180, 10, m_LARGPS, m_HAUTPS);
    gStyle->SetOptStat(111110);
    sprintf(titre, "dump.PullIP%s", m_comment);
    txtFile = fopen(titre,"w");
    pullIP->Divide(nzon,3); ipos = 0;
    for (int k=0; k<nzon; k++) { ipos++; pullIP->cd(ipos); fitHist(IPpulP[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; pullIP->cd(ipos); fitHist(IPpulF[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; pullIP->cd(ipos); fitHist(IPpulT[k], txtFile); }
    fclose(txtFile);
    pullIP->Update();

    pullMS = new TCanvas("pullMS", "pullMS", 180, 10, m_LARGPS, m_HAUTPS);
    gStyle->SetOptStat(111110);
    sprintf(titre, "dump.PullMS%s", m_comment);
    txtFile = fopen(titre,"w");
    pullMS->Divide(nzon,3); ipos = 0;
    for (int k=0; k<nzon; k++) { ipos++; pullMS->cd(ipos); fitHist(MSpulP[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; pullMS->cd(ipos); fitHist(MSpulF[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; pullMS->cd(ipos); fitHist(MSpulT[k], txtFile); }
    fclose(txtFile);
    pullMS->Update();

    pullST = new TCanvas("pullST", "pullST", 180, 10, m_LARGPS, m_HAUTPS);
    sprintf(titre, "dump.PullST%s", m_comment);
    txtFile = fopen(titre,"w");
    pullST->Divide(nzon,3); ipos = 0;
    for (int k=0; k<nzon; k++) { ipos++; pullST->cd(ipos); fitHist(STpulP[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; pullST->cd(ipos); fitHist(STpulF[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; pullST->cd(ipos); fitHist(STpulT[k], txtFile); }
    fclose(txtFile);
    pullST->Update();

    errp = new TCanvas("errp", "errp", 180, 10, m_LARGPS, m_HAUTPS);
    errp->Divide(2,2); ipos = 0;
    for (int k=0; k<nzon; k++) { ipos++; errp->cd(ipos); errP[k]->Draw(); }
    errp->Update();

    ercsc = new TCanvas("ercsc", "ercsc", 180, 10, m_LARGPS, m_HAUTPS);
    ercsc->Divide(1,2);
//    ercsc->cd(1); errcsc->SetMarkerSize(0.4); errcsc->Draw();
    ercsc->cd(1); nbrcsc->SetMarkerSize(0.4); nbrcsc->Draw();
    ercsc->cd(2); nbocsc->SetMarkerSize(0.4); nbocsc->Draw();
    ercsc->Update();

    resol = new TCanvas("resol", "resol", 180, 10, m_LARGPS, m_HAUTPS);
    sprintf(titre, "dump.resol%s", m_comment);
    txtFile = fopen(titre,"w");
    resol->Divide(nzon,3); ipos = 0;
    for (int k=0; k<nzon; k++) { ipos++; resol->cd(ipos); fitHist(momMBreso[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; resol->cd(ipos); fitHist(momCOreso[k], txtFile); }
    for (int k=0; k<nzon; k++) { ipos++; resol->cd(ipos); fitHist(momSTreso[k], txtFile); }
    fclose(txtFile);
    resol->Update();

    eloss = new TCanvas("eloss", "eloss", 180, 10, m_LARGPS, m_HAUTPS);
    eloss->Divide(2,2); ipos = 0;
//    for (int k=0; k<nzon; k++) { ipos++; eloss->cd(ipos); d1SPvsP[k]->Draw("E"); }
    for (int k=0; k<nzon; k++) { ipos++; eloss->cd(ipos); EsSEr[k]->Draw("E"); }
    eloss->Update();

    sprintf(titre, "resol%s.ps(", m_comment); canvasMS->Print(titre);
    sprintf(titre, "resol%s.ps",  m_comment); canvasIP->Print(titre);
                                              MSdPvsPhi1->Print(titre);
                                              MSdPvsPhi2->Print(titre);
                                              IPdPvsPhi1->Print(titre);
                                              IPdPvsPhi2->Print(titre);
                                              biaisP->Print(titre);
                                              resoMS->Print(titre);
                                              resoIP->Print(titre);
                                              resoST->Print(titre);
                                              pullMS->Print(titre);
                                              pullIP->Print(titre);
                                              pullST->Print(titre);
					        errp->Print(titre);
					       ercsc->Print(titre);
    sprintf(titre, "resol%s.ps)", m_comment);  resol->Print(titre);

    sprintf(titre, "eloss%s.ps", m_comment);  eloss->Print(titre);

// Writeout histos in Root File
    char  HistFile[80];
    sprintf(HistFile, "resol%s.root", m_comment);
    TFile *pHistFile;
    pHistFile = new TFile(HistFile,"RECREATE");
    canvasMS->Write();
    canvasIP->Write();
    biaisP->Write();
    resoMS->Write();
    resoIP->Write();
    resoST->Write();
    pullMS->Write();
    pullIP->Write();
    pullST->Write();
    pHistFile->Close();
 
  }

}


void Resolution::fitHist(TH1F *histo, FILE *File)
{
  double v1 = histo->GetMean(); 
  double v2 = histo->GetRMS();
  double vmax1 = v1 +2.5*v2; 
  double vmin1 = v1 -2.5*v2;
  double delta = 9999.;
  
  int icount=0;
  while ( (delta > 0.001) && (icount < 100) ) { 
    histo->Fit("gaus","QR","0", vmin1, vmax1);

    TF1 *fun1 = histo->GetFunction("gaus");
    double par1 = fun1->GetParameter(1); 
    double par2 = fun1->GetParameter(2); 

    delta = fabs(par2-v2);
    if( par2 > 0. ) delta = delta/par2;
    vmax1 = par1 +2.*par2; 
    vmin1 = par1 -2.*par2;
    
    v2 = par2;
    icount++;
      
  } 
  //  histo->SetFillColor(kYellow);
  //  histo->Fit("gaus","R"," ", vmin1, vmax1); 
  //  histo->Draw("same");
  histo->Fit("gaus","QR"," ", vmin1, vmax1);
  histo->Draw("");
  
  TF1 *fun1 = histo->GetFunction("gaus");
  double par10 = fun1->GetParameter(0); 
  double par14 = fun1->GetParameter(1); 
  double par24 = fun1->GetParameter(2);
  double par20 = fun1->GetParError(0); 
  double par34 = fun1->GetParError(1); 
  double par44 = fun1->GetParError(2);  
 
  fprintf (File,"%d %f\t%f\t%f\t%f\t%f\t%f\n", p_evt->Run, par14, par34, par24, par44, par10, par20);

}


int Resolution::getEtaZone(float eta) {
  int izon = nzon;
  for (int k=0; k<nzon; k++)  if(fabs(eta) >= etazon[k] && fabs(eta) < etazon[k+1])  izon = k;
  return izon;
}
