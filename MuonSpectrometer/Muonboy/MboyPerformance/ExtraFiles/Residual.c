/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Residual.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TPaveStats.h"
#include <iostream>
#include "TStyle.h"
#include "TString.h"

TH1F *Rreco[6];
TH1F *Rsimu[6];
TH1F *Refit[6];
TH1F *Rresi[6];
TH1F *Rpull[6], *RpullBon[6], *RpullBad[6], *RpullB[6], *RpullE[6];
TH1F *RresiTEC[61], *RpullTEC[61];
TH1F *RresiBOL[9], *RresiBOS[9], *RresiBML[9], *RresiBMS[9];
TH1F *RresiEOL[9], *RresiEOS[9], *RresiEML[9], *RresiEMS[9];
TH1F *RresiBOLAll, *RresiBOSAll, *RresiEOLAll, *RresiEOSAll;
TH1F *RresiRPCeta, *RresiRPCphi, *RresiTGCeta, *RresiTGCphi;
TH1F *problemTGCtyp, *problemTGCeta, *problemTGCphi, *problemTGCgap;
TH1F *RJTYP;
const TString station[61] = { "000",
                              "BIL", "BIS", "BML", "BMS", "BOL", "BOS", "BEE", "BIR", "BMF", "BOF",
                              "BOG", "BOH", "EIC", "EIL", "EEL", "EES", "EMC", "EML", "EMS", "EOC",
                              "EOL", "EOS", "T1C", "T1L", "T1S", "T2C", "T2L", "T2S", "T3C", "T3L",
                              "T3S", "CI1", "CI2", "CI3", "CI4", "FIL", "FIS", "FML", "FMS", "FOL",
                              "FOS", "T1F", "T1E", "T2F", "T2E", "T3F", "T3E", "T4F", "T4E", "EIS",
                              "CSS", "CSL", "BIM", "054", "055", "056", "057", "058", "059", "060" };

TH2F *ResVsRadB, *ResVsRadE;
TH2F *SveriBML, *SveriBMS, *SveriBOL, *SveriBOS, *SveriEML, *SveriEMS, *SveriEOL, *SveriEOS;

Residual::Residual():Analysis() {}

Residual::~Residual() {}

void Residual::initialize(char* option) {
  char techno[4];
  char nom[80], titre[80];
  for (int i=0; i<6; i++) {
    if( i == 0)        strcpy(techno, "MDT");
    else if( i == 1 )  strcpy(techno, "RPC");
    else if( i == 2 )  strcpy(techno, "TGC");
    else if( i == 3 )  strcpy(techno, "CS1");  // strips, obsolete 
    else if( i == 4 )  strcpy(techno, "CS2");  // strips, obsolete
    else               strcpy(techno, "CSC");  // clusters 
    sprintf(nom  , "reco%s", techno);
    sprintf(titre, "Radius rec %s (%s)", techno, m_comment);
    if( i != 1 && i != 2 ) Rreco[i] = new TH1F(nom, titre, 100, -20., 20.);
    else                   Rreco[i] = new TH1F(nom, titre, 100, -2000., 2000.);
    sprintf(nom  , "simu%s", techno);
    sprintf(titre, "Radius sim %s (%s)", techno, m_comment);
    if( i != 1 && i != 2 ) Rsimu[i] = new TH1F(nom, titre, 100, -20., 20.);
    else                   Rsimu[i] = new TH1F(nom, titre, 100, -2000., 2000.);
    sprintf(nom  , "efit%s", techno);
    sprintf(titre, "Error fit %s (%s)", techno, m_comment);
    Refit[i] = new TH1F(nom, titre, 100, 0., 0.3);
    sprintf(nom  , "resi%s", techno);
    sprintf(titre, "Residual %s (%s)", techno, m_comment);
    if( i != 1 && i != 2 ) Rresi[i] = new TH1F(nom, titre, 100, -20., 20.);
    else                   Rresi[i] = new TH1F(nom, titre, 100, -50., 50.);
    sprintf(nom  , "pull%s", techno);
    sprintf(titre, "Pull %s (%s)", techno, m_comment);
    Rpull[i] = new TH1F(nom, titre, 100, -10., 10.);
    sprintf(nom  , "pullBon%s", techno);
    sprintf(titre, "Pull (Assoc) %s (%s)", techno, m_comment);
    RpullBon[i] = new TH1F(nom, titre, 100, -10., 10.);
    sprintf(nom  , "pullBad%s", techno);
    sprintf(titre, "Pull (no Assoc) %s (%s)", techno, m_comment);
    RpullBad[i] = new TH1F(nom, titre, 100, -10., 10.);
    sprintf(nom  , "pullBarrel%s", techno);
    sprintf(titre, "Pull (Barrel) %s (%s)", techno, m_comment);
    RpullB[i] = new TH1F(nom, titre, 100, -20., 20.);
    sprintf(nom  , "pullEndCap%s", techno);
    sprintf(titre, "Pull (EndCap) %s (%s)", techno, m_comment);
    RpullE[i] = new TH1F(nom, titre, 100, -20., 20.);
  }
// residus detaillés pour RPC/TGC
  RresiRPCeta = new TH1F("resiRPCeta","resiRPCeta",100,-50.,50.);
  RresiRPCphi = new TH1F("resiRPCphi","resiRPCphi",100,-50.,50.);
  RresiTGCeta = new TH1F("resiTGCeta","resiTGCeta",100,-50.,150.);
  RresiTGCphi = new TH1F("resiTGCphi","resiTGCphi",100,-50.,150.);
  problemTGCtyp = new TH1F("problemTGCtyp","problemTGCtyp",70,0.,70.);
  problemTGCeta = new TH1F("problemTGCeta","problemTGCeta",40,-20.,20.);
  problemTGCphi = new TH1F("problemTGCphi","problemTGCphi",10,0.,10.);
  problemTGCgap = new TH1F("problemTGCgap","problemTGCgap",5,0.,5.);
// residus par technologie
  RJTYP = new TH1F("rjtyp","jtyp",70,0.,70.);
  TString tnom, ttitre;
  char s[80];
  for (int i=0; i<60; i++) {
    tnom = "Resi"+ station[i];
    ttitre = "Resi " + station[i];
    RresiTEC[i] = new TH1F(tnom, ttitre, 100, -10., 10.);
    tnom = "Pull"+ station[i];
    ttitre = "Pull " + station[i];
    RpullTEC[i] = new TH1F(tnom, ttitre, 100, -5., 5.);   
    for (int k=0; k<9; k++) {
      strcpy(s, "*******");
      if( i == 3 || i == 5 || i == 18 || i == 21 ) {
        if(      k == 0 ) strcpy(s, "|S| < 400");
        else if( k == 1 ) strcpy(s, "400 < |S| < 1000");
        else if( k == 2 ) strcpy(s, "|S| > 1000");
        else if( k == 3 ) strcpy(s, "vertical stations");
        else if( k == 4 ) strcpy(s, "inclined stations");
        else if( k == 5 ) strcpy(s, "horizontal stations"); 
      } else if( i == 4 || i == 6 || i == 19 || i == 22 ) {
        if(      k == 0 ) strcpy(s, "|S| < 400");
        else if( k == 1 ) strcpy(s, "400 < |S| < 1000");
        else if( k == 2 ) strcpy(s, "|S| > 1000");
        else if( k == 3 ) strcpy(s, "nearly vertical stations");
        else if( k == 4 ) strcpy(s, "nearly horizontal stations");
      }
      if( i == 3 || i == 4 || i == 5 || i == 6 ) {
        if(      k == 6 ) strcpy(s, "|eta| < 0.4");
        else if( k == 7 ) strcpy(s, "0.4 < |eta| < 0.8");
        else if( k == 8 ) strcpy(s, "|eta| > 0.8");
      } else if ( i == 18 || i == 19 || i == 21 || i == 22 ) {
        if(      k == 6 ) strcpy(s, "|eta| < 1.6");
        else if( k == 7 ) strcpy(s, "1.6 < |eta| < 2.1");
        else if( k == 8 ) strcpy(s, "|eta| > 2.1");
      }
      if( i == 3 ) {
        sprintf( nom,   "ResiBML%u", k);
        sprintf( titre, "Resi BML %s", s);
        RresiBML[k] = new TH1F(nom, titre, 100, -0.8,  0.8);
      } else if( i == 4 ) {
        sprintf( nom, "ResiBMS%u", k);
        sprintf( titre, "Resi BMS %s", s);
        RresiBMS[k] = new TH1F(nom, titre, 100, -0.8,  0.8);
      } else if( i == 5 ) {
        sprintf( nom,   "ResiBOL%u", k);
        sprintf( titre, "Resi BOL %s", s);
        RresiBOL[k] = new TH1F(nom, titre, 100, -0.8,  0.8);
      } else if( i == 6 ) {
        sprintf( nom, "ResiBOS%u", k);
        sprintf( titre, "Resi BOS %s", s);
        RresiBOS[k] = new TH1F(nom, titre, 100, -0.8,  0.8);
      } else if( i == 18 ) {
        sprintf( nom, "ResiEML%u", k);
        sprintf( titre, "Resi EML %s", s);
        RresiEML[k] = new TH1F(nom, titre, 100, -0.8,  0.8);
      } else if( i == 19 ) {
        sprintf( nom, "ResiEMS%u", k);
        sprintf( titre, "Resi EMS %s", s);
        RresiEMS[k] = new TH1F(nom, titre, 100, -0.8,  0.8);
      } else if( i == 21 ) {
        sprintf( nom, "ResiEOL%u", k);
        sprintf( titre, "Resi EOL %s", s);
        RresiEOL[k] = new TH1F(nom, titre, 100, -0.8,  0.8);
      } else if( i == 22 ) {
        sprintf( nom, "ResiEOS%u", k);
        sprintf( titre, "Resi EOS %s", s);
        RresiEOS[k] = new TH1F(nom, titre, 100, -0.8,  0.8);
      }
    }
  }
  RresiBOLAll = new TH1F("ResiBOLALL", "Resi BOL", 100, -0.8,  0.8);
  RresiBOSAll = new TH1F("ResiBOSALL", "Resi BOS", 100, -0.8,  0.8);
  RresiEOLAll = new TH1F("ResiEOLALL", "Resi EOL", 100, -0.8,  0.8);
  RresiEOSAll = new TH1F("ResiEOSALL", "Resi EOS", 100, -0.8,  0.8);
  SveriBML = new TH2F("SveriBML", "S vs phi BML", 45, 0., 45., 50, -3000.,3000.);
  SveriBMS = new TH2F("SveriBMS", "S vs phi BMS", 45, 0., 45., 50, -3000.,3000.);
  SveriBOL = new TH2F("SveriBOL", "S vs phi BOL", 45, 0., 45., 50, -3000.,3000.);
  SveriBOS = new TH2F("SveriBOS", "S vs phi BOS", 45, 0., 45., 50, -3000.,3000.);
  SveriEML = new TH2F("SveriEML", "S vs phi EML", 45, 0., 45., 50, -3000.,3000.);
  SveriEMS = new TH2F("SveriEMS", "S vs phi EMS", 45, 0., 45., 50, -3000.,3000.);
  SveriEOL = new TH2F("SveriEOL", "S vs phi EOL", 45, 0., 45., 50, -3000.,3000.);
  SveriEOS = new TH2F("SveriEOS", "S vs phi EOS", 45, 0., 45., 50, -3000.,3000.);
}

void Residual::execute() {
// on ne garde que les evts avec une seule trace simulee et reconstruite pour eviter
// les problemes d'association sim-rec

  if( p_evt->MuonEntRec_Num == 1 && p_evt->MboyTrkMB_NTrk == 1 && p_assocMS->getSim(0) == 0 ) {
    int ndig = (p_evt->MboyTrkVT_digits)->at(0);
    int fdig = (p_evt->MboyTrkVT_HitPosition)->at(0) - 1;
    for (int i=fdig; i<fdig+ndig; i++) {
      int itec = (p_evt->MboyTrkVT_TrHi_ChamberTech)->at(i) - 1;
      if( itec == 0 || itec == 5 ) {   // MDT(0) or CSC(5)
        Rreco[itec]->Fill( (p_evt->MboyTrkVT_TrHi_Rdigi)->at(i) );
        Rsimu[itec]->Fill( (p_evt->MboyTrkVT_TrHi_RZSsim)->at(i) );
        float diff = (p_evt->MboyTrkVT_TrHi_Rdigi)->at(i) - (p_evt->MboyTrkVT_TrHi_RZSsim)->at(i);
        float pull =  diff / (p_evt->MboyTrkVT_TrHi_EZSfit)->at(i);
        Rresi[itec]->Fill( diff );
        Refit[itec]->Fill( (p_evt->MboyTrkVT_TrHi_EZSfit)->at(i) );
        Rpull[itec]->Fill( pull );
        if( p_assocMS->getSim(0) == 0 )   RpullBon[itec]->Fill( pull ); 
        else                              RpullBad[itec]->Fill( pull );
        if( fabs( (p_evt->MboyTrkVT_CotTh)->at(0) ) < 1. ) RpullB[itec]->Fill( pull );
        else                                        RpullE[itec]->Fill( pull );
        if( itec == 0 ) {
          int jtyp = (p_evt->MboyTrkVT_TrHi_JTYP)->at(i);
          RJTYP->Fill( jtyp );
          RresiTEC[jtyp]->Fill( diff );
          RpullTEC[jtyp]->Fill( pull );
          float stub = (p_evt->MboyTrkVT_TrHi_Sdca)->at(i);
          int fi = (p_evt->MboyTrkVT_TrHi_JFF)->at(i);
          if( jtyp == 3 || jtyp == 4 || jtyp == 5 || jtyp == 6 || jtyp == 18 || jtyp == 19 || jtyp == 21 || jtyp == 22 ) {
            if( jtyp ==  5 ) RresiBOLAll->Fill(diff);
            if( jtyp ==  6 ) RresiBOSAll->Fill(diff);
            if( jtyp == 21 ) RresiEOLAll->Fill(diff);
            if( jtyp == 22 ) RresiEOSAll->Fill(diff);
            int ss;
            if(      fabs(stub) <  400. ) ss = 0;
            else if( fabs(stub) < 1000. ) ss = 1;
            else                          ss = 2;
            if( jtyp ==  3 ) RresiBML[ss]->Fill(diff);
            if( jtyp ==  4 ) RresiBMS[ss]->Fill(diff);
            if( jtyp ==  5 ) RresiBOL[ss]->Fill(diff);
            if( jtyp ==  6 ) RresiBOS[ss]->Fill(diff);
            if( jtyp == 18 ) RresiEML[ss]->Fill(diff);
            if( jtyp == 19 ) RresiEMS[ss]->Fill(diff);
            if( jtyp == 21 ) RresiEOL[ss]->Fill(diff);
            if( jtyp == 22 ) RresiEOS[ss]->Fill(diff);
            if( jtyp == 3 || jtyp == 5 || jtyp == 18 | jtyp == 21 ) {  // large chambers
              if(      fi == 1 || fi == 5 ) ss = 3;   // vertical
              else if( fi == 3 || fi == 7 ) ss = 5;   // horizontal
              else                          ss = 4;   // inclined
            } else {                                                      // small chambers
              if( fi == 1 || fi == 4 || fi == 5 || fi == 8 ) ss = 3;  // nearly verical
	      else                                           ss = 4;  // nearly horizontal        
            }
            if( jtyp ==  3 ) RresiBML[ss]->Fill(diff);
            if( jtyp ==  4 ) RresiBMS[ss]->Fill(diff);
            if( jtyp ==  5 ) RresiBOL[ss]->Fill(diff);
            if( jtyp ==  6 ) RresiBOS[ss]->Fill(diff);
            if( jtyp == 18 ) RresiEML[ss]->Fill(diff);
            if( jtyp == 19 ) RresiEMS[ss]->Fill(diff);
            if( jtyp == 21 ) RresiEOL[ss]->Fill(diff);
            if( jtyp == 22 ) RresiEOS[ss]->Fill(diff);
            if( jtyp == 3 || jtyp == 4 || jtyp == 5 | jtyp == 6 ) {  // barrel chambers
              if(      fabs( p_assocMS->getEtaSim(0) ) < 0.4 ) ss = 6;
	      else if( fabs( p_assocMS->getEtaSim(0) ) < 0.8 ) ss = 7;
	      else                                             ss = 8;
            } else {                                                 // endcap chambers
              if(      fabs( p_assocMS->getEtaSim(0) ) < 1.6 ) ss = 6;
	      else if( fabs( p_assocMS->getEtaSim(0) ) < 2.1 ) ss = 7;
	      else                                             ss = 8;
            }
            if( jtyp ==  3 ) RresiBML[ss]->Fill(diff);
            if( jtyp ==  4 ) RresiBMS[ss]->Fill(diff);
            if( jtyp ==  5 ) RresiBOL[ss]->Fill(diff);
            if( jtyp ==  6 ) RresiBOS[ss]->Fill(diff);
            if( jtyp == 18 ) RresiEML[ss]->Fill(diff);
            if( jtyp == 19 ) RresiEMS[ss]->Fill(diff);
            if( jtyp == 21 ) RresiEOL[ss]->Fill(diff);
            if( jtyp == 22 ) RresiEOS[ss]->Fill(diff);
            float fifi = p_assocMS->getPhiSim(0) * 180. / 3.14159;
            while ( fifi > 45. ) fifi = fifi - 45.;
            while ( fifi <  0. ) fifi = fifi + 45.;
	    if( jtyp ==  3 ) SveriBML->Fill(fifi, stub);
	    if( jtyp ==  4 ) SveriBMS->Fill(fifi, stub);
	    if( jtyp ==  5 ) SveriBOL->Fill(fifi, stub);
	    if( jtyp ==  6 ) SveriBOS->Fill(fifi, stub);
	    if( jtyp == 18 ) SveriEML->Fill(fifi, stub);
	    if( jtyp == 19 ) SveriEMS->Fill(fifi, stub);
	    if( jtyp == 21 ) SveriEOL->Fill(fifi, stub);
	    if( jtyp == 22 ) SveriEOS->Fill(fifi, stub);
          }
        }
      } else if( itec == 1 || itec == 2 ) {         // RPC(1) and TGC(2)
        Rreco[itec]->Fill( (p_evt->MboyTrkVT_TrHi_Rdigi)->at(i) );
        Rsimu[itec]->Fill( (p_evt->MboyTrkVT_TrHi_RZSsim)->at(i) );
        float diff = (p_evt->MboyTrkVT_TrHi_Rdigi)->at(i) - (p_evt->MboyTrkVT_TrHi_RZSsim)->at(i);
        Rresi[itec]->Fill( diff );
        Rpull[itec]->Fill( diff / (p_evt->MboyTrkVT_TrHi_EZSfit)->at(i) );
        if( itec == 1 ) {
          if( (p_evt->MboyTrkVT_TrHi_MeasuresPhi)->at(i) == 1 ) RresiRPCphi->Fill(diff);
          else                                                  RresiRPCeta->Fill(diff);
        } else {
          if( (p_evt->MboyTrkVT_TrHi_IsStrip)->at(i) == 1 )     RresiTGCphi->Fill(diff);
          else                                                  RresiTGCeta->Fill(diff);
          if( (p_evt->MboyTrkVT_TrHi_IsStrip)->at(i) == 0 ) {
            if( diff > 40. ) {
              problemTGCtyp->Fill( (p_evt->MboyTrkVT_TrHi_JTYP)->at(i) );
              problemTGCeta->Fill( (p_evt->MboyTrkVT_TrHi_StationEta)->at(i) );
	      problemTGCphi->Fill( (p_evt->MboyTrkVT_TrHi_StationPhi)->at(i) );
	      problemTGCgap->Fill( (p_evt->MboyTrkVT_TrHi_GasGap)->at(i) );
            }
          }
        }
      } else {
        cout << "============= itec = " << itec << " =====================" << endl;
      }
    }
  }
}

void Residual::finalize() {
  char nom[80];
  TCanvas *canvas1 = new TCanvas("Resi1", "Resi1", 60, 10, m_LARGPS, m_HAUTPS); canvas1->Divide(2,3);
  canvas1->cd(1); Rreco[0]->SetMarkerSize(0.6); Rreco[0]->Draw("E0");
  canvas1->cd(2); Rsimu[0]->SetMarkerSize(0.6); Rsimu[0]->Draw("E0");
  canvas1->cd(3); Rreco[3]->SetMarkerSize(0.6); Rreco[3]->Draw("E0");
  canvas1->cd(4); Rsimu[3]->SetMarkerSize(0.6); Rsimu[3]->Draw("E0");
  canvas1->cd(5); Rreco[4]->SetMarkerSize(0.6); Rreco[4]->Draw("E0");
  canvas1->cd(6); Rsimu[4]->SetMarkerSize(0.6); Rsimu[4]->Draw("E0");
  canvas1->Update();
  TCanvas *canvas2 = new TCanvas("Resi2", "Resi2", 60, 10, m_LARGPS, m_HAUTPS); canvas2->Divide(2,3);
  canvas2->cd(1); Rresi[0]->SetMarkerSize(0.6); Rresi[0]->Draw("E0");
  canvas2->cd(2); Rpull[0]->SetMarkerSize(0.6); Rpull[0]->Draw("E0");
  canvas2->cd(3); Rresi[3]->SetMarkerSize(0.6); Rresi[3]->Draw("E0");
  canvas2->cd(4); Rpull[3]->SetMarkerSize(0.6); Rpull[3]->Draw("E0");
  canvas2->cd(5); Rresi[4]->SetMarkerSize(0.6); Rresi[4]->Draw("E0");
  canvas2->cd(6); Rpull[4]->SetMarkerSize(0.6); Rpull[4]->Draw("E0");
  canvas2->Update();
  TCanvas *canvas3 = new TCanvas("Resi3", "Resi3", 60, 10, m_LARGPS, m_HAUTPS); canvas3->Divide(2,2);
  canvas3->cd(1); Rreco[1]->SetMarkerSize(0.6); Rreco[1]->Draw("E0");
  canvas3->cd(2); Rsimu[1]->SetMarkerSize(0.6); Rsimu[1]->Draw("E0");
  canvas3->cd(3); Rreco[2]->SetMarkerSize(0.6); Rreco[2]->Draw("E0");
  canvas3->cd(4); Rsimu[2]->SetMarkerSize(0.6); Rsimu[2]->Draw("E0");
  canvas3->Update();
  TCanvas *canvas4 = new TCanvas("Resi4", "Resi4", 60, 10, m_LARGPS, m_HAUTPS); canvas4->Divide(2,2);
  canvas4->cd(1); Rresi[1]->SetMarkerSize(0.6); Rresi[1]->Draw("E0");
  canvas4->cd(2); Rpull[1]->SetMarkerSize(0.6); Rpull[1]->Draw("E0");
  canvas4->cd(3); Rresi[2]->SetMarkerSize(0.6); Rresi[2]->Draw("E0");
  canvas4->cd(4); Rpull[2]->SetMarkerSize(0.6); Rpull[2]->Draw("E0");
  canvas4->Update();

  TCanvas *canvas5 = new TCanvas("Resi5", "Resi5", 60, 10, m_LARGPS, m_HAUTPS); canvas5->Divide(2,2);
  canvas5->cd(1); RpullBon[0]->SetMarkerSize(0.6); RpullBon[0]->Draw("E0"); gStyle->SetOptStat(111110);
  canvas5->cd(2); RpullBad[0]->SetMarkerSize(0.6); RpullBad[0]->Draw("E0"); gStyle->SetOptStat(111110);
  canvas5->cd(3);   RpullB[0]->SetMarkerSize(0.6);   RpullB[0]->Draw("E0"); gStyle->SetOptStat(111110);
  canvas5->cd(4);   RpullE[0]->SetMarkerSize(0.6);   RpullE[0]->Draw("E0"); gStyle->SetOptStat(111110);
  canvas5->Update();

  TCanvas * canvasA = new TCanvas("ResiA", "ResiA", 60, 10, m_LARGPS, m_HAUTPS);
  RJTYP->Draw();
  canvasA->Update();

  TCanvas * canvasB = new TCanvas("ResiB", "ResiB", 60, 10, m_LARGPS, m_HAUTPS); canvasB->Divide(2,3);
  canvasB->cd(1); RresiTEC[ 1]->SetMarkerSize(0.6); RresiTEC[ 1]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasB->cd(2); RresiTEC[ 2]->SetMarkerSize(0.6); RresiTEC[ 2]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasB->cd(3); RresiTEC[ 3]->SetMarkerSize(0.6); RresiTEC[ 3]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasB->cd(4); RresiTEC[ 4]->SetMarkerSize(0.6); RresiTEC[ 4]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasB->cd(5); RresiTEC[ 5]->SetMarkerSize(0.6); RresiTEC[ 5]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasB->cd(6); RresiTEC[ 6]->SetMarkerSize(0.6); RresiTEC[ 6]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasB->Update();

  TCanvas * canvasC = new TCanvas("ResiC", "ResiC", 60, 10, m_LARGPS, m_HAUTPS); canvasC->Divide(2,3);
  canvasC->cd(1); RresiTEC[14]->SetMarkerSize(0.6); RresiTEC[14]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasC->cd(2); RresiTEC[50]->SetMarkerSize(0.6); RresiTEC[50]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasC->cd(3); RresiTEC[18]->SetMarkerSize(0.6); RresiTEC[18]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasC->cd(4); RresiTEC[19]->SetMarkerSize(0.6); RresiTEC[19]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasC->cd(5); RresiTEC[21]->SetMarkerSize(0.6); RresiTEC[21]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasC->cd(6); RresiTEC[22]->SetMarkerSize(0.6); RresiTEC[22]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasC->Update();

  TCanvas * canvasD = new TCanvas("ResiD", "ResiD", 60, 10, m_LARGPS, m_HAUTPS); canvasD->Divide(2,3);
  canvasD->cd(1); RresiTEC[10]->SetMarkerSize(0.6); RresiTEC[10]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasD->cd(2); RresiTEC[11]->SetMarkerSize(0.6); RresiTEC[11]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasD->cd(3); RresiTEC[ 9]->SetMarkerSize(0.6); RresiTEC[ 9]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasD->cd(4); RresiTEC[ 7]->SetMarkerSize(0.6); RresiTEC[ 7]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasD->cd(5); RresiTEC[ 8]->SetMarkerSize(0.6); RresiTEC[ 8]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasD->cd(6); RresiTEC[53]->SetMarkerSize(0.6); RresiTEC[53]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasD->Update();

  TCanvas * canvasE = new TCanvas("ResiE", "ResiE", 60, 10, m_LARGPS, m_HAUTPS); canvasE->Divide(2,3);
  canvasE->cd(1); RpullTEC[ 1]->SetMarkerSize(0.6); RpullTEC[ 1]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasE->cd(2); RpullTEC[ 2]->SetMarkerSize(0.6); RpullTEC[ 2]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasE->cd(3); RpullTEC[ 3]->SetMarkerSize(0.6); RpullTEC[ 3]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasE->cd(4); RpullTEC[ 4]->SetMarkerSize(0.6); RpullTEC[ 4]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasE->cd(5); RpullTEC[ 5]->SetMarkerSize(0.6); RpullTEC[ 5]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasE->cd(6); RpullTEC[ 6]->SetMarkerSize(0.6); RpullTEC[ 6]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasE->Update();

  TCanvas * canvasF = new TCanvas("ResiF", "ResiF", 60, 10, m_LARGPS, m_HAUTPS); canvasF->Divide(2,3);
  canvasF->cd(1); RpullTEC[14]->SetMarkerSize(0.6); RpullTEC[14]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasF->cd(2); RpullTEC[50]->SetMarkerSize(0.6); RpullTEC[50]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasF->cd(3); RpullTEC[18]->SetMarkerSize(0.6); RpullTEC[18]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasF->cd(4); RpullTEC[19]->SetMarkerSize(0.6); RpullTEC[19]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasF->cd(5); RpullTEC[21]->SetMarkerSize(0.6); RpullTEC[21]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasF->cd(6); RpullTEC[22]->SetMarkerSize(0.6); RpullTEC[22]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasF->Update();

  TCanvas * canvasG = new TCanvas("ResiG", "ResiG", 60, 10, m_LARGPS, m_HAUTPS); canvasG->Divide(2,3);
  canvasG->cd(1); RpullTEC[10]->SetMarkerSize(0.6); RpullTEC[10]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasG->cd(2); RpullTEC[11]->SetMarkerSize(0.6); RpullTEC[11]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasG->cd(3); RpullTEC[ 9]->SetMarkerSize(0.6); RpullTEC[ 9]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasG->cd(4); RpullTEC[ 7]->SetMarkerSize(0.6); RpullTEC[ 7]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasG->cd(5); RpullTEC[ 8]->SetMarkerSize(0.6); RpullTEC[ 8]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasG->cd(6); RpullTEC[53]->SetMarkerSize(0.6); RpullTEC[53]->Draw("E0"); gStyle->SetOptStat(111110);
  canvasG->Update();

  TCanvas * canvasH = new TCanvas("ResiH", "ResiH", 60, 10, m_LARGPS, m_HAUTPS); canvasH->Divide(2,2);
  canvasH->cd(1); RresiRPCeta->SetMarkerSize(0.6); RresiRPCeta->Draw("E0"); gStyle->SetOptStat(111110);
  canvasH->cd(2); RresiTGCeta->SetMarkerSize(0.6); RresiTGCeta->Draw("E0"); gStyle->SetOptStat(111110);
  canvasH->cd(3); RresiRPCphi->SetMarkerSize(0.6); RresiRPCphi->Draw("E0"); gStyle->SetOptStat(111110);
  canvasH->cd(4); RresiTGCphi->SetMarkerSize(0.6); RresiTGCphi->Draw("E0"); gStyle->SetOptStat(111110);
  canvasH->Update();

  TCanvas * canvasI = new TCanvas("ResiI", "ResiI", 60, 10, m_LARGPS, m_HAUTPS); canvasI->Divide(2,2);
  canvasI->cd(1); problemTGCtyp->SetMarkerSize(0.6); problemTGCtyp->Draw("E0"); gStyle->SetOptStat(111110);
  canvasI->cd(2); problemTGCeta->SetMarkerSize(0.6); problemTGCeta->Draw("E0"); gStyle->SetOptStat(111110);
  canvasI->cd(3); problemTGCphi->SetMarkerSize(0.6); problemTGCphi->Draw("E0"); gStyle->SetOptStat(111110);
  canvasI->cd(4); problemTGCgap->SetMarkerSize(0.6); problemTGCgap->Draw("E0"); gStyle->SetOptStat(111110);
  canvasI->Update();

  TCanvas * canvAll = new TCanvas("ResiAll", "ResiAll",  60, 10, m_LARGPS, m_HAUTPS); canvAll->Divide(2,2);
  canvAll->cd(1); drawHist( RresiBOLAll );
  canvAll->cd(2); drawHist( RresiBOSAll );
  canvAll->cd(3); drawHist( RresiEOLAll );
  canvAll->cd(4); drawHist( RresiEOSAll );
  canvAll->Update();

  TCanvas * canvBML = new TCanvas("ResiBML", "ResiBML",  60, 10, m_LARGPS, m_HAUTPS); canvBML->Divide(3,3);
  for (int i = 0; i < 9; i++) { canvBML->cd(i+1); drawHist( RresiBML[i] ); }
  canvBML->Update();

  TCanvas * canvBMS = new TCanvas("ResiBMS", "ResiBMS",  60, 10, m_LARGPS, m_HAUTPS); canvBMS->Divide(3,3);
  for (int i = 0; i < 9; i++) { canvBMS->cd(i+1); drawHist( RresiBMS[i] ); }
  canvBMS->Update();

  TCanvas * canvBOL = new TCanvas("ResiBOL", "ResiBOL",  60, 10, m_LARGPS, m_HAUTPS); canvBOL->Divide(3,3);
  for (int i = 0; i < 9; i++) { canvBOL->cd(i+1); drawHist( RresiBOL[i] ); }
  canvBOL->Update();

  TCanvas * canvBOS = new TCanvas("ResiBOS", "ResiBOS",  60, 10, m_LARGPS, m_HAUTPS); canvBOS->Divide(3,3);
  for (int i = 0; i < 9; i++) { canvBOS->cd(i+1); drawHist( RresiBOS[i] ); }
  canvBOS->Update();

  TCanvas * canvEML = new TCanvas("ResiEML", "ResiEML",  60, 10, m_LARGPS, m_HAUTPS); canvEML->Divide(3,3);
  for (int i = 0; i < 9; i++) { canvEML->cd(i+1); drawHist( RresiEML[i] ); }
  canvEML->Update();

  TCanvas * canvEMS = new TCanvas("ResiEMS", "ResiEMS",  60, 10, m_LARGPS, m_HAUTPS); canvEMS->Divide(3,3);
  for (int i = 0; i < 9; i++) { canvEMS->cd(i+1); drawHist( RresiEMS[i] ); }
  canvEMS->Update();

  TCanvas * canvEOL = new TCanvas("ResiEOL", "ResiEOL",  60, 10, m_LARGPS, m_HAUTPS); canvEOL->Divide(3,3);
  for (int i = 0; i < 9; i++) { canvEOL->cd(i+1); drawHist( RresiEOL[i] ); }
  canvEOL->Update();

  TCanvas * canvEOS = new TCanvas("ResiEOS", "ResiEOS",  60, 10, m_LARGPS, m_HAUTPS); canvEOS->Divide(3,3);
  for (int i = 0; i < 9; i++) { canvEOS->cd(i+1); drawHist( RresiEOS[i] ); }
  canvEOS->Update();

  TCanvas * verif = new TCanvas("verif", "verif",  60, 10, m_LARGPS, m_HAUTPS); verif->Divide(4,2);
  verif->cd(1); drawHist2( SveriBML );
  verif->cd(2); drawHist2( SveriBMS );
  verif->cd(3); drawHist2( SveriBOL );
  verif->cd(4); drawHist2( SveriBOS );
  verif->cd(5); drawHist2( SveriEML );
  verif->cd(6); drawHist2( SveriEMS );
  verif->cd(7); drawHist2( SveriEOL );
  verif->cd(8); drawHist2( SveriEOS );
  verif->Update();

  sprintf(nom, "Residual%s.ps(", m_comment); canvas1->Print(nom);
  sprintf(nom, "Residual%s.ps" , m_comment); canvas2->Print(nom);
                                             canvas3->Print(nom);
                                             canvas4->Print(nom);
                                             canvas5->Print(nom);
                                             canvasA->Print(nom);
                                             canvasB->Print(nom);
                                             canvasC->Print(nom);
                                             canvasD->Print(nom);
                                             canvasE->Print(nom);
                                             canvasF->Print(nom);
                                             canvasG->Print(nom);
                                             canvasH->Print(nom);
  sprintf(nom, "Residual%s.ps)", m_comment); canvasI->Print(nom);


  sprintf(nom, "Resisag%s.ps(", m_comment); canvAll->Print(nom);
  sprintf(nom, "Resisag%s.ps" , m_comment); canvBML->Print(nom);
                                            canvBMS->Print(nom);
                                            canvBOL->Print(nom);
                                            canvBOS->Print(nom);
                                            canvEML->Print(nom);
                                            canvEMS->Print(nom);
                                            canvEOL->Print(nom);
  sprintf(nom, "Resisag%s.ps)", m_comment); canvEOS->Print(nom);

  sprintf(nom, "SvsPhi%s.ps()", m_comment); verif->Print(nom);
}

void Residual::drawHist(TH1F * histo) {
  histo->SetMarkerSize(0.6);
  histo->Draw("E0");
  gStyle->SetOptStat(111110);
  TPaveStats *st = (TPaveStats*)histo->GetListOfFunctions()->FindObject("stats"); st->SetX1NDC(0.68);
}

void Residual::drawHist2(TH2F * histo) {
  histo->SetMarkerSize(0.1);
  histo->Draw();
  gStyle->SetOptStat(110010);
}
